class KOTH_ShopGunUI : KOTH_ShopGunClass
{
	override void OnMenuOpen()
	{
		super.OnMenuOpen();

		m_wRoot = GetRootWidget();
		SCR_NavigationButtonComponent cancel = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Cancel", m_wRoot);
		if (cancel)
			{
				cancel.m_OnActivated.Insert(OnClickEscape);
			}

		SCR_ButtonBaseComponent shopCategorieThrowable = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Throwable", m_wRoot);
		if (shopCategorieThrowable)
			{
				shopCategorieThrowable.m_OnClicked.Insert(OnClickThrowable);
			}

		SCR_ButtonBaseComponent shopCategorieGadget = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Gadget", m_wRoot);
		if (shopCategorieGadget)
			{
				shopCategorieGadget.m_OnClicked.Insert(OnClickGadget);
			}
		
		SCR_ButtonBaseComponent shopCategorieWeapons = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Weapons", m_wRoot);
		if (shopCategorieWeapons)
			{
				shopCategorieWeapons.m_OnClicked.Insert(OnClickWeapons);
			}
				
		AddItemsFromConfig();
	}
	
	//===============================================================================================================
	override void AddItemsFromConfig()
	{
		KOTH_SCR_ShopGunItemList itemList = SCR_ConfigHelperT<KOTH_SCR_ShopGunItemList>.GetConfigObject("{232D181B9F9FE8D1}Configs/ShopGunItemList.conf");
		VerticalLayoutWidget contentContainer = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("ContentContainer"));

		foreach (int index, KOTH_SCR_ShopGunItem item : itemList.GetItems())
		{
			onForeach("{20EF71DE68A5887E}UI/Layouts/HUD/Shop/ShopGun_ItemList.layout", KOTH_SCR_ShopGunItem.Cast(item), contentContainer, index);
		}
	}
	//===============================================================================================================
	
	// TODO: equip weapon,magazines ...
	override protected void OnClickBuyOnce(SCR_ButtonBaseComponent button)
	{
		Widget row = button.GetRootWidget().GetParent().GetParent().GetParent();
		TextWidget priceOnceWidget = TextWidget.Cast(row.FindAnyWidget("PriceOnceText"));
		TextWidget configItemIndexWidget = TextWidget.Cast(row.FindAnyWidget("ConfigItemIndex"));

		PlayerController controller = GetGame().GetPlayerController();
		IEntity cont = controller.GetControlledEntity();
		KOTH_SCR_PlayerShopComponent kothPlayerComp = KOTH_SCR_PlayerShopComponent.Cast(controller.FindComponent(KOTH_SCR_PlayerShopComponent));
		
		int price = priceOnceWidget.GetText().ToInt();
		kothPlayerComp.DoRpcBuy(configItemIndexWidget.GetText().ToInt(), m_playerUID, controller.GetPlayerId());

		// set widget equiped visible
		ButtonWidget purchaseOnceButton = ButtonWidget.Cast(row.FindAnyWidget("PurchaseOnceButton"));
		OverlayWidget purchaseOnceHINT = OverlayWidget.Cast(row.FindAnyWidget("PurchaseOnceHINT"));
		purchaseOnceButton.SetVisible(false);
		purchaseOnceHINT.SetVisible(true);
	}
}
