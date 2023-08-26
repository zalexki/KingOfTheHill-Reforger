class KOTH_SpawnPrefabClass : SCR_BaseTriggerEntityClass{}
class KOTH_SpawnPrefab : SCR_BaseTriggerEntity
{
	bool IsSomethingInside()
	{
		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		if (outEntities.Count() > 0) {
			return true;
		}
		
		return false;
	}
	
	bool Spawn(ResourceName m_prefabName)
	{
		vector pos;
		IEntity entity = IEntity.Cast(this);
		bool spawnEmpty = IsSomethingInside();
		if (spawnEmpty == false)
		{
			IEntity child = GetChildren();
			KOTH_SpawnPrefab kothSP = KOTH_SpawnPrefab.Cast(child);
			spawnEmpty = kothSP.IsSomethingInside();
			if (!spawnEmpty)
			{
				for (int i; i++; i < 100)
				{
					child = child.GetSibling();
					kothSP = KOTH_SpawnPrefab.Cast(child);
					spawnEmpty = kothSP.IsSomethingInside();
					if (!spawnEmpty)
						break;
				}
			} 
			
			entity = child;
			if (spawnEmpty)
				return false;
		}
		
		SCR_WorldTools.FindEmptyTerrainPosition(pos, entity.GetOrigin(), 5);
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		GetTransform(params.Transform);
		
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