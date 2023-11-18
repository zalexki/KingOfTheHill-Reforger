modded class SCR_AmbientVehicleSpawnPointComponent : ScriptComponent
{
	override Vehicle SpawnVehicle()
	{
		m_sPrefab = "{04BDACC0BB83284E}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered_MERDC.et";
		return super.SpawnVehicle();
	}
}
