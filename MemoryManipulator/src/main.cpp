/**
 * @file main.cpp
 * @brief 主程序入口文件,负责初始化和启动各个功能模块
 */

#include <iostream>
#include "memory/memorymanager.h"
#include "cheat/cheat.h"

int main()
{
    // 目标进程名称
    constexpr const wchar_t* targetName = L"Notepad.exe";
    
    // 初始化内存管理器
    const auto memory = std::make_shared<MemoryManager>(targetName, targetName);
    // 初始化渲染覆盖层
    const auto overlay = std::make_shared<Overlay>(L"Consolas", 18.0);
    // 初始化作弊功能模块
    const auto deceit2 = std::make_shared<Deceit2>();

    // 获取目标进程PID和句柄
    memory->GetPID();
    memory->GetHandle();

    // 获取目标进程基址
    const std::uint64_t base = memory->GetBaseAddress();

    // 设置作弊模块的依赖
    deceit2->memory = memory;
    deceit2->overlay = overlay;

    // 初始化渲染覆盖层
    overlay->init();
    overlay->startup_d2d();

    // 启动各个功能线程
    std::thread(&Deceit2::UpdateBaseData, deceit2).detach();      // 更新基础数据线程
    std::thread(&Deceit2::UpdateEntityData, deceit2).detach();    // 更新实体数据线程
    std::thread(&Deceit2::UpdatePositionalData, deceit2).detach();// 更新位置数据线程
    std::thread(&Deceit2::Render, deceit2).detach();             // 渲染线程
}