class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	protected KOTH_ScoringGameModeComponent m_scoreComp;
	protected ref array<string> m_unlockedItems = {};
	array<string> GetUnlockedItemList() { return m_unlockedItems; }

	override protected void OnPostInit(IEntity owner)
	{
		if (SCR_Global.IsEditMode(owner))
			return;
		
		bool ismaster = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).IsMaster();
		if (Replication.IsServer() && !ismaster)
			return;

		m_scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
		GetGame().GetCallqueue().CallLater(AskRpc_PlayerProfile, 1000, false);
	}

	void AskRpc_PlayerProfile()
	{
		Rpc(RpcAsk_PlayerProfile, GetGame().GetPlayerController().GetPlayerId());
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_PlayerProfile(int playerId)
	{
		KOTH_PlayerProfileJson profile = null;
		foreach (int index, KOTH_PlayerProfileJson savedProfile : m_scoreComp.m_listPlayerProfiles)
		{
			if (savedProfile.m_playerId == playerId) {
				profile = savedProfile;
			}
		}
		
		if (profile == null) {
			GetGame().GetCallqueue().CallLater(RpcAsk_PlayerProfile, 1000, false, playerId);
			return;
		}
		
		
	}
	void DoRpc_PlayerProfile(KOTH_PlayerProfileJson profile)
	{
		Rpc(RpcDo_PlayerProfile, profile.m_unlockedItems);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_PlayerProfile(array<string> unlockedItems)
	{
		m_unlockedItems = unlockedItems;
	}
}