class KOTH_PresenceTriggerEntityClass : BaseGameTriggerEntityClass
{
}

class KOTH_PresenceTriggerEntity : BaseGameTriggerEntity
{
	SCR_BaseGameMode m_gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	KOTH_SCR_MapDescriptorComponent m_mapDescriptor;
	private int m_counterTickPoints = 0;
	
	override protected void EOnInit(IEntity owner)
	{
		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(owner);
		if (!trigger) 
			return;
		
		trigger.AddClassType(ChimeraCharacter);
		trigger.SetUpdateRate(1);
		
		m_gameMode.m_kothTrigger = this;
		m_mapDescriptor = KOTH_SCR_MapDescriptorComponent.Cast(this.FindComponent(KOTH_SCR_MapDescriptorComponent));
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
			m_counterTickPoints = 0;
		}
	}
	
	override protected void OnActivate(IEntity ent)
	{
		Log("OnActivate");
		if (!m_gameMode.IsRunning())
			return;
		
		m_counterTickPoints++;
		
		if (m_counterTickPoints >= 2) {
			array<IEntity> outEntities = {};
			GetEntitiesInside(outEntities);
			
			KOTH_SCR_MapDescriptorComponent mapDescriptor = KOTH_SCR_MapDescriptorComponent.Cast(this.FindComponent(KOTH_SCR_MapDescriptorComponent));
			
			
			int blueforPlayerNumber = 0;
			int greenforPlayerNumber = 0;
			int redforPlayerNumber = 0;
			foreach (IEntity entity: outEntities)
			{
				FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
				
				if (targetFactionComp) {
					Faction faction = targetFactionComp.GetAffiliatedFaction();
					if (faction) {
						m_mapDescriptor.ChangeMarker(faction.GetFactionName());
						
						if (faction.GetFactionName() == "BLUFOR") {
							blueforPlayerNumber++;
						}
						if (faction.GetFactionName() == "OPFOR") {
							redforPlayerNumber++;
						}
						if (faction.GetFactionName() == "INDFOR") {
							greenforPlayerNumber++;
						}
					}
				}
			}
			
			bool isZoneContested = true;
			
			if (blueforPlayerNumber > greenforPlayerNumber && blueforPlayerNumber > redforPlayerNumber) {
				isZoneContested = false;
				m_gameMode.m_blueforPoints++;
			}
			
			if (greenforPlayerNumber > blueforPlayerNumber && greenforPlayerNumber > redforPlayerNumber) {
				isZoneContested = false;
				m_gameMode.m_greenforPoints++;
			}
			
			if (redforPlayerNumber > greenforPlayerNumber && redforPlayerNumber > blueforPlayerNumber) {
				isZoneContested = false;
				m_gameMode.m_redforPoints++;
			}
			
			if (true == isZoneContested) 
				m_mapDescriptor.ChangeMarker("contested");

			m_counterTickPoints = 0;
			m_gameMode.CheckGameEnd();
		}
	}
	
	
}
