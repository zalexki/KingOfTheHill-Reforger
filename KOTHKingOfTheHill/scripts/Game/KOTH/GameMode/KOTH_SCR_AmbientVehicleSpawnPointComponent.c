modded class SCR_AmbientVehicleSpawnPointComponent : ScriptComponent
{
	override protected void Update(SCR_Faction faction)
	{
		super.Update(faction);

		m_sPrefab = "{04BDACC0BB83284E}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered_MERDC.et";
	}
}
