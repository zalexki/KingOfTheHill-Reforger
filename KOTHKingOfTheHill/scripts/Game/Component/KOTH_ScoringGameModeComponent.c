class KOTH_ScoringGameModeComponentClass : SCR_BaseGameModeComponentClass {}
class KOTH_ScoringGameModeComponent : SCR_BaseGameModeComponent
{
	const string saveFilePath = "$profile:koth_profiles.json";
	ref KOTH_ListPlayerProfileJson listPlayerProfilesJson = new KOTH_ListPlayerProfileJson();
	
	[RplProp()]
    int m_blueforPoints = 0;

	[RplProp()]
    int m_redforPoints = 0;

	[RplProp()]
    int m_greenforPoints = 0;
	
	// TODO: rethink this ugly thing
	[RplProp()]
	ref array<ref KOTH_PlayerProfileJson> m_listPlayerProfiles = new array<ref KOTH_PlayerProfileJson>();
	
	void AddBlueforPoint()
	{
		m_blueforPoints++;
	}
	
	void AddRedforPoint()
	{
		m_redforPoints++;
	}
	
	void AddGreenforPoint()
	{
		m_greenforPoints++;
	}
	
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
		
		GetGame().GetCallqueue().CallLater(SavePlayersProfile, 10000, true);
	}
	
	override void OnGameEnd()
	{
		if (!Replication.IsServer())
			return;
		
		SavePlayersProfile();
	}

	void BuyStuff(int amount, string playerName)
	{
		Log("----------- BuyStuff from rpc call");
		if (!Replication.IsServer())
			return;
		
		foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles) 
		{
			if (savedProfile.m_name == playerName) {
				savedProfile.BuyStuff(amount);
				m_listPlayerProfiles.Set(index, savedProfile);
			}
		}
		
		Replication.BumpMe();
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
				
				if (savedProfile.m_name == killerName) {
					savedProfile.RemoveFriendlyKillXpAndMoney();
					m_listPlayerProfiles.Set(index, savedProfile);
					killerIsInList = true;
				}	
			}
			
			Rpc(RpcDo_NotifFriendlyKill, killerId);
			
			if (!killerIsInList) {
				KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
				profile.RemoveFriendlyKillXpAndMoney();
				profile.m_name = killerName;
				m_listPlayerProfiles.Insert(profile);
			}
		} else {
			// kill
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_listPlayerProfiles) 
			{
				if (savedProfile.m_name == playerName) {
					savedProfile.m_deaths++;
					m_listPlayerProfiles.Set(index, savedProfile);
					playerIsInList = true;
				}
				
				if (savedProfile.m_name == killerName) {
					savedProfile.AddKillXpAndMoney();
					m_listPlayerProfiles.Set(index, savedProfile);
					killerIsInList = true;
				}
			}
			
			Rpc(RpcDo_NotifEnemyKill, killerId);
			
			if (!killerIsInList) {
				KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
				profile.AddKillXpAndMoney();
				profile.m_name = killerName;
				m_listPlayerProfiles.Insert(profile);
			}
			
			if (!playerIsInList) {
				KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
				profile.m_deaths++;
				profile.m_name = playerName;
				m_listPlayerProfiles.Insert(profile);
			}
		}
		
		Replication.BumpMe();
		return true;
	}
	
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	void RpcDo_NotifEnemyKill(int killerId)
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
	void RpcDo_NotifFriendlyKill(int killerId)
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
	
	void SavePlayersProfile()
	{
		if (!Replication.IsServer())
			return;
		
		listPlayerProfilesJson.m_list = m_listPlayerProfiles;
			
		bool success = listPlayerProfilesJson.SaveToFile(saveFilePath);
		Log(" --------- SAVING IS " + success);
	}
	
	void BumpMe()
	{
		Replication.BumpMe();
	}
}