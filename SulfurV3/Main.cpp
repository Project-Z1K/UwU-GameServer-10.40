#include "framework.h"
bool rettrue() { return true; }
bool crashaf(__int64 A1) {
    std::cout << std::format("crashtqu2wg2gwgwwrgb: 0x{:x}\n", __int64(_ReturnAddress()) - __int64(GetModuleHandleW(0)));

    return false;
}
bool crashaf2() {
    std::cout << std::format("afwqq23f: 0x{:x}\n", __int64(_ReturnAddress()) - __int64(GetModuleHandleW(0)));
    return false;
}
bool retfalse() { return false; }
void printretaddress() { std::cout << std::format("0x{:x}\n", __int64(_ReturnAddress()) - __int64(GetModuleHandleW(0))); }
void printretaddressaa() { std::cout << std::format("BUGHA 0x{:x}\n", __int64(_ReturnAddress()) - __int64(GetModuleHandleW(0))); }

static void Main()
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "BaseAddr: " << std::hex << (uintptr_t)GetModuleHandle(0) << '\n';

    auto PlayerController = GEngine->GameInstance->LocalPlayers[0]->PlayerController;

    PlayerController->SwitchLevel(L"Athena_Terrain?game=athena");

    GEngine->GameInstance->LocalPlayers.Remove(0);

    srand(time(0));

    Hooks::Init();
    Game::Init();
}

int __stdcall DllMain(void*, int dwReason)
{
    if (dwReason == 1)
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&Main, 0, 0, 0);

    return 1;
}

