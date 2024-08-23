#pragma once
#include "../dependencies/SDK.hpp"
#include "../memory/memorymanager.h"

class Deceit2
{
public:
	std::shared_ptr<MemoryManager> memory;
	std::shared_ptr<Overlay> overlay;

	void UpdateBaseData();
	void UpdateEntityData();
	void UpdatePositionalData();
	void Render();

private:
	BaseData baseData;
	EntityData entityData;
	PositionData positionData;

	std::mutex Mutex;
};