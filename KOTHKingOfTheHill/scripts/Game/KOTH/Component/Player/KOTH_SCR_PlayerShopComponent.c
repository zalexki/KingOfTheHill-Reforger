class KOTH_SCR_PlayerShopComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerShopComponent : ScriptComponent 
{
	protected ref array<ref KOTH_ShopItem> m_shopItemList;
	protected string m_playerUID;

	override void OnPostInit(IEntity owner)
	{
		m_shopItemList = SCR_ConfigHelperT<KOTH_ShopItemList>.GetConfigObject("{232D181B9F9FE8D1}Configs/Shop/ShopWeaponItemList.conf").GetItems();
		PlayerController controller = GetGame().GetPlayerController();
		if (controller)
			m_playerUID = GetGame().GetBackendApi().GetPlayerUID(controller.GetPlayerId());
	}
	
	void DoRpcBuy(string resourceName, int playerId, bool permanentBuy = false)
	{
		Rpc(RpcAsk_Buy, resourceName, playerId, permanentBuy);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_Buy(string resourceName, int playerId, bool permanentBuy)
	{
		Log("----------- RpcAsk_Buy "+resourceName+" for "+playerId+" is permanentBuy "+permanentBuy);
		KOTH_ShopItem item = FindShopItemByresourceName(resourceName);

		if (!item)
		{
			Log("no item found for resourceName "+resourceName);
			return;
		}
		
		KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));

		if (permanentBuy)
		{
			Log("/!\/!\/!\/!\ Not implemented yet ");	
		} 
		else 
		{
			bool buySuccess = scoreComp.TryBuy(item.m_priceOnce, playerId);
			if (buySuccess) {
				// check category for vehicles
				switch (item.m_category) {
					case KOTH_ShopItemCategory.Vehicle:
						Log("buy vehicle");
						// get player Faction
						// found spawns for faction
						FoundSpawnForVehicle("vehicleSpawnFirst", resourceName);
					break;
					default:
						bool isSuccess = RemoveOldItemsAndAddNewOnes(item, playerId);
				
						if (!isSuccess) {
							DoRpc_Notif_Failed_NoSpace();
						}
					break;
				}
			}
			else 
			{
				DoRpc_Notif_Failed_NoMoney();
			}		
		}
	}
	
	KOTH_ShopItem FindShopItemByresourceName(string resourceName)
	{
		KOTH_ShopItem item;
		foreach(KOTH_ShopItem iteminlist : m_shopItemList)
		{
			if (iteminlist.m_itemResource == resourceName)
			{
				item = iteminlist;
			}
		}
		
		if (!item) {
		 	array< ref KOTH_ShopItem> shopList = SCR_ConfigHelperT<KOTH_ShopItemList>.GetConfigObject("{9F317D1901DBE909}Configs/Shop/ShopVehicleItemList.conf").GetItems();
			foreach(KOTH_ShopItem iteminlist : shopList)
			{
				if (iteminlist.m_itemResource == resourceName)
				{
					item = iteminlist;
				}
			}
		}
		
		return item;
	}
	
	void FoundSpawnForVehicle(string faction, string resourceName)
	{
		KOTH_SpawnPrefab firstSpawn = KOTH_SpawnPrefab.Cast(GetGame().GetWorld().FindEntityByName("vehicleSpawnFirst"));
		firstSpawn.Spawn(resourceName);
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
		
		KOTH_ShopUI shopLayout = KOTH_ShopUI.Cast(menu);
		if (!shopLayout)
			return;
		
		shopLayout.NotifErrorShop("You can't buy the weapon", "your inventory are full");
	}
	
	void DoRpc_Notif_Failed_NoMoney()
	{
		Rpc(RpcDo_NotifBuy_Failed_NoMoney);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_NotifBuy_Failed_NoMoney()
	{
		ChimeraMenuBase menu = ChimeraMenuBase.CurrentChimeraMenu();
		if (!menu)
			return;
		
		KOTH_ShopUI shopLayout = KOTH_ShopUI.Cast(menu);
		if (!shopLayout)
			return;
		
		shopLayout.NotifErrorShop("You can't buy the weapon", "not enough money");
	}
	
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
		
		// add new weapon
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		
		if (false == AddMags(inventory, item.m_magazineResource, item.m_magazineNumber)) {
			// refund
			KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
			scoreComp.Refund(item.m_priceOnce, playerId);

			return false;
		}
		
		if (item.m_secondaryMagazineResource != "") {
			if (false == AddMags(inventory, item.m_secondaryMagazineResource, item.m_secondaryMagazineNumber)) {
				// refund
				KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
				scoreComp.Refund(item.m_priceOnce, playerId);
				return false;
			}
		}

		IEntity itemBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_itemResource));
        inventory.TryInsertItem(itemBought);
		
		DoRpc_Notif_Succeed(item.m_priceOnce);
		
		return true; // ---------- dont remove mags, for now ?
		
		// remove all mags
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
				return false;
			
			// not sure if needed or not
			if (!inventoryStorage.TryRemoveItemFromStorage(entity, inventoryStorageComponent))
			{
				RplComponent.DeleteRplEntity(entity, false)
			}
		}
	}
	
	KOTH_ShopItemCategory FindEntityKOTH_ShopItemCategory(IEntity entity)
	{
		KOTH_ShopItemCategory currentItemCategory;
		
		foreach(KOTH_ShopItem shopItem : m_shopItemList)
		{
			string prefabData = entity.GetPrefabData().GetPrefabName();
			string itemName = shopItem.m_itemResource;
			if (prefabData == itemName) {
				currentItemCategory = shopItem.m_category;
			}
		}
		
		return currentItemCategory;
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

			DoRpc_Notif_Failed_NoSpace();
			
			return false;
		}
		
		return true;
	}
}