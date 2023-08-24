class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	protected string m_playerUID;
	
	override protected void OnPostInit(IEntity owner)
	{
//		if ()
//		m_playerUID = GetGame().GetBackendApi().GetPlayerUID(GetGame().GetPlayerController().GetPlayerId());
	}
}