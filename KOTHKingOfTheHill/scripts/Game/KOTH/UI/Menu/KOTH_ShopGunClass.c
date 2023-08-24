class KOTH_ShopGunClass : ChimeraMenuBase
{
	protected Widget m_wRoot;
	protected SCR_PlayerController m_playerController;
	protected ItemPreviewManagerEntity m_PreviewManager;
	protected KOTH_ScoringGameModeComponent m_scoreComp;
	protected string m_playerUID;
	
	override void OnMenuInit()
	{
		super.OnMenuInit();
		m_PreviewManager = GetGame().GetItemPreviewManager();
		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
		m_playerUID = GetGame().GetBackendApi().GetPlayerUID(GetGame().GetPlayerController().GetPlayerId());
	}
	
	//informe player error in buy
	void NotifBuyFailedNoMoney(int configItemIndex)
	{
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_scrollListshop = VerticalLayoutWidget.Cast(root.FindWidget("Overlay.VerticalLayout1.ScrollList.NotifContainer"));

		Widget w = GetGame().GetWorkspace().CreateWidgets("{993FD33C936EB12C}UI/Layouts/HUD/KingOfTheHill/KOTH_ShopNotification.layout", koth_scrollListshop);

		TextWidget Seconde = TextWidget.Cast(w.FindAnyWidget("Seconde"));
		Seconde.SetText("You can't buy the weapon");

		TextWidget Third = TextWidget.Cast(w.FindAnyWidget("Third"));
		Third.SetText("You don't have enough money");

		SCR_FadeUIComponent compFade = SCR_FadeUIComponent.Cast(w.FindHandler(SCR_FadeUIComponent));
		compFade.DelayedFadeOut(10000, true);
		
		GetGame().GetCallqueue().CallLater(DelayNotif, 10, false, configItemIndex);
	}

	void NotifBuyFailedNoSpace(int configItemIndex)
	{
		
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_scrollListshop = VerticalLayoutWidget.Cast(root.FindWidget("Overlay.VerticalLayout1.ScrollList.NotifContainer"));

		Widget w = GetGame().GetWorkspace().CreateWidgets("{993FD33C936EB12C}UI/Layouts/HUD/KingOfTheHill/KOTH_ShopNotification.layout", koth_scrollListshop);

		TextWidget Seconde = TextWidget.Cast(w.FindAnyWidget("Seconde"));
		Seconde.SetText("You can't buy the weapon");

		TextWidget Third = TextWidget.Cast(w.FindAnyWidget("Third"));
		Third.SetText("Your inventory are full");

		SCR_FadeUIComponent compFade = SCR_FadeUIComponent.Cast(w.FindHandler(SCR_FadeUIComponent));
		compFade.DelayedFadeOut(10000, true);
		
		GetGame().GetCallqueue().CallLater(DelayNotif, 10, false, configItemIndex);
	}
	
	void DelayNotif(int configItemIndex)
	{
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_ContentContainer = VerticalLayoutWidget.Cast(root.FindWidget("Overlay.VerticalLayout0.HorizontalLayout0.Container.Content.Scroll.ContentContainer"));
		
		bool needSearch = true;
		int i = -1;
		Widget widget = koth_ContentContainer.GetChildren();
		TextWidget textIndex = TextWidget.Cast(koth_ContentContainer.FindAnyWidget("ConfigItemIndex"));
		
		if (textIndex.GetText() == configItemIndex.ToString())
		{
			Print("TROUVER premier coup");
			needSearch = false;
		}
		
		while (needSearch && i < 50 )
		{
			i++;
			
			widget = widget.GetSibling();
			textIndex = TextWidget.Cast(widget.FindAnyWidget("ConfigItemIndex"));
			
			if (textIndex.GetText() == configItemIndex.ToString())
				{
					Print("TROUVER mais pas au premier coup");
					needSearch = false;
				}
		}
		
		ButtonWidget purchaseOnceButton = ButtonWidget.Cast(widget.FindAnyWidget("PurchaseOnceButton"));	
		OverlayWidget purchaseOnceHINT = OverlayWidget.Cast(widget.FindAnyWidget("PurchaseOnceHINT"));
		purchaseOnceButton.SetVisible(true);
		purchaseOnceHINT.SetVisible(false);
	}
	
	
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

		/*SCR_ButtonBaseComponent shopCategorieThrowable = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Throwable", m_wRoot);
		if (shopCategorieThrowable)
			{
				shopCategorieThrowable.m_OnClicked.Insert(OnClickThrowable);
			}
		
		SCR_ButtonBaseComponent shopCategorieThrowable = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Throwable", m_wRoot);
		if (shopCategorieThrowable)
			{
				shopCategorieThrowable.m_OnClicked.Insert(OnClickThrowable);
			}
		
		SCR_ButtonBaseComponent shopCategorieThrowable = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Throwable", m_wRoot);
		if (shopCategorieThrowable)
			{
				shopCategorieThrowable.m_OnClicked.Insert(OnClickThrowable);
			}*/
				
		AddItemsFromConfig();
	}
	
	//===============================================================================================================
	protected void AddItemsFromConfig()
	{
		KOTH_SCR_ShopGunItemList itemList = SCR_ConfigHelperT<KOTH_SCR_ShopGunItemList>.GetConfigObject("{232D181B9F9FE8D1}Configs/ShopGunItemList.conf");
		VerticalLayoutWidget contentContainer = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("ContentContainer"));

		foreach (int index, KOTH_SCR_ShopGunItem item : itemList.GetItems())
		{
			onForeach("{20EF71DE68A5887E}UI/Layouts/HUD/Shop/ShopGun_ItemList.layout", KOTH_SCR_ShopGunItem.Cast(item), contentContainer, index);
		}
	}
	//===============================================================================================================
	void onForeach(string itemListlayout, KOTH_SCR_ShopGunItem item, VerticalLayoutWidget contentContainer, int index)
	{
		Widget newRow = GetGame().GetWorkspace().CreateWidgets(itemListlayout, contentContainer);
			
		// add click events
		SCR_ButtonBaseComponent buyOnce = SCR_ButtonBaseComponent.GetButtonBase("PurchaseOnceButton", newRow);
		if (buyOnce)
			buyOnce.m_OnClicked.Insert(OnClickBuyOnce);

		/*SCR_ButtonBaseComponent buyPermanent = SCR_ButtonBaseComponent.GetButtonBase("PurchasePermanentButton", newRow);
		if (buyPermanent)
			buyPermanent.m_OnClicked.Insert(OnClickBuyPermanent); */

		// set text infos 
		TextWidget nameWidget = TextWidget.Cast(newRow.FindAnyWidget("ItemName"));
		nameWidget.SetText(item.m_itemName);
		TextWidget ConfigItemIndexWidget = TextWidget.Cast(newRow.FindAnyWidget("ConfigItemIndex"));
		ConfigItemIndexWidget.SetText(index.ToString());
		TextWidget priceOnceWidget = TextWidget.Cast(newRow.FindAnyWidget("PriceOnceText"));
			priceOnceWidget.SetText(item.m_priceOnce.ToString() + "$");
			TextWidget pricePermWidget = TextWidget.Cast(newRow.FindAnyWidget("PricePermText"));
			pricePermWidget.SetText(item.m_pricePermanent.ToString() + "$");

			//get player level
			int playerLevel = 1;
			KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
			foreach (KOTH_PlayerProfileJson profile : scoreComp.m_listPlayerProfiles) {
				if (profile.m_playerUID == m_playerUID) {
					playerLevel = profile.GetLevel();
					break;
				}
			}
			
			if (playerLevel < item.m_level)
			{
				ButtonWidget purchaseOnceButton = ButtonWidget.Cast(newRow.FindAnyWidget("PurchaseOnceButton"));
				ButtonWidget purchasePermanentButton = ButtonWidget.Cast(newRow.FindAnyWidget("PurchasePermanentButton"));
				
				OverlayWidget purchaseOnceNoLVL = OverlayWidget.Cast(newRow.FindAnyWidget("PurchaseOnceNoLVL"));
				OverlayWidget purchasePermNoLVL = OverlayWidget.Cast(newRow.FindAnyWidget("PurchasePermNoLVL"));
				OverlayWidget desactivatePermHINT = OverlayWidget.Cast(newRow.FindAnyWidget("DesactivatePermHINT"));
				
				ImageWidget fadeLVLReq = ImageWidget .Cast(newRow.FindAnyWidget("FadeLVLReq"));
				TextWidget textLevelReq = TextWidget.Cast(newRow.FindAnyWidget("TextLevelReq"));
				
				purchaseOnceButton.SetVisible(false);
				
				if (purchasePermanentButton)
				{
					purchasePermanentButton.SetVisible(false);
				}
				
				if (desactivatePermHINT)
				{
					desactivatePermHINT.SetVisible(false);
				}
				
				purchaseOnceNoLVL.SetVisible(true);
				
				if (purchasePermNoLVL)
				{
					purchasePermNoLVL.SetVisible(true);
				}

				
				fadeLVLReq.SetVisible(true);
				textLevelReq.SetVisible(true);
				
				textLevelReq.SetText("LEVEL " + item.m_level.ToString() + " REQUIRED");
			}
			else 
			{
				// set visible or not buy
				
				// TODO: can buy from current money
				ButtonWidget purchaseOnceButton = ButtonWidget.Cast(newRow.FindAnyWidget("PurchaseOnceButton"));
				OverlayWidget purchaseOnceHINT = OverlayWidget.Cast(newRow.FindAnyWidget("PurchaseOnceHINT"));
				purchaseOnceButton.SetVisible(true);
				purchaseOnceHINT.SetVisible(false);

			}


			// add item preview
			
			ImageWidget imageWeapon = ImageWidget .Cast(newRow.FindAnyWidget("ImageWeapon"));
			imageWeapon.LoadImageTexture(0, item.m_textureResource);
			imageWeapon.LoadMaskTexture(item.m_maskResource);
			
			
			/*ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(newRow.FindAnyWidget("Preview_Image"));
			if (wRenderTarget && m_PreviewManager)
			{
				Log("add to shop" + item.m_itemResource);
				m_PreviewManager.SetPreviewItemFromPrefab(wRenderTarget, item.m_itemResource, null, false);
			}*/
	}
	//===============================================================================================================
	void OnClickThrowable()
	{
		GetGame().GetMenuManager().CloseMenu(this);
		//GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.KOTH_ShopThrowable);
	
	}
	//===============================================================================================================
	void OnClickGadget()
	{
		GetGame().GetMenuManager().CloseMenu(this);
		//GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.KOTH_ShopGadget);
	
	}
	//===============================================================================================================
	void OnClickWeapons()
	{
		GetGame().GetMenuManager().CloseMenu(this);
		//GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.KOTH_ShopGun);
	
	}
	//===============================================================================================================
	
	// TODO: equip weapon,magazines ...
	protected void OnClickBuyOnce(SCR_ButtonBaseComponent button)
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

	protected void OnClickBuyPermanent(SCR_ButtonBaseComponent button)
	{
		Log("not implemented yet");
//		Widget row = button.GetRootWidget().GetParent().GetParent().GetParent();
//		TextWidget priceOnceWidget = TextWidget.Cast(row.FindAnyWidget("PricePermText"));
//		TextWidget itemResourceNameWidget = TextWidget.Cast(row.FindAnyWidget("ItemResourceName"));
//
//		PlayerManager playerManager = GetGame().GetPlayerManager();
//		PlayerController controller = GetGame().GetPlayerController();
//		int playerId = controller.GetPlayerId();
//		string playerName = playerManager.GetPlayerName(playerId);
//
//
//		KOTH_SCR_PlayerShopComponent kothPlayerComp = KOTH_SCR_PlayerShopComponent.Cast(controller.FindComponent(KOTH_SCR_PlayerShopComponent));
//
//		int price = priceOnceWidget.GetText().ToInt();
//		string itemResourceName = itemResourceNameWidget.GetText();
//		kothPlayerComp.DoRpcBuy(price, playerName, itemResourceName);
//		HUD_NotifBuy(price);
	}

	private void UpdateItemsFromCurrentMoney()
	{
		//TODO: when money is spend we need to update what can be buy or no
	}

	void HUD_NotifBuy(int amount)
	{
		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifBuy(amount);
			}
		}
	}

	protected void OnClickEscape()
	{
		GetGame().GetMenuManager().CloseMenu(this);
		
	}
}
