modded class SCR_BaseGameMode
{
	const string saveFilePath = "$profile:koth_profiles.json";
	const int m_winnerPointsNeeded = 100;
	
	IEntity m_kothTrigger;
	
	void CloseGame()
	{
		GetGame().RequestClose();
	}

	void CheckGameEnd()
	{
		if (!Replication.IsServer())
			return;
		
		array<Faction> factions = {};
		GetGame().GetFactionManager().GetFactionsList(factions);
		
		string factionName;
		KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(FindComponent(KOTH_ScoringGameModeComponent));
		if (!scoreComp) {
			Log("Missing KOTH_ScoringGameModeComponent on gameMode", LogLevel.FATAL);
			return;
		}
		if (scoreComp.m_blueforPoints >= m_winnerPointsNeeded) { factionName = "BLUFOR"; };
		if (scoreComp.m_redforPoints >= m_winnerPointsNeeded) { factionName = "OPFOR"; };
		if (scoreComp.m_greenforPoints >= m_winnerPointsNeeded) { factionName = "INDFOR"; };
		
		foreach (Faction faction : factions)
		{
			if (faction.GetFactionName() == factionName) {
				int factionIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
				SCR_GameModeEndData gameModeEndData = SCR_GameModeEndData.CreateSimple(EGameOverTypes.FACTION_VICTORY_SCORE, winnerFactionId: factionIndex);
				EndGameMode(gameModeEndData);
				GetGame().GetCallqueue().CallLater(CloseGame, 15000, false);
			}
		}
	}

	protected override void OnGameStart()
	{
		super.OnGameStart();
		
		IEntity firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		IEntity secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		IEntity thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");
		
		GetGame().GetCallqueue().CallLater(AttachProperFlag, 1000, false, firstSpawn);
		GetGame().GetCallqueue().CallLater(AttachProperFlag, 1000, false, secondSpawn);
		GetGame().GetCallqueue().CallLater(AttachProperFlag, 1000, false, thirdSpawn);
	}
	
	void AttachProperFlag(IEntity spawn)
	{
		SCR_SpawnPoint sp = SCR_SpawnPoint.Cast(FindSpawnPoint(spawn));
		IEntity flagPoleEntity = FindFlag(spawn);
		Resource res;
		
		if (sp.GetFactionKey() == "US") {
			res = Resource.Load("{31F54FB5494520B8}Prefabs/Props/Fabric/Flags/Flag_1_Blufor.et");
		}
		if (sp.GetFactionKey() == "USSR") {
			res = Resource.Load("{167B394478139624}Prefabs/Props/Fabric/Flags/Flag_1_Opfor.et");
		}
		if (sp.GetFactionKey() == "FIA") {
			res = Resource.Load("{AC8F5AFDF613616F}Prefabs/Props/Fabric/Flags/Flag_1_Indfor.et");
		}
		
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		IEntity flagEntity = GetGame().SpawnEntityPrefab(res, params: spawnParams);
		
		SlotManagerComponent compSec = SlotManagerComponent.Cast(flagPoleEntity.FindComponent(SlotManagerComponent));
		EntitySlotInfo slotInfo = compSec.GetSlotByName("Flag");
		slotInfo.AttachEntity(flagEntity);
	}
	
	// called only server side 
	override void StartGameMode()
	{
		IEntity firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		IEntity spawnPointFirst = FindSpawnPoint(firstSpawn);
		IEntity firstFlag = FindFlag(firstSpawn);
		
		IEntity secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		IEntity spawnPointSecond = FindSpawnPoint(secondSpawn);
		IEntity secondFlag = FindFlag(secondSpawn);
		
		IEntity thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");
		IEntity spawnPointThird = FindSpawnPoint(thirdSpawn);
		IEntity thirdFlag = FindFlag(thirdSpawn);
		
		SCR_SpawnPoint spf = SCR_SpawnPoint.Cast(spawnPointFirst);
		SCR_SpawnPoint sps = SCR_SpawnPoint.Cast(spawnPointSecond);
		SCR_SpawnPoint spt = SCR_SpawnPoint.Cast(spawnPointThird);

		// randomize
		array<int> randomInts = GetUniqueRandomInts();
		array<string> factions = {};
		factions.Insert("US");
		factions.Insert("USSR");
		factions.Insert("FIA");
		
		Log("rand "+randomInts[0]);
		Log("rand "+randomInts[1]);
		Log("rand "+randomInts[2]);
		spf.SetFactionKey(factions.Get(randomInts[0]));
		sps.SetFactionKey(factions.Get(randomInts[1]));
		spt.SetFactionKey(factions.Get(randomInts[2]));
		
		super.StartGameMode();
	}
	
	IEntity FindFlag(IEntity parent) 
	{
		IEntity child = parent.GetChildren();
		KOTH_FlagComponent flag;
		for (int i = 0; i < 100; i++) 
		{
			Log(i.ToString());
			if (child) {
				flag = KOTH_FlagComponent.Cast(child.FindComponent(KOTH_FlagComponent));
				if (flag)
					return child;
				
				child = child.GetSibling();
			}
		}
		
		return null;
	}
	
	IEntity FindSpawnPoint(IEntity parent) 
	{
		IEntity child = parent.GetChildren();
		SCR_SpawnPoint spawn;
		for (int i = 0; i < 100; i++) 
		{
			spawn = SCR_SpawnPoint.Cast(child);
			if (spawn)
				break;
				
			child = child.GetSibling();
		}
		
		if (spawn) {
			return child;
		} else {
			return null;
		}
	}
	
	array<int> GetUniqueRandomInts()
	{
		array<int> valueUsed = {};
		for (int i = 0; i < 100000; i++) 
		{
			int randomInt = Math.RandomInt(0, 3);
			if (false == valueUsed.Contains(randomInt))
			{
				valueUsed.Insert(randomInt);
				if (valueUsed.Count() == 3) 
				{
					break;
				}
			}
		}
		
		return valueUsed;
	}
};

