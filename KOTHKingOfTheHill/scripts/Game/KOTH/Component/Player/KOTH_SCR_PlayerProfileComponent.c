class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	protected string m_playerUID;
	
	override protected void OnPostInit(IEntity owner)
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (controller)
			m_playerUID = GetGame().GetBackendApi().GetPlayerUID(controller.GetPlayerId());
	}
}