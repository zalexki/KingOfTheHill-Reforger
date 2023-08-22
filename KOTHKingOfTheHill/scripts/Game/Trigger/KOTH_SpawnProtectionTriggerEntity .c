class KOTH_SpawnProtectionTriggerEntityClass : SCR_BaseTriggerEntityClass{}
class KOTH_SpawnProtectionTriggerEntity : SCR_BaseTriggerEntity
{
	// check restriction zones / players
	override protected void EOnInit(IEntity owner)
	{
		BaseGameTriggerEntity trigger = BaseGameTriggerEntity.Cast(owner);
		if (!trigger) 
			return;
		
		trigger.AddClassType(ChimeraCharacter);
		trigger.SetUpdateRate(0.5);
		trigger.EnablePeriodicQueries(true);
		trigger.SetSphereRadius(125);
	}
	
	bool IsPlayerInside(int playerId)
	{
		bool isPlayerInside = false;
		Log("OnActivation for "+playerId);
		
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		PlayerManager playerManager = GetGame().GetPlayerManager();
		foreach (IEntity entity: outEntities)
		{
			if (playerId == playerManager.GetPlayerIdFromControlledEntity(entity)) 
			{
				isPlayerInside = true;
			}
		}
		
		return isPlayerInside;
	}
}
