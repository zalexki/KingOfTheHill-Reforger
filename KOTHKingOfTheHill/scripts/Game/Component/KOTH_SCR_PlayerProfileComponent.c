class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}
class KOTH_SCR_PlayerProfileComponent : ScriptComponent 
{
	bool m_playerIsProtected = false;
	
	
	override protected void OnPostInit(IEntity owner)
	{ 
		Log("KOTH_SCR_PlayerProfileComponent OnPostInit");
		
		GetGame().GetCallqueue().CallLater(checkForProtection, 500, true);
	}
	
	void checkForProtection()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();
		bool isPlayerInAnyProtectionZone = false;
		IEntity firstSpawn = GetGame().GetWorld().FindEntityByName("KOTH_FirstSpawn");
		IEntity secondSpawn = GetGame().GetWorld().FindEntityByName("KOTH_SecondSpawn");
		IEntity thirdSpawn = GetGame().GetWorld().FindEntityByName("KOTH_ThirdSpawn");
		
		KOTH_SpawnProtectionTriggerEntity firstProtect = FindSpawnProtection(firstSpawn);
		if (firstProtect.IsPlayerInside(playerId))
			isPlayerInAnyProtectionZone = true;
		
		KOTH_SpawnProtectionTriggerEntity secondProtect = FindSpawnProtection(secondSpawn);
		if (secondProtect.IsPlayerInside(playerId))
			isPlayerInAnyProtectionZone = true;
		
		KOTH_SpawnProtectionTriggerEntity thirdProtect = FindSpawnProtection(thirdSpawn);
		if (thirdProtect.IsPlayerInside(playerId))
			isPlayerInAnyProtectionZone = true;
		
		if (true == m_playerIsProtected && false == isPlayerInAnyProtectionZone)
		{
			DoAskRpcStopProtection();
			m_playerIsProtected = false;
		}
		
		if (false == m_playerIsProtected && true == isPlayerInAnyProtectionZone)
		{
			DoAskRpcProtection();
			m_playerIsProtected = true;
		}
		Log("m_playerIsProtected "+m_playerIsProtected);
		Log("isPlayerInAnyProtectionZone "+isPlayerInAnyProtectionZone);
		Log("playerId "+playerId);
		
	}
	
	KOTH_SpawnProtectionTriggerEntity FindSpawnProtection(IEntity parent) 
	{
		IEntity child = parent.GetChildren();
		KOTH_SpawnProtectionTriggerEntity spawn;
		for (int i = 0; i < 100; i++) 
		{
			spawn = KOTH_SpawnProtectionTriggerEntity.Cast(child);
			if (spawn)
				break;
				
			child = child.GetSibling();
		}
		
		if (spawn) {
			return KOTH_SpawnProtectionTriggerEntity.Cast(child);
		} else {
			return null;
		}
	}
	
	void DoAskRpcProtection()
	{
		Rpc(DoAsk_Protection, GetGame().GetPlayerController().GetPlayerId());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void DoAsk_Protection(int playerId)
	{
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		Log("DoAsk_Protection playerId "+playerId);
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		Log("DoAsk_Protection playerId "+playerId);
		IEntity entity = playerManager.GetPlayerControlledEntity(playerId);
		if (!entity)
			return;
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
		if (!damageManager)
			return;

		Log("DoAsk_Protection playerId "+playerId);
		// Disable damage handling
		damageManager.EnableDamageHandling(false);
	}
	
	void DoAskRpcStopProtection()
	{
		Rpc(DoAsk_StopProtection, GetGame().GetPlayerController().GetPlayerId());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void DoAsk_StopProtection(int playerId)
	{
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		Log("DoAsk_StopProtection playerId "+playerId);
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		Log("DoAsk_StopProtection playerId "+playerId);
		IEntity entity = playerManager.GetPlayerControlledEntity(playerId);
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(entity.FindComponent(SCR_DamageManagerComponent));
		if (!damageManager)
			return;
		
		Log("DoAsk_StopProtection playerId "+playerId);
		// Disable damage handling
		damageManager.EnableDamageHandling(true);
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