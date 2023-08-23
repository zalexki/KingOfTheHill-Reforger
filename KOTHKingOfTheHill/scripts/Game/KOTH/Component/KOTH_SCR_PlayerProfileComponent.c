class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	private ref array<ref KOTH_SCR_ShopGunItem> m_shopItemList;
	
	override void OnPostInit(IEntity owner)
	{
		m_shopItemList = SCR_ConfigHelperT<KOTH_SCR_ShopGunItemList>.GetConfigObject("{232D181B9F9FE8D1}Configs/ShopGunItemList.conf").GetItems();
	}
	
	void DoRpcBuy(int configItemIndex)
	{
		Rpc(RpcAsk_BuyStuff, configItemIndex, GetGame().GetPlayerController().GetPlayerId());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_BuyStuff(int configItemIndex, int playerId)
	{
		Log("----------- BuyStuff from rpc call");		
		KOTH_SCR_ShopGunItem item = m_shopItemList.Get(configItemIndex);
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		IEntity controlledEntity = playerManager.GetPlayerControlledEntity(playerId);
		KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
		
		bool buySuccess = scoreComp.TryBuy(item.m_priceOnce, playerManager.GetPlayerName(playerId));
		if (buySuccess) {
			RemoveOldItemsAndAddNewOnes(controlledEntity, item);
		} else {
			DoRpc_Notif_Failed_NoSpace();
		}
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
	
	void RemoveOldItemsAndAddNewOnes(IEntity player, KOTH_SCR_ShopGunItem item)
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
		
		IEntity ent1 = inventoryStorageComponent.GetSlot(0).GetAttachedEntity();
		IEntity ent2 = inventoryStorageComponent.GetSlot(1).GetAttachedEntity();
		if (ent1)
		{
			WeaponCategory category = FindEntityWeaponCategory(ent1);
			
			if (category == item.m_category)
			{
				inventoryStorage.TryRemoveItemFromStorage(ent1, inventoryStorageComponent);
				//SCR_EntityHelper.DeleteEntityAndChildren(ent1);
				RplComponent.DeleteRplEntity(ent1, false);
			}
		}
		
		if (ent2)
		{
			WeaponCategory category = FindEntityWeaponCategory(ent2);
			
			if (category == item.m_category)
			{
				inventoryStorage.TryRemoveItemFromStorage(ent2, inventoryStorageComponent);
				//SCR_EntityHelper.DeleteEntityAndChildren(ent1);
				RplComponent.DeleteRplEntity(ent1, false);
			}
		}
		
		// remove primary and secondary weapon slot
		
		
		// add new weapon
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		
		if (false == AddMags(inventory, item.m_magazineResource, item.m_magazineNumber))
			return;
		
		if (item.m_secondaryMagazineResource != "") {
			if (false == AddMags(inventory, item.m_secondaryMagazineResource, item.m_secondaryMagazineNumber))
			return;
		}

		IEntity itemBought = GetGame().SpawnEntityPrefab(Resource.Load(item.m_itemResource));
        inventory.EquipWeapon(itemBought);
		
		DoRpc_Notif_Succeed();
		
		return; // ---------- dont remove mags, for now ?
		
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
				return;
			
			// not sure if needed or not
			if (!inventoryStorage.TryRemoveItemFromStorage(entity, inventoryStorageComponent))
			{
				RplComponent.DeleteRplEntity(entity, false)
			}
		}
	}
	
	WeaponCategory FindEntityWeaponCategory(IEntity entity)
	{
		WeaponCategory currentItemCategory;
		
		foreach(KOTH_SCR_ShopGunItem shopItem : m_shopItemList)
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