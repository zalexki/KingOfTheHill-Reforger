class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	protected bool m_hasBeenDroppedNearZone = false;
	bool HasBeenDroppedNearZone()
	{
		return m_hasBeenDroppedNearZone;
	}
	void ResetHasBeenDroppedNearZone()
	{
		m_hasBeenDroppedNearZone = false;
	}
	void DroppedNearZone()
	{
		m_hasBeenDroppedNearZone = true;
		GetGame().GetCallqueue().CallLater(ResetHasBeenDroppedNearZone, 60000 * 2);
	}
	
	protected int m_sessionEndGameBonus = 0;
	protected int m_sessionXpEarned = 0;
	protected int m_sessionMoneyEarned = 0;
	protected int m_sessionPointsWhenFactionWasJoined = 0;
	void AddSessionInZoneXpAndMoney() { 
		m_sessionXpEarned = m_sessionXpEarned + 10; 
		m_sessionMoneyEarned = m_sessionMoneyEarned + 10; 
	}
	void AddSessionKillXpAndMoney() { 
		m_sessionXpEarned = m_sessionXpEarned + 100; 
		m_sessionMoneyEarned = m_sessionMoneyEarned + 100; 
	}
	void AddSessionFriendlyKillXpAndMoney() { 
		m_sessionXpEarned = m_sessionXpEarned - 100; 
		m_sessionMoneyEarned = m_sessionMoneyEarned - 100; 
	}
	int GetSessionEndGameBonus() { return m_sessionEndGameBonus; }
	int GetSessionXpEarned() { return m_sessionXpEarned; }
	int GetSessionMoneyEarned() { return m_sessionMoneyEarned; }
	void SetSessionPointsWhenFactionWasJoined(int points) { m_sessionPointsWhenFactionWasJoined = points; }
	int GetSessionPointsWhenFactionWasJoined() { return m_sessionPointsWhenFactionWasJoined; }
	
	protected KOTH_PlayerProfileManagerGameModeComponent m_playerProfiles;
	protected ref array<string> m_unlockedItems = {};
	array<string> GetUnlockedItemList() { return m_unlockedItems; }
	
	protected int m_money = 0;
	protected int m_level = 0;
	protected int m_xp = 0;
	
	int GetXp() { return m_xp; }
	int GetMoney() { return m_money; }
	int GetLevel() { return m_level; }
	int GetXpNextLevel() { return (m_level + m_level - 1) * 100; } // TODO: up to 1000 for release

	void AddInZoneXpAndMoney()
	{
		m_sessionXpEarned = m_sessionXpEarned + 10;
		m_sessionMoneyEarned = m_sessionMoneyEarned + 10;
	}

	void AddKillXpAndMoney()
	{
		m_sessionXpEarned = m_sessionXpEarned + 100;
		m_sessionMoneyEarned = m_sessionMoneyEarned + 100;
	}
	
	void AddFriendlyDropNearZoneXpAndMoney()
	{
		m_sessionXpEarned = m_sessionXpEarned + 100;
		m_sessionMoneyEarned = m_sessionMoneyEarned + 100;
	}
	
	override protected void OnPostInit(IEntity owner)
	{
		if (SCR_Global.IsEditMode(owner))
			return;
		
		bool ismaster = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).IsMaster();
		if (Replication.IsServer() && !ismaster)
			return;
		
		m_playerProfiles = KOTH_PlayerProfileManagerGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_PlayerProfileManagerGameModeComponent));
		GetGame().GetCallqueue().CallLater(AskRpc_PlayerProfile, 1000, false);
	}

	void AskRpc_PlayerProfile()
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (!controller)
			return;

		int playerId = controller.GetPlayerId();
		if (!playerId || playerId == 0)
			return;
		
		Rpc(RpcAsk_PlayerProfile, GetGame().GetPlayerController().GetPlayerId());
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_PlayerProfile(int playerId)
	{
		KOTH_PlayerProfileJson profile = null;
		foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfiles.m_listPlayerProfiles)
		{
			if (savedProfile.m_playerId == playerId) {
				profile = savedProfile;
				DoRpc_SyncPlayerProfile(savedProfile);
				break;
			}
		}
		
		if (profile == null) {
			GetGame().GetCallqueue().CallLater(RpcAsk_PlayerProfile, 1000, false, playerId);
			return;
		}
	}

	void DoRpc_SyncPlayerProfile(KOTH_PlayerProfileJson profile)
	{
		Rpc(RpcDo_SyncPlayerProfile, profile.m_unlockedItems, profile.GetMoney(), profile.GetLevel(), profile.GetXp());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_SyncPlayerProfile(array<string> unlockedItems, int money, int level, int xp)
	{
		m_unlockedItems = unlockedItems;
		m_money = money;
		m_level = level;
		m_xp = xp;
	}
	
	void DoRpc_NotifCapture()
	{
		Rpc(RpcDo_NotifCapture);
	}
	
	[RplRpc(RplChannel.Unreliable, RplRcver.Owner)]
	void RpcDo_NotifCapture()
	{
		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifCapture();
			}
		}
	}
	
	void DoRpc_Notif_EnemyKill()
	{
		Rpc(RpcDo_Notif_EnemyKill);
	}
	[RplRpc(RplChannel.Unreliable, RplRcver.Owner)]
	void RpcDo_Notif_EnemyKill()
	{
		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifEnemyKill();
			}
		}
	}
	
	void DoRpc_Notif_FriendlyKill()
	{
		Rpc(RpcDo_Notif_FriendlyKill);
	}
	[RplRpc(RplChannel.Unreliable, RplRcver.Owner)]
	void RpcDo_Notif_FriendlyKill()
	{
		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifFriendlyKill();
			}
		}
	}
	
	void DoRpc_NotifDropFriendly()
	{
		Rpc(RpcDo_NotifDropFriendly);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_NotifDropFriendly()
	{
		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (hudManager) {
			KOTH_HUD kothHud = KOTH_HUD.Cast(hudManager.FindInfoDisplay(KOTH_HUD));
			if (kothHud) {
				kothHud.NotifDropFriendly();
			}
		}
	}
	
	void DoRpc_SetEndGameBonus(int bonus)
	{
		Rpc(RpcDo_SetEndGameBonus, bonus);
	}
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_SetEndGameBonus(int bonus)
	{
		m_sessionEndGameBonus = bonus;
	}
}