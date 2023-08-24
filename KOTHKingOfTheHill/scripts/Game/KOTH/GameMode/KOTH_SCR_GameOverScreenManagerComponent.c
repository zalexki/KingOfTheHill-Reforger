modded class SCR_GameOverScreenManagerComponent: SCR_BaseGameModeComponent
{
	override void OnGameModeEnd(SCR_GameModeEndData data)
	{
		GetGame().GetCallqueue().CallLater(OnInitDelayedEndGame, 2000);
	}
}