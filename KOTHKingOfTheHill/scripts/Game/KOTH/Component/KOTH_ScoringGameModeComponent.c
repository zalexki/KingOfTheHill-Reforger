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
	
	[RplProp()]
	ref array<ref KOTH_PlayerProfileJson> listPlayerProfiles = new array<ref KOTH_PlayerProfileJson>();
	
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
		
		listPlayerProfiles = listPlayerProfilesJson.m_list;
		Replication.BumpMe();
		
		GetGame().GetCallqueue().CallLater(SavePlayersProfile, 10000, true);
	}
	
	override void OnGameEnd()
	{
		if (!Replication.IsServer())
			return;
		
		SavePlayersProfile();
	}
	
	override bool HandlePlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		if (!Replication.IsServer())
			return false;
		
		FactionAffiliationComponent playerFactionComp = FactionAffiliationComponent.Cast(player.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent killerFactionComp = FactionAffiliationComponent.Cast(killer.FindComponent(FactionAffiliationComponent));
		Log("OnPlayerKilled");
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return false;
		
		int killerId = playerManager.GetPlayerIdFromControlledEntity(killer);				
		string killerName = playerManager.GetPlayerName(killerId);
		Log("killerName " + killerName);
		
		if (playerFactionComp.GetAffiliatedFaction() == killerFactionComp.GetAffiliatedFaction())
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : listPlayerProfiles) 
			{
				if (savedProfile.m_name == killerName) {
					savedProfile.RemoveFriendlyKillXpAndMoney();
					listPlayerProfiles.Set(index, savedProfile);
				}
			}
		} else {
			foreach (int index, KOTH_PlayerProfileJson savedProfile : listPlayerProfiles) 
			{
				if (savedProfile.m_name == killerName) {
					savedProfile.AddKillXpAndMoney();
					listPlayerProfiles.Set(index, savedProfile);
					
					SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
					if (hudManager) {
						KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
						if (kothHud) {
							kothHud.NotifKill();
						}
					}
				}
			}
		}
		
		Replication.BumpMe();
		return true;
	}
	
	void SavePlayersProfile()
	{
		if (!Replication.IsServer())
			return;
		
		listPlayerProfilesJson.m_list = listPlayerProfiles;
			
		bool success = listPlayerProfilesJson.SaveToFile(saveFilePath);
		Log(" --------- SAVING IS " + success);
	}
	
	void BumpMe()
	{
		Replication.BumpMe();
	}
}