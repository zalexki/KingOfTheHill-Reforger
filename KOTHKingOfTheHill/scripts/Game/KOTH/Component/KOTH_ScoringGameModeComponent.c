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
	ref array<ref KOTH_PlayerProfileJson> listPlayerProfiles = new array<ref KOTH_PlayerProfileJson>;
	
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
	
	void SavePlayersProfile()
	{
		if (!Replication.IsServer())
			return;
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
		listPlayerProfilesJson.m_list = listPlayerProfiles;
		
		bool success = listPlayerProfilesJson.SaveToFile(saveFilePath);
		Log(" --------- SAVING IS " + success);
	}
	
	/*override bool RplSave(ScriptBitWriter writer)
    {
		super.RplSave(writer);
		
        writer.WriteInt(m_blueforPoints);
        writer.WriteInt(m_redforPoints);
        writer.WriteInt(m_greenforPoints);
		
		int players = listPlayerProfiles.Count();
        writer.WriteInt(players);
		foreach (KOTH_PlayerProfileJson profile : listPlayerProfiles)
		{
			profile.RplSave(writer);
		}
		return true;
    }
	
	override bool RplLoad(ScriptBitReader reader)
    {
		reader.ReadInt(m_blueforPoints);
		reader.ReadInt(m_redforPoints);
		reader.ReadInt(m_greenforPoints);
	
		int players;
		if (!reader.ReadInt(players))
			return false;
		
		for (int i = 0; i < players; i++)
		{
			ref KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();

			if (!profile.RplLoad(reader))
				return false;

			listPlayerProfiles.Insert(profile);
		}

        return true;
    }*/
	
	void BumpMe()
	{
		Replication.BumpMe();
	}
}