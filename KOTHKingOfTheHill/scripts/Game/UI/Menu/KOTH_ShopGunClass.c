class KOTH_ShopGunClass : ChimeraMenuBase {

	protected Widget m_wRoot;  
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_wRoot = GetRootWidget();
		SCR_NavigationButtonComponent cancel = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Cancel", m_wRoot);
		if (cancel)
		{
			cancel.m_OnActivated.Insert(OnClickEscape);
		}
		
		
		SCR_ButtonBaseComponent buy = SCR_ButtonBaseComponent.GetButtonBase("PurchaseOnceButton", m_wRoot);
		if (buy)
		{
			buy.m_OnClicked.Insert(OnClickBuy);
		}	
	

		ItemPreviewManagerEntity m_PreviewManager = GetGame().GetItemPreviewManager();
		
		ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(m_wRoot.FindAnyWidget("Preview_Image"));
		if (wRenderTarget && m_PreviewManager)
		{			
			m_PreviewManager.SetPreviewItemFromPrefab(wRenderTarget, "{FA5C25BF66A53DCF}Prefabs/Weapons/Rifles/AK74/Rifle_AK74.et", null, true);
		}
		
	}
	
	protected void OnClickBuy() 
	{
		PlayerManager playerManager = GetGame().GetPlayerManager();
		PlayerController controller = GetGame().GetPlayerController();
		//string playerName = playerManager.GetPlayerName(playerId);
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		KOTH_ScoringGameModeComponent m_scoreComp = KOTH_ScoringGameModeComponent.Cast(gameMode.FindComponent(KOTH_ScoringGameModeComponent));
		
		int amount = 3;
		//m_scoreComp.DoRpcBuy(amount, playerName);
		
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