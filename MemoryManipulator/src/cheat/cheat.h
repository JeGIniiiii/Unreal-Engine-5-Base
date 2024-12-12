/**
 * @file cheat.h
 * @brief 作弊功能类的声明,提供游戏数据更新和渲染功能
 */

#pragma once
#include "../dependencies/SDK.hpp"
#include "../memory/memorymanager.h"

/**
 * @class Deceit2
 * @brief 提供游戏作弊功能的实现
 */
class Deceit2
{
public:
	std::shared_ptr<MemoryManager> memory;  // 内存管理器
	std::shared_ptr<Overlay> overlay;       // 渲染覆盖层

	void UpdateBaseData();        // 更新基础游戏数据
	void UpdateEntityData();      // 更新实体数据
	void UpdatePositionalData();  // 更新位置数据
	void Render();               // 渲染ESP

private:
	BaseData baseData;           // 基础游戏数据
	EntityData entityData;       // 实体数据
	PositionData positionData;   // 位置数据

	std::mutex Mutex;            // 线程同步互斥锁
};