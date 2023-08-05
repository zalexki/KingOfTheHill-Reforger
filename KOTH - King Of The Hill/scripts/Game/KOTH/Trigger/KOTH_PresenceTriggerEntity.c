class KOTH_PresenceTriggerEntityClass : BaseGameTriggerEntityClass
{
}

class KOTH_PresenceTriggerEntity : BaseGameTriggerEntity
{   
	// proto external int GetEntitiesInside(out notnull array<IEntity> outEntities);
    // proto external void SetUpdateRate(float updateRate);
	// proto external void AddClassType(typename classType);
	// proto external void AddFilterName(string name);
	private int counterTickPoints = 0;
	

	
	override protected void OnActivate(IEntity ent)
	{
		Log("OnActivate");
		counterTickPoints++;
		
		if (counterTickPoints == 2) {
			array<IEntity> outEntities = {};
			GetEntitiesInside(outEntities);
			
			int blueforPlayerNumber = 0;
			int greenforPlayerNumber = 0;
			int redforPlayerNumber = 0;
			foreach (IEntity entity: outEntities)
			{
				FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
				
				if (targetFactionComp) {
					Faction faction = targetFactionComp.GetAffiliatedFaction();
					if (faction) {
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
			
			SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			if (blueforPlayerNumber > greenforPlayerNumber && blueforPlayerNumber > redforPlayerNumber)
			{
				gameMode.m_blueforPoints++;
			}
			
			if (greenforPlayerNumber > blueforPlayerNumber && greenforPlayerNumber > redforPlayerNumber)
			{
				gameMode.m_greenforPoints++;
			}
			
			if (redforPlayerNumber > greenforPlayerNumber && redforPlayerNumber > blueforPlayerNumber)
			{
				gameMode.m_redforPoints++;
			}
			
			
			gameMode.checkGameEnd();
			
			counterTickPoints = 0;
		}
	}
	
	override protected void OnDeactivate(IEntity ent)
	{
		Log("OnDeactivate");
		
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		
		if (outEntities.Count() < 1) 
		{
			counterTickPoints = 0;
		}
	}
	
	
	override protected void EOnInit(IEntity owner)
	{
		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(owner);
		if (!trigger) 
			return;
		
		trigger.AddClassType(ChimeraCharacter);
		trigger.SetUpdateRate(1);
	}
}
