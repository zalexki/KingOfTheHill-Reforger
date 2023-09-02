class KOTH_ShopWeaponUI : KOTH_ShopUI
{
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		AddItemsFromConfig("{232D181B9F9FE8D1}Configs/Shop/ShopWeaponItemList.conf");
	}
	
	override void OnSelectedTab()
	{
		ClearItemList();
		
		if (m_tabViewComponent.GetShownTab() == 0) {
			AddItemsFromConfig("{232D181B9F9FE8D1}Configs/Shop/ShopWeaponItemList.conf");
		}
		if (m_tabViewComponent.GetShownTab() == 1) {
			AddItemsFromConfig("{01BEF9B6FC671AF0}Configs/Shop/ShopThrowableItemlList.conf");
		}
		
		super.OnSelectedTab();
	}
}
