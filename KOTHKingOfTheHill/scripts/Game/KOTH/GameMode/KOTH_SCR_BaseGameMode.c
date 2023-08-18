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

	override void StartGameMode()
	{
		return;
		
		if (!Replication.IsServer())
			return;

		Log("---------------------- StartGameMode -----------------");
		array<vector> placesWorld = new array<vector>;
		array<vector> placesLocal = new array<vector>;
		vector worldTransform[4];
		vector localTransform[4];
		
		IEntity firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		firstSpawn.GetWorldTransform(worldTransform);
		firstSpawn.GetLocalTransform(localTransform);
		placesWorld.Insert(worldTransform);
		placesLocal.Insert(localTransform);
		
		IEntity secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		secondSpawn.GetWorldTransform(worldTransform);
		secondSpawn.GetLocalTransform(localTransform);
		placesWorld.Insert(worldTransform);
		placesLocal.Insert(localTransform);
		
		IEntity thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");
		thirdSpawn.GetWorldTransform(worldTransform);
		thirdSpawn.GetLocalTransform(localTransform);
		
		placesWorld.Insert(worldTransform);
		placesLocal.Insert(localTransform);
		
		// randomize
		int randomIndex = placesWorld.GetRandomIndex();
		firstSpawn.SetWorldTransform(placesWorld.Get(randomIndex));
		firstSpawn.SetLocalTransform(placesLocal.Get(randomIndex));
		placesWorld.Remove(randomIndex);
		placesLocal.Remove(randomIndex);
		
		randomIndex = placesWorld.GetRandomIndex();
		secondSpawn.SetWorldTransform(placesWorld.Get(randomIndex));
		secondSpawn.SetLocalTransform(placesLocal.Get(randomIndex));
		placesWorld.Remove(randomIndex);
		placesLocal.Remove(randomIndex);
		
		randomIndex = placesWorld.GetRandomIndex();
		thirdSpawn.SetWorldTransform(placesWorld.Get(randomIndex));
		thirdSpawn.SetLocalTransform(placesLocal.Get(randomIndex));
		placesWorld.Remove(randomIndex);
		placesLocal.Remove(randomIndex);
	}
};