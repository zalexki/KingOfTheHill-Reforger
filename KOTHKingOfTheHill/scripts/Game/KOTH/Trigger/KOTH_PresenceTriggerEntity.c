class KOTH_PresenceTriggerEntityClass : SCR_BaseTriggerEntityClass
{
}

class KOTH_PresenceTriggerEntity : SCR_BaseTriggerEntity
{
	SCR_BaseGameMode m_gameMode;
	KOTH_SCR_MapDescriptorComponent m_mapDescriptor;
	KOTH_ScoringGameModeComponent m_scoreComp;
	KOTH_PlayerProfileManagerGameModeComponent m_playerProfileManager;
	
	override protected void EOnInit(IEntity owner)
	{
		if (SCR_Global.IsEditMode(owner))
			return;

		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(owner);
		if (!trigger)
			return;
		
		m_gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		m_gameMode.m_kothTrigger = this;
		m_mapDescriptor = KOTH_SCR_MapDescriptorComponent.Cast(this.FindComponent(KOTH_SCR_MapDescriptorComponent));
		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(m_gameMode.FindComponent(KOTH_ScoringGameModeComponent));
		m_playerProfileManager = KOTH_PlayerProfileManagerGameModeComponent.Cast(m_gameMode.FindComponent(KOTH_PlayerProfileManagerGameModeComponent));
		
		if (!Replication.IsServer())
			return;

		trigger.AddClassType(ChimeraCharacter);
		trigger.SetUpdateRate(0);
		trigger.EnablePeriodicQueries(false);
		trigger.SetSphereRadius(100);

		GetGame().GetCallqueue().CallLater(PeriodicCall, 10000, true);
	}

	protected void PeriodicCall()
	{
		if (!Replication.IsServer())
			return;
		
		if (!m_gameMode.IsRunning())
			return;

		PlayerManager playerManager = GetGame().GetPlayerManager();
		int blueforPlayerNumber = 0;
		int greenforPlayerNumber = 0;
		int redforPlayerNumber = 0;

		QueryEntitiesInside();
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		
		foreach (IEntity entity : outEntities)
		{
			// TODO: temp fix since AddClassType seems broken rn
			if (!ChimeraCharacter.Cast(entity))
				continue;
			
			// verify player is not dead or unconscious
			CharacterControllerComponent controllerComp = ChimeraCharacter.Cast(entity).GetCharacterController();
			if (controllerComp.IsDead() || controllerComp.IsUnconscious())
				continue;
			
			int playerId = playerManager.GetPlayerIdFromControlledEntity(entity);
		
			// add xp/money to players server side
			bool playerIsInList = false;
			string playerUID = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
			if (!playerUID || playerUID == string.Empty)
			{
				Log("could not find playerUID for playerId "+playerId+" named "+ playerManager.GetPlayerName(playerId), LogLevel.ERROR);
				//playerUID = playerId.ToString();
			}

			PlayerController playerController = playerManager.GetPlayerController(playerId);
			if (!playerController)
				continue;
			
			KOTH_SCR_PlayerProfileComponent profileComp = KOTH_SCR_PlayerProfileComponent.Cast(playerController.FindComponent(KOTH_SCR_PlayerProfileComponent));
			if (!profileComp)
				continue;
			
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId && savedProfile.m_playerUID == playerUID) {
					savedProfile.AddInZoneXpAndMoney();
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
					profileComp.AddSessionInZoneXpAndMoney();
					profileComp.DoRpc_SyncPlayerProfile(savedProfile);
					playerIsInList = true;
					break;
				}
			}

			if (!playerIsInList) {
				KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
				profile.AddInZoneXpAndMoney();
				profile.m_playerUID = playerUID;
				profile.m_playerId = playerId;
				profile.m_playerName = playerManager.GetPlayerName(playerId);
				m_playerProfileManager.m_listPlayerProfiles.Insert(profile);
				profileComp.AddSessionInZoneXpAndMoney();
				profileComp.DoRpc_SyncPlayerProfile(profile);
			}
			
			// show notif for players inside zone
			profileComp.DoRpc_NotifCapture();

			FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
			if (targetFactionComp) {
				Faction faction = targetFactionComp.GetAffiliatedFaction();
				if (faction) {
	
					if (faction.GetFactionName() == KOTH_Faction.BLUFOR)
						blueforPlayerNumber++;

					if (faction.GetFactionName() == KOTH_Faction.OPFOR)
						redforPlayerNumber++;

					if (faction.GetFactionName() == KOTH_Faction.INDFOR)
						greenforPlayerNumber++;
				}
			}
		}

		bool isZoneEmptyORContested = true;

		if (blueforPlayerNumber > greenforPlayerNumber && blueforPlayerNumber > redforPlayerNumber) {
			isZoneEmptyORContested = false;
			m_scoreComp.AddBlueforPoint();
			if (m_mapDescriptor.GetState() != KOTH_Faction.BLUFOR)
				m_mapDescriptor.SetState(KOTH_Faction.BLUFOR);
		}

		if (greenforPlayerNumber > blueforPlayerNumber && greenforPlayerNumber > redforPlayerNumber) {
			isZoneEmptyORContested = false;
			m_scoreComp.AddGreenforPoint();
			if (m_mapDescriptor.GetState() != KOTH_Faction.INDFOR)
				m_mapDescriptor.SetState(KOTH_Faction.INDFOR);
		}

		if (redforPlayerNumber > greenforPlayerNumber && redforPlayerNumber > blueforPlayerNumber) {
			isZoneEmptyORContested = false;
			m_scoreComp.AddRedforPoint();
			if (m_mapDescriptor.GetState() != KOTH_Faction.OPFOR)
				m_mapDescriptor.SetState(KOTH_Faction.OPFOR);
		}

		// send new stats to clients
		m_scoreComp.BumpMe();

		// update zone marker
		if (true == isZoneEmptyORContested) {
			if (outEntities.Count() < 1)
			{
				if (m_mapDescriptor.GetState() != "none")
					m_mapDescriptor.SetState("none");
			} else {
				if (m_mapDescriptor.GetState() != "contested")
					m_mapDescriptor.SetState("contested");
			}
		}

		m_gameMode.CheckGameEnd();
	}
}
