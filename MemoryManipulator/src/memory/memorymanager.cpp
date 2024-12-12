#include "memorymanager.h"

void MemoryManager::GetPID()
{
    // 创建进程快照结构体
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(PROCESSENTRY32);
    
    // 创建系统进程快照
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to create snapshot\n";
        return;
    }
    
    // 遍历进程列表查找目标进程
    if (Process32First(snapshot, &pEntry))
    {
        do
        {
            // 比较进程名,不区分大小写
            if (_wcsicmp(pEntry.szExeFile, targetName) == 0)
            {
                targetPID = pEntry.th32ProcessID;
                CloseHandle(snapshot);
                return;
            }
        } while (Process32Next(snapshot, &pEntry));
    }
    
    std::cerr << "Process not found\n";
    CloseHandle(snapshot);
}

std::uint64_t MemoryManager::GetBaseAddress()
{
    // 检查是否已获取PID
    if (targetPID == 0)
    {
        std::cerr << "PID not set. Call GetPID() first.\n";
        return 0;
    }
    
    // 创建模块快照结构体
    MODULEENTRY32 mEntry;
    mEntry.dwSize = sizeof(MODULEENTRY32);
    
    // 创建目标进程的模块快照
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, targetPID);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to create module snapshot\n";
        return 0;
    }
    
    // 遍历模块列表查找目标模块
    if (Module32First(snapshot, &mEntry))
    {
        do
        {
            // 比较模块名,不区分大小写
            if (_wcsicmp(mEntry.szModule, targetModule) == 0)
            {
                CloseHandle(snapshot);
                std::cout << "Found module base address: " << std::hex << (std::uint64_t)mEntry.modBaseAddr << std::dec << std::endl;
                return (std::uint64_t)mEntry.modBaseAddr;
            }
        } while (Module32Next(snapshot, &mEntry));
    }
    
    std::cerr << "Module not found\n";
    CloseHandle(snapshot);
    return 0;
}

void MemoryManager::GetHandle()
{
    // 检查是否已获取PID
    if (targetPID == 0)
    {
        std::cerr << "PID not set. Call GetPID() first.\n";
        return;
    }
    
    // 打开目标进程,获取完全访问权限的句柄
    hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPID);
    if (hProc == NULL)
    {
        std::cerr << "Failed to open process. Error code: " << GetLastError() << std::endl;
    }
    else
    {
        std::cout << "Successfully opened process handle\n";
    }
}