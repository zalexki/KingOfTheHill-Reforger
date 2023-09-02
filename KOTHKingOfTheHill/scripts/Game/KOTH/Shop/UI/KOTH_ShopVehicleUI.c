class KOTH_ShopVehicleUI : KOTH_ShopUI
{
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		AddItemsFromConfig("{9F317D1901DBE909}Configs/Shop/ShopVehicleItemList.conf");
		m_tabViewComponent.RemoveTab(1);
	}
}
