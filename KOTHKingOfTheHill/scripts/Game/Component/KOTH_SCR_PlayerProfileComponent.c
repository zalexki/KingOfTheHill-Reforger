class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	bool m_playerIsProtected = false;

	void DoAskRpcBuy(int amount, string playerName)
	{
		Log("----------- DoAskRpcBuy");
		Rpc(RpcAsk_BuyStuff, amount, playerName);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_BuyStuff(int amount, string playerName)
	{
		Log("----------- BuyStuff from rpc call");
		if (!Replication.IsServer())
			return;
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(gameMode.FindComponent(KOTH_ScoringGameModeComponent));
		scoreComp.BuyStuff(amount, playerName);
	}
}