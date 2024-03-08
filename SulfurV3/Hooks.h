#pragma once
#include "framework.h"
#include "Globals.h"
namespace Hooks
{
	static bool ReadyToStartMatchHook(__int64 a1)
	{
		auto GameMode = Cast<AFortGameModeAthena>(GetWorld()->AuthorityGameMode);
		auto GameState = Cast<AFortGameStateAthena>(GetWorld()->GameState);

		static bool bPreLoaded = false;

		if (!bPreLoaded)
		{
			bPreLoaded = true;

			Game::PreLoad();
		}

		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFortPlayerStartWarmup::StaticClass(), &Actors);

		if (Actors.IsEmpty())
			return false;

		static bool bLoadedPlaylist = false;

		if (!bLoadedPlaylist)
		{
			bLoadedPlaylist = true;
			GameState->OnRep_CurrentPlaylistInfo();
		}

		if (!GameState->MapInfo)
			return false;

		Game::Start();
		Game::Listen();

		return Native::ReadyToStartMatch(a1);
	}

	static void TickFlushHook(UNetDriver* NetDriver)
	{
		if (NetDriver->ReplicationDriver)
			Native::ServerReplicateActors(NetDriver->ReplicationDriver);

		Native::TickFlush(NetDriver);
	}

	static void KickPlayerHook(void*, void*)
	{

	}

	static void HandleNewStartingPlayerHook(__int64 a1, APlayerController* PlayerController)
	{
		if (auto FortPlayerController = Cast<AFortPlayerControllerAthena>(PlayerController))
			Game::HandlePlayer(FortPlayerController);

		return Native::HandleNewStartingPlayer(a1, PlayerController);
	}

	static char CanActivateAbilityHook()
	{
		return 1;
	}

	static __int64 GetNetModeHook(__int64)
	{
		return ((uintptr_t)_ReturnAddress() - (uintptr_t)GetModuleHandle(0)) == 0x15d0385 ? 2 : 1;
	}

	static char IsDedicatedServerHook()
	{
		return true;
	}

	static void Init()
	{
		CREATE_HOOK(ReadyToStartMatchHook, Native::ReadyToStartMatch);
		CREATE_HOOK(TickFlushHook, Native::TickFlush);
		CREATE_HOOK(KickPlayerHook, Native::KickPlayer);
		CREATE_HOOK(HandleNewStartingPlayerHook, Native::HandleNewStartingPlayer);
		CREATE_HOOK(CanActivateAbilityHook, Native::CanActivateAbility);
		CREATE_HOOK(GetNetModeHook, Native::GetNetMode_World);
		CREATE_HOOK(GetNetModeHook, Native::GetNetMode_Actor);
		CREATE_HOOK(IsDedicatedServerHook, Native::IsDedicatedServer);
	}
}

