#include "pch.h"
#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <vector>
#include <string>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")

// Глобальный hModule — для путей
HINSTANCE g_hModule = nullptr;

// CLSID
const CLSID CLSID_MySearchMenu = { 0xA1B2C3D4, 0xE5F6, 0x47A8, {0xB9, 0xC0, 0xD1, 0xE2, 0xF3, 0x04, 0x15, 0x26} };

// Локализация меню
std::wstring GetLocalizedMenuText() {
    LANGID lang = GetUserDefaultUILanguage();
    if (lang == 1049) {
        return L"Поиск с MySearch";
    }
    return L"Search with MySearch";
}

// Класс меню
class CMySearchExt : public IShellExtInit, public IContextMenu {
private:
    LONG m_cRef;
    std::vector<std::wstring> m_files;
public:
    CMySearchExt() : m_cRef(1) {}
    ~CMySearchExt() = default;

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IContextMenu) {
            *ppv = static_cast<IContextMenu*>(this);
        }
        else if (riid == IID_IShellExtInit) {
            *ppv = static_cast<IShellExtInit*>(this);
        }
        else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_cRef); }
    STDMETHODIMP_(ULONG) Release() {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0) delete this;
        return cRef;
    }

    STDMETHODIMP Initialize(LPCITEMIDLIST, IDataObject* pDataObj, HKEY) {
        m_files.clear();
        if (!pDataObj) return E_INVALIDARG;
        FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg = { TYMED_HGLOBAL, nullptr, nullptr };
        if (SUCCEEDED(pDataObj->GetData(&fe, &stg))) {
            HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
            if (hDrop) {
                UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                for (UINT i = 0; i < count; ++i) {
                    wchar_t szPath[MAX_PATH] = {};
                    if (DragQueryFileW(hDrop, i, szPath, MAX_PATH)) {
                        if (_wcsicmp(PathFindExtensionW(szPath), L".txt") == 0) {
                            m_files.emplace_back(szPath);
                        }
                    }
                }
                GlobalUnlock(stg.hGlobal);
            }
            ReleaseStgMedium(&stg);
        }
        return m_files.empty() ? E_FAIL : S_OK;
    }

    // IContextMenu
    STDMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT, UINT) {
        if (m_files.empty()) return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
        std::wstring menuText = GetLocalizedMenuText();
        InsertMenuW(hMenu, indexMenu, MF_BYPOSITION | MF_STRING, idCmdFirst, menuText.c_str());
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
    }

    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
        if (!pici || HIWORD(pici->lpVerb) != 0) return E_INVALIDARG;
        if (LOWORD(pici->lpVerb) != 0) return E_FAIL;
        if (m_files.empty()) return E_FAIL;

        wchar_t szExePath[MAX_PATH] = {};
        if (!GetModuleFileNameW(g_hModule, szExePath, MAX_PATH)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        PathRemoveFileSpecW(szExePath);
        PathCombineW(szExePath, szExePath, L"MySearchApp.exe");
        if (!PathFileExistsW(szExePath)) return E_FAIL;

        std::wstring params;
        for (const auto& file : m_files) {
            if (!params.empty()) params += L" ";
            params += L"\"" + file + L"\"";
        }

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.lpVerb = L"open";
        sei.lpFile = szExePath;
        sei.lpParameters = params.empty() ? nullptr : params.c_str();
        sei.nShow = SW_SHOWNORMAL;

        if (!ShellExecuteExW(&sei)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }
        return S_OK;
    }

    STDMETHODIMP GetCommandString(UINT_PTR, UINT, UINT*, LPSTR, UINT) {
        return E_NOTIMPL;
    }
};

