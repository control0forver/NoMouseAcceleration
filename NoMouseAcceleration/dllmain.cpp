// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <detours.h>

typedef BOOL(WINAPI* SystemParametersInfoW_t)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);

SystemParametersInfoW_t TrueSystemParametersInfoW = SystemParametersInfoW;

BOOL WINAPI HookedSystemParametersInfoW(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
    if (uiAction == SPI_SETMOUSE && uiParam == 0 && pvParam == (PVOID)0x00000003 && fWinIni == (SPIF_UPDATEINIFILE | SPIF_SENDCHANGE))
        return TRUE;

    return TrueSystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)TrueSystemParametersInfoW, HookedSystemParametersInfoW);
        DetourTransactionCommit();
        break;
    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)TrueSystemParametersInfoW, HookedSystemParametersInfoW);
        DetourTransactionCommit();
        break;
    }
    return TRUE;
}
