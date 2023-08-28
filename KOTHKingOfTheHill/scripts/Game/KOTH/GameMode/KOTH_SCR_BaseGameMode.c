modded class SCR_BaseGameMode
{
	const string saveFilePath = "$profile:koth_profiles.json";
	const int m_winnerPointsNeeded = 1;

	IEntity m_kothTrigger;

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
		if (scoreComp.GetBlueforPoint() >= m_winnerPointsNeeded) { factionName = "BLUFOR"; };
		if (scoreComp.GetRedforPoint() >= m_winnerPointsNeeded) { factionName = "OPFOR"; };
		if (scoreComp.GetGreenforPoint() >= m_winnerPointsNeeded) { factionName = "INDFOR"; };

		foreach (Faction faction : factions)
		{
			if (faction.GetFactionName() == factionName) {
				scoreComp.OnBeforeGameEnd();
				int factionIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
				SCR_GameModeEndData gameModeEndData = SCR_GameModeEndData.CreateSimple(EGameOverTypes.FACTION_VICTORY_SCORE, winnerFactionId: factionIndex);
				EndGameMode(gameModeEndData);
				GetGame().GetCallqueue().CallLater(CloseGame, 15000, false);
			}
		}
	}
	void CloseGame()
	{
		GetGame().RequestClose();
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

		if (sp.GetFactionKey() == "BLUFOR") {
			res = Resource.Load("{31F54FB5494520B8}Prefabs/Props/Fabric/Flags/Flag_1_Blufor.et");
		}
		if (sp.GetFactionKey() == "OPFOR") {
			res = Resource.Load("{167B394478139624}Prefabs/Props/Fabric/Flags/Flag_1_Opfor.et");
		}
		if (sp.GetFactionKey() == "INDFOR") {
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
		GetGame().GetCallqueue().CallLater(PlayersProtection, 5000, true);

		KOTH_SpawnPrefab firstVehicleSpawn = KOTH_SpawnPrefab.Cast(GetGame().GetWorld().FindEntityByName("KOTH_FirstVehicleSpawn"));
		KOTH_SpawnPrefab secondVehicleSpawn = KOTH_SpawnPrefab.Cast(GetGame().GetWorld().FindEntityByName("KOTH_SecondVehicleSpawn"));
		KOTH_SpawnPrefab thirdVehicleSpawn = KOTH_SpawnPrefab.Cast(GetGame().GetWorld().FindEntityByName("KOTH_ThirdVehicleSpawn"));
		
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
		factions.Insert("BLUFOR");
		factions.Insert("OPFOR");
		factions.Insert("INDFOR");

		
		spf.SetFactionKey(factions.Get(randomInts[0]));
		firstVehicleSpawn.SetFactionKey(factions.Get(randomInts[0]));
		sps.SetFactionKey(factions.Get(randomInts[1]));
		secondVehicleSpawn.SetFactionKey(factions.Get(randomInts[1]));
		spt.SetFactionKey(factions.Get(randomInts[2]));
		thirdVehicleSpawn.SetFactionKey(factions.Get(randomInts[2]));
		

		super.StartGameMode();
	}

	void PlayersProtection()
	{
		array<int> playerIds = new array<int>();
		PlayerManager playerManager = GetGame().GetPlayerManager();
		playerManager.GetPlayers(playerIds);

		IEntity firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		IEntity secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		IEntity thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");

		foreach (int playerId : playerIds)
		{
			bool isPlayerInAnyProtectionZone = false;

			KOTH_SpawnProtectionTriggerEntity firstProtect = FindSpawnProtection(firstSpawn);
			if (firstProtect.IsPlayerInside(playerId))
				isPlayerInAnyProtectionZone = true;

			KOTH_SpawnProtectionTriggerEntity secondProtect = FindSpawnProtection(secondSpawn);
			if (secondProtect.IsPlayerInside(playerId))
				isPlayerInAnyProtectionZone = true;

			KOTH_SpawnProtectionTriggerEntity thirdProtect = FindSpawnProtection(thirdSpawn);
			if (thirdProtect.IsPlayerInside(playerId))
				isPlayerInAnyProtectionZone = true;

			//Log("playerID "+playerId+ " is inside " + isPlayerInAnyProtectionZone);
			IEntity entity = playerManager.GetPlayerControlledEntity(playerId);
			if (!entity)
				return;
			SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
			if (!damageManager)
				return;

			if (isPlayerInAnyProtectionZone)
			{
				damageManager.EnableDamageHandling(false);
			}
			else
			{
				damageManager.EnableDamageHandling(true);
			}
		}
	}

	KOTH_SpawnProtectionTriggerEntity FindSpawnProtection(IEntity parent)
	{
		IEntity child = parent.GetChildren();
		KOTH_SpawnProtectionTriggerEntity spawn;
		for (int i = 0; i < 100; i++)
		{
			spawn = KOTH_SpawnProtectionTriggerEntity.Cast(child);
			if (spawn)
				break;

			if (child)
				child = child.GetSibling();
		}

		if (spawn) {
			return KOTH_SpawnProtectionTriggerEntity.Cast(child);
		} else {
			return null;
		}
	}

	IEntity FindFlag(IEntity parent)
	{
		IEntity child = parent.GetChildren();
		KOTH_FlagComponent flag;
		for (int i = 0; i < 100; i++)
		{
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
}
