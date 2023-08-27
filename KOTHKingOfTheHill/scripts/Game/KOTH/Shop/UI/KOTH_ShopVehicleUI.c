class KOTH_ShopVehicleUI : KOTH_ShopUI
{
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		AddItemsFromConfig("{9F317D1901DBE909}Configs/Shop/ShopVehicleItemList.conf");
		m_parentShopCategorie_Weapons.SetVisible(false);
		m_parentShopCategorie_Ammo.SetVisible(false);
		m_parentShopCategorie_Gadget.SetVisible(false);
		
		m_nameContainer.SetText("Vehicle Shop");
	}
}
