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

	// you want very dodgy way to do randomization of spawns, brace yourself and read below
	// TODO: fix this shit
	override void StartGameMode()
	{
		if (!Replication.IsServer())
			return;
		
		super.StartGameMode();
		Log(" --------- StartGameMode");
			
		vector worldTransformFirst[4];
		vector worldTransformFirstFlag[4];
		vector worldTransformSecond[4];
		vector worldTransformSecondFlag[4];
		vector worldTransformThird[4];
		vector worldTransformThirdFlag[4];
		IEntity firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		IEntity spawnPointFirst = FindSpawnPoint(firstSpawn);
		spawnPointFirst.GetWorldTransform(worldTransformFirst);
		IEntity firstFlag = FindFlag(firstSpawn);
		firstFlag.GetWorldTransform(worldTransformFirstFlag);
		
		Rpc(DoRpcFlag);
		
		IEntity secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		IEntity spawnPointSecond = FindSpawnPoint(secondSpawn);
		spawnPointSecond.GetWorldTransform(worldTransformSecond);
		IEntity secondFlag = FindFlag(secondSpawn);
		secondFlag.GetWorldTransform(worldTransformSecondFlag);
		
		IEntity thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");
		IEntity spawnPointThird = FindSpawnPoint(thirdSpawn);
		spawnPointThird.GetWorldTransform(worldTransformThird);
		IEntity thirdFlag = FindFlag(thirdSpawn);
		thirdFlag.GetWorldTransform(worldTransformThirdFlag);
		
		
		SCR_SpawnPoint spf = SCR_SpawnPoint.Cast(spawnPointFirst);
		spf.SetFactionKey("US");
		SCR_SpawnPoint sps = SCR_SpawnPoint.Cast(spawnPointSecond);
		sps.SetFactionKey("US");
		
		Resource res = Resource.Load("{31F54FB5494520B8}Prefabs/Props/Fabric/Flags/Flag_1_Blufor.et");
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Parent = this;
		IEntity flagEntity = GetGame().SpawnEntityPrefab(res, params: spawnParams);
		
		SlotManagerComponent compSec = SlotManagerComponent.Cast(secondFlag.FindComponent(SlotManagerComponent));
		EntitySlotInfo slotInfo = compSec.GetSlotByName("Flag");
		slotInfo.AttachEntity(flagEntity);
		SCR_SpawnPoint spt = SCR_SpawnPoint.Cast(spawnPointThird);
		spt.SetFactionKey("US");
		
		return;
		
		
		
		
		
		// randomize
		array<int> randomInts = GetUniqueRandomInts();
		switch (randomInts[0])
		{
			case 0:
				spawnPointFirst.SetWorldTransform(worldTransformFirst);
				//firstFlag.SetWorldTransform(worldTransformFirstFlag);
			break;
			case 1:
				spawnPointFirst.SetWorldTransform(worldTransformSecond);
				//firstFlag.SetWorldTransform(worldTransformFirstFlag);
			break;
			case 2:
				spawnPointFirst.SetWorldTransform(worldTransformThird);
				//firstFlag.SetWorldTransform(worldTransformFirstFlag);
			break;
		}
		switch (randomInts[1])
		{
			case 0:
				secondSpawn.SetWorldTransform(worldTransformFirst);
				//secondFlag.SetWorldTransform(worldTransformFirstFlag);
			break;
			case 1:
				secondSpawn.SetWorldTransform(worldTransformSecond);
				//secondFlag.SetWorldTransform(worldTransformSecondFlag);
			break;
			case 2:
				secondSpawn.SetWorldTransform(worldTransformThird);
				//secondFlag.SetWorldTransform(worldTransformThirdFlag);
			break;
		}
		switch (randomInts[2])
		{
			case 0:
				thirdSpawn.SetWorldTransform(worldTransformFirst);
				//thirdFlag.SetWorldTransform(worldTransformFirstFlag);
			break;
			case 1:
				thirdSpawn.SetWorldTransform(worldTransformSecond);
				//thirdFlag.SetWorldTransform(worldTransformSecondFlag);
			break;
			case 2:
				thirdSpawn.SetWorldTransform(worldTransformThird);
				//thirdFlag.SetWorldTransform(worldTransformThirdFlag);
			break;
		}
		
		//ApplyTransform(randomInts[0], spawnPointFirst, firstFlag, worldTransformFirst, worldTransformSecond, worldTransformThird);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void DoRpcFlag()
	{
		IEntity firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		IEntity spawnPointFirst = FindSpawnPoint(firstSpawn);
		SCR_SpawnPoint spf = SCR_SpawnPoint.Cast(spawnPointFirst);
		spf.SetFactionKey("US");
		
		IEntity secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		IEntity spawnPointSecond = FindSpawnPoint(secondSpawn);
		SCR_SpawnPoint sps = SCR_SpawnPoint.Cast(spawnPointSecond);
		sps.SetFactionKey("US");
		
		Resource res = Resource.Load("{31F54FB5494520B8}Prefabs/Props/Fabric/Flags/Flag_1_Blufor.et");
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.Parent = this;
		IEntity flagEntity = GetGame().SpawnEntityPrefab(res, params: spawnParams);
				IEntity secondFlag = FindFlag(secondSpawn);

		SlotManagerComponent compSec = SlotManagerComponent.Cast(secondFlag.FindComponent(SlotManagerComponent));
		EntitySlotInfo slotInfo = compSec.GetSlotByName("Flag");
		slotInfo.AttachEntity(flagEntity);
		
		
		IEntity thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");
		IEntity spawnPointThird = FindSpawnPoint(thirdSpawn);
		
		//IEntity flag;
		//SlotManagerComponent comp = SlotManagerComponent.Cast(flag.FindComponent(SlotManagerComponent));
		//comp.GetSlotByName("Flag").AttachEntity("{AC8F5AFDF613616F}Prefabs/Props/Fabric/Flags/Flag_1_Indfor.et");
	}
	
	void ApplyTransform(int index, IEntity spawnpoint, SlotManagerComponent flagComp, vector worldTransformFirst, vector worldTransformSecond, vector worldTransformThird)
	{
		switch (index)
		{
			case 0:
				spawnpoint.SetWorldTransform(worldTransformFirst);
			break;
			case 1:
				spawnpoint.SetWorldTransform(worldTransformSecond);
			break;
			case 2:
				spawnpoint.SetWorldTransform(worldTransformThird);
			break;
		}
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