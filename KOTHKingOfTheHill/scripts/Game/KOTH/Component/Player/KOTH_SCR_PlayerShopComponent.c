class KOTH_SCR_PlayerShopComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerShopComponent : ScriptComponent 
{
	protected ref array<ref KOTH_ShopItem> m_shopItemList;
	protected string m_playerUID;

	override void OnPostInit(IEntity owner)
	{
		m_shopItemList = SCR_ConfigHelperT<KOTH_ShopItemList>.GetConfigObject("{232D181B9F9FE8D1}Configs/ShopGunItemList.conf").GetItems();
		PlayerController controller = GetGame().GetPlayerController();
		if (controller)
			m_playerUID = GetGame().GetBackendApi().GetPlayerUID(controller.GetPlayerId());
	}
	
	void DoRpcBuy(int configItemIndex, int playerId)
	{
		Rpc(RpcAsk_BuyStuff, configItemIndex, playerId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_BuyStuff(int configItemIndex, int playerId)
	{
		Log("----------- BuyStuff from rpc call");		
		KOTH_ShopItem item = m_shopItemList.Get(configItemIndex);
		IEntity controlledEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
		
		bool buySuccess = scoreComp.TryBuy(item.m_priceOnce, playerId);
		if (buySuccess) {
			bool isSuccess = RemoveOldItemsAndAddNewOnes(controlledEntity, item, playerId);
			
			if (!isSuccess) {
				DoRpc_Notif_Failed_NoSpace();
			}
		} 
		else 
		{
			DoRpc_Notif_Failed_NoMoney();
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
		
		shopLayout.TestRpcBuySucceed();
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
		
		shopLayout.TestRpcBuyFailed();
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
		
		shopLayout.TestRpcBuyFailed();
	}
	
	bool RemoveOldItemsAndAddNewOnes(IEntity player, KOTH_ShopItem item, int playerId)
	{
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