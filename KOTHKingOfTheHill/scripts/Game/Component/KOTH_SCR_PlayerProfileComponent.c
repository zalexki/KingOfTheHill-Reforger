class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	string m_name;
    int m_kills = 0;
    int m_friendlyKills = 0;
    int m_money = 0;
	int m_level = 1;
	int m_xp = 0;
	
	void LoadFromSave(KOTH_PlayerProfileJson saved)
	{
		m_money = saved.m_money;
		m_xp = saved.m_xp;
		m_level = saved.m_level;
	}
	
	void Update(int money, int level, int xp)
	{
		Rpc(RpcDo_Update, money, level, xp);
	}
	
	int GetXpNextLevel()
	{
		return (m_level + m_level - 1) * 1000;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_Update(int money, int level, int xp)
	{
		m_money = money;
		m_level = level;
		m_xp = xp;
	}
	
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