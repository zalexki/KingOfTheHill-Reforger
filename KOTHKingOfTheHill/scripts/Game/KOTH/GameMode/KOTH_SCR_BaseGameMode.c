modded class SCR_BaseGameMode
{
	const int WINNER_POINTS_NEEDED = 100;
	
	const string scenarioHistoryFilePath = "$profile:koth_scenarioHistory.json";
	string m_nextScenario = string.Empty;

	IEntity m_kothTrigger;
	
	IEntity m_firstSpawn;
	IEntity m_secondSpawn;
	IEntity m_thirdSpawn;
	SCR_SpawnPoint m_firstSpawnPoint;
	SCR_SpawnPoint m_secondSpawnPoint;
	
	SCR_SpawnPoint m_thirdSpawnPoint;
	KOTH_SpawnProtectionTriggerEntity m_firstProtect;
	KOTH_SpawnProtectionTriggerEntity m_secondProtect;
	KOTH_SpawnProtectionTriggerEntity m_thirdProtect;

	protected override void OnGameStart()
	{
		super.OnGameStart();

		// handle history scenario
		KOTH_ScenarioHistoryJson scenarioHistoryJson = new KOTH_ScenarioHistoryJson();
		scenarioHistoryJson.LoadFromFile(scenarioHistoryFilePath);
		foreach (string scenar : scenarioHistoryJson.m_list)
		{
			Log("previous scenar = " + scenar);
		}
		
		SCR_MissionHeader header = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());
		if (header) {
			string name = header.GetHeaderResourceName();
			if (name != string.Empty)
				scenarioHistoryJson.m_list.Insert(name);
		}
		
		if (scenarioHistoryJson.m_list.Count() > 2)
			scenarioHistoryJson.m_list.Remove(2);
		
		scenarioHistoryJson.SaveToFile(scenarioHistoryFilePath);
		
		// handle spawns
		m_firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		m_secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		m_thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");
		
		m_firstSpawnPoint = SCR_SpawnPoint.Cast(KOTH_SpawnHelper.FindSpawnPoint(m_firstSpawn));
		m_secondSpawnPoint = SCR_SpawnPoint.Cast(KOTH_SpawnHelper.FindSpawnPoint(m_secondSpawn));
		m_thirdSpawnPoint = SCR_SpawnPoint.Cast(KOTH_SpawnHelper.FindSpawnPoint(m_thirdSpawn));
		
		m_firstProtect = FindSpawnProtection(m_firstSpawn);
		m_secondProtect = FindSpawnProtection(m_secondSpawn);
		m_thirdProtect = FindSpawnProtection(m_thirdSpawn);

		GetGame().GetCallqueue().CallLater(AttachProperFlag, 1000, false, m_firstSpawn);
		GetGame().GetCallqueue().CallLater(AttachProperFlag, 1000, false, m_secondSpawn);
		GetGame().GetCallqueue().CallLater(AttachProperFlag, 1000, false, m_thirdSpawn);
		
		KOTH_SpawnHelper.SpawnFreeVehicles();
		GetGame().GetCallqueue().CallLater(SpawnFreeVehicles, 60000 * 5, true);
	}
	void SpawnFreeVehicles()
	{
		KOTH_SpawnHelper.SpawnFreeVehicles();
	}
	
	void AttachProperFlag(IEntity spawn)
	{
		SCR_SpawnPoint sp = SCR_SpawnPoint.Cast(KOTH_SpawnHelper.FindSpawnPoint(spawn));
		IEntity flagPoleEntity = FindFlag(spawn);
		Resource res;

		if (sp.GetFactionKey() == KOTH_Faction.BLUFOR) {
			res = Resource.Load("{31F54FB5494520B8}Prefabs/Props/Fabric/Flags/Flag_1_Blufor.et");
		}
		if (sp.GetFactionKey() == KOTH_Faction.OPFOR) {
			res = Resource.Load("{167B394478139624}Prefabs/Props/Fabric/Flags/Flag_1_Opfor.et");
		}
		if (sp.GetFactionKey() == KOTH_Faction.INDFOR) {
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
		
		IEntity firstFlag = FindFlag(m_firstSpawnPoint);
		IEntity secondFlag = FindFlag(m_secondSpawnPoint);
		IEntity thirdFlag = FindFlag(m_thirdSpawnPoint);

		// randomize
		array<int> randomInts = GetUniqueRandomInts();
		array<string> factions = {};
		factions.Insert(KOTH_Faction.BLUFOR);
		factions.Insert(KOTH_Faction.OPFOR);
		factions.Insert(KOTH_Faction.INDFOR);

		m_firstSpawnPoint.SetFactionKey(factions.Get(randomInts[0]));
		firstVehicleSpawn.SetFactionKey(factions.Get(randomInts[0]));
		
		m_secondSpawnPoint.SetFactionKey(factions.Get(randomInts[1]));
		secondVehicleSpawn.SetFactionKey(factions.Get(randomInts[1]));
		
		m_thirdSpawnPoint.SetFactionKey(factions.Get(randomInts[2]));
		thirdVehicleSpawn.SetFactionKey(factions.Get(randomInts[2]));

		super.StartGameMode();	
	}

	void PlayersProtection()
	{
		array<int> playerIds = new array<int>();
		PlayerManager playerManager = GetGame().GetPlayerManager();
		playerManager.GetPlayers(playerIds);

		foreach (int playerId : playerIds)
		{
			bool isPlayerInProtectionZone = false;
			IEntity controlledEntity = playerManager.GetPlayerControlledEntity(playerId);
			if (!controlledEntity) 
				continue;
			
			FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(controlledEntity.FindComponent(FactionAffiliationComponent));
			Faction faction = targetFactionComp.GetAffiliatedFaction();
			if (!faction)
				continue;
			
			FactionKey userFactionKey = faction.GetFactionKey();
				
			if (m_firstSpawnPoint.GetFactionKey() == userFactionKey) {
				if (m_firstProtect.IsPlayerInside(playerId))
					isPlayerInProtectionZone = true;
			}
				
			if (m_secondSpawnPoint.GetFactionKey() == userFactionKey) {
				if (m_secondProtect.IsPlayerInside(playerId))
					isPlayerInProtectionZone = true;
			}
			

			if (m_thirdSpawnPoint.GetFactionKey() == userFactionKey) {
				if (m_thirdProtect.IsPlayerInside(playerId))
					isPlayerInProtectionZone = true;
			}

			SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(controlledEntity.FindComponent(SCR_DamageManagerComponent));
			if (!damageManager)
				continue;

			if (isPlayerInProtectionZone)
			{
				if (damageManager.IsDamageHandlingEnabled())
					damageManager.EnableDamageHandling(false);
			}
			else
			{
				if (!damageManager.IsDamageHandlingEnabled())
					damageManager.EnableDamageHandling(true);
			}
			
			//Log("IsDamageHandlingEnabled " + damageManager.IsDamageHandlingEnabled());
		}
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
		if (scoreComp.GetBlueforPoint() >= WINNER_POINTS_NEEDED) { factionName = KOTH_Faction.BLUFOR; };
		if (scoreComp.GetRedforPoint() >= WINNER_POINTS_NEEDED) { factionName = KOTH_Faction.OPFOR; };
		if (scoreComp.GetGreenforPoint() >= WINNER_POINTS_NEEDED) { factionName = KOTH_Faction.INDFOR; };

		foreach (Faction faction : factions)
		{
			if (faction.GetFactionName() == factionName) {
				scoreComp.OnBeforeGameEnd();
				int factionIndex = GetGame().GetFactionManager().GetFactionIndex(faction);
				SCR_GameModeEndData gameModeEndData = SCR_GameModeEndData.CreateSimple(EGameOverTypes.FACTION_VICTORY_SCORE, winnerFactionId: factionIndex);
				EndGameMode(gameModeEndData);
				
				// random next scenario
				m_nextScenario = ChooseNextScenario();
				GetGame().GetCallqueue().CallLater(ChangeScenario, 15000, false);
			}
		}
	}
	
	void ChangeScenario()
	{
		GameStateTransitions.RequestScenarioChangeTransition(m_nextScenario, GetAddonsGUIDs());
	}
	
	private string ChooseNextScenario()
	{
		KOTH_ScenarioHistoryJson scenarioHistoryJson = new KOTH_ScenarioHistoryJson();
		scenarioHistoryJson.LoadFromFile(scenarioHistoryFilePath);
		
		array<string> all = {
			"{08096F926A21678B}Worlds/Scenario/KOTH-Saintphilipe-V1.conf",
			"{8ADD561C1F2F256F}Worlds/Scenario/KOTH-Montignac-V1.conf",
			"{43A2DC866897E5FE}Worlds/Scenario/KOTH-Arleville-V1.conf", 
			"{F8C8A21273E32FB9}Worlds/Scenario/KOTH-Beauregard-V1.conf",
			"{DD04E8FC104D01F5}Worlds/Scenario/KOTH-Chotain-V1.conf",
		};

		foreach (string scenar : scenarioHistoryJson.m_list)
		{
			all.RemoveItem(scenar);
		}
		
		return all.GetRandomElement();
	}
	
	private string GetAddonsGUIDs()
	{
		string addonIDs;
		
		array<string> addonsGUIDs = {};
		GameProject.GetLoadedAddons(addonsGUIDs);
		
		foreach (string GUID: addonsGUIDs)
		{
			if (!GameProject.IsVanillaAddon(GUID))
			{
				if (!addonIDs.IsEmpty())
					addonIDs += ",";
				
				addonIDs += GUID;
			}
		}
		
		return addonIDs;
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
