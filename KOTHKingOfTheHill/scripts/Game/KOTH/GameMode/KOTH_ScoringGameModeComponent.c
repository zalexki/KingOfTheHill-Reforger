class KOTH_ScoringGameModeComponentClass : SCR_BaseGameModeComponentClass {}
class KOTH_ScoringGameModeComponent : SCR_BaseGameModeComponent
{
	[RplProp()]
	protected int m_blueforPoints = 0;
	int GetBlueforPoint() { return m_blueforPoints; }
	void AddBlueforPoint(int i = 0) { 
		if (i == 0) {
			m_blueforPoints++; 
		} else {
			m_blueforPoints = m_blueforPoints + i;
		}
	}
	void RemoveBlueforPoint(int i = 0) 
	{ 
		if (i == 0) {
			m_blueforPoints--; 
		} else {
			m_blueforPoints = m_blueforPoints - i;
		}
		
		if (m_blueforPoints < 0)
			m_blueforPoints = 0;
	}
	
	[RplProp()]
	protected int m_redforPoints = 0;
	int GetRedforPoint() { return m_redforPoints; }
	void AddRedforPoint(int i = 0) { 
		if (i == 0) {
			m_redforPoints++; 
		} else {
			m_redforPoints = m_redforPoints + i;
		}
	}
	void RemoveRedforPoint(int i = 0) 
	{ 
		if (i == 0) {
			m_redforPoints--; 
		} else {
			m_redforPoints = m_redforPoints - i;
		}
		
		if (m_redforPoints < 0)
			m_redforPoints = 0;
	}
	
	[RplProp()]
	protected int m_greenforPoints = 0;
	int GetGreenforPoint() { return m_greenforPoints; }
	void AddGreenforPoint(int i = 0) { 
		if (i == 0) {
			m_greenforPoints++; 
		} else {
			m_greenforPoints = m_greenforPoints + i;
		}
	}
	void RemoveGreenforPoint(int i = 0) 
	{ 
		if (i == 0) {
			m_greenforPoints--; 
		} else {
			m_greenforPoints = m_greenforPoints - i;
		}
		
		if (m_greenforPoints < 0)
			m_greenforPoints = 0;
	}
	
	[RplProp()]
	protected int m_bluePlayers = 0;
	int GetBluePlayers() { return m_bluePlayers; }
	
	[RplProp()]
	protected int m_redPlayers = 0;
	int GetRedPlayers() { return m_redPlayers; }
	
	[RplProp()]
	protected int m_greenPlayers = 0;
	int GetGreenPlayers() { return m_greenPlayers; }

	[RplProp()]	
	int m_blueBonus;
	[RplProp()]
	int m_redBonus;
	[RplProp()]
	int m_greenBonus;
	
	int m_bluforArmedVehiclesCount = 0;
	int m_opforArmedVehiclesCount = 0;
	int m_indforArmedVehiclesCount = 0;
	
	KOTH_PlayerProfileManagerGameModeComponent m_playerProfileManager;

	override void OnGameModeStart()
	{
		super.OnGameModeStart();

		if (!Replication.IsServer())
			return;
		
		GetGame().GetCallqueue().CallLater(UpdatePlayerCount, 5000, true);
		
		m_playerProfileManager = KOTH_PlayerProfileManagerGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_PlayerProfileManagerGameModeComponent));
	}
	
	void OnBeforeGameEnd()
	{
		// add end game bonus xp / money
		int base = 10000;	
		float blueFactor = m_blueforPoints / 100;
		float blueWinOrNot = 0.5;
		if (m_blueforPoints == 100) { blueWinOrNot = 1; }
		float blueBonusFloat = base * blueFactor * blueWinOrNot;
		m_blueBonus = blueBonusFloat.ToString(lenDec: 0).ToInt();

		float redFactor = m_redforPoints / 100;
		float redWinOrNot = 0.5;
		if (m_redforPoints == 100) { redWinOrNot = 1; }
		float redBonusFloat = base * redFactor * redWinOrNot;
		m_redBonus = redBonusFloat.ToString(lenDec: 0).ToInt();
		
		float greenFactor = m_greenforPoints / 100;
		float greenWinOrNot = 0.5;
		if (m_greenforPoints == 100) { greenWinOrNot = 1; }
		float greenBonusFloat = base * greenFactor * greenWinOrNot;
		m_greenBonus = greenBonusFloat.ToString(lenDec: 0).ToInt();
		
		array<int> playerIds = new array<int>();
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;

		playerManager.GetPlayers(playerIds);

		foreach (int playerId : playerIds)
		{
			IEntity controlledEntity = playerManager.GetPlayerControlledEntity(playerId);
			string playerUID = GetGame().GetBackendApi().GetPlayerUID(playerId);
			
			if (controlledEntity) {
				FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(controlledEntity.FindComponent(FactionAffiliationComponent));
				if (targetFactionComp) {
					Faction faction = targetFactionComp.GetAffiliatedFaction();
					if (faction) {
						int bonus;
						if (faction.GetFactionName() == KOTH_Faction.BLUFOR)
							bonus = m_blueBonus;
							
						if (faction.GetFactionName() == KOTH_Faction.OPFOR)
							bonus = m_redBonus;

						if (faction.GetFactionName() == KOTH_Faction.INDFOR)
							bonus = m_greenBonus;
						
						
						foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
						{
							if (savedProfile.m_playerId == playerId) {
								Rpc(RpcDo_Notif_EndGameBonus, playerId, bonus);
								savedProfile.AddEndGameBonusXpAndMoney(bonus);
								m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
							}
						}
						Replication.BumpMe();
					}
				}
			}
		}
	}
	
	// should only be server side
	void Refund(int price, int playerId)
	{
		foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
		{
			if (savedProfile.m_playerId == playerId) {
				savedProfile.Refund(price);
				m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
			}
		}

		Replication.BumpMe();
	}

	// should only be server side
	bool TryBuy(int price, int playerId)
	{
		bool hasEnoughMoney = true;
		foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
		{
			if (savedProfile.m_playerId == playerId) {
				if (price > savedProfile.GetMoney()) {
					hasEnoughMoney = false;
					break;
				}
				
				savedProfile.Buy(price);
				m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
			}
		}
		
		if (hasEnoughMoney)
			Replication.BumpMe();
		
		return hasEnoughMoney;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_Notif_EndGameBonus(int playerId, int bonus)
	{
		if (GetGame().GetPlayerController().GetPlayerId() != playerId)
			return;

		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.EndGameBonus(bonus.ToString());
			}
		}
	}
	
	void UpdatePlayerCount()
	{
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
						if (faction.GetFactionName() == KOTH_Faction.BLUFOR)
							countBluefor++;
						
						if (faction.GetFactionName() == KOTH_Faction.OPFOR)
							countRedfor++;
						
						if (faction.GetFactionName() == KOTH_Faction.INDFOR)
							countGreenfor++;
					}
				}
			}
		}
		
		m_bluePlayers = countBluefor;
		m_redPlayers = countRedfor;
		m_greenPlayers = countGreenfor;
		
		Replication.BumpMe();
	}
	
	void BumpMe()
	{
		Replication.BumpMe();
	}
}
