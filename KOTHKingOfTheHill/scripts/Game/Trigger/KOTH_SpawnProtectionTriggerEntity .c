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
		//GetOnActivate().Insert(OnActivation);
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
	
	protected void OnActivation(IEntity ent)
	{
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		int playerId = playerManager.GetPlayerIdFromControlledEntity(ent);
		bool isPlayerInside = false;
		Log("OnActivation for "+playerId);

		foreach (IEntity entity: outEntities)
		{
			if (playerId == playerManager.GetPlayerIdFromControlledEntity(entity)) 
			{
				isPlayerInside = true;
			}
		}
		
		if (true == isPlayerInside) {
			PlayerController playerController = GetGame().GetPlayerController();
			KOTH_SCR_PlayerProfileComponent kothPlayerComp = KOTH_SCR_PlayerProfileComponent.Cast(playerController.FindComponent(KOTH_SCR_PlayerProfileComponent));
			Print(kothPlayerComp);
			if (kothPlayerComp)
				kothPlayerComp.DoAskRpcProtection();
		}
	
		
		Log("OnActivation done "+isPlayerInside);
	}

	override protected void OnDeactivate(IEntity ent)
	{
		return;
		
		m_OnDeactivate.Invoke();
		
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		int playerId = playerManager.GetPlayerIdFromControlledEntity(ent);
		bool isPlayerInside = false;
		Log("OnDeactivate for "+playerId);

		foreach (IEntity entity: outEntities)
		{
			if (playerId == playerManager.GetPlayerIdFromControlledEntity(entity)) 
			{
				isPlayerInside = true;
			}
		}
		
		if (false == isPlayerInside) 
		{
			PlayerController playerController = GetGame().GetPlayerController();
			KOTH_SCR_PlayerProfileComponent kothPlayerComp = KOTH_SCR_PlayerProfileComponent.Cast(playerController.FindComponent(KOTH_SCR_PlayerProfileComponent));
			Print(kothPlayerComp);
			if (kothPlayerComp)
				kothPlayerComp.DoAskRpcStopProtection();
		}
		
		
		Log("OnDeactivate done for "+isPlayerInside);
	}
}
