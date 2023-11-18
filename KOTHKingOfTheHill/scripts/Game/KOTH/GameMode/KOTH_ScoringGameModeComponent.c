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
	
	// compute team and playtime bonus for each players
	void OnBeforeGameEnd()
	{
		float blueFactor = m_blueforPoints / SCR_BaseGameMode.WINNER_POINTS_NEEDED;
		float redFactor = m_redforPoints / SCR_BaseGameMode.WINNER_POINTS_NEEDED;
		float greenFactor = m_greenforPoints / SCR_BaseGameMode.WINNER_POINTS_NEEDED;
		
		array<int> playerIds = new array<int>();
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;

		playerManager.GetPlayers(playerIds);

		foreach (int playerId : playerIds)
		{
			IEntity controlledEntity = playerManager.GetPlayerControlledEntity(playerId);
			string playerUID = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
			KOTH_SCR_PlayerProfileComponent profileComp = KOTH_SCR_PlayerProfileComponent.Cast(playerManager.GetPlayerController(playerId).FindComponent(KOTH_SCR_PlayerProfileComponent));
			
			if (controlledEntity) {
				FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(controlledEntity.FindComponent(FactionAffiliationComponent));
				if (targetFactionComp) {
					Faction faction = targetFactionComp.GetAffiliatedFaction();
					if (faction) {
						float teambonus;
						float playtimebonus;
						if (faction.GetFactionName() == KOTH_Faction.BLUFOR) {
							teambonus = blueFactor * profileComp.GetSessionXpEarned() * 0.5;
							float teampointsfactor = (m_blueforPoints - profileComp.GetSessionPointsWhenFactionWasJoined()) / SCR_BaseGameMode.WINNER_POINTS_NEEDED;
							playtimebonus = blueFactor * teampointsfactor * 0.5;
						}
							
						if (faction.GetFactionName() == KOTH_Faction.OPFOR) {
							teambonus = redFactor * profileComp.GetSessionXpEarned() * 0.5;
							float teampointsfactor = (m_redforPoints - profileComp.GetSessionPointsWhenFactionWasJoined()) / SCR_BaseGameMode.WINNER_POINTS_NEEDED;
							playtimebonus = redFactor * teampointsfactor * 0.5;
						}

						if (faction.GetFactionName() == KOTH_Faction.INDFOR) {
							teambonus = greenFactor * profileComp.GetSessionXpEarned() * 0.5;
							float teampointsfactor = (m_greenforPoints - profileComp.GetSessionPointsWhenFactionWasJoined()) / SCR_BaseGameMode.WINNER_POINTS_NEEDED;
							playtimebonus = greenFactor * teampointsfactor * 0.5;
						}
						 
						int bonus = (playtimebonus + teambonus).ToString(lenDec: 0).ToInt();
						
						foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
						{
							if (savedProfile.m_playerId == playerId) {
								profileComp.DoRpc_SetEndGameBonus(bonus);
								savedProfile.AddEndGameBonusXpAndMoney(bonus);
								m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
								break;
							}
						}
					}
				}
			}
		}
	}
	
	// should only be server side
	void Refund(int price, int playerId)
	{
		string playerUID = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
		{
			if (savedProfile.m_playerId == playerId) {
				savedProfile.Refund(price);
				m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
				
				PlayerManager playerManager = GetGame().GetPlayerManager();
				PlayerController playerController = playerManager.GetPlayerController(playerId);
				KOTH_SCR_PlayerProfileComponent profileComp = KOTH_SCR_PlayerProfileComponent.Cast(playerController.FindComponent(KOTH_SCR_PlayerProfileComponent));
				profileComp.DoRpc_SyncPlayerProfile(savedProfile);
				break;
			}
		}
	}

	// should only be server side
	bool TryBuy(int price, int playerId)
	{
		string playerUID = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
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
				
				PlayerManager playerManager = GetGame().GetPlayerManager();
				PlayerController playerController = playerManager.GetPlayerController(playerId);
				KOTH_SCR_PlayerProfileComponent profileComp = KOTH_SCR_PlayerProfileComponent.Cast(playerController.FindComponent(KOTH_SCR_PlayerProfileComponent));
				profileComp.DoRpc_SyncPlayerProfile(savedProfile);
				break;
			}
		}
		
		return hasEnoughMoney;
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
	
	bool PlayersCanChangeFaction()
	{
		if (m_redforPoints > 85 || m_greenforPoints > 85 || m_blueforPoints > 85)
			return false;
		
		return true;
	}
}
