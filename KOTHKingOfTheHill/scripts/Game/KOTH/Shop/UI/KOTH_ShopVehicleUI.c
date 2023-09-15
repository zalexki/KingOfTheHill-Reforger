class KOTH_ShopVehicleUI : KOTH_ShopUI
{
	override void OnMenuInit()
	{
		super.OnMenuInit();
		
		// here add the custom lists
		m_listShopItemList = {
			"{9F317D1901DBE909}Configs/Shop/ShopVehicleItemList.conf"
		};
	}
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		// here you must change the tabs accordingly to the list above
		m_tabViewComponent.SetTabText(0, "Vehicles");
		m_tabViewComponent.RemoveTab(1);
	}
}
