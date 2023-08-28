modded class ArmaReforgerScripted
{
	override bool OnGameStart()
	{
		super.OnGameStart();
		
		KOTH_DebugMenu.Init();
		
		return true;
	}
	
	override void OnUpdate(BaseWorld world, float timeslice)
	{
		super.OnUpdate(world, timeslice);
		
		#ifdef WORKBENCH
		KOTH_DebugMenu.UpdateMenus();
		#endif
	}
}