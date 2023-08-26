class KOTH_ShopWeaponUI : KOTH_ShopUI
{
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		AddItemsFromConfig("{232D181B9F9FE8D1}Configs/Shop/ShopWeaponItemList.conf");
	}
}
