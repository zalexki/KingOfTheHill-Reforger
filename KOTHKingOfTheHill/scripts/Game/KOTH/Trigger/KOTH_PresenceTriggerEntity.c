class KOTH_PresenceTriggerEntityClass : SCR_BaseTriggerEntityClass
{
}

class KOTH_PresenceTriggerEntity : SCR_BaseTriggerEntity
{
	SCR_BaseGameMode m_gameMode;
	KOTH_SCR_MapDescriptorComponent m_mapDescriptor;
	KOTH_ScoringGameModeComponent m_scoreComp;
	private int m_counterTick = 0;

	override protected void EOnInit(IEntity owner)
	{

		if (SCR_Global.IsEditMode(owner))
			return;

		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(owner);
		if (!trigger)
			return;

		trigger.AddClassType(ChimeraCharacter);
		trigger.SetUpdateRate(10);

		m_gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		m_gameMode.m_kothTrigger = this;
		m_mapDescriptor = KOTH_SCR_MapDescriptorComponent.Cast(this.FindComponent(KOTH_SCR_MapDescriptorComponent));
		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(m_gameMode.FindComponent(KOTH_ScoringGameModeComponent));

		GetOnDeactivate().Insert(OnDeactivation);
		//GetOnActivate().Insert(OnActivation);
		if (!m_mapDescriptor)
		{
			Log("missing KOTH_SCR_MapDescriptorComponent on KOTH_PresenceTriggerEntity", LogLevel.ERROR);
		}


		GetGame().GetCallqueue().CallLater(OnActivation, 10000, true);
	}

	protected void OnDeactivation(IEntity ent)
	{
		//Log("OnDeactivate");

		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);

		if (outEntities.Count() < 1)
		{
			m_mapDescriptor.ChangeMarker("none");
			m_counterTick = 0;
		}
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void NotifCapture(int killerId)
	{
		if (GetGame().GetPlayerController().GetPlayerId() != killerId)
			return;

		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifCapture();
			}
		}
	}

	protected void OnActivation()
	{
		if (!m_gameMode.IsRunning())
			return;

		//m_counterTick++;

		//if (m_counterTick >= 10) {
			array<IEntity> outEntities = {};
			GetEntitiesInside(outEntities);

			PlayerManager playerManager = GetGame().GetPlayerManager();
			int blueforPlayerNumber = 0;
			int greenforPlayerNumber = 0;
			int redforPlayerNumber = 0;

			foreach (IEntity entity : outEntities)
			{
				// verify player is not dead or unconscious
				CharacterControllerComponent controllerComp = ChimeraCharacter.Cast(entity).GetCharacterController();
				if (controllerComp.IsDead() || controllerComp.IsUnconscious())
					continue;

				// show notif for players inside zone
				int playerId = playerManager.GetPlayerIdFromControlledEntity(entity);
				if (GetGame().GetPlayerController().GetPlayerId() == playerId)
				{
					SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
					if (hudManager) {
						KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
						if (kothHud) {
							kothHud.NotifCapture();
						}
					}
				}

				// add xp/money to players server side
				if (Replication.IsServer()) {
					string playerName = playerManager.GetPlayerName(playerId);
					bool playerIsInList = false;

					foreach (int index, KOTH_PlayerProfileJson savedProfile : m_scoreComp.m_listPlayerProfiles)
					{
						if (savedProfile.m_name == playerName) {
							savedProfile.AddInZoneXpAndMoney();
							m_scoreComp.m_listPlayerProfiles.Set(index, savedProfile);
							playerIsInList = true;
						}
					}

					if (!playerIsInList) {
						KOTH_PlayerProfileJson profile = new KOTH_PlayerProfileJson();
						profile.AddInZoneXpAndMoney();
						profile.m_name = playerName;
						m_scoreComp.m_listPlayerProfiles.Insert(profile);
					}
				}

				FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
				if (targetFactionComp) {
					Faction faction = targetFactionComp.GetAffiliatedFaction();
					if (faction) {
						m_mapDescriptor.ChangeMarker(faction.GetFactionName());

						if (faction.GetFactionName() == "BLUFOR")
							blueforPlayerNumber++;

						if (faction.GetFactionName() == "OPFOR")
							redforPlayerNumber++;

						if (faction.GetFactionName() == "INDFOR")
							greenforPlayerNumber++;
					}
				}
			}

			bool isZoneContested = true;

			if (blueforPlayerNumber > greenforPlayerNumber && blueforPlayerNumber > redforPlayerNumber) {
				isZoneContested = false;
				if (Replication.IsServer())
					m_scoreComp.AddBlueforPoint();
			}

			if (greenforPlayerNumber > blueforPlayerNumber && greenforPlayerNumber > redforPlayerNumber) {
				isZoneContested = false;
				if (Replication.IsServer())
					m_scoreComp.AddGreenforPoint();
			}

			if (redforPlayerNumber > greenforPlayerNumber && redforPlayerNumber > blueforPlayerNumber) {
				isZoneContested = false;
				if (Replication.IsServer())
					m_scoreComp.AddRedforPoint();
			}

			// send new stats to clients
			if (Replication.IsServer())
				m_scoreComp.BumpMe();

			// update zone marker
			if (true == isZoneContested)
				m_mapDescriptor.ChangeMarker("contested");

			//m_counterTick = 0;
			m_gameMode.CheckGameEnd();
		//}
	}
}
