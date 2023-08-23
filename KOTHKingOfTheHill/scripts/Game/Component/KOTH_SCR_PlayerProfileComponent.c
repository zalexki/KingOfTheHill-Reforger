class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	void DoRpcBuy(int configItemIndex)
	{
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
		EquipedWeaponStorageComponent equipedWeaponStorage = EquipedWeaponStorageComponent.Cast(controlledEntity.FindComponent(EquipedWeaponStorageComponent));
		
		
		// same logic as magazine but execute first
		//		IEntity secondMagazineResourceName = GetGame().SpawnEntityPrefab(Resource.Load(secondMagazineResourceName));
		//      inventory.TryInsertItem(secondMagazineResourceName);
		//      inventory.TryInsertItem(secondMagazineResourceName);
		
		FindAndClearMagazineInInventory(controlledEntity);
		
		bool minimumMagInsertOk = true;
		array<IEntity> magazineList = new array<IEntity>; 
		for (int i = 1; i <= item.m_magazineNumber; i++)
		{
			IEntity magazineBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_magazineResource));
        	bool tryInsert = inventory.TryInsertItem(magazineBought);
			if (i == 2 && tryInsert == false) {
				minimumMagInsertOk = false;
				break;
			}
			
			magazineList.Insert(magazineBought);
			Log("mag "+ magazineBought);
		}
		
		if (false == minimumMagInsertOk)
		{
			foreach (IEntity magazine : magazineList)
			{
				inventory.TryDeleteItem(magazine);
			}
			
			DoRpc_Notif_Failed_NoSpace();
			
			return;
		}

		scoreComp.BuyStuff(item.m_priceOnce, playerManager.GetPlayerName(playerId));
		IEntity itemBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_itemResource));
        inventory.EquipWeapon(itemBought);
		DoRpc_Notif_Succeed();
		
		//GetGame().GetCallqueue().CallLater(retardstuff, 100, false, controlledEntity);
		
	}

	void retardstuff(IEntity controlledEntity)
	{
		
		FindAndClearAnyWeaponsInInventory(controlledEntity);
		// TODO: delete only category 

		
//		IEntity itemBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_itemResource));
//        inventory.EquipWeapon(itemBought);
//		DoRpc_Notif_Succeed();
		
		
		// TODO: try to equip weapon
		//Print(equipedWeaponStorage);
		//inventory.EquipItem(equipedWeaponStorage, itemBought);

	}
	
	void DoRpc_Notif_Succeed()
	{
		Rpc(RpcDo_NotifBuy_Succeed);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_NotifBuy_Succeed()
	{
		ChimeraMenuBase menu = ChimeraMenuBase.CurrentChimeraMenu();
		if (!menu)
			return;
		
		KOTH_ShopGunClass shopLayout = KOTH_ShopGunClass.Cast(menu);
		if (!shopLayout)
			return;
		
		shopLayout.TestRpcBuySucceed();
	}
	
	void DoRpc_Notif_Failed_NoSpace()
	{
		Rpc(RpcDo_NotifBuy_Failed_NoSpace);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_NotifBuy_Failed_NoSpace()
	{
		ChimeraMenuBase menu = ChimeraMenuBase.CurrentChimeraMenu();
		if (!menu)
			return;
		
		KOTH_ShopGunClass shopLayout = KOTH_ShopGunClass.Cast(menu);
		if (!shopLayout)
			return;
		
		shopLayout.TestRpcBuyFailed();
	}
	
	
	
	
	
	
	
	
	
	void FindAndClearAnyWeaponsInInventory(IEntity player)
	{
		SCR_CharacterInventoryStorageComponent characterInventoryStorageComp = SCR_CharacterInventoryStorageComponent.Cast(player.FindComponent(SCR_CharacterInventoryStorageComponent));
		
		if (!characterInventoryStorageComp) 
		{
			Log("SCR_CharacterInventoryStorageComponent not found in player entity " + player);
			return;	
		}
		
		InventoryStorageManagerComponent inventoryStorageManagerComp = InventoryStorageManagerComponent.Cast(player.FindComponent(InventoryStorageManagerComponent));
        if (!inventoryStorageManagerComp) 
		{
			Log("BaseInventoryStorageComponent not found in player entity " + player);
			return;	
		}
		
		BaseInventoryStorageComponent inventoryStorageComponent = characterInventoryStorageComp.GetWeaponStorage();
        if (!inventoryStorageComponent) 
		{
			Log("BaseInventoryStorageComponent not found in player entity " + player);
			return;	
		}
		
		// TODO: if people has random stuff deleted, this is must be this
		
		// remove primary and secondary item
		InventoryStorageSlot slot1 = inventoryStorageComponent.GetSlot(0);
        InventoryStorageSlot slot2 = inventoryStorageComponent.GetSlot(1);
		
		IEntity ent1 = slot1.GetAttachedEntity();
		IEntity ent2 = slot2.GetAttachedEntity();
		
		if (ent1)
		{
			inventoryStorageManagerComp.TryRemoveItemFromStorage(ent1, inventoryStorageComponent);
			//SCR_EntityHelper.DeleteEntityAndChildren(ent1);
			RplComponent.DeleteRplEntity(ent1, false);
		}
		
		if (ent2) 
		{
			inventoryStorageManagerComp.TryRemoveItemFromStorage(ent2, inventoryStorageComponent);
			//SCR_EntityHelper.DeleteEntityAndChildren(ent2);
			RplComponent.DeleteRplEntity(ent2, false);
		}
	}
	
	void FindAndClearMagazineInInventory(IEntity player)
	{
		InventoryStorageManagerComponent inventoryStorage = InventoryStorageManagerComponent.Cast(player.FindComponent(InventoryStorageManagerComponent));
		if (!inventoryStorage)
			return;
		
		SCR_CharacterInventoryStorageComponent characterInventoryStorageComp = SCR_CharacterInventoryStorageComponent.Cast(player.FindComponent(SCR_CharacterInventoryStorageComponent));
		if (!characterInventoryStorageComp)
			return;

		BaseInventoryStorageComponent inventoryStorageComponent = characterInventoryStorageComp.GetWeaponStorage();
		if (!inventoryStorageComponent)
			return;
		
		InventoryStorageSlot slot1 = inventoryStorageComponent.GetSlot(0);
        InventoryStorageSlot slot2 = inventoryStorageComponent.GetSlot(1);
		
		IEntity ent1 = slot1.GetAttachedEntity();
		IEntity ent2 = slot2.GetAttachedEntity();
		
		if (ent1)
		{
			inventoryStorage.TryRemoveItemFromStorage(ent1, inventoryStorageComponent);
			//SCR_EntityHelper.DeleteEntityAndChildren(ent1);
			RplComponent.DeleteRplEntity(ent1, false);
		}
		
		if (ent2) 
		{
			inventoryStorage.TryRemoveItemFromStorage(ent2, inventoryStorageComponent);
			//SCR_EntityHelper.DeleteEntityAndChildren(ent2);
			RplComponent.DeleteRplEntity(ent2, false);
		}
		
		array<typename> components = {};
		array<IEntity> entities = {};
		components.Insert(BaseMagazineComponent);
		inventoryStorage.FindItemsWithComponents(entities, components, EStoragePurpose.PURPOSE_ANY);
		
		foreach (IEntity entity : entities)
		{
			//inventoryStorage.TryRemoveItemFromStorage(entity, inventoryStorageComponent);
			//SCR_EntityHelper.DeleteEntityAndChildren(entity);
			BaseWeaponComponent wpComp = BaseWeaponComponent.Cast(entity.FindComponent(BaseWeaponComponent));
			if (wpComp)
				return;
			
			// not sure if needed or not
			if (!inventoryStorage.TryRemoveItemFromStorage(entity, inventoryStorageComponent))
			{
				RplComponent.DeleteRplEntity(entity, false)
			}
		}
	}
}