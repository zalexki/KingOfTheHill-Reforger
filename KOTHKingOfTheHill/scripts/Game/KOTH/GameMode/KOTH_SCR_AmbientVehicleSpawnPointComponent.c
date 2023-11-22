modded class SCR_AmbientVehicleSpawnPointComponent : ScriptComponent
{
	Vehicle SpawnTruck()
	{
		m_sPrefab = "{04BDACC0BB83284E}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered_MERDC.et";
		return SpawnVehicle();
	}
	
	Vehicle SpawnHelo()
	{
		m_sPrefab = "{70BAEEFC2D3FEE64}Prefabs/Vehicles/Helicopters/UH1H/UH1H.et";
		return SpawnVehicle();
	}
}
