class KOTH_ShopGunClass : ChimeraMenuBase {

	protected Widget m_wRoot;  
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		m_wRoot = GetRootWidget();
		SCR_NavigationButtonComponent cancel = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Cancel", m_wRoot);
		if (cancel)
			cancel.m_OnActivated.Insert(OnClickEscape);
		
		SCR_NavigationButtonComponent buy = SCR_NavigationButtonComponent.GetNavigationButtonComponent("PurchaseOnceButton", m_wRoot);
		if (buy)
			buy.m_OnActivated.Insert(OnClickBuy);
	}
	
	protected void OnClickBuy() 
	{
		PlayerManager playerManager = GetGame().GetPlayerManager();
		PlayerController controller = GetGame().GetPlayerController();
		string playerName = playerManager.GetPlayerName(playerId);
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		KOTH_ScoringGameModeComponent m_scoreComp = KOTH_ScoringGameModeComponent.Cast(gameMode.FindComponent(KOTH_ScoringGameModeComponent));
		
		int amount = 3;
		m_scoreComp.DoRpcBuy(amount, playerName);
		
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