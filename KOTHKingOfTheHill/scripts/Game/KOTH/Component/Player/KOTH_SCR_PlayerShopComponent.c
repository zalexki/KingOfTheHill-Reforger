class KOTH_SCR_PlayerShopComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerShopComponent : ScriptComponent 
{
	protected string m_playerUID;
	protected KOTH_ScoringGameModeComponent m_scoreComp;
	protected KOTH_PlayerProfileManagerGameModeComponent m_playerProfileManager;
	protected ref array<string> m_shopItemListResources = {
		"{232D181B9F9FE8D1}Configs/Shop/ShopWeaponItemList.conf",
		"{01BEF9B6FC671AF0}Configs/Shop/ShopThrowableItemlList.conf",
		"{C9E28F21A8EC7DB7}Configs/Shop/ShopAccessoryItemlList.conf",
		"{9F317D1901DBE909}Configs/Shop/ShopVehicleItemList.conf"
	};

	override void OnPostInit(IEntity owner)
	{
		if (SCR_Global.IsEditMode(owner))
			return;
		
		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
		m_playerProfileManager = KOTH_PlayerProfileManagerGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_PlayerProfileManagerGameModeComponent));
		PlayerController controller = GetGame().GetPlayerController();
		
		if (controller)
			m_playerUID = GetGame().GetBackendApi().GetPlayerUID(controller.GetPlayerId());
	}
	
	bool SpawnVehicle(string resourceName, int playerId)
	{
		KOTH_SpawnPrefab firstSpawn = KOTH_SpawnPrefab.Cast(GetGame().GetWorld().FindEntityByName("KOTH_FirstVehicleSpawn"));
		KOTH_SpawnPrefab secondSpawn = KOTH_SpawnPrefab.Cast(GetGame().GetWorld().FindEntityByName("KOTH_SecondVehicleSpawn"));
		KOTH_SpawnPrefab thirdSpawn = KOTH_SpawnPrefab.Cast(GetGame().GetWorld().FindEntityByName("KOTH_ThirdVehicleSpawn"));
		if (!firstSpawn || !secondSpawn || !thirdSpawn)
			return false;

		Faction playerFaction = SCR_FactionManager.Cast(GetGame().GetFactionManager()).GetPlayerFaction(playerId);
		if (firstSpawn.GetFactionKey() == playerFaction.GetFactionKey())
			return firstSpawn.Spawn(resourceName);
		
		if (secondSpawn.GetFactionKey() == playerFaction.GetFactionKey())
			return secondSpawn.Spawn(resourceName); 
		
		if (thirdSpawn.GetFactionKey() == playerFaction.GetFactionKey())
			return thirdSpawn.Spawn(resourceName);
		
//		EventHandlerManagerComponent handler = EventHandlerManagerComponent.Cast(m_Vehicle.FindComponent(EventHandlerManagerComponent));
//		
//		if (handler)
//			handler.RegisterScriptHandler("OnDestroyed", this, OnVehicleDestroyed);
		
		return false;
	}
	
	bool CanBuyVehicleArmed(Faction playerFaction, KOTH_ShopItem item, int playerId)
	{
		switch (playerFaction.GetFactionKey())
		{
			case KOTH_Faction.BLUFOR:
				if (m_scoreComp.m_bluforArmedVehiclesCount >= 3)
				{
					m_scoreComp.Refund(item.m_priceOnce, playerId);
					DoRpc_Notif_Failed("cannot spawn vehicle", "too much armed vehicle in your team "+"\n"+" (max 3)");
					return false;
				}
			break;
			case KOTH_Faction.OPFOR:
				if (m_scoreComp.m_opforArmedVehiclesCount >= 3)
				{
					m_scoreComp.Refund(item.m_priceOnce, playerId);
					DoRpc_Notif_Failed("cannot spawn vehicle", "too much armed vehicle in your team "+"\n"+"(max 3)");
					return false;
				}
			break;
			case KOTH_Faction.INDFOR:
				if (m_scoreComp.m_indforArmedVehiclesCount >= 3)
				{
					m_scoreComp.Refund(item.m_priceOnce, playerId);
					DoRpc_Notif_Failed("cannot spawn vehicle", "too much armed vehicle in your team "+"\n"+" (max 3)");
					return false;
				}
			break;
			default:
				DoRpc_Notif_Failed("cannot spawn vehicle", "error: your faction was not found");
				return false;
			break;
		}
		
		return true;
	}
	
	void AddCountVehicleArmed(Faction playerFaction)
	{
		switch (playerFaction.GetFactionKey())
		{
			case KOTH_Faction.BLUFOR:
				m_scoreComp.m_bluforArmedVehiclesCount++;
			break;
			case KOTH_Faction.OPFOR:
				m_scoreComp.m_opforArmedVehiclesCount++;
			break;
			case KOTH_Faction.INDFOR:
				m_scoreComp.m_indforArmedVehiclesCount++;
			break;
			default:
				DoRpc_Notif_Failed("cannot spawn vehicle", "error: your faction was not found");
				return;
			break;
		}
	}
	
	// --------------------- RPC START
	
	
	void AskRpc_Equip(string resourceName, int playerId)
	{
		Rpc(RpcAsk_Equip, resourceName, playerId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_Equip(string resourceName, int playerId, bool permanentBuy)
	{
		KOTH_ShopItem item = FindShopItemByResourceName(resourceName);

		if (!item)
		{
			Log("no item found for resourceName "+resourceName);
			return;
		}
		
		bool isSuccess = RemoveOldItemsAndAddNewOnes(item, playerId);
		if (!isSuccess) {
			DoRpc_Notif_Failed("You can't buy the weapon", "your inventory are full");
			return;
		}
	}
	
	void DoRpcBuy(string resourceName, int playerId, bool permanentBuy = false)
	{
		Rpc(RpcAsk_Buy, resourceName, playerId, permanentBuy);			
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_Buy(string resourceName, int playerId, bool permanentBuy)
	{
		Log("----------- RpcAsk_Buy "+resourceName+" for "+playerId+" is permanentBuy "+permanentBuy);
		KOTH_ShopItem item = FindShopItemByResourceName(resourceName);

		if (!item)
		{
			Log("no item found for resourceName "+resourceName);
			return;
		}
		int price = item.m_priceOnce;
		if (permanentBuy)
			price = item.m_pricePermanent;
		
		bool buySuccess = m_scoreComp.TryBuy(price, playerId);
		if (!buySuccess) {
			DoRpc_Notif_Failed("You can't buy the weapon", "not enough money");
			return;
		}
		
		if (permanentBuy)
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.m_unlockedItems.Insert(item.m_itemResource);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
					
					break;
				}
			}
			GetGame().GetCallqueue().CallLater(DoRpc_NotifBuy_PermanentBuySuccess, 250, false, item.m_itemResource);
		} 
		else 
		{
			// check category for vehicles
			// TODO: refactor this
			switch (item.m_category) {
				case KOTH_ShopItemCategory.Vehicle:
					if (SpawnVehicle(resourceName, playerId))
					{
						DoRpc_Notif_Succeed(item.m_priceOnce);
					} else {
						m_scoreComp.Refund(item.m_priceOnce, playerId);
						DoRpc_Notif_Failed("cannot spawn vehicle", "no place found");
					}
				break;
				case KOTH_ShopItemCategory.VehicleArmed:
					Faction playerFaction = SCR_FactionManager.Cast(GetGame().GetFactionManager()).GetPlayerFaction(playerId);
					
					if (!CanBuyVehicleArmed(playerFaction, item, playerId))
						return;
				
					if (SpawnVehicle(resourceName, playerId))
					{
						AddCountVehicleArmed(playerFaction);
						DoRpc_Notif_Succeed(item.m_priceOnce);
					} else {
						m_scoreComp.Refund(item.m_priceOnce, playerId);
						DoRpc_Notif_Failed("cannot spawn vehicle", "no place found");
					}
				break;
				default:
					bool isSuccess = RemoveOldItemsAndAddNewOnes(item, playerId);
					if (!isSuccess) {
						m_scoreComp.Refund(item.m_priceOnce, playerId);
						DoRpc_Notif_Failed("You can't buy the weapon", "your inventory are full");
						return;
					}
				
					DoRpc_Notif_Succeed(item.m_priceOnce);
				break;
			}		
		}
	}
	
	void DoRpc_Notif_Succeed(int price)
	{
		Rpc(RpcDo_NotifBuy_Succeed, price);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_NotifBuy_Succeed(int price)
	{
		ChimeraMenuBase menu = ChimeraMenuBase.CurrentChimeraMenu();
		if (!menu)
			return;
		
		KOTH_ShopUI shopLayout = KOTH_ShopUI.Cast(menu);
		if (!shopLayout)
			return;
		
		shopLayout.HUD_NotifBuy(price);
	}
	
	void DoRpc_Notif_Failed(string firstLine, string secondLine)
	{
		Rpc(RpcDo_NotifBuy_Failed, firstLine, secondLine);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_NotifBuy_Failed(string firstLine, string secondLine)
	{
		ChimeraMenuBase menu = ChimeraMenuBase.CurrentChimeraMenu();
		if (!menu)
			return;
		
		KOTH_ShopUI shopLayout = KOTH_ShopUI.Cast(menu);
		if (!shopLayout)
			return;
		
		shopLayout.NotifErrorShop(firstLine, secondLine);
	}
	
	void DoRpc_NotifBuy_PermanentBuySuccess(string itemResourceName)
	{
		Rpc(RpcDo_NotifBuy_PermanentBuySuccess, itemResourceName);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_NotifBuy_PermanentBuySuccess(string itemResourceName)
	{
		ChimeraMenuBase menu = ChimeraMenuBase.CurrentChimeraMenu();
		if (!menu)
			return;
		
		KOTH_ShopUI shopLayout = KOTH_ShopUI.Cast(menu);
		if (!shopLayout)
			return;
		
		shopLayout.NotifBuy_PermanentBuySuccess(itemResourceName);
		KOTH_SCR_PlayerProfileComponent playerProfileComp = KOTH_SCR_PlayerProfileComponent.Cast(GetGame().GetPlayerController().FindComponent(KOTH_SCR_PlayerProfileComponent));
		playerProfileComp.AskRpc_PlayerProfile();
	}
	
	// --------------------- RPC END
	
	bool RemoveOldItemsAndAddNewOnes(KOTH_ShopItem item, int playerId)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		InventoryStorageManagerComponent inventoryStorage = InventoryStorageManagerComponent.Cast(player.FindComponent(InventoryStorageManagerComponent));
		if (!inventoryStorage)
			return false;
		
		SCR_CharacterInventoryStorageComponent characterInventoryStorageComp = SCR_CharacterInventoryStorageComponent.Cast(player.FindComponent(SCR_CharacterInventoryStorageComponent));
		if (!characterInventoryStorageComp)
			return false;

		BaseInventoryStorageComponent inventoryStorageComponent = characterInventoryStorageComp.GetWeaponStorage();
		if (!inventoryStorageComponent)
			return false;
		
		if (item.m_category == KOTH_ShopItemCategory.Primary) 
		{
			// remove primary and secondary weapon slot
			IEntity ent1 = inventoryStorageComponent.GetSlot(0).GetAttachedEntity();
			IEntity ent2 = inventoryStorageComponent.GetSlot(1).GetAttachedEntity();
			if (ent1)
			{
				KOTH_ShopItemCategory category = FindEntityKOTH_ShopItemCategory(ent1);
				
				if (category == item.m_category)
				{
					inventoryStorage.TryRemoveItemFromStorage(ent1, inventoryStorageComponent);
	
					RplComponent.DeleteRplEntity(ent1, false);
				}
			}
			
			if (ent2)
			{
				KOTH_ShopItemCategory category = FindEntityKOTH_ShopItemCategory(ent2);
				
				if (category == item.m_category)
				{
					inventoryStorage.TryRemoveItemFromStorage(ent2, inventoryStorageComponent);
					RplComponent.DeleteRplEntity(ent2, false);
				}
			}
		}
		
		if (item.m_category == KOTH_ShopItemCategory.Handgun) 
		{
			// remove handgun weapon slot
			IEntity ent = inventoryStorageComponent.GetSlot(2).GetAttachedEntity();
			if (ent)
			{
				KOTH_ShopItemCategory category = FindEntityKOTH_ShopItemCategory(ent);
				
				if (category == item.m_category)
				{
					inventoryStorage.TryRemoveItemFromStorage(ent, inventoryStorageComponent);
	
					RplComponent.DeleteRplEntity(ent, false);
				}
			}
		}
				
		
		// add new weapon mags
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (item.m_magazineResource)
		{
			if (false == AddMags(inventory, item.m_magazineResource, item.m_magazineNumber)) {
				return false;
			}
		}
			
		if (item.m_secondaryMagazineResource != "") {
			if (false == AddMags(inventory, item.m_secondaryMagazineResource, item.m_secondaryMagazineNumber)) {
				return false;
			}
		}
		
		// add new weapon
		IEntity itemBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_itemResource));
        bool successAddInInventory = false;
		if (item.m_category == KOTH_ShopItemCategory.Primary || 
			item.m_category == KOTH_ShopItemCategory.Handgun ||
			item.m_category == KOTH_ShopItemCategory.Launcher) {
			successAddInInventory = inventory.TryInsertItem(itemBought, EStoragePurpose.PURPOSE_WEAPON_PROXY);
			if (successAddInInventory)
				inventory.EquipWeapon(itemBought);
		} else {
			successAddInInventory = inventory.TryInsertItem(itemBought);
		}
		
		Log("successAddInInventory "+successAddInInventory);
		return successAddInInventory;
	}
	
	KOTH_ShopItemCategory FindEntityKOTH_ShopItemCategory(IEntity entity)
	{
		string prefabData = entity.GetPrefabData().GetPrefabName();
		KOTH_ShopItemCategory currentItemCategory;
	
		foreach(string shopItemListResourceName : m_shopItemListResources)
		{
			array<ref KOTH_ShopItem> shopList = SCR_ConfigHelperT<KOTH_ShopItemList>
				.GetConfigObject(shopItemListResourceName)
				.GetItems();
			
			foreach(KOTH_ShopItem shopItem : shopList)
			{
				if (shopItem.m_itemResource == prefabData) {
					currentItemCategory = shopItem.m_category;
					break;
				}
			}
		}
		
		return currentItemCategory;
	}
	
	KOTH_ShopItem FindShopItemByResourceName(string resourceName)
	{
		KOTH_ShopItem item;
		
		foreach(string shopItemListResourceName : m_shopItemListResources)
		{
			array<ref KOTH_ShopItem> shopList = SCR_ConfigHelperT<KOTH_ShopItemList>
				.GetConfigObject(shopItemListResourceName)
				.GetItems();
			
			foreach(KOTH_ShopItem shopItem : shopList)
			{
				if (shopItem.m_itemResource == resourceName)
				{
					item = shopItem;
					break;
				}
			}
		}
		
		return item;
	}
	
	
	bool AddMags(SCR_InventoryStorageManagerComponent inventory, string itemResource, int itemNumber)
	{
		bool minimumMagInsertOk = true;
		array<IEntity> magazineList = new array<IEntity>;
		for (int i = 1; i <= itemNumber; i++)
		{
			IEntity magazineBought = GetGame().SpawnEntityPrefab(Resource.Load(itemResource));
        	bool tryInsert = inventory.TryInsertItem(magazineBought);
			if (i == 2 && tryInsert == false) {
				minimumMagInsertOk = false;
				break;
			}
			
			magazineList.Insert(magazineBought);
		}
		
		if (false == minimumMagInsertOk) {
			foreach (IEntity magazine : magazineList) {
				inventory.TryDeleteItem(magazine);
			}

			return false;
		}
		
		return true;
	}
}