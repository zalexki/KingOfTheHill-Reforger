class KOTH_SpawnHelper
{
    static void SpawnFreeVehicles()
	{
		IEntity vehicleSpawnFirstTruck = GetGame().GetWorld().FindEntityByName("KOTH_FirstFreeTruck");
		if (vehicleSpawnFirstTruck)
		{
			SCR_AmbientVehicleSpawnPointComponent compSecFirstTruck = SCR_AmbientVehicleSpawnPointComponent.Cast(vehicleSpawnFirstTruck.FindComponent(SCR_AmbientVehicleSpawnPointComponent));
			compSecFirstTruck.SpawnTruck();
		}
		IEntity vehicleSpawnFirstHelo = GetGame().GetWorld().FindEntityByName("KOTH_FirstFreeHelo");
		if (vehicleSpawnFirstHelo)
		{
			SCR_AmbientVehicleSpawnPointComponent compSecFirstHelo = SCR_AmbientVehicleSpawnPointComponent.Cast(vehicleSpawnFirstHelo.FindComponent(SCR_AmbientVehicleSpawnPointComponent));
			compSecFirstHelo.SpawnHelo();
		}
		
		IEntity vehicleSpawnSecondTruck = GetGame().GetWorld().FindEntityByName("KOTH_SecondFreeTruck");
		if (vehicleSpawnSecondTruck)
		{
			SCR_AmbientVehicleSpawnPointComponent compSecSecondTruck = SCR_AmbientVehicleSpawnPointComponent.Cast(vehicleSpawnSecondTruck.FindComponent(SCR_AmbientVehicleSpawnPointComponent));
			compSecSecondTruck.SpawnTruck();
		}
		IEntity vehicleSpawnSecondHelo = GetGame().GetWorld().FindEntityByName("KOTH_SecondFreeHelo");
		if (vehicleSpawnSecondHelo)
		{
			SCR_AmbientVehicleSpawnPointComponent compSecSecondHelo = SCR_AmbientVehicleSpawnPointComponent.Cast(vehicleSpawnSecondHelo.FindComponent(SCR_AmbientVehicleSpawnPointComponent));
			compSecSecondHelo.SpawnHelo();
		}
		
		IEntity vehicleSpawnThirdTruck = GetGame().GetWorld().FindEntityByName("KOTH_ThirdFreeTruck");
		if (vehicleSpawnThirdTruck)
		{
			SCR_AmbientVehicleSpawnPointComponent compSecThreeTruck = SCR_AmbientVehicleSpawnPointComponent.Cast(vehicleSpawnThirdTruck.FindComponent(SCR_AmbientVehicleSpawnPointComponent));
			compSecThreeTruck.SpawnTruck();
		}
		IEntity vehicleSpawnThirdHelo = GetGame().GetWorld().FindEntityByName("KOTH_ThirdFreeHelo");
		if (vehicleSpawnThirdHelo)
		{
			SCR_AmbientVehicleSpawnPointComponent compSecThreeHelo = SCR_AmbientVehicleSpawnPointComponent.Cast(vehicleSpawnThirdHelo.FindComponent(SCR_AmbientVehicleSpawnPointComponent));
			compSecThreeHelo.SpawnHelo();
		}
	}

    static IEntity FindSpawnPoint(IEntity parent)
	{
		IEntity child = parent.GetChildren();
		SCR_SpawnPoint spawn;
		for (int i = 0; i < 100; i++)
		{
			spawn = SCR_SpawnPoint.Cast(child);
			if (spawn)
				break;

			child = child.GetSibling();
		}

		if (spawn) {
			return child;
		} else {
			return null;
		}
	}
}