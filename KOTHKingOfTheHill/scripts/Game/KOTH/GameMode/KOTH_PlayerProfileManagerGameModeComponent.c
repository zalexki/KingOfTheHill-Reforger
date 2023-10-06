class KOTH_PlayerProfileManagerGameModeComponentClass : SCR_BaseGameModeComponentClass {}
class KOTH_PlayerProfileManagerGameModeComponent : SCR_BaseGameModeComponent
{
	const string saveFilePath = "$profile:koth_profiles.json";
	ref KOTH_ListPlayerProfileJson listPlayerProfilesJson = new KOTH_ListPlayerProfileJson();

	ref array<ref KOTH_PlayerProfileJson> m_listPlayerProfiles = new array<ref KOTH_PlayerProfileJson>();

	override void OnGameModeStart()
	{
		super.OnGameModeStart();

		if (!Replication.IsServer())
			return;

		if (FileIO.FileExists(saveFilePath)) {
			listPlayerProfilesJson.LoadFromFile(saveFilePath);
			Log(" ---------------- saveFile loaded");
		} else {
			listPlayerProfilesJson.SaveToFile(saveFilePath);
			Log(" ---------------- saveFile saved");
		}

		m_listPlayerProfiles = listPlayerProfilesJson.m_list;

		GetGame().GetCallqueue().CallLater(SavePlayersProfile, 30000, true);
	}
	
	void SavePlayersProfile()
	{
		if (!Replication.IsServer())
			return;

		listPlayerProfilesJson.m_list = m_listPlayerProfiles;

		bool success = listPlayerProfilesJson.SaveToFile(saveFilePath);
		Log("SavePlayersProfile is ok = " + success);
	}
	
	override void OnPlayerConnected(int playerId)
	{
		if (!Replication.IsServer())
			return;

		Log("OnPlayerConnected "+playerId);
		
		GetGame().GetCallqueue().CallLater(MapPlayerUIDToPlayerId, 1000, false, playerId);
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (playerController)
		{
			SCR_PlayerFactionAffiliationComponent playerFactionAff = SCR_PlayerFactionAffiliationComponent.Cast(playerController.FindComponent(SCR_PlayerFactionAffiliationComponent));
			if (playerFactionAff)
			{
				playerFactionAff.GetOnPlayerFactionResponseInvoker_S().Insert(OnPlayerFactionResponse_S);
			}
		}
	}
	
	protected void OnPlayerFactionResponse_S(SCR_PlayerFactionAffiliationComponent component, int factionIndex, bool response)
	{
		if (!Replication.IsServer())
			return;
		if (factionIndex == -1)
			return;
		
		KOTH_ScoringGameModeComponent m_scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
		KOTH_SCR_PlayerProfileComponent profileComp = KOTH_SCR_PlayerProfileComponent.Cast(component.GetPlayerController().FindComponent(KOTH_SCR_PlayerProfileComponent));
		FactionManager factionManager = GetGame().GetFactionManager();
		Faction faction = factionManager.GetFactionByIndex(factionIndex);
		
		switch (faction.GetFactionKey())
		{
			case KOTH_Faction.BLUFOR:
				profileComp.SetSessionPointsWhenFactionWasJoined(m_scoreComp.GetBlueforPoint());
			break;
			case KOTH_Faction.OPFOR:
				profileComp.SetSessionPointsWhenFactionWasJoined(m_scoreComp.GetRedforPoint());
			break;
			case KOTH_Faction.INDFOR:
				profileComp.SetSessionPointsWhenFactionWasJoined(m_scoreComp.GetGreenforPoint());
			break;
		}
	}
	
	void MapPlayerUIDToPlayerId(int playerId)
	{
		bool isInList = false;
		string playerUID = GetGame().GetBackendApi().GetPlayerUID(playerId);
		
		if (SCR_Global.IsEditMode()) {
			playerUID = GetGame().GetPlayerManager().GetPlayerName(playerId);
		}
		
		if (playerUID == string.Empty)
		{
			Log("empty playerUID for "+playerId);
			GetGame().GetCallqueue().CallLater(MapPlayerUIDToPlayerId, 1000, false, playerId);
			return;
		}
		
		foreach (int index, KOTH_PlayerProfileJson profile : m_listPlayerProfiles)
		{
			if (profile.m_playerUID == playerUID)
			{
				profile.m_playerId = playerId;
				
				Log("found playerId "+playerId+" is now "+playerUID);
				m_listPlayerProfiles.Set(index, profile);
				isInList = true;
			}
		}
		
		if (!isInList) {
			KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
			profile.m_playerUID = playerUID;
			profile.m_playerId = playerId;
			profile.m_playerName = GetGame().GetPlayerManager().GetPlayerName(playerId);
			m_listPlayerProfiles.Insert(profile);
			Log("added playerId "+playerId+" to "+playerUID);
		}
	}
	
