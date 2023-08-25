class KOTH_ShopUI : ChimeraMenuBase
{
	protected Widget m_wRoot;
	
	protected Widget m_parentShopCategorie_Weapons;
	protected Widget m_parentShopCategorie_Ammo;
	protected Widget m_parentShopCategorie_Gadget;
	
	protected VerticalLayoutWidget m_contentContainer;
	
	protected KOTH_ScoringGameModeComponent m_scoreComp;
	
	protected int m_playerId;
	
	override void OnMenuInit()
	{
		super.OnMenuInit();
		
		m_playerId = GetGame().GetPlayerController().GetPlayerId();
		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
	}
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_wRoot = GetRootWidget();
		
		// add listeners
		SCR_NavigationButtonComponent cancel = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Cancel", m_wRoot);
		if (cancel) { cancel.m_OnActivated.Insert(OnClickEscape); }
		SCR_ButtonBaseComponent weaponBtn = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Weapons", m_wRoot);
		if (weaponBtn) { weaponBtn.m_OnClicked.Insert(OnClickWeaponCategory); }
		SCR_ButtonBaseComponent ammoBtn = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Ammo", m_wRoot);
		if (ammoBtn) { ammoBtn.m_OnClicked.Insert(OnClickAmmoCategory); }
		SCR_ButtonBaseComponent gadgetBtn = SCR_ButtonBaseComponent.GetButtonBase("ShopCategorie_Gadget", m_wRoot);
		if (gadgetBtn) { gadgetBtn.m_OnClicked.Insert(OnClickGadgetCategory); }
		
		m_parentShopCategorie_Weapons = m_wRoot.FindAnyWidget("ShopCategorie_Weapons");
		TextWidget text = TextWidget.Cast(m_parentShopCategorie_Weapons.FindAnyWidget("Content"));
		text.SetColor(Color.Gray75);
		m_parentShopCategorie_Ammo = m_wRoot.FindAnyWidget("ShopCategorie_Ammo");
		m_parentShopCategorie_Gadget = m_wRoot.FindAnyWidget("ShopCategorie_Gadget");
		
		m_contentContainer = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("ContentContainer"));
	}

	protected void AddItemsFromConfig(string itemListResource)
	{
		KOTH_ShopItemList itemList = SCR_ConfigHelperT<KOTH_ShopItemList>.GetConfigObject(itemListResource);

		foreach (int index, KOTH_ShopItem item : itemList.GetItems())
		{
			Widget newRow = GetGame().GetWorkspace().CreateWidgets("{20EF71DE68A5887E}UI/Layouts/HUD/Shop/ShopWeapon_ItemList.layout", m_contentContainer);
			
			// add click events
			SCR_ButtonBaseComponent buyOnce = SCR_ButtonBaseComponent.GetButtonBase("PurchaseOnceButton", newRow);
			if (buyOnce)
				buyOnce.m_OnClicked.Insert(OnClickBuyOnce);
			
			if (item.m_pricePermanent > 1) {
				ShowNotOwned(newRow);
			}
			SCR_ButtonBaseComponent buyPermanent = SCR_ButtonBaseComponent.GetButtonBase("PurchasePermanentButton", newRow);
			if (buyPermanent)
				buyPermanent.m_OnClicked.Insert(OnClickBuyPermanent);

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
				if (profile.m_playerId == m_playerId) {
					playerLevel = profile.GetLevel();
					break;
				}
			}
			
			if (playerLevel < item.m_level)
			{
				ButtonWidget purchaseOnceButton = ButtonWidget.Cast(newRow.FindAnyWidget("PurchaseOnceButton"));
				ButtonWidget PurchasePermanentButton = ButtonWidget.Cast(newRow.FindAnyWidget("PurchasePermanentButton"));
				
				OverlayWidget purchaseOnceNoLVL = OverlayWidget.Cast(newRow.FindAnyWidget("PurchaseOnceNoLVL"));
				OverlayWidget purchasePermNoLVL = OverlayWidget.Cast(newRow.FindAnyWidget("PurchasePermNoLVL"));
				OverlayWidget desactivatePermHINT = OverlayWidget.Cast(newRow.FindAnyWidget("DesactivatePermHINT"));
				
				ImageWidget fadeLVLReq = ImageWidget .Cast(newRow.FindAnyWidget("FadeLVLReq"));
				TextWidget textLevelReq = TextWidget.Cast(newRow.FindAnyWidget("TextLevelReq"));
				
				purchaseOnceButton.SetVisible(false);
				PurchasePermanentButton.SetVisible(false);
				desactivatePermHINT.SetVisible(false);
				
				purchaseOnceNoLVL.SetVisible(true);
				purchasePermNoLVL.SetVisible(true);
				
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
		}
	}
	
	void ShowNotOwned(Widget newRow)
	{
		OverlayWidget overlayOwned = OverlayWidget.Cast(newRow.FindAnyWidget("PurchasePermHINT"));
		overlayOwned.SetVisible(false);
		OverlayWidget overlayDeactived = OverlayWidget.Cast(newRow.FindAnyWidget("DesactivatePermHINT"));
		overlayDeactived.SetVisible(false);
		ButtonWidget overlayBuyPerm = ButtonWidget.Cast(newRow.FindAnyWidget("PurchasePermanentButton"));
		overlayBuyPerm.SetVisible(true);
	}
	
	void ShowOwned(Widget newRow)
	{
		OverlayWidget overlayOwned = OverlayWidget.Cast(newRow.FindAnyWidget("PurchasePermHINT"));
		overlayOwned.SetVisible(true);
		
		OverlayWidget overlayDeactived = OverlayWidget.Cast(newRow.FindAnyWidget("DesactivatePermHINT"));
		overlayDeactived.SetVisible(false);
		ButtonWidget overlayBuyPerm = ButtonWidget.Cast(newRow.FindAnyWidget("PurchasePermanentButton"));
		overlayBuyPerm.SetVisible(false);
	}
	
	void ClearItemList()
	{
		int i = 0;
		bool clearFinished = false;
		while (!clearFinished && i < 1000) 
		{
			i++;
			Widget child = m_contentContainer.GetChildren();
			if (child) 
			{
				m_contentContainer.RemoveChild(child);
			} else {
				clearFinished = true;
			}
		}
	}

	//ShopCategorie_Weapons
	void OnClickWeaponCategory()
	{
		Log("OnClickWeaponCategory");
		// empty list
		TextWidget text = TextWidget.Cast(m_parentShopCategorie_Gadget.FindAnyWidget("Content"));
		text.SetColor(Color.White);
		text = TextWidget.Cast(m_parentShopCategorie_Ammo.FindAnyWidget("Content"));
		text.SetColor(Color.White);
		text = TextWidget.Cast(m_parentShopCategorie_Weapons.FindAnyWidget("Content"));
		text.SetColor(Color.Gray25);
		
		ClearItemList();
		AddItemsFromConfig("{232D181B9F9FE8D1}Configs/Shop/ShopWeaponItemList.conf");
	}
	
	//ShopCategorie_Ammo
	void OnClickAmmoCategory()
	{
		Log("OnClickAmmoCategory");
		TextWidget text = TextWidget.Cast(m_parentShopCategorie_Gadget.FindAnyWidget("Content"));
		text.SetColor(Color.White);
		text = TextWidget.Cast(m_parentShopCategorie_Ammo.FindAnyWidget("Content"));
		text.SetColor(Color.Gray25);
		text = TextWidget.Cast(m_parentShopCategorie_Weapons.FindAnyWidget("Content"));
		text.SetColor(Color.White);
		
		ClearItemList();
		AddItemsFromConfig("{01BEF9B6FC671AF0}Configs/Shop/ShopThrowableItemlList.conf");
	}
	
	//ShopCategorie_Gadget
	void OnClickGadgetCategory()
	{
		TextWidget text = TextWidget.Cast(m_parentShopCategorie_Gadget.FindAnyWidget("Content"));
		text.SetColor(Color.Gray25);
		text = TextWidget.Cast(m_parentShopCategorie_Ammo.FindAnyWidget("Content"));
		text.SetColor(Color.White);
		text = TextWidget.Cast(m_parentShopCategorie_Weapons.FindAnyWidget("Content"));
		text.SetColor(Color.White);
		//m_itemList = "{01BEF9B6FC671AF0}Configs/Shop/ShopThrowableItemlList.conf";
	}
	
	void TestRpcBuySucceed()
	{
		TextWidget contentContainer = TextWidget.Cast(m_wRoot.FindAnyWidget("NameContainer"));
		//contentContainer.SetText("TestRpcBuySucceed");
	}

	void TestRpcBuyFailed()
	{
		NotifErrorShop();
	}

	protected void OnClickBuyOnce(SCR_ButtonBaseComponent button)
	{
		Widget row = button.GetRootWidget().GetParent().GetParent().GetParent();
		TextWidget priceOnceWidget = TextWidget.Cast(row.FindAnyWidget("PriceOnceText"));
		TextWidget configItemIndexWidget = TextWidget.Cast(row.FindAnyWidget("ConfigItemIndex"));

		PlayerController controller = GetGame().GetPlayerController();
		IEntity cont = controller.GetControlledEntity();
		KOTH_SCR_PlayerShopComponent kothPlayerComp = KOTH_SCR_PlayerShopComponent.Cast(controller.FindComponent(KOTH_SCR_PlayerShopComponent));

		int price = priceOnceWidget.GetText().ToInt();
		kothPlayerComp.DoRpcBuy(configItemIndexWidget.GetText().ToInt(), controller.GetPlayerId());

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
	
	//informe player error in buy
	void NotifErrorShop()
	{
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_scrollListshop = VerticalLayoutWidget.Cast(root.FindWidget("Overlay.VerticalLayout1.ScrollList.NotifContainer"));

		Widget w = GetGame().GetWorkspace().CreateWidgets("{993FD33C936EB12C}UI/Layouts/HUD/KingOfTheHill/KOTH_ShopNotification.layout", koth_scrollListshop);

		TextWidget Seconde = TextWidget.Cast(w.FindAnyWidget("Seconde"));
		Seconde.SetText("You can't buy the weapon");

		TextWidget Third = TextWidget.Cast(w.FindAnyWidget("Third"));
		Third.SetText("your inventory are full");

		SCR_FadeUIComponent compFade = SCR_FadeUIComponent.Cast(w.FindHandler(SCR_FadeUIComponent));
		compFade.DelayedFadeOut(10000, true);
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
