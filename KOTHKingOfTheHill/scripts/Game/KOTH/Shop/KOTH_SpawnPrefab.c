class KOTH_SpawnPrefab : GenericEntity
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, desc: "Prefab to be spawned",  params: "et")]
	protected ResourceName m_prefabName;
	
	protected Vehicle m_prefabSpawned;

	void SCR_BasePrefabSpawner(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT | EntityEvent.FRAME );
		Activate();
	}

	protected override void EOnInit(IEntity owner)
	{
	}

	protected override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!Replication.IsServer())
			return;
		if (CanSpawn())
			Spawn();
	}

	protected bool CanSpawn()
	{
		 return false;
	}

	protected bool Spawn()
	{
		BaseWorld myWorld = GetGame().GetWorld();
		if (!myWorld || m_rnPrefab.IsEmpty())
			return false;
		Resource res = Resource.Load(m_rnPrefab);
		EntitySpawnParams params();

		vector mat[4];
		GetWorldTransform(mat);
		params.Transform = mat;
		IEntity newEnt = GetGame().SpawnEntityPrefab(res, myWorld, params);
		if (!newEnt)
			return false;

		newEnt.SetFlags(EntityFlags.VISIBLE, true);

		OnSpawn(newEnt);

		return true;
	}

	protected void OnSpawn(IEntity newEnt)
	{
	}
};

class KOTH_SpawnPrefabClass : GenericEntityClass
{
};