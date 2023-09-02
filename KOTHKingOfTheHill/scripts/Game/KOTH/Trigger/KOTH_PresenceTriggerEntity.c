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
		trigger.SetUpdateRate(2);
		trigger.EnablePeriodicQueries(true);
		trigger.SetSphereRadius(100);

		GetGame().GetCallqueue().CallLater(OnActivation, 10000, true);
	}

	protected void OnActivation()
	{
		if (!Replication.IsServer())
			return;
		
		if (!m_gameMode.IsRunning())
			return;

		PlayerManager playerManager = GetGame().GetPlayerManager();
		int blueforPlayerNumber = 0;
		int greenforPlayerNumber = 0;
		int redforPlayerNumber = 0;

		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		
		foreach (IEntity entity : outEntities)
		{
			// verify player is not dead or unconscious
			CharacterControllerComponent controllerComp = ChimeraCharacter.Cast(entity).GetCharacterController();
			if (controllerComp.IsDead() || controllerComp.IsUnconscious())
				continue;

			 int playerId = playerManager.GetPlayerIdFromControlledEntity(entity);
			
			// show notif for players inside zone
			DoRpc_NotifCapture(playerId);
			
			// add xp/money to players server side
			bool playerIsInList = false;
			string playerUID = GetGame().GetBackendApi().GetPlayerUID(playerId);

			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.AddInZoneXpAndMoney();
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
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
			}

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

		bool isZoneContested = true;

		if (blueforPlayerNumber > greenforPlayerNumber && blueforPlayerNumber > redforPlayerNumber) {
			isZoneContested = false;
			m_mapDescriptor.SetState(KOTH_Faction.BLUFOR);
			m_scoreComp.AddBlueforPoint();
		}

		if (greenforPlayerNumber > blueforPlayerNumber && greenforPlayerNumber > redforPlayerNumber) {
			isZoneContested = false;
			m_mapDescriptor.SetState(KOTH_Faction.INDFOR);
			m_scoreComp.AddGreenforPoint();
		}

		if (redforPlayerNumber > greenforPlayerNumber && redforPlayerNumber > blueforPlayerNumber) {
			isZoneContested = false;
			m_mapDescriptor.SetState(KOTH_Faction.OPFOR);
			m_scoreComp.AddRedforPoint();
		}

		// send new stats to clients
		m_scoreComp.BumpMe();
		m_playerProfileManager.BumpMe();

		// update zone marker
		if (true == isZoneContested) {
			if (outEntities.Count() < 1)
			{
				Log("SetState to none");
				m_mapDescriptor.SetState("none");
			} else {
				Log("SetState to contested");
				m_mapDescriptor.SetState("contested");
			}
		}

		m_gameMode.CheckGameEnd();
	}
	
	void DoRpc_NotifCapture(int playerId)
	{
		Rpc(RpcDo_NotifCapture, playerId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_NotifCapture(int playerId)
	{
		if (GetGame().GetPlayerController().GetPlayerId() != playerId)
			return;

		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifCapture();
			}
		}
	}
}