inline APawn* SpawnDefaultPawnForHook(AGameModeBase* GameModeA, AController* NewPlayer, AActor* StartSpot)
{
	auto GameMode = Cast<AFortGameModeAthena>(GameModeA);

	if (!GameMode)
		return nullptr;

	// std::cout << std::format("SpawnDefaultPawnForHook: 0x{:x}\n", __int64(_ReturnAddress()) - __int64(GetModuleHandleW(0)));

	auto SpawnTransform = StartSpot->GetTransform();
	// SpawnLocation.Translation = FVector{ 1250, 1818, 3284 };

	auto Controller = Cast<AFortPlayerControllerAthena>(NewPlayer);

	bool bIsRespawning = false;

	auto PlayerState = Cast<AFortPlayerStateAthena>(Controller->PlayerState);

	if (Controller)
	{
		if (PlayerState)
		{
			auto& RespawnData = PlayerState->RespawnData;

			if (RespawnData.bServerIsReady && RespawnData.bRespawnDataAvailable) // && GameState->IsRespawningAllowed(PlayerState);
			{
				SpawnTransform.Translation = PlayerState->RespawnData.RespawnLocation;
				// SpawnTransform.Rotation = Quaternion(PlayerState->RespawnData.RespawnRotation);

				bIsRespawning = true;
			}
		}
	}

	auto newpawn = GameMode->SpawnDefaultPawnAtTransform(NewPlayer, SpawnTransform);
	std::cout << "newpawn: " << newpawn << '\n';
	std::cout << "bIsRespawning: " << bIsRespawning << '\n';

	bool bFoundPickaxe = false;

	auto& ItemInstances = Controller->WorldInventory->Inventory.ItemInstances;

	for (int i = 0; i < ItemInstances.Num() && !bFoundPickaxe; ++i)
	{
		bFoundPickaxe = ItemInstances[i]->ItemEntry.ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass());
	}

	bool bShouldGiveStartingItems = !bFoundPickaxe;

	if (bShouldGiveStartingItems)
	{
		Controller->CosmeticLoadoutPC.Pickaxe; // UObject::FindObject<UAthenaPickaxeItemDefinition>("/Game/Athena/Items/Cosmetics/Pickaxes/DefaultPickaxe.DefaultPickaxe");

		/*
		static auto WallPiece = UObject::FindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
		static auto FloorPiece = UObject::FindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
		static auto StairPiece = UObject::FindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");
		static auto RoofPiece = UObject::FindObject<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");
		GiveItem(NewPlayer, WallPiece, 1);
		GiveItem(NewPlayer, FloorPiece, 1);
		GiveItem(NewPlayer, StairPiece, 1);
		GiveItem(NewPlayer, RoofPiece, 1);
		*/

		for (int i = 0; i < GameMode->StartingItems.Num(); i++)
		{
			auto& StartingItem = GameMode->StartingItems[i];
			auto ItemDef = StartingItem.Item;

			if (!ItemDef)
				continue;

			// std::cout << std::format("[{}] {}\n", i, ItemDef->GetFullName());

		}

		/*
		auto MutatorListComponent = GameState->MutatorListComponent;
		std::cout << "MutatorListComponent: " << MutatorListComponent << '\n';
		auto Mutator = (AFortAthenaMutator_InventoryOverride*)MutatorListComponent->GetMutatorByClass(AFortAthenaMutator_InventoryOverride::StaticClass());
		std::cout << "Mutator: " << Mutator << '\n';
		if (Mutator)
		{
			std::cout << "size: " << Mutator->InventoryLoadouts.Num() << '\n';
			for (int i = 0; i < Mutator->InventoryLoadouts.Num(); i++)
			{
				auto& InventoryLoadout = Mutator->InventoryLoadouts[i];
				for (int z = 0; i < InventoryLoadout.Loadout.Num(); z++)
				{
					auto& StartingItem = InventoryLoadout.Loadout[z];
					auto ItemDef = StartingItem.Item;
					if (!ItemDef)
						continue;
					std::cout << std::format("[{}] {}\n", z, ItemDef->GetFullName());
					GiveItem(NewPlayer, StartingItem.Item, StartingItem.Count);
				}
			}
		}
		*/
	}

	if (newpawn)
	{
		std::cout << "bUseControllerRotationPitch: " << (int)newpawn->bUseControllerRotationPitch << '\n';
		std::cout << "bUseControllerRotationRoll: " << (int)newpawn->bUseControllerRotationRoll << '\n';
		std::cout << "bUseControllerRotationYaw: " << (int)newpawn->bUseControllerRotationYaw << '\n';

		auto PawnAsAthena = Cast<AFortPlayerPawnAthena>(newpawn);

		if (PawnAsAthena && Controller)
		{
			std::cout << "wtf!\n";

			PawnAsAthena->CosmeticLoadout = Controller->CosmeticLoadoutPC;
			PawnAsAthena->OnRep_CosmeticLoadout();

			// ApplyCustomizationToCharacter(PlayerState);

			// ApplyCID(PlayerState, PawnAsAthena->CosmeticLoadout.Character, PawnAsAthena);

			if (bIsRespawning)
			{
				PawnAsAthena->SetShield(100);

				auto& ItemInstances = Controller->WorldInventory->Inventory.ItemInstances;

				for (int i = 0; i < ItemInstances.Num(); i++)
				{
					auto ItemInstance = ItemInstances[i];

					if (auto WeaponDef = Cast<UFortWeaponItemDefinition>(ItemInstance->ItemEntry.ItemDefinition))
					{
						Controller->WorldInventory->Inventory.MarkItemDirty(ItemInstance->ItemEntry);
					}
				}
			}
		}
	}

	return newpawn;
}
