
#include "Main.h"


uintptr_t GamePid = 0;
uintptr_t GameBaseAddress = 0;

typedef bool (CEntity::* EntityPtrDef)();
uintptr_t eptr(EntityPtrDef method) {
    return *(uintptr_t*)&method;
}

void LoadProtectedFunctions() {
    uintptr_t t = milliseconds_now();
    BYTE xorkey = 0x0;
    for (DWORD i = 0; i < 8; i++) {
        xorkey = ((BYTE*)&t)[i];
        if (xorkey > 0x3 && xorkey < 0xf0) {
            break;
        }
    }
    if (xorkey <= 0x3 || xorkey >= 0xf0) {
        xorkey = 0x56;
    }

    addFunc({ LoadProtectedFunctions, (uintptr_t)CheckDriverStatus - (uintptr_t)LoadProtectedFunctions - 0x3, xorkey, false });
    addFunc({ CheckDriverStatus, (uintptr_t)GetProcessIdByName - (uintptr_t)CheckDriverStatus - 0x3, xorkey, false });
    addFunc({ GetProcessIdByName, (uintptr_t)milliseconds_now - (uintptr_t)GetProcessIdByName - 0x3, xorkey, false });
    addFunc({ mainThread, (uintptr_t)DllMain - (uintptr_t)mainThread - 0x3, xorkey, false });
    addFunc({ DllMain, (uintptr_t)std::_Narrow_char_traits<char, int>::eof - (uintptr_t)DllMain - 0x3, xorkey, false });


    addFunc({ Driver::SendCommand, (uintptr_t)Driver::GetBaseAddress - (uintptr_t)Driver::SendCommand - 0x3, xorkey, false });
    addFunc({ Driver::GetBaseAddress, (uintptr_t)Driver::copy_memory - (uintptr_t)Driver::GetBaseAddress - 0x3, xorkey, false });
    addFunc({ Driver::copy_memory, (uintptr_t)GetKernelModuleExport - (uintptr_t)Driver::copy_memory - 0x3, xorkey, false });
    addFunc({ GetKernelModuleExport, (uintptr_t)GetKernelModuleAddress - (uintptr_t)GetKernelModuleExport - 0x3, xorkey, false });
    addFunc({ GetKernelModuleAddress, (uintptr_t)Driver::initialize - (uintptr_t)GetKernelModuleAddress - 0x3, xorkey, false });
    addFunc({ Driver::initialize, (uintptr_t)Driver::read_memory - (uintptr_t)Driver::initialize - 0x3, xorkey, false });
    addFunc({ Driver::read_memory, (uintptr_t)Driver::write_memory - (uintptr_t)Driver::read_memory - 0x3, xorkey, false });
    
    for (size_t i = 0; i < funcCount; i++) {
        if (functions[i].address != LoadProtectedFunctions)
            Protect(functions[i].address);
    }
    Unprotect(_ReturnAddress());
}

bool CheckDriverStatus() {
    int icheck = 29;
    NTSTATUS status = 0;
    SLog(L"getting base");
    Unprotect(Driver::GetBaseAddress);
    uintptr_t BaseAddr = Driver::GetBaseAddress(Driver::currentProcessId);
    SLog(std::wstring(L"base: "+std::to_wstring(BaseAddr)).c_str());
    if (BaseAddr == 0) {
        return false;
    }
    Protect(Driver::GetBaseAddress);

    SLog(L"reading var");
    int checked = Driver::read<int>(Driver::currentProcessId, (uintptr_t)&icheck, &status);
    SLog(std::wstring(L"readed: " + std::to_wstring(checked)).c_str());
    if (checked != icheck) {
        return false;
    }
    
    return true;
}

DWORD GetProcessIdByName(wchar_t * name) {
    Protect(_ReturnAddress());

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (_wcsicmp(entry.szExeFile, name) == 0) {
                Unprotect(_ReturnAddress());
                return entry.th32ProcessID;
            }
        }
    }

    CloseHandle(snapshot);
    Unprotect(_ReturnAddress());
    return 0;
}

uintptr_t milliseconds_now() {
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    }
    else {
        return GetTickCount();
    }
}

template <typename T>
T* GetInterface(const char* interfaceName, const char* moduleName)
{
    const auto handle = GetModuleHandle(moduleName);

    if (!handle)
    {
        return nullptr;
    }

    using Fn = T * (*)(const char*, int*);
    const auto CreateInterface = reinterpret_cast<Fn>(GetProcAddress(handle, "CreateInterface"));

    if (!CreateInterface)
    {
        return nullptr;
    }

    return CreateInterface(interfaceName, nullptr);
}

void HackThread()
{
    Protect(_ReturnAddress());
    client = GetInterface<Client>("VClient018", "client.dll");
    entityList = GetInterface<EntityList>("VClientEntityList003", "client.dll");

    SetupNetvars();

    for (auto i = 1; i <= 64; ++i)
    {
        const auto entity = entityList->GetClientEntity(i);

        if (!entity)
            continue;

        entity->CurrentWeapon() = 0;
    }
}

DWORD WINAPI mainThread(PVOID) {
    SLog(L"Protecting");
    LoadProtectedFunctions();
    Protect(LoadProtectedFunctions);
    SLog(L"Connecting driver");
    Unprotect(Driver::initialize);
    Unprotect(CheckDriverStatus);
    if (!Driver::initialize() || !CheckDriverStatus()) {
        wchar_t VarName[] = { 'F','a','s','t','B','o','o','t','O','p','t','i','o','n','\0' };
        UNICODE_STRING FVariableName = UNICODE_STRING();
        FVariableName.Buffer = VarName;
        FVariableName.Length = 28;
        FVariableName.MaximumLength = 30;
        //UNICODE_STRING VariableName = RTL_CONSTANT_STRING(VARIABLE_NAME);
        myNtSetSystemEnvironmentValueEx(
            &FVariableName,
            &DummyGuid,
            0,
            0,
            ATTRIBUTES);//delete var
        memset(VarName, 0, sizeof(VarName));
        //memset(VariableName.Buffer, 0, VariableName.Length);
        //VariableName.Length = 0;
        Beep(600, 1000);
        char tx[] = { 'N','O',' ','E','F','I',' ',';','(','\n', 0 };
        printf(tx);
        ProtectedSleep(3000);
        exit(1);
        return 1;
    }
    Protect(Driver::initialize);
    Protect(CheckDriverStatus);

    Beep(900, 200);
    Beep(1100, 200);
    Beep(1300, 200);
    Beep(1500, 300);
    Beep(1700, 400);
    Beep(1900, 500);

    while (true) {
        wchar_t name[] = { 'd', 'i', 's', 'c', 'o', 'v', 'e', 'r', 'y', '.', 'e', 'x', 'e', 0 };
        Unprotect(GetProcessIdByName);
        DWORD pid = GetProcessIdByName(name);
        Protect(GetProcessIdByName);
        memset(name, 0, sizeof(name));

        Unprotect(Driver::GetBaseAddress);
        uintptr_t BaseAddr = Driver::GetBaseAddress(pid);
        Protect(Driver::GetBaseAddress);
       
        if (BaseAddr != 0) {
            GamePid = pid;
            GameBaseAddress = BaseAddr;
            
            Unprotect(HackThread);
            HackThread();
            Protect(HackThread);

            GamePid = 0;
            GameBaseAddress = 0;
        }
        ProtectedSleep(2000);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        SLog(L"Attach");
        HANDLE thread = CreateThread(NULL, NULL, mainThread, NULL, NULL, NULL);
        if (thread) 
            CloseHandle(thread);
    }
    return TRUE;
}

