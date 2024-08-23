#include "cheat.h"

void Deceit2::UpdateBaseData()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		std::uint64_t UWorld = memory->Read<std::uint64_t>(memory->GetBaseAddress() + UE5::GWORLD);
		std::uint64_t GameInstance = memory->Read<std::uint64_t>(UWorld + UE5::GAMEINSTANCE);
		std::uint64_t GameState = memory->Read<std::uint64_t>(UWorld + UE5::GAMESTATE);

		std::uint64_t LocalPlayer = memory->Read<std::uint64_t>(memory->Read<std::uint64_t>(GameInstance + UE5::LOCALPLAYER));
		std::uint64_t PlayerController = memory->Read<std::uint64_t>(LocalPlayer + UE5::PLAYERCONTROLLER);
		std::uint64_t PlayerCameraManager = memory->Read<std::uint64_t>(PlayerController + UE5::PLAYERCAMERAMANAGER);

		std::uint64_t PlayerArray = memory->Read<std::uint64_t>(GameState + UE5::PLAYERARRAY);
		std::uint32_t PlayerCount = memory->Read<std::uint32_t>(GameState + UE5::PLAYERCOUNT);

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
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		std::vector<std::uint64_t> RootComponents;

		const auto& PlayerArray = baseData.PlayerArray;
		const auto& PlayerCount = baseData.PlayerCount;

		for (int i = 0; i < PlayerCount; i++)
		{
			std::uint64_t PlayerState = memory->Read<std::uint64_t>(PlayerArray + i * 0x8);
			std::uint64_t Pawn = memory->Read<std::uint64_t>(PlayerState + UE5::APAWN); 
			std::uint64_t RootComponent = memory->Read<std::uint64_t>(Pawn + UE5::ROOTCOMPONENT);

			RootComponents.push_back(RootComponent);
		}

		Mutex.lock();
		entityData.RootComponents = RootComponents;
		Mutex.unlock();
	}
}

void Deceit2::UpdatePositionalData()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(6));

		std::vector<Vector3> PlayerPositions;

		const auto& RootComponents = entityData.RootComponents;
		const auto& PlayerCameraManager = baseData.PlayerCameraManager;

		for (std::uint64_t RootComponent : RootComponents)
		{
			PlayerPositions.push_back(memory->Read<Vector3>(RootComponent + UE5::RELATIVELOCATION));
		}

		positionData.ViewInfo = memory->Read<FMinimalViewInfo>(PlayerCameraManager + UE5::CAMERACACHEPRIVATE + 0x10);
		positionData.PlayerPositions = PlayerPositions;

	}
}

void Deceit2::Render()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(0));

		const auto& ViewInfo = positionData.ViewInfo;
		const auto& PlayerPositions = positionData.PlayerPositions;

		overlay->begin_scene();
		overlay->clear_scene();

		for (Vector3 PlayerPosition : PlayerPositions)
		{
			Vector3 Position = WorldToScreen(ViewInfo, PlayerPosition);

			// render
			overlay->draw_line(960, 1080, Position.x, Position.y, D2D1::ColorF::White);
		}

		overlay->end_scene();
		
	}
}