class KOTH_ScoringGameModeComponentClass : SCR_BaseGameModeComponentClass {}
class KOTH_ScoringGameModeComponent : SCR_BaseGameModeComponent
{
	const string saveFilePath = "$profile:koth_profiles.json";
	ref KOTH_ListPlayerProfileJson listPlayerProfilesJson = new KOTH_ListPlayerProfileJson();

	[RplProp()]
	protected int m_blueforPoints = 0;
	int GetBlueforPoint() { return m_blueforPoints; }
	void AddBlueforPoint() { m_blueforPoints++; }
	
	[RplProp()]
	protected int m_redforPoints = 0;
	int GetRedforPoint() { return m_redforPoints; }
	void AddRedforPoint() { m_redforPoints++; }
	
	[RplProp()]
	protected int m_greenforPoints = 0;
	int GetGreenforPoint() { return m_greenforPoints; }
	void AddGreenforPoint() { m_greenforPoints++; }
	
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
	string m_areaState = "none";
	
	// TODO: rethink this ugly thing
	[RplProp()]
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
		Replication.BumpMe();

		GetGame().GetCallqueue().CallLater(SavePlayersProfile, 30000, true);
		GetGame().GetCallqueue().CallLater(UpdatePlayerCount, 5000, true);
	}

	
	void OnBeforeGameEnd()
	{
		// add end game bonus xp / money
		int base = 10000;	
		float blueFactor = m_blueforPoints / 100;
		float blueWinOrNot = 0.5;
		if (m_blueforPoints == 100) { blueWinOrNot = 1; }
		float blueBonusFloat = base * blueFactor * blueWinOrNot;
		int blueBonus = blueBonusFloat.ToString(lenDec: 0).ToInt();

		float redFactor = m_redforPoints / 100;
		float redWinOrNot = 0.5;
		if (m_redforPoints == 100) { redWinOrNot = 1; }
		float redBonusFloat = base * redFactor * redWinOrNot;
		int redBonus = redBonusFloat.ToString(lenDec: 0).ToInt();
		
		float greenFactor = m_greenforPoints / 100;
		float greenWinOrNot = 0.5;
		if (m_greenforPoints == 100) { greenWinOrNot = 1; }
		float greenBonusFloat = base * greenFactor * greenWinOrNot;
		int greenBonus = greenBonusFloat.ToString(lenDec: 0).ToInt();
		
		
		Log("blueBonus "+blueBonus);
		Log("redBonus "+redBonus);
		Log("greenBonus "+greenBonus);
		
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
						if (faction.GetFactionName() == "BLUFOR")
							bonus = blueBonus;
							
						if (faction.GetFactionName() == "OPFOR")
							bonus = redBonus;

						if (faction.GetFactionName() == "INDFOR")
							bonus = greenBonus;
						
						foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles)
						{
							if (savedProfile.m_playerId == playerId) {
								Rpc(RpcDo_Notif_EndGameBonus, playerId, bonus);
								savedProfile.AddEndGameBonusXpAndMoney(bonus);
								m_listPlayerProfiles.Set(index, savedProfile);
							}
						}
					}
				}
			}
		}
	}

	override void OnGameEnd()
	{
		if (!Replication.IsServer())
			return;

		SavePlayersProfile();
	}
	
	// should only be server side
	void Refund(int price, int playerId)
	{
		foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles)
		{
			if (savedProfile.m_playerId == playerId) {
				savedProfile.Refund(price);
				m_listPlayerProfiles.Set(index, savedProfile);
			}
		}

		Replication.BumpMe();
	}

	// should only be server side
	bool TryBuy(int price, int playerId)
	{
		bool hasEnoughMoney = true;
		foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles)
		{
			if (savedProfile.m_playerId == playerId) {
				if (price > savedProfile.GetMoney()) {
					hasEnoughMoney = false;
					break;
				}
				
				savedProfile.Buy(price);
				m_listPlayerProfiles.Set(index, savedProfile);
			}
		}
		
		if (hasEnoughMoney)
			Replication.BumpMe();
		
		return hasEnoughMoney;
	}

	override bool HandlePlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		FactionAffiliationComponent playerFactionComp = FactionAffiliationComponent.Cast(player.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent killerFactionComp = FactionAffiliationComponent.Cast(killer.FindComponent(FactionAffiliationComponent));
		Log("OnPlayerKilled");

		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return false;

		if (!Replication.IsServer())
			return true;

		string playerName = playerManager.GetPlayerName(playerId);
		int killerId = playerManager.GetPlayerIdFromControlledEntity(killer);
		string killerName = playerManager.GetPlayerName(killerId);
		bool killerIsInList = false;
		bool playerIsInList = false;
		Log("killerName " + killerName);

		if (playerFactionComp.GetAffiliatedFaction() == killerFactionComp.GetAffiliatedFaction()) {
			// teamkill
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles)
			{

				if (savedProfile.m_playerName == killerName) {
					savedProfile.RemoveFriendlyKillXpAndMoney();
					m_listPlayerProfiles.Set(index, savedProfile);
					killerIsInList = true;
				}
			}

			Rpc(RpcDo_Notif_FriendlyKill, killerId);

			if (!killerIsInList) {
				KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
				profile.RemoveFriendlyKillXpAndMoney();
				profile.m_playerName = killerName;
				profile.m_playerId = killerId;
				m_listPlayerProfiles.Insert(profile);
			}
		} else {
			// kill
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles)
			{
				if (savedProfile.m_playerName == playerName) {
					savedProfile.m_deaths++;
					m_listPlayerProfiles.Set(index, savedProfile);
					playerIsInList = true;
				}

				if (savedProfile.m_playerName == killerName) {
					savedProfile.AddKillXpAndMoney();
					m_listPlayerProfiles.Set(index, savedProfile);
					killerIsInList = true;
				}
			}

			Rpc(RpcDo_Notif_EnemyKill, killerId);

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

		Replication.BumpMe();
		return true;
	}

	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	void RpcDo_Notif_EnemyKill(int killerId)
	{
		if (GetGame().GetPlayerController().GetPlayerId() != killerId)
			return;

		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifEnemyKill();
			}
		}
	}

	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	void RpcDo_Notif_FriendlyKill(int killerId)
	{
		if (GetGame().GetPlayerController().GetPlayerId() != killerId)
			return;

		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifFriendlyKill();
			}
		}
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

	void SavePlayersProfile()
	{
		if (!Replication.IsServer())
			return;

		listPlayerProfilesJson.m_list = m_listPlayerProfiles;

		bool success = listPlayerProfilesJson.SaveToFile(saveFilePath);
		Log("SAVING IS " + success);
	}

	override void OnPlayerConnected(int playerId)
	{
		if (!Replication.IsServer())
			return;

		Log("OnPlayerConnected "+playerId);
		
		GetGame().GetCallqueue().CallLater(whilePlayerUID, 500, false, playerId);
	}
	
	void whilePlayerUID(int playerId)
	{
		bool isInList = false;
		string playerUID = GetGame().GetBackendApi().GetPlayerUID(playerId);
		if (playerUID == string.Empty)
		{
			Log("empty playerUID for "+playerId);
			GetGame().GetCallqueue().CallLater(whilePlayerUID, 500, false, playerId);
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
		
		Replication.BumpMe();
	}
	
	override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		if (!Replication.IsServer())
			return;

		Log("OnPlayerDisconnected "+playerId);
		
		foreach (int index, KOTH_PlayerProfileJson profile : m_listPlayerProfiles)
		{
			if (profile.m_playerId == playerId)
			{
				profile.m_playerId = -1;
				m_listPlayerProfiles.Set(index, profile);
			}
		}
		
		Replication.BumpMe();
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
						if (faction.GetFactionName() == "BLUFOR")
							countBluefor++;
						
						if (faction.GetFactionName() == "OPFOR")
							countRedfor++;
						
						if (faction.GetFactionName() == "INDFOR")
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
