class KOTH_SpawnPrefabClass : SCR_BaseTriggerEntityClass{}
class KOTH_SpawnPrefab : SCR_BaseTriggerEntity
{
	bool isSpawnEmpty()
	{
		QueryEntitiesInside();
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		if (outEntities.Count() > 0) {
			return false;
		}
		
		return true;
	}
	
	bool Spawn(ResourceName m_prefabName)
	{
		vector pos;
		IEntity entity = IEntity.Cast(this);
		bool isSpawnEmpty = isSpawnEmpty();
		if (!isSpawnEmpty)
		{
			IEntity child = GetChildren();
			KOTH_SpawnPrefab kothSP = KOTH_SpawnPrefab.Cast(child);
			isSpawnEmpty = kothSP.isSpawnEmpty();
			if (!isSpawnEmpty)
			{
				for (int i = 0; i < 100; i++)
				{
					child = child.GetSibling();
					kothSP = KOTH_SpawnPrefab.Cast(child);
					if (!kothSP)
						return false;
					
					isSpawnEmpty = kothSP.isSpawnEmpty();
					if (isSpawnEmpty)
						break;
				}
			} 
			
			entity = child;
			if (!isSpawnEmpty)
				return false;
		}
		
		SCR_WorldTools.FindEmptyTerrainPosition(pos, entity.GetOrigin(), 5);
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		entity.GetTransform(params.Transform);
		
		IEntity newEnt = GetGame().SpawnEntityPrefab(Resource.Load(m_prefabName), null, params);
		
		if (!newEnt)
			return false;
		
		CarControllerComponent_SA carController = CarControllerComponent_SA.Cast(newEnt.FindComponent(CarControllerComponent_SA));
		
		// Activate handbrake so the vehicles don't go downhill on their own when spawned
		if (carController)
			carController.SetPersistentHandBrake(true);
		
		Physics physicsComponent = newEnt.GetPhysics();
		
		// Snap to terrain
		if (physicsComponent)
			physicsComponent.SetVelocity("0 -1 0");

		newEnt.SetFlags(EntityFlags.VISIBLE, true);

		return true;
	}
};
