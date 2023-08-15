class KOTH_HUD : SCR_InfoDisplay
{
	TextWidget m_blueforPointsText;
	TextWidget m_greenforPointsText;
	TextWidget m_redforPointsText;
	
	TextWidget m_blueforPlayersText;
	TextWidget m_greenforPlayersText;
	TextWidget m_redforPlayersText;
	
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		Widget root = GetRootWidget();

		if (root) {
	        FrameWidget koth_hub = FrameWidget.Cast(root.FindWidget("KOTH_HUD"));
			if (!koth_hub)
				return;
			
	        m_blueforPointsText = TextWidget.Cast(koth_hub.FindWidget("VerticalLayout0.CountPoint_Footer.BlueforPoints"));
			m_greenforPointsText = TextWidget.Cast(koth_hub.FindWidget("VerticalLayout0.CountPoint_Footer.GreenforPoints"));
			m_redforPointsText = TextWidget.Cast(koth_hub.FindWidget("VerticalLayout0.CountPoint_Footer.RedforPoints"));

			m_blueforPointsText.SetText("0");
			m_greenforPointsText.SetText("0");
			m_redforPointsText.SetText("0");

			m_blueforPlayersText = TextWidget.Cast(koth_hub.FindWidget("VerticalLayout0.CountPlayer_Footer.BluforPlayers"));
			m_greenforPlayersText = TextWidget.Cast(koth_hub.FindWidget("VerticalLayout0.CountPlayer_Footer.GreenforPlayers"));
			m_redforPlayersText = TextWidget.Cast(koth_hub.FindWidget("VerticalLayout0.CountPlayer_Footer.RedforPlayers"));

			m_blueforPlayersText.SetText("0");
			m_greenforPlayersText.SetText("0");
			m_redforPlayersText.SetText("0");
		}
	}
	
	protected override event void UpdateValues(IEntity owner, float timeSlice)
	{
		super.UpdateValues(owner, timeSlice);
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return;
		
		m_blueforPointsText.SetText(gameMode.m_blueforPoints.ToString());
		m_greenforPointsText.SetText(gameMode.m_greenforPoints.ToString());
		m_redforPointsText.SetText(gameMode.m_redforPoints.ToString());
		
		// ----------------
		
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