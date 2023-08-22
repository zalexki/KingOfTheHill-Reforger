class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	void DoRpcBuy(int configItemIndex)
	{
//		KOTH_RpcShopPurchase purchase = new KOTH_RpcShopPurchase;
//		purchase.price = price;
//		purchase.itemResourceName = itemResourceName;
//		purchase.magazineResourceName = magazineResourceName;
//		purchase.secondaryMagazineResourceName = secondMagazineResourceName;
//		
//		purchase.playerId = GetGame().GetPlayerController().GetPlayerId();
//		purchase.playerName = playerName;
		
		Rpc(RpcAsk_BuyStuff, configItemIndex, GetGame().GetPlayerController().GetPlayerId());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_BuyStuff(int configItemIndex, int playerId)
	{
		Log("----------- BuyStuff from rpc call");		
	
		KOTH_SCR_ShopGunItemList itemList = SCR_ConfigHelperT<KOTH_SCR_ShopGunItemList>.GetConfigObject("{232D181B9F9FE8D1}Configs/ShopGunItemList.conf");
		KOTH_SCR_ShopGunItem item = itemList.GetItems().Get(configItemIndex);

		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		PlayerManager playerManager = GetGame().GetPlayerManager();
		KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(gameMode.FindComponent(KOTH_ScoringGameModeComponent));

		scoreComp.BuyStuff(item.m_priceOnce, playerManager.GetPlayerName(playerId));
		IEntity controlledEntity = playerManager.GetPlayerControlledEntity(playerId);
		
		FindAndClearAnyWeaponsInInventory(controlledEntity);
		FindAndClearMagazineInInventory(controlledEntity);
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(controlledEntity.FindComponent(SCR_InventoryStorageManagerComponent));

		IEntity itemBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_itemResource));
        inventory.EquipWeapon(itemBought);
		
//		IEntity magazineBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_magazineResource));
//		for(int i = 0; i++; i == item.m_magazineNumber)
//		{
//        	inventory.TryInsertItem(magazineBought);
//		}
		
//		IEntity secondMagazineResourceName = GetGame().SpawnEntityPrefab(Resource.Load(secondMagazineResourceName));
//        inventory.TryInsertItem(secondMagazineResourceName);
//        inventory.TryInsertItem(secondMagazineResourceName);
	}
	
	void FindAndClearAnyWeaponsInInventory(IEntity player)
	{
		SCR_CharacterInventoryStorageComponent inventoryStorage = SCR_CharacterInventoryStorageComponent.Cast(player.FindComponent(SCR_CharacterInventoryStorageComponent));
		if (!inventoryStorage) 
		{
			Log("SCR_CharacterInventoryStorageComponent not found in player entity " + player);
			return;	
		}
		
		BaseInventoryStorageComponent inventoryComponent = inventoryStorage.GetWeaponStorage();
        
		// TODO: if people has random stuff deleted, this is must be this
		InventoryStorageSlot slot1 = inventoryComponent.GetSlot(0);
        InventoryStorageSlot slot2 = inventoryComponent.GetSlot(1);
        InventoryStorageSlot slot3 = inventoryComponent.GetSlot(2);
		
		SCR_EntityHelper.DeleteEntityAndChildren(slot1.GetAttachedEntity());
		SCR_EntityHelper.DeleteEntityAndChildren(slot2.GetAttachedEntity());
		SCR_EntityHelper.DeleteEntityAndChildren(slot3.GetAttachedEntity());
	}
	
	void FindAndClearMagazineInInventory(IEntity player)
	{
		InventoryStorageManagerComponent inventoryStorage = InventoryStorageManagerComponent.Cast(player.FindComponent(InventoryStorageManagerComponent));
		if (!inventoryStorage)
			return;
		
		array<typename> components = {};
		array<IEntity> entities = {};
		components.Insert(BaseMagazineComponent);
		inventoryStorage.FindItemsWithComponents(entities, components, EStoragePurpose.PURPOSE_ANY);
		
		foreach (IEntity entity : entities)
		{
			if (!inventoryStorage.TryDeleteItem(entity))
			{
				RplComponent.DeleteRplEntity(entity, false)
			}
		}
	}
}