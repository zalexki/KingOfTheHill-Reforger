class KOTH_PresenceTriggerEntityClass : BaseGameTriggerEntityClass
{
}

class KOTH_PresenceTriggerEntity : BaseGameTriggerEntity
{
	SCR_BaseGameMode m_gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	private int m_counterTickPoints = 0;
	
	override protected void EOnInit(IEntity owner)
	{
		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(owner);
		if (!trigger) 
			return;
		
		trigger.AddClassType(ChimeraCharacter);
		trigger.SetUpdateRate(1);
		
		m_gameMode.m_kothTrigger = this;
	}
	
	override protected void OnDeactivate(IEntity ent)
	{
		Log("OnDeactivate");
		
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		
		if (outEntities.Count() < 1) 
		{
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
			if (!mapDescriptor)
			{
				Log("missing KOTH_SCR_MapDescriptorComponent on KOTH_PresenceTriggerEntity", LogLevel.ERROR);
			}
			
			int blueforPlayerNumber = 0;
			int greenforPlayerNumber = 0;
			int redforPlayerNumber = 0;
			foreach (IEntity entity: outEntities)
			{
				FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
				
				if (targetFactionComp) {
					Faction faction = targetFactionComp.GetAffiliatedFaction();
					if (faction) {
						mapDescriptor.changeMarker(faction.GetFactionKey());
						
						if (faction.GetFactionKey() == "US") {
							blueforPlayerNumber++;
						}
						if (faction.GetFactionKey() == "USSR") {
							redforPlayerNumber++;
						}
						if (faction.GetFactionKey() == "FIA") {
							greenforPlayerNumber++;
						}
					}
				}
			}
			
			if (blueforPlayerNumber > greenforPlayerNumber && blueforPlayerNumber > redforPlayerNumber)
				m_gameMode.m_blueforPoints++;
			
			if (greenforPlayerNumber > blueforPlayerNumber && greenforPlayerNumber > redforPlayerNumber)
				m_gameMode.m_greenforPoints++;
			
			if (redforPlayerNumber > greenforPlayerNumber && redforPlayerNumber > blueforPlayerNumber)
				m_gameMode.m_redforPoints++;
			
			m_counterTickPoints = 0;
			m_gameMode.CheckGameEnd();
		}
	}
	
	
}
