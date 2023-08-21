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
		
		//GetOnDeactivate().Insert(OnDeactivation);
		GetOnActivate().Insert(OnActivation);
		Log("OnDeactivate");
	}

	override protected void OnDeactivate(IEntity ent)
	{
		m_OnDeactivate.Invoke();
		
		Log("OnDeactivate");
		
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(ent.FindComponent(SCR_DamageManagerComponent));
		if (!damageManager)
			return;

		// Disable damage handling
		damageManager.EnableDamageHandling(true);
		
		Log("OnDeactivate done");
	}
	
	protected void OnActivation(IEntity entity)
	{
		Log("OnActivation");
		
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
		if (!damageManager)
			return;

		// Disable damage handling
		damageManager.EnableDamageHandling(false);
		
		Log("OnActivation done");
	}
}