	override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		if (!Replication.IsServer())
			return;
	
		foreach (int index, KOTH_PlayerProfileJson profile : m_listPlayerProfiles)
		{
			if (profile.m_playerId == playerId)
			{
				profile.m_playerId = -1;
				m_listPlayerProfiles.Set(index, profile);
			}
		}
	}
	
	override bool HandlePlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		if (!Replication.IsServer())
			return false;
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return false;

		string playerName = playerManager.GetPlayerName(playerId);
		string playerUID = GetGame().GetBackendApi().GetPlayerUID(playerId);
		FactionAffiliationComponent playerFactionComp = FactionAffiliationComponent.Cast(player.FindComponent(FactionAffiliationComponent));
		bool playerIsInList = false;
		
		int killerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(killer);
		PlayerController killerController = GetGame().GetPlayerManager().GetPlayerController(killerId);
		FactionAffiliationComponent killerFactionComp = FactionAffiliationComponent.Cast(killer.FindComponent(FactionAffiliationComponent));
		KOTH_SCR_PlayerProfileComponent killerProfileComp = KOTH_SCR_PlayerProfileComponent.Cast(killerController.FindComponent(KOTH_SCR_PlayerProfileComponent));		
		
		string killerUID = GetGame().GetBackendApi().GetPlayerUID(killerId);
		string killerName = playerManager.GetPlayerName(killerId);
		bool killerIsInList = false;
		
		if (playerFactionComp.GetAffiliatedFaction() == killerFactionComp.GetAffiliatedFaction()) {
			Log("teamkill killer = "+killerName+" player = "+playerName);
			Log("teamkill killerUID = "+killerUID+" playerUID = "+playerUID);
			// teamkill
			if (killerUID == playerUID)
                return true;
			
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == killerId && savedProfile.m_playerUID == killerUID) {
					savedProfile.RemoveFriendlyKillXpAndMoney();
					m_listPlayerProfiles.Set(index, savedProfile);
					killerIsInList = true;
				}
			}

			killerProfileComp.AddSessionFriendlyKillXpAndMoney();
			killerProfileComp.DoRpc_Notif_FriendlyKill();

			if (!killerIsInList) {
				KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
				profile.RemoveFriendlyKillXpAndMoney();
				profile.m_playerName = killerName;
				profile.m_playerId = killerId;
				m_listPlayerProfiles.Insert(profile);
			}
		} 
		else 
		{
			// kill
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId && savedProfile.m_playerUID == playerUID) {
					savedProfile.m_deaths++;
					m_listPlayerProfiles.Set(index, savedProfile);
					playerIsInList = true;
				}

				if (savedProfile.m_playerId == killerId && savedProfile.m_playerUID == killerUID) {
					savedProfile.AddKillXpAndMoney();
					m_listPlayerProfiles.Set(index, savedProfile);
					killerIsInList = true;
				}
			}

			killerProfileComp.AddSessionKillXpAndMoney();
			killerProfileComp.DoRpc_Notif_EnemyKill();

			if (!killerIsInList) {
				KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
				profile.AddKillXpAndMoney();
				profile.m_playerName = killerName;
				profile.m_playerId = killerId;
				m_listPlayerProfiles.Insert(profile);
			}

			if (!playerIsInList) {
				KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
				profile.m_deaths++;
				profile.m_playerName = playerName;
				profile.m_playerId = playerId;
				m_listPlayerProfiles.Insert(profile);
			}
		}

		return true;
	}
	
	override void OnGameEnd()
	{
		if (!Replication.IsServer())
			return;
		
		Log("OnGameEnd launch SavePlayersProfile()");
		SavePlayersProfile();
	}
}
