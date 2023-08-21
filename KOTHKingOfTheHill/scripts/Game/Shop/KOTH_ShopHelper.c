class KOTH_ShopHelper
{
//----------------------------------------------------------------------------------------------
/*	void PurchaseItem(SCR_ButtonComponent button)
	{
		KOTH_ShopHelper uiItem = FindShopUIItem(button.GetRootWidget());
		if (uiItem)
		{
			PlayerController playerController = GetGame().GetPlayerController();
			ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(playerController.FindComponent(ADM_PlayerShopManagerComponent));
			if (!playerShopManager) return;
			
			KOTH_ShopMerch merch = uiItem.GetMerchandise();
			playerShopManager.AskPurchase(m_Shop, merch, uiItem.GetQuantity());
		}
	}
	//----------------------------------------------------------------------------------------------
	bool Deliver(IEntity player, ADM_ShopBaseComponent shop)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return;
	
		array<IEntity> deliveredItems = {};
		
		// give item/weapon/magazine/clothing to player	
		IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_sPrefab));	
		bool putInInventory = KOTH_Utils.InsertAutoEquipItem(inventory, item);
		
		// If we cant do with conditions false, delete it
		if (!putInInventory)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(item);
		} else {
			deliveredItems.Insert(item);
		}
	}
	//----------------------------------------------------------------------------------------------
	static KOTH_ShopHelper FindShopUIItem(Widget w)
	{
		Widget parent = w.GetParent();
		while (parent)
		{
			KOTH_ShopHelper uiItem = KOTH_ShopHelper.Cast(parent.FindHandler(KOTH_ShopHelper));
			if (uiItem)
				return uiItem;
			else
				parent = parent.GetParent();
		}
		
		return null;
	}
	//----------------------------------------------------------------------------------------------
	void SetMerchandise(KOTH_MerchandiseType merch)
	{
		m_Merchandise = merch;
	}
	//get prefab to give
	ResourceName GetPrefab() 
	{ 
		return m_sPrefab; 
	}
};
//----------------------------------------------------------------------------------------------
//array of the list of prefab in shop
class KOTH_ShopMerch
{
	KOTH_MerchandiseType GetMerchandise() 
	{ 
		return m_Merchandise; 
	}
}
//----------------------------------------------------------------------------------------------
class KOTH_Utils
{
	static bool InsertAutoEquipItem(SCR_InventoryStorageManagerComponent inventory, IEntity item)
	{
		EStoragePurpose purpose = EStoragePurpose.PURPOSE_ANY;
		if (item.FindComponent(WeaponComponent)) purpose = EStoragePurpose.PURPOSE_WEAPON_PROXY;
		if (item.FindComponent(BaseLoadoutClothComponent)) purpose = EStoragePurpose.PURPOSE_LOADOUT_PROXY;
		if (item.FindComponent(SCR_GadgetComponent)) purpose = EStoragePurpose.PURPOSE_GADGET_PROXY;
		
		bool insertedItem = inventory.TryInsertItem(item, purpose, null);
		if (!insertedItem) 
			insertedItem = inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY, null);
		
		return insertedItem;
	}*/
};
//----------------------------------------------------------------------------------------------

