#include "cheat.h"

void Deceit2::UpdateBaseData()
{
	while (true)
	{
		// 每秒更新一次基础数据
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		// 读取UWorld和相关实例
		std::uint64_t UWorld = memory->Read<std::uint64_t>(memory->GetBaseAddress() + UE5::GWORLD);
		std::uint64_t GameInstance = memory->Read<std::uint64_t>(UWorld + UE5::GAMEINSTANCE);
		std::uint64_t GameState = memory->Read<std::uint64_t>(UWorld + UE5::GAMESTATE);

		// 读取本地玩家相关数据
		std::uint64_t LocalPlayer = memory->Read<std::uint64_t>(memory->Read<std::uint64_t>(GameInstance + UE5::LOCALPLAYER));
		std::uint64_t PlayerController = memory->Read<std::uint64_t>(LocalPlayer + UE5::PLAYERCONTROLLER);
		std::uint64_t PlayerCameraManager = memory->Read<std::uint64_t>(PlayerController + UE5::PLAYERCAMERAMANAGER);

		// 读取玩家数组信息
		std::uint64_t PlayerArray = memory->Read<std::uint64_t>(GameState + UE5::PLAYERARRAY);
		std::uint32_t PlayerCount = memory->Read<std::uint32_t>(GameState + UE5::PLAYERCOUNT);

		// 更新基础数据,使用互斥锁保护
		Mutex.lock();
		baseData.PlayerCameraManager = PlayerCameraManager;
		baseData.PlayerArray = PlayerArray;
		baseData.PlayerCount = PlayerCount;
		Mutex.unlock();
	}
}

void Deceit2::UpdateEntityData()
{
	while (true)
	{
		// 每秒更新一次实体数据
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		std::vector<std::uint64_t> RootComponents;

		// 获取玩家数组和数量
		const auto& PlayerArray = baseData.PlayerArray;
		const auto& PlayerCount = baseData.PlayerCount;

		// 遍历所有玩家
		for (int i = 0; i < PlayerCount; i++)
		{
			// 读取玩家状态和Pawn
			std::uint64_t PlayerState = memory->Read<std::uint64_t>(PlayerArray + i * 0x8);
			std::uint64_t Pawn = memory->Read<std::uint64_t>(PlayerState + UE5::APAWN); 
			std::uint64_t RootComponent = memory->Read<std::uint64_t>(Pawn + UE5::ROOTCOMPONENT);

			// 保存根组件地址
			RootComponents.push_back(RootComponent);
		}

		// 更新实体数据,使用互斥锁保护
		Mutex.lock();
		entityData.RootComponents = RootComponents;
		Mutex.unlock();
	}
}

void Deceit2::UpdatePositionalData()
{
	while (true)
	{
		// 每6毫秒更新一次位置数据
		std::this_thread::sleep_for(std::chrono::milliseconds(6));

		std::vector<Vector3> PlayerPositions;

		// 获取根组件列表和相机管理器
		const auto& RootComponents = entityData.RootComponents;
		const auto& PlayerCameraManager = baseData.PlayerCameraManager;

		// 遍历所有玩家的根组件
		for (std::uint64_t RootComponent : RootComponents)
		{
			// 读取玩家位置
			PlayerPositions.push_back(memory->Read<Vector3>(RootComponent + UE5::RELATIVELOCATION));
		}

		// 读取相机视图信息
		positionData.ViewInfo = memory->Read<FMinimalViewInfo>(PlayerCameraManager + UE5::CAMERACACHEPRIVATE + 0x10);
		positionData.PlayerPositions = PlayerPositions;
	}
}

void Deceit2::Render()
{
	while (true)
	{
		// 尽可能快速地进行渲染
		std::this_thread::sleep_for(std::chrono::milliseconds(0));

		// 获取视图信息和玩家位置
		const auto& ViewInfo = positionData.ViewInfo;
		const auto& PlayerPositions = positionData.PlayerPositions;

		// 开始渲染
		overlay->begin_scene();
		overlay->clear_scene();

		// 遍历所有玩家位置
		for (Vector3 PlayerPosition : PlayerPositions)
		{
			// 将3D世界坐标转换为2D屏幕坐标
			Vector3 Position = WorldToScreen(ViewInfo, PlayerPosition);

			// 渲染ESP线条
			overlay->draw_line(960, 1080, Position.x, Position.y, D2D1::ColorF::White);
		}

		// 结束渲染
		overlay->end_scene();
	}
}