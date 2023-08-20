class KOTH_HUD : SCR_InfoDisplay
{
	TextWidget m_blueforPointsText;
	TextWidget m_greenforPointsText;
	TextWidget m_redforPointsText;
	
	TextWidget m_blueforPlayersText;
	TextWidget m_greenforPlayersText;
	TextWidget m_redforPlayersText;
	
	TextWidget m_moneyText;
	TextWidget m_lvlText;
	TextWidget m_xpText;
	SCR_WLibProgressBarComponent m_xpProgressBar;

	KOTH_ScoringGameModeComponent m_scoreComp;
	string m_playerName;
	
	float timeUpdateNetwork = 0;

	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		Widget root = GetRootWidget();

		if (root) {
	        OverlayWidget koth_hub = OverlayWidget.Cast(root.FindWidget("OverlayRoot.VerticalLayoutRoot.HorizontalLayoutRoot.KOTH_HUD"));
			if (!koth_hub)
				return;
			
			// points
	        m_blueforPointsText = TextWidget.Cast(koth_hub.FindWidget("Front.CountPoint_Footer.BlueforPoints"));
			m_greenforPointsText = TextWidget.Cast(koth_hub.FindWidget("Front.CountPoint_Footer.GreenforPoints"));
			m_redforPointsText = TextWidget.Cast(koth_hub.FindWidget("Front.CountPoint_Footer.RedforPoints"));

			// players
			m_blueforPlayersText = TextWidget.Cast(koth_hub.FindWidget("Front.CountPlayer_Footer.BlueforPlayers"));
			m_greenforPlayersText = TextWidget.Cast(koth_hub.FindWidget("Front.CountPlayer_Footer.GreenforPlayers"));
			m_redforPlayersText = TextWidget.Cast(koth_hub.FindWidget("Front.CountPlayer_Footer.RedforPlayers"));
			
			// money/xp/lvl
			SizeLayoutWidget m_xpSizeLayout = SizeLayoutWidget.Cast(koth_hub.FindWidget("Back.HorizontalLayoutXpLVL.ProgressBar_EXP"));			
			m_xpProgressBar = SCR_WLibProgressBarComponent.Cast(m_xpSizeLayout.FindHandler(SCR_WLibProgressBarComponent));
			
			m_xpText = TextWidget.Cast(koth_hub.FindWidget("Front.EXPERIENCE_Footer.Exp"));
			m_lvlText = TextWidget.Cast(koth_hub.FindWidget("Demi_Front.Demi_EXPERIENCE_Footer.Level"));
			m_moneyText = TextWidget.Cast(koth_hub.FindWidget("Back.HorizontalLayout2.Money"));
			
			SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			m_scoreComp = KOTH_ScoringGameModeComponent.Cast(gameMode.FindComponent(KOTH_ScoringGameModeComponent));
			PlayerManager playerManager = GetGame().GetPlayerManager();
			PlayerController controller = GetGame().GetPlayerController();
			int playerId = controller.GetPlayerId();
			m_playerName = playerManager.GetPlayerName(playerId);
			
			UpdateMoneyAndXp();
		}
		
	}
	
	void NotifEnemyKill()
	{
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_scrollList = VerticalLayoutWidget.Cast(root.FindWidget("OverlayRoot.VerticalLayoutRoot.ScrollList.NotifContainer"));
		
		Widget w = GetGame().GetWorkspace().CreateWidgets("{74686613FDE00759}UI/Layouts/HUD/KingOfTheHill/KOTH_Notification.layout", koth_scrollList);
		
		TextWidget TextNotif = TextWidget.Cast(w.FindAnyWidget("TextNotif"));
		TextNotif.SetText("Enemy killed");
		
		TextWidget XpNotif = TextWidget.Cast(w.FindAnyWidget("XpNotif"));
		XpNotif.SetText("100 xp");
		
		TextWidget MoneyNotif = TextWidget.Cast(w.FindAnyWidget("MoneyNotif"));
		MoneyNotif.SetText("100 $");
		
		SCR_FadeUIComponent compFade = SCR_FadeUIComponent.Cast(w.FindHandler(SCR_FadeUIComponent));
		compFade.DelayedFadeOut(5000, true);
	}
	
	void NotifFriendlyKill()
	{
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_scrollList = VerticalLayoutWidget.Cast(root.FindWidget("OverlayRoot.VerticalLayoutRoot.ScrollList.NotifContainer"));
		
		Widget w = GetGame().GetWorkspace().CreateWidgets("{74686613FDE00759}UI/Layouts/HUD/KingOfTheHill/KOTH_Notification.layout", koth_scrollList);
		
		TextWidget TextNotif = TextWidget.Cast(w.FindAnyWidget("TextNotif"));
		TextNotif.SetText("Friendly killed");
		
		TextWidget XpNotif = TextWidget.Cast(w.FindAnyWidget("XpNotif"));
		XpNotif.SetText("- 300 xp");
		XpNotif.SetColor(Color.DarkRed);
		
		TextWidget MoneyNotif = TextWidget.Cast(w.FindAnyWidget("MoneyNotif"));
		MoneyNotif.SetText("- 300 $");
		MoneyNotif.SetColor(Color.Red);
		
		SCR_FadeUIComponent compFade = SCR_FadeUIComponent.Cast(w.FindHandler(SCR_FadeUIComponent));
		compFade.DelayedFadeOut(2000, true);
	}
	
	void NotifBuy(int amount)
	{
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_scrollList = VerticalLayoutWidget.Cast(root.FindWidget("OverlayRoot.VerticalLayoutRoot.ScrollList.NotifContainer"));
		
		Widget w = GetGame().GetWorkspace().CreateWidgets("{74686613FDE00759}UI/Layouts/HUD/KingOfTheHill/KOTH_Notification.layout", koth_scrollList);
		
		TextWidget TextNotif = TextWidget.Cast(w.FindAnyWidget("TextNotif"));
		TextNotif.SetText("Baguette tax");
		
		TextWidget XpNotif = TextWidget.Cast(w.FindAnyWidget("XpNotif"));
		XpNotif.SetVisible(false);
		
		TextWidget MoneyNotif = TextWidget.Cast(w.FindAnyWidget("MoneyNotif"));
		MoneyNotif.SetText("—"+amount+"$");
		MoneyNotif.SetColor(Color.Red);
		
		SCR_FadeUIComponent compFade = SCR_FadeUIComponent.Cast(w.FindHandler(SCR_FadeUIComponent));
		compFade.DelayedFadeOut(5000, true);
	}
	
	void NotifCapture()
	{
		Widget root = GetRootWidget();
		VerticalLayoutWidget koth_scrollList = VerticalLayoutWidget.Cast(root.FindWidget("OverlayRoot.VerticalLayoutRoot.ScrollList.NotifContainer"));
		
		Widget w = GetGame().GetWorkspace().CreateWidgets("{74686613FDE00759}UI/Layouts/HUD/KingOfTheHill/KOTH_Notification.layout", koth_scrollList);
		
		TextWidget TextNotif = TextWidget.Cast(w.FindAnyWidget("TextNotif"));
		TextNotif.SetText("Objectif offensive");
		
		TextWidget XpNotif = TextWidget.Cast(w.FindAnyWidget("XpNotif"));
		XpNotif.SetText("10 xp");
		
		TextWidget MoneyNotif = TextWidget.Cast(w.FindAnyWidget("MoneyNotif"));
		MoneyNotif.SetText("10 $");
		
		SCR_FadeUIComponent compFade = SCR_FadeUIComponent.Cast(w.FindHandler(SCR_FadeUIComponent));
		compFade.DelayedFadeOut(2000, true);
	}
	
	private void UpdateMoneyAndXp()
	{
		KOTH_PlayerProfileJson currentProfile = new KOTH_PlayerProfileJson();
		foreach (KOTH_PlayerProfileJson savedProfile : m_scoreComp.m_listPlayerProfiles) 
		{
			if (savedProfile.m_name == m_playerName) {
				currentProfile = savedProfile;
			}
		}
		
		m_moneyText.SetText(currentProfile.GetMoney().ToString() + " $");
		m_xpText.SetText(currentProfile.GetXp().ToString() + " / " + currentProfile.GetXpNextLevel().ToString());
		m_lvlText.SetText(currentProfile.GetLevel().ToString());
		m_xpProgressBar.SetValue(currentProfile.GetXp() / currentProfile.GetXpNextLevel(),true);
	}
	
	protected override event void UpdateValues(IEntity owner, float timeSlice)
	{
		super.UpdateValues(owner, timeSlice);

		// money/xp
		timeUpdateNetwork = timeUpdateNetwork + timeSlice;
		if (timeUpdateNetwork > 1) {			
			UpdateMoneyAndXp();
		} 
		
		// teamPoints
		if (!m_scoreComp) {
			Log("Missing KOTH_ScoringGameModeComponent on gameMode", LogLevel.FATAL);
			return;
		}
		m_blueforPointsText.SetText(m_scoreComp.m_blueforPoints.ToString());
		m_greenforPointsText.SetText(m_scoreComp.m_greenforPoints.ToString());
		m_redforPointsText.SetText(m_scoreComp.m_redforPoints.ToString());
		
		// playerCounts
		array<int> playerIds = new array<int>();
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
		playerManager.GetPlayers(playerIds);
		
		int countBluefor;
		int countGreenfor;
		int countRedfor;
		
		foreach (int playerId : playerIds) 
		{
			IEntity entity = playerManager.GetPlayerControlledEntity(playerId);
			if (entity) {
				FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
				if (targetFactionComp) {
					Faction faction = targetFactionComp.GetAffiliatedFaction();
					if (faction) {
						if (faction.GetFactionName() == "BLUFOR") {
							countBluefor++;
						}
						if (faction.GetFactionName() == "OPFOR") {
							countRedfor++;
						}
						if (faction.GetFactionName() == "INDFOR") {
							countGreenfor++;
						}
					}
				}
			}
		}
		
		m_blueforPlayersText.SetText(countBluefor.ToString());
		m_greenforPlayersText.SetText(countGreenfor.ToString());
		m_redforPlayersText.SetText(countRedfor.ToString());
	}
}