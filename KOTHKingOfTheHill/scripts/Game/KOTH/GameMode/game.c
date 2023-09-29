modded class ArmaReforgerScripted
{
	override bool OnGameStart()
	{
		super.OnGameStart();
		
		#ifdef WORKBENCH
		KOTH_DebugMenu.Init();
		#endif
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