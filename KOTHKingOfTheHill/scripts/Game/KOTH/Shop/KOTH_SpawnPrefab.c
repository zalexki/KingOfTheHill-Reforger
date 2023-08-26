class KOTH_SpawnPrefabClass : GenericEntityClass{}
class KOTH_SpawnPrefab : GenericEntity
{
	protected Vehicle m_prefabSpawned;
	protected bool m_isUsed = false;

	bool CanSpawn()
	{
		 return m_isUsed;
	}

	bool Spawn(ResourceName m_prefabName)
	{
		if (m_isUsed)
			return false;
		
		BaseWorld myWorld = GetGame().GetWorld();
		if (!myWorld || m_prefabName.IsEmpty())
			return false;
		Resource res = Resource.Load(m_prefabName);
		EntitySpawnParams params();

		vector mat[4];
		GetWorldTransform(mat);
		params.Transform = mat;
		IEntity newEnt = GetGame().SpawnEntityPrefab(res, myWorld, params);
		if (!newEnt)
			return false;

		newEnt.SetFlags(EntityFlags.VISIBLE, true);

		m_isUsed = true;

		return true;
	}
	
	void CheckVehicleIsFarEnough()
	{
		return;
	}
};
