class KOTH_ShopUI : ChimeraMenuBase
{
	protected Widget m_wRoot;
	protected TextWidget m_nameContainer;
	
	protected Widget m_parentShopCategorie_Weapons;
	protected Widget m_parentShopCategorie_Ammo;
	protected Widget m_parentShopCategorie_Gadget;
	
	protected VerticalLayoutWidget m_contentContainer;
	
	protected KOTH_SCR_PlayerShopComponent m_playerShopComp;
	protected KOTH_SCR_PlayerProfileComponent m_playerProfileComp;
	protected KOTH_ScoringGameModeComponent m_scoreComp;
	protected int m_playerId;
	
	override void OnMenuInit()
	{
		super.OnMenuInit();
		
		m_playerId = GetGame().GetPlayerController().GetPlayerId();
		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
		
		PlayerController controller = GetGame().GetPlayerController();
		int playerId = controller.GetPlayerId();
		
		m_playerShopComp = KOTH_SCR_PlayerShopComponent.Cast(controller.FindComponent(KOTH_SCR_PlayerShopComponent));
		m_playerProfileComp = KOTH_SCR_PlayerProfileComponent.Cast(controller.FindComponent(KOTH_SCR_PlayerProfileComponent));
	}
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_wRoot = GetRootWidget();
		m_nameContainer = TextWidget.Cast(m_wRoot.FindAnyWidget("NameContainer"));
		m_contentContainer = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("ContentContainer"));
		
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
		text.SetColor(Color.Gray25);
		m_parentShopCategorie_Ammo = m_wRoot.FindAnyWidget("ShopCategorie_Ammo");
		m_parentShopCategorie_Gadget = m_wRoot.FindAnyWidget("ShopCategorie_Gadget");
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
			
			SCR_ButtonBaseComponent equipButton = SCR_ButtonBaseComponent.GetButtonBase("EquipButton", newRow);
			if (equipButton)
				equipButton.m_OnClicked.Insert(OnClickEquip);

			// set text infos
			TextWidget resourceNameWidget = TextWidget.Cast(newRow.FindAnyWidget("ResourceName"));
			resourceNameWidget.SetText(item.m_itemResource);
			TextWidget nameWidget = TextWidget.Cast(newRow.FindAnyWidget("ItemName"));
			nameWidget.SetText(item.m_itemName);
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
			
			OverlayWidget desactivatePermHINT = OverlayWidget.Cast(newRow.FindAnyWidget("DesactivatePermHINT"));
			ButtonWidget purchaseOnceButton = ButtonWidget.Cast(newRow.FindAnyWidget("PurchaseOnceButton"));
			ButtonWidget purchasePermanentButton = ButtonWidget.Cast(newRow.FindAnyWidget("PurchasePermanentButton"));
			
			OverlayWidget purchaseOnceNoLVL = OverlayWidget.Cast(newRow.FindAnyWidget("PurchaseOnceNoLVL"));
			OverlayWidget purchasePermNoLVL = OverlayWidget.Cast(newRow.FindAnyWidget("PurchasePermNoLVL"));
			
			if (playerLevel < item.m_level)
			{				
				ImageWidget fadeLVLReq = ImageWidget .Cast(newRow.FindAnyWidget("FadeLVLReq"));
				TextWidget textLevelReq = TextWidget.Cast(newRow.FindAnyWidget("TextLevelReq"));
				
				purchaseOnceButton.SetVisible(false);
				purchasePermanentButton .SetVisible(false);
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
				OverlayWidget purchaseOnceHINT = OverlayWidget.Cast(newRow.FindAnyWidget("PurchaseOnceHINT"));
				purchaseOnceButton.SetVisible(true);
				purchaseOnceHINT.SetVisible(false);
				
				if (m_playerProfileComp.GetUnlockedItemList().Contains(item.m_itemResource))
				{
					purchaseOnceButton.SetVisible(false);
					equipButton.SetVisible(true);
					
					OverlayWidget purchasePermHINT = OverlayWidget.Cast(newRow.FindAnyWidget("PurchasePermHINT"));
					purchasePermanentButton.SetVisible(false);
					desactivatePermHINT.SetVisible(false);
					purchasePermHINT.SetVisible(true);
				}
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

	protected void OnClickBuyOnce(SCR_ButtonBaseComponent button)
	{
		Widget row = button.GetRootWidget().GetParent().GetParent().GetParent();
		TextWidget priceOnceWidget = TextWidget.Cast(row.FindAnyWidget("PriceOnceText"));
		TextWidget resourceNameWidget = TextWidget.Cast(row.FindAnyWidget("ResourceName"));

		m_playerShopComp.DoRpcBuy(resourceNameWidget.GetText(), m_playerId);

		// set widget equiped visible
		ButtonWidget purchaseOnceButton = ButtonWidget.Cast(row.FindAnyWidget("PurchaseOnceButton"));
		OverlayWidget purchaseOnceHINT = OverlayWidget.Cast(row.FindAnyWidget("PurchaseOnceHINT"));
		purchaseOnceButton.SetVisible(false);
		purchaseOnceHINT.SetVisible(true);
	}

	protected void OnClickBuyPermanent(SCR_ButtonBaseComponent button)
	{
		Log("not implemented yet");
		Widget row = button.GetRootWidget().GetParent().GetParent().GetParent();
		TextWidget pricePermWidget = TextWidget.Cast(row.FindAnyWidget("PricePermText"));
		TextWidget resourceNameWidget = TextWidget.Cast(row.FindAnyWidget("ResourceName"));

		m_playerShopComp.DoRpcBuy(resourceNameWidget.GetText(), m_playerId, true);
		
		// set widget owned visible
		ButtonWidget purchasePermanentButton = ButtonWidget.Cast(row.FindAnyWidget("PurchasePermanentButton"));
		OverlayWidget purchasePermHINT = OverlayWidget.Cast(row.FindAnyWidget("PurchasePermHINT"));
		purchasePermanentButton.SetVisible(false);
		purchasePermHINT.SetVisible(true);
	}
	
	protected void OnClickEquip(SCR_ButtonBaseComponent button)
	{
		Widget row = button.GetRootWidget().GetParent().GetParent().GetParent();
		TextWidget resourceNameWidget = TextWidget.Cast(row.FindAnyWidget("ResourceName"));
		m_playerShopComp.AskRpc_Equip(resourceNameWidget.GetText(), m_playerId);
		
		// set widget equiped visible
		OverlayWidget purchaseOnceHINT = OverlayWidget.Cast(row.FindAnyWidget("PurchaseOnceHINT"));
		button.SetVisible(false);
		purchaseOnceHINT.SetVisible(true);
	}
	
	
	//informe player error in buy
	void NotifErrorShop(string firstLine, string secondLine)
	{
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_scrollListshop = VerticalLayoutWidget.Cast(root.FindWidget("Overlay.VerticalLayout1.ScrollList.NotifContainer"));

		Widget w = GetGame().GetWorkspace().CreateWidgets("{993FD33C936EB12C}UI/Layouts/HUD/KingOfTheHill/KOTH_ShopNotification.layout", koth_scrollListshop);
		TextWidget Seconde = TextWidget.Cast(w.FindAnyWidget("Seconde"));
		Seconde.SetText(firstLine);

		TextWidget Third = TextWidget.Cast(w.FindAnyWidget("Third"));
		Third.SetText(secondLine);

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
	
	void NotifBuy_PermanentBuySuccess(string itemResource)
	{
		int i = 0;
		bool foundRow = false;
		Widget child = m_contentContainer.GetChildren();
		TextWidget resourceNameWidget = TextWidget.Cast(child.FindAnyWidget("ResourceName"));
		
		if (resourceNameWidget.GetText() == itemResource) {
			Widget PurchaseOnceButton = child.FindAnyWidget("PurchaseOnceButton");
			PurchaseOnceButton.SetVisible(false);
			OverlayWidget EquipButton = OverlayWidget.Cast(child.FindAnyWidget("EquipButton"));
			EquipButton.SetVisible(true);
		}
		while (!foundRow && i < 1000) 
		{
			i++;
			child = child.GetSibling();
			resourceNameWidget = TextWidget.Cast(child.FindAnyWidget("ResourceName"));
			if (resourceNameWidget.GetText() == itemResource) 
			{
				Widget PurchaseOnceButton = child.FindAnyWidget("PurchaseOnceButton");
				PurchaseOnceButton.SetVisible(false);
				Widget EquipButton = child.FindAnyWidget("EquipButton");
				EquipButton.SetVisible(true);
				foundRow = true;
			}
		}
	}

	protected void OnClickEscape()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}
}
