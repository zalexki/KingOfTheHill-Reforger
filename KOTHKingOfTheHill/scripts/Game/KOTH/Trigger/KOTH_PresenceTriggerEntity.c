class KOTH_PresenceTriggerEntityClass : BaseGameTriggerEntityClass
{
}

class KOTH_PresenceTriggerEntity : BaseGameTriggerEntity
{
	SCR_BaseGameMode m_gameMode;
	KOTH_SCR_MapDescriptorComponent m_mapDescriptor;
	KOTH_ScoringGameModeComponent m_scoreComp;
	private int m_counterTick = 0;
	
	override protected void EOnInit(IEntity owner)
	{
		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(owner);
		if (!trigger) 
			return;
		
		trigger.AddClassType(ChimeraCharacter);
		trigger.SetUpdateRate(1);
		
		m_gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		m_gameMode.m_kothTrigger = this;
		m_mapDescriptor = KOTH_SCR_MapDescriptorComponent.Cast(this.FindComponent(KOTH_SCR_MapDescriptorComponent));
		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(m_gameMode.FindComponent(KOTH_ScoringGameModeComponent));

		if (!m_mapDescriptor)
		{
			Log("missing KOTH_SCR_MapDescriptorComponent on KOTH_PresenceTriggerEntity", LogLevel.ERROR);
		}
	}
	
	override protected void OnDeactivate(IEntity ent)
	{
		Log("OnDeactivate");
		
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		
		if (outEntities.Count() < 1) 
		{
			m_mapDescriptor.ChangeMarker("none");
			m_counterTick = 0;
		}
	}
	
	override protected void OnActivate(IEntity ent)
	{
		Log("OnActivate");
		if (!m_gameMode.IsRunning())
			return;
		
		m_counterTick++;
		
		if (m_counterTick >= 15) {
			array<IEntity> outEntities = {};
			GetEntitiesInside(outEntities);

			PlayerManager playerManager = GetGame().GetPlayerManager();
			int blueforPlayerNumber = 0;
			int greenforPlayerNumber = 0;
			int redforPlayerNumber = 0;
			
			foreach (IEntity entity: outEntities)
			{
				if (m_gameMode.IsMaster()) {
					int playerId = playerManager.GetPlayerIdFromControlledEntity(entity);				
					string playerName = playerManager.GetPlayerName(playerId);
					foreach (int index, KOTH_PlayerProfileJson savedProfile : m_scoreComp.listPlayerProfiles) 
					{
						if (savedProfile.m_name == playerName) {
							savedProfile.AddInZoneXpAndMoney();
							m_scoreComp.listPlayerProfiles.Set(index, savedProfile);
						}
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
				if (m_gameMode.IsMaster())
					m_scoreComp.m_blueforPoints++;
			}
			
			if (greenforPlayerNumber > blueforPlayerNumber && greenforPlayerNumber > redforPlayerNumber) {
				isZoneContested = false;
				if (m_gameMode.IsMaster())
					m_scoreComp.m_greenforPoints++;
			}
			
			if (redforPlayerNumber > greenforPlayerNumber && redforPlayerNumber > blueforPlayerNumber) {
				isZoneContested = false;
				if (m_gameMode.IsMaster())
					m_scoreComp.m_redforPoints++;
			}
			
			if (m_gameMode.IsMaster())
				m_scoreComp.BumpMe();
					
			if (true == isZoneContested) 
				m_mapDescriptor.ChangeMarker("contested");

			
			m_counterTick = 0;
			m_gameMode.CheckGameEnd();
		}
	}
}
