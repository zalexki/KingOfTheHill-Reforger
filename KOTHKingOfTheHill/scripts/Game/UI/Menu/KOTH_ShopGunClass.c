class KOTH_ShopGunClass : ChimeraMenuBase 
{
	protected Widget m_wRoot;  
	protected SCR_PlayerController m_playerController;
	protected ItemPreviewManagerEntity m_PreviewManager;
	
	override void OnMenuInit()
	{
		super.OnMenuInit();
		m_PreviewManager = GetGame().GetItemPreviewManager();
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
		
		foreach (KOTH_SCR_ShopGunItem item : itemList.GetItems())
		{
			Log("add item to shop " + item.m_itemName);
			Widget newRow = GetGame().GetWorkspace().CreateWidgets("{20EF71DE68A5887E}UI/Layouts/HUD/Shop/ItemListUILayout.layout", contentContainer);
			
			// add click events
			SCR_ButtonBaseComponent buyOnce = SCR_ButtonBaseComponent.GetButtonBase("PurchaseOnceButton", newRow);
			if (buyOnce)
				buyOnce.m_OnClicked.Insert(OnClickBuyOnce);

			
			SCR_ButtonBaseComponent buyPermanent = SCR_ButtonBaseComponent.GetButtonBase("PurchasePermanentButton", newRow);
			if (buyPermanent)
				buyPermanent.m_OnClicked.Insert(OnClickBuyPermanent);

			// set text infos
			TextWidget nameWidget = TextWidget.Cast(newRow.FindAnyWidget("ItemName"));
			nameWidget.SetText(item.m_itemName);
			TextWidget priceOnceWidget = TextWidget.Cast(newRow.FindAnyWidget("PriceOnceText"));
			priceOnceWidget.SetText(item.m_priceOnce.ToString());
			TextWidget pricePermWidget = TextWidget.Cast(newRow.FindAnyWidget("PricePermText"));
			pricePermWidget.SetText(item.m_pricePermanent.ToString());
			
			// add item preview
			ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(newRow.FindAnyWidget("Preview_Image"));
			if (wRenderTarget && m_PreviewManager)
			{
				Log(item.m_itemResource.GetPath());
				m_PreviewManager.SetPreviewItemFromPrefab(wRenderTarget, item.m_itemResource, null, false);
			}
		}
	}
 	
	// TODO: equip weapon,magazines ...
	protected void OnClickBuyOnce(SCR_ButtonBaseComponent button) 
	{
		Widget row = button.GetRootWidget().GetParent().GetParent();
		TextWidget priceOnceWidget = TextWidget.Cast(row.FindAnyWidget("PriceOnceText"));

		PlayerManager playerManager = GetGame().GetPlayerManager();
		PlayerController controller = GetGame().GetPlayerController();
		int playerId = controller.GetPlayerId();
		string playerName = playerManager.GetPlayerName(playerId);

		IEntity cont = controller.GetControlledEntity();
		KOTH_SCR_PlayerProfileComponent kothPlayerComp = KOTH_SCR_PlayerProfileComponent.Cast(controller.FindComponent(KOTH_SCR_PlayerProfileComponent));
		
		int amount = priceOnceWidget.GetText().ToInt();
		kothPlayerComp.DoAskRpcBuy(amount, playerName);
		
		HUD_NotifBuy(amount);
	}
	
	protected void OnClickBuyPermanent(SCR_ButtonBaseComponent button) 
	{
		Widget row = button.GetRootWidget().GetParent().GetParent();
		TextWidget priceOnceWidget = TextWidget.Cast(row.FindAnyWidget("PricePermText"));

		PlayerManager playerManager = GetGame().GetPlayerManager();
		PlayerController controller = GetGame().GetPlayerController();
		int playerId = controller.GetPlayerId();
		string playerName = playerManager.GetPlayerName(playerId);

		IEntity cont = controller.GetControlledEntity();
		KOTH_SCR_PlayerProfileComponent kothPlayerComp = KOTH_SCR_PlayerProfileComponent.Cast(controller.FindComponent(KOTH_SCR_PlayerProfileComponent));
		
		int amount = priceOnceWidget.GetText().ToInt();
		kothPlayerComp.DoAskRpcBuy(amount, playerName);
		
		HUD_NotifBuy(amount);
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
};