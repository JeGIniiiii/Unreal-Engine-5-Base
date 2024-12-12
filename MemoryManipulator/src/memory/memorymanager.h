/**
 * @file memorymanager.h
 * @brief 内存管理器类的声明,提供对目标进程内存的读写操作
 */

#pragma once
#include "../dependencies/common.h"

/**
 * @class MemoryManager
 * @brief 提供对目标进程内存的管理和操作功能
 */
class MemoryManager
{
public:
	/**
	 * @brief 构造函数
	 * @param targetName 目标进程名称
	 * @param targetModule 目标模块名称
	 */
	MemoryManager(const wchar_t* targetName, const wchar_t* targetModule) : targetName(targetName), targetModule(targetModule) {}

	/**
	 * @brief 获取目标进程的PID
	 */
	void GetPID();

	/**
	 * @brief 获取目标进程的基址
	 * @return 返回进程基址
	 */
	std::uint64_t GetBaseAddress();

	/**
	 * @brief 获取目标进程的句柄
	 */
	void GetHandle();

	/**
	 * @brief 从指定地址读取数据
	 * @tparam T 要读取的数据类型
	 * @param address 要读取的内存地址
	 * @return 返回读取到的数据
	 */
	template<typename T>
	T Read(std::uint64_t address)
	{
		T buffer{};
		if (hProc != INVALID_HANDLE_VALUE)
		{
			if (ReadProcessMemory(hProc, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), NULL))
			{
				return buffer;
			}
		}
	}

	/**
	 * @brief 向指定地址写入数据
	 * @tparam T 要写入的数据类型
	 * @param address 要写入的内存地址
	 * @param value 要写入的值
	 * @return 写入是否成功
	 */
	template<typename T>
	bool Write(std::uint64_t address, const T& value)
	{
		if (hProc != INVALID_HANDLE_VALUE)
		{
			if (WriteProcessMemory(hProc, reinterpret_cast<LPVOID>(address), &value, sizeof(T), NULL))
			{
				return true;
			}
		}

		return false;
	}

private:
	DWORD targetPID = 0;           // 目标进程ID
	const wchar_t* targetName;      // 目标进程名称
	const wchar_t* targetModule;    // 目标模块名称
	HANDLE hProc = 0;              // 进程句柄
};