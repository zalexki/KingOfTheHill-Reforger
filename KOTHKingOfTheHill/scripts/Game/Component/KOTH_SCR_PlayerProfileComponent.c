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
		
		IEntity controlledEntity = playerManager.GetPlayerControlledEntity(playerId);
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(controlledEntity.FindComponent(SCR_InventoryStorageManagerComponent));
//		
//		bool firstMagInsertOk = false;
//		array<IEntity> magazineList = new array<IEntity>; 
//		for (int i = 1; i <= item.m_magazineNumber; i++)
//		{
//			IEntity magazineBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_magazineResource));
//        	bool tryInsert = inventory.TryInsertItem(magazineBought);
//			if (i == 3 && tryInsert == false) {
//				break;
//			}
//			firstMagInsertOk = true;
//			Log("mag "+ magazineBought);
//		}
//		
//		if (false == firstMagInsertOk)
//		{
//			// return money
//			// remove weapon
//		}
//		
		scoreComp.BuyStuff(item.m_priceOnce, playerManager.GetPlayerName(playerId));
	
		
		FindAndClearAnyWeaponsInInventory(controlledEntity);
		FindAndClearMagazineInInventory(controlledEntity);

		IEntity itemBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_itemResource));
        inventory.EquipWeapon(itemBought);
		
//		IEntity secondMagazineResourceName = GetGame().SpawnEntityPrefab(Resource.Load(secondMagazineResourceName));
//      inventory.TryInsertItem(secondMagazineResourceName);
//      inventory.TryInsertItem(secondMagazineResourceName);
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
        if (!inventoryComponent) 
		{
			Log("BaseInventoryStorageComponent not found in player entity " + player);
			return;	
		}
		
		// TODO: if people has random stuff deleted, this is must be this
		InventoryStorageSlot slot1 = inventoryComponent.GetSlot(0);
        InventoryStorageSlot slot2 = inventoryComponent.GetSlot(1);
        InventoryStorageSlot slot3 = inventoryComponent.GetSlot(2);
		
		IEntity ent1 = slot1.GetAttachedEntity();
//		IEntity ent2 = slot2.GetAttachedEntity();
//		IEntity ent3 = slot3.GetAttachedEntity();
		
		if (ent1)
			RplComponent.DeleteRplEntity(ent1, false)
			SCR_EntityHelper.DeleteEntityAndChildren(ent1);
		
//		if (ent2)
//			SCR_EntityHelper.DeleteEntityAndChildren(ent2);
//		
//		if (ent3)
//			SCR_EntityHelper.DeleteEntityAndChildren(ent3);
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
			inventoryStorage.TryDeleteItem(entity);
			
//			if (!inventoryStorage.TryDeleteItem(entity))
//			{
//				RplComponent.DeleteRplEntity(entity, false)
//			}
		}
	}
}