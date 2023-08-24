class KOTH_ShopGunClass : ChimeraMenuBase
{
	private Widget m_wRoot;
	private SCR_PlayerController m_playerController;
	private ItemPreviewManagerEntity m_PreviewManager;
	private KOTH_ScoringGameModeComponent m_scoreComp;
	private string m_playerUID;
	
	override void OnMenuInit()
	{
		super.OnMenuInit();
		m_PreviewManager = GetGame().GetItemPreviewManager();
		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
		m_playerUID = GetGame().GetBackendApi().GetPlayerUID(GetGame().GetPlayerController().GetPlayerId());
	}

	void TestRpcBuySucceed()
	{
		TextWidget contentContainer = TextWidget.Cast(m_wRoot.FindAnyWidget("NameContainer"));
		contentContainer.SetText("TestRpcBuySucceed");
	}

	void TestRpcBuyFailed()
	{
		TextWidget contentContainer = TextWidget.Cast(m_wRoot.FindAnyWidget("NameContainer"));
		contentContainer.SetText("TestRpcBuyFailed");
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

		AddItemsFromConfig();
	}

	protected void AddItemsFromConfig()
	{
		KOTH_SCR_ShopGunItemList itemList = SCR_ConfigHelperT<KOTH_SCR_ShopGunItemList>.GetConfigObject("{232D181B9F9FE8D1}Configs/ShopGunItemList.conf");
		VerticalLayoutWidget contentContainer = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("ContentContainer"));

		foreach (int index, KOTH_SCR_ShopGunItem item : itemList.GetItems())
		{
			Widget newRow = GetGame().GetWorkspace().CreateWidgets("{20EF71DE68A5887E}UI/Layouts/HUD/Shop/ShopGun_ItemList.layout", contentContainer);
			
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
			priceOnceWidget.SetText(item.m_priceOnce.ToString());
			TextWidget pricePermWidget = TextWidget.Cast(newRow.FindAnyWidget("PricePermText"));
			pricePermWidget.SetText(item.m_pricePermanent.ToString());

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
				ImageWidget FadeLVLReq = ImageWidget .Cast(newRow.FindAnyWidget("FadeLVLReq"));
				TextWidget TextLevelReq = TextWidget.Cast(newRow.FindAnyWidget("TextLevelReq"));
				
				FadeLVLReq.SetVisible(true);
				TextLevelReq.SetVisible(true);
				TextLevelReq.SetText("LEVEL " + item.m_level.ToString() + " REQUIRED");
			}
			
			
			// set visible or not buy
			
			// TODO: can buy from current money
			ButtonWidget purchaseOnceButton = ButtonWidget.Cast(newRow.FindAnyWidget("PurchaseOnceButton"));
			OverlayWidget purchaseOnceHINT = OverlayWidget.Cast(newRow.FindAnyWidget("PurchaseOnceHINT"));
			purchaseOnceButton.SetVisible(true);
			purchaseOnceHINT.SetVisible(false);

			// add item preview
			ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(newRow.FindAnyWidget("Preview_Image"));
			if (wRenderTarget && m_PreviewManager)
			{
				Log("add to shop" + item.m_itemResource);
				m_PreviewManager.SetPreviewItemFromPrefab(wRenderTarget, item.m_itemResource, null, false);
			}
		}
	}

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

		// TODO: notif from server after purchase validated
		HUD_NotifBuy(price);
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

	private void HUD_NotifBuy(int amount)
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
