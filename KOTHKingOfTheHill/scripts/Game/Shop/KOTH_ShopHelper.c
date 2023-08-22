class KOTH_ShopHelper
{
	
	//-----------------------------------------------------------------------------------
	void OnBuy(IEntity controlledEntity)
    {
		
        SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(controlledEntity.FindComponent(SCR_InventoryStorageManagerComponent));

		IEntity itemtest = GetGame().SpawnEntityPrefab(Resource.Load("{3E413771E1834D2F}Prefabs/Weapons/Rifles/M16/Rifle_M16A2.et"));
        inventory.EquipWeapon(itemtest);
        inventory.TryInsertItem(itemtest);

    }
	
	//----------------------------------------------------------------------------------
	
	void FindAndClearAnyWeaponsInInventoty(IEntity controlledEntity)
	{
		
		SCR_CharacterInventoryStorageComponent inventoryStorage = SCR_CharacterInventoryStorageComponent.Cast(controlledEntity.FindComponent(SCR_CharacterInventoryStorageComponent));
		
		BaseInventoryStorageComponent inventoryComponent = inventoryStorage.GetWeaponStorage();
        
		InventoryStorageSlot slot1 = inventoryComponent.GetSlot(0);
        InventoryStorageSlot slot2 = inventoryComponent.GetSlot(1);
        InventoryStorageSlot slot3 = inventoryComponent.GetSlot(2);
		
		SCR_EntityHelper.DeleteEntityAndChildren(slot1.GetAttachedEntity());
		SCR_EntityHelper.DeleteEntityAndChildren(slot2.GetAttachedEntity());
		SCR_EntityHelper.DeleteEntityAndChildren(slot3.GetAttachedEntity());

	}
	
	//----------------------------------------------------------------------------------
	
	void FindAndClearMagazineInInventory(IEntity user)
	{
		InventoryStorageManagerComponent inventoryStorage = InventoryStorageManagerComponent.Cast(user.FindComponent(InventoryStorageManagerComponent));
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
	//-----------------------------------------------------------------------------------
};

	
