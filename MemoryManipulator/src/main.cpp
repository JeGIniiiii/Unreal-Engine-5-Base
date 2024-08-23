#include <iostream>
#include "memory/memorymanager.h"
#include "cheat/cheat.h"

int main()
{
    constexpr const wchar_t* targetName = L"Notepad.exe";
    const auto memory = std::make_shared<MemoryManager>(targetName, targetName);
    const auto overlay = std::make_shared<Overlay>(L"Consolas", 18.0);
    const auto deceit2 = std::make_shared<Deceit2>();

    memory->GetPID();
    memory->GetHandle();

    const std::uint64_t base = memory->GetBaseAddress();

    deceit2->memory = memory;
    deceit2->overlay = overlay;

    overlay->init();
    overlay->startup_d2d();

    std::thread(&Deceit2::UpdateBaseData, deceit2).detach();
    std::thread(&Deceit2::UpdateEntityData, deceit2).detach();
    std::thread(&Deceit2::UpdatePositionalData, deceit2).detach();
    std::thread(&Deceit2::Render, deceit2).detach();
}