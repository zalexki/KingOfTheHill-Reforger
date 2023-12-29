class KOTH_ShopWeaponUI : KOTH_ShopUI
{
	override void OnMenuInit()
	{
		super.OnMenuInit();
		
		// here add the custom lists (dont forget to add them in the KOTH_SCR_PlayerShopComponent)
		m_listShopItemList = {
			"{E791EE08D968E844}Configs/Shop/ShopWeaponItemList.conf",
			"{01BEF9B6FC671AF0}Configs/Shop/ShopThrowableItemlList.conf"
		};
	}
	
	// layout was done for weapon first so no further changes needed
}
