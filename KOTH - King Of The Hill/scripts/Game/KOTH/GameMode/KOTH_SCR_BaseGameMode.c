modded class SCR_BaseGameMode
{
	int m_winnerPointsNeeded = 10;
	
	IEntity m_kothTrigger;
	
    int m_blueforPoints = 0;
    int m_redforPoints = 0;
    int m_greenforPoints = 0;
	
	ref array<int> winnersFactionId = {};
	
	void EndGameWithFactionWin(int factionIndex)
	{
		SCR_GameModeEndData gameModeEndData = SCR_GameModeEndData.CreateSimple(EGameOverTypes.FACTION_VICTORY_SCORE, winnerFactionId: factionIndex);
		EndGameMode(gameModeEndData);
		GetGame().GetCallqueue().CallLater(RequestClose, 15000, false);
	}
	
	void CloseGame()
	{
		GetGame().RequestClose();
	}
	
	void CheckGameEnd()
	{
		array<Faction> factions = {};
		GetGame().GetFactionManager().GetFactionsList(factions);
		
		foreach (Faction faction : factions)
		{
			if (m_blueforPoints >= m_winnerPointsNeeded) {
				if (faction.GetFactionName() == "BLUFOR") {
					int factionIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
					EndGameWithFactionWin(factionIndex);
				}
			}
			
			if (m_redforPoints >= m_winnerPointsNeeded) {
				if (faction.GetFactionName() == "OPFOR") {
					int factionIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
					EndGameWithFactionWin(factionIndex);
				}
			}
			
			if (m_greenforPoints >= m_winnerPointsNeeded) {
				if (faction.GetFactionName() == "INDFOR") {
					int factionIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
					EndGameWithFactionWin(factionIndex);
				}
			}
		}
	}
	
	
	
	/*override void StartGameMode()
	{
		
		IEntity firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		IEntity secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		IEntity thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");
		
		SCR_SpawnPoint firstSP = SCR_SpawnPoint.Cast(firstSpawn);
		SCR_SpawnPoint secondtSP = SCR_SpawnPoint.Cast(secondSpawn);
		SCR_SpawnPoint thirdSP = SCR_SpawnPoint.Cast(thirdSpawn);

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

		m_factionKeyList.Insert("US");
		m_factionKeyList.Insert("FIA");
		m_factionKeyList.Insert("USSR");

		firstSP.SetFactionKey(m_factionKeyList[valueUsed[0]]);
		Log(m_factionKeyList[valueUsed[0]]);
		secondtSP.SetFactionKey(m_factionKeyList[valueUsed[1]]);
		Log(m_factionKeyList[valueUsed[1]]);
		thirdSP.SetFactionKey(m_factionKeyList[valueUsed[2]]);
		Log(m_factionKeyList[valueUsed[2]]);
		
		
		
		
		
		
		array<Resource> m_factionResSPList = {};
		Resource one = Resource.Load("{4FEA25583D0975F7}Prefabs/MP/Spawning/SpawnPoint_USSR.et");
		Resource sec = Resource.Load("{5A2220DD0C16958B}Prefabs/MP/Spawning/SpawnPoint_US.et");
		Resource thi = Resource.Load("{2B1758F237E4820E}Prefabs/MP/Spawning/SpawnPoint_FIA.et");
		if (one) {
			m_factionResSPList.Insert(one);
		}
		if (sec) {
			m_factionResSPList.Insert(sec);
		}
		if (thi) {
			m_factionResSPList.Insert(thi);
		}
		
		EntitySpawnParams spawnParamsFirst = new EntitySpawnParams();
		vector matFirst[4]; 
		firstSpawn.GetWorldTransform(matFirst);
		spawnParamsFirst.Transform = matFirst;
		GetGame().SpawnEntityPrefab(m_factionResSPList[valueUsed[0]], params: spawnParamsFirst);
		
		EntitySpawnParams spawnParamsSecond = new EntitySpawnParams();
		vector matSecond[4]; 
		secondSpawn.GetWorldTransform(matSecond);
		spawnParamsFirst.Transform = matSecond;
		GetGame().SpawnEntityPrefab(m_factionResSPList[valueUsed[1]], params: spawnParamsSecond);	
		
		EntitySpawnParams spawnParamsThird= new EntitySpawnParams();
		vector matThird[4]; 
		thirdSpawn.GetWorldTransform(matThird);
		spawnParamsThird.Transform = matThird;
		GetGame().SpawnEntityPrefab(m_factionResSPList[valueUsed[2]], params: spawnParamsThird);
	}*/
};