class CMySearchExtFactory : public IClassFactory {
private:
    LONG m_cRef;
public:
    CMySearchExtFactory() : m_cRef(1) {}
    ~CMySearchExtFactory() = default;

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
        if (riid == IID_IUnknown || riid == IID_IClassFactory) {
            *ppv = this;
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_cRef); }
    STDMETHODIMP_(ULONG) Release() {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0) delete this;
        return cRef;
    }

    // IClassFactory
    STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv) {
        if (pUnkOuter) return CLASS_E_NOAGGREGATION;
        CMySearchExt* pExt = new (std::nothrow) CMySearchExt();
        if (!pExt) return E_OUTOFMEMORY;
        HRESULT hr = pExt->QueryInterface(riid, ppv);
        pExt->Release();
        return hr;
    }
    STDMETHODIMP LockServer(BOOL) { return S_OK; }
};

// Функции DLL
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    if (rclsid != CLSID_MySearchMenu) return CLASS_E_CLASSNOTAVAILABLE;
    CMySearchExtFactory* pFactory = new (std::nothrow) CMySearchExtFactory();
    if (!pFactory) return E_OUTOFMEMORY;
    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow() { return S_OK; }

// Регистрация
STDAPI DllRegisterServer()
{
    wchar_t szCLSID[64] = {};
    StringFromGUID2(CLSID_MySearchMenu, szCLSID, _countof(szCLSID));
    wchar_t szModule[MAX_PATH] = {};
    GetModuleFileNameW(g_hModule, szModule, MAX_PATH);
    HKEY hKey = nullptr;

    // CLSID
    std::wstring clsidKey = L"CLSID\\" + std::wstring(szCLSID);
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, clsidKey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS)
    {
        RegSetValueExW(hKey, nullptr, 0, REG_SZ, (BYTE*)L"MySearch Context Menu Handler", 56 * sizeof(wchar_t));
        RegCloseKey(hKey);
    }

    // InprocServer32
    std::wstring inproc = clsidKey + L"\\InprocServer32";
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, inproc.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS)
    {
        RegSetValueExW(hKey, nullptr, 0, REG_SZ, (BYTE*)szModule, (wcslen(szModule) + 1) * sizeof(wchar_t));
        RegSetValueExW(hKey, L"ThreadingModel", 0, REG_SZ, (BYTE*)L"Apartment", 10 * sizeof(wchar_t));
        RegCloseKey(hKey);
    }

    // ContextMenuHandlers
    std::wstring handlerKey = L"txtfile\\shellex\\ContextMenuHandlers\\MySearch";
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, handlerKey.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS)
    {
        RegSetValueExW(hKey, nullptr, 0, REG_SZ, (BYTE*)szCLSID, (wcslen(szCLSID) + 1) * sizeof(wchar_t));
        RegCloseKey(hKey);
    }

    // Approved (Win10/11)
    std::wstring approved = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, approved.c_str(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS)
    {
        const wchar_t* desc = L"MySearch – поиск в .txt файлах";
        RegSetValueExW(hKey, szCLSID, 0, REG_SZ, (BYTE*)desc, (wcslen(desc) + 1) * sizeof(wchar_t));
        RegCloseKey(hKey);
    }
    return S_OK;
}

// Снятие регистрации
STDAPI DllUnregisterServer()
{
    wchar_t szCLSID[64] = {};
    StringFromGUID2(CLSID_MySearchMenu, szCLSID, _countof(szCLSID));
    HKEY hKey = nullptr;

    // Удаляем handler
    RegDeleteTreeW(HKEY_CLASSES_ROOT, L"txtfile\\shellex\\ContextMenuHandlers\\MySearch");

    // Удаляем CLSID
    std::wstring clsidKey = L"CLSID\\" + std::wstring(szCLSID);
    RegDeleteTreeW(HKEY_CLASSES_ROOT, clsidKey.c_str());

    // Удаляем Approved
    std::wstring approvedKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved";
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, approvedKey.c_str(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
    {
        RegDeleteValueW(hKey, szCLSID);
        RegCloseKey(hKey);
    }
    return S_OK;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);
    }
    return TRUE;
}
