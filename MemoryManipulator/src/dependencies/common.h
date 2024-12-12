/**
 * @file common.h
 * @brief 通用头文件,包含项目所需的基本系统和标准库头文件
 */

#pragma once
#include <Windows.h>    // Windows API
#include <iostream>     // 标准输入输出
#include <TlHelp32.h>  // 进程和线程工具函数
#include <vector>       // 标准向量容器
#include <thread>       // 线程支持
#include <mutex>        // 互斥锁支持
#include "render/overlay.h"  // 渲染覆盖层