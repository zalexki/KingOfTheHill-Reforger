class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	protected KOTH_PlayerProfileManagerGameModeComponent m_playerProfiles;
	protected ref array<string> m_unlockedItems = {};
	array<string> GetUnlockedItemList() { return m_unlockedItems; }
	
	protected int m_money = 0;
	protected int m_level = 0;
	protected int m_xp = 0;
	
	int GetMoney() { return m_money; }
	int GetXp() { return m_xp; }
	int GetXpNextLevel() { 
		// TODO: up to 1000 for release
		return (m_level + m_level - 1) * 100;
	}
	int GetLevel() { return m_level; }

	
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
				DoRpc_PlayerProfile(savedProfile);
				break;
			}
		}
		
		if (profile == null) {
			GetGame().GetCallqueue().CallLater(RpcAsk_PlayerProfile, 1000, false, playerId);
			return;
		}
	}

	void DoRpc_PlayerProfile(KOTH_PlayerProfileJson profile)
	{
		Rpc(RpcDo_PlayerProfile, profile.m_unlockedItems, profile.GetMoney(), profile.GetLevel(), profile.GetXp());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_PlayerProfile(array<string> unlockedItems, int money, int level, int xp)
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
		
	
}