modded enum SCR_DebugMenuID
{
	KOTH_MENU,
	KOTH_SHOW_PLAYER_PANEL,
	KOTH_SHOW_GAMEMODE_PANEL
}

class KOTH_DebugMenu
{
	static const string DEBUG_MENU_NAME = "KOTH";
	
	static void Init()
	{
		DiagMenu.RegisterMenu(SCR_DebugMenuID.KOTH_MENU, DEBUG_MENU_NAME, "");
		
		DiagMenu.RegisterBool(SCR_DebugMenuID.KOTH_SHOW_PLAYER_PANEL, "", "Player Panel", DEBUG_MENU_NAME);
		DiagMenu.RegisterBool(SCR_DebugMenuID.KOTH_SHOW_GAMEMODE_PANEL, "", "GameMode Panel", DEBUG_MENU_NAME);
	}
	
	static void DrawGameModePanel()
	{
		DbgUI.Begin("KOTH GameMode Panel");
		PlayerController playerController = GetGame().GetPlayerController();
		if (playerController)
			int playerID = playerController.GetPlayerId();
		
		KOTH_ScoringGameModeComponent m_scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));
			
		DbgUI.Text(KOTH_Faction.BLUFOR);
		if (DbgUI.Button("Add 1 point to "+KOTH_Faction.BLUFOR))
		{
			m_scoreComp.AddBlueforPoint();
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("Add 10 point to "+KOTH_Faction.BLUFOR))
		{
			m_scoreComp.AddBlueforPoint(10);
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("remove 1 point to blufor"))
		{
			m_scoreComp.RemoveBlueforPoint();
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("remove 10 point to blufor"))
		{
			m_scoreComp.RemoveBlueforPoint(10);
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		
		DbgUI.Text("REDFOR");
		if (DbgUI.Button("Add 1 point to redfor"))
		{
			m_scoreComp.AddRedforPoint();
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("Add 10 point to redfor"))
		{
			m_scoreComp.AddRedforPoint(10);
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("remove 1 point to redfor"))
		{
			m_scoreComp.RemoveRedforPoint();
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("remove 10 point to redfor"))
		{
			m_scoreComp.RemoveRedforPoint(10);
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		
		DbgUI.Text("INDFOR");
		if (DbgUI.Button("Add 1 point to indfor"))
		{
			m_scoreComp.AddGreenforPoint();
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("Add 10 point to indfor"))
		{
			m_scoreComp.AddGreenforPoint(10);
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("remove 1 point to indfor"))
		{
			m_scoreComp.RemoveGreenforPoint();
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		if (DbgUI.Button("remove 10 point to indfor"))
		{
			m_scoreComp.RemoveGreenforPoint(10);
			SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_PICK_UP);
		}
		
		DbgUI.End();
	}
	
	static void DrawPlayerPanel()
	{
		DbgUI.Begin("KOTH Player Panel");
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
		
		int playerId = playerController.GetPlayerId();
		
		KOTH_PlayerProfileManagerGameModeComponent m_playerProfileManager = KOTH_PlayerProfileManagerGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_PlayerProfileManagerGameModeComponent));
	
		if (DbgUI.Button("Add me 100$"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.AddMoney(100);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
				}
			}
			m_playerProfileManager.BumpMe();
			
		}
		if (DbgUI.Button("Remove me 100$"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.RemoveMoney(100);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
				}
			}
			m_playerProfileManager.BumpMe();
		}
		
		if (DbgUI.Button("Add me 1000$"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.AddMoney(1000);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
				}
			}
			m_playerProfileManager.BumpMe();
		}
		if (DbgUI.Button("Add me 1000000$"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.AddMoney(1000000);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
				}
			}
			m_playerProfileManager.BumpMe();
		}
		if (DbgUI.Button("Remove me 1000$"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.RemoveMoney(1000);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
					break;
				}
			}
			m_playerProfileManager.BumpMe();
		}
		
		if (DbgUI.Button("Add me 1LVL"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.AddLevel(1);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
					break;
				}
			}
			m_playerProfileManager.BumpMe();
		}
		if (DbgUI.Button("Remove me 1LVL"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.RemoveLevel(1);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
					break;
				}
			}
			m_playerProfileManager.BumpMe();
		}
		
		if (DbgUI.Button("Add me 10LVL"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.AddLevel(10);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
					break;
				}
			}
			m_playerProfileManager.BumpMe();
		}
		if (DbgUI.Button("Remove me 10LVL"))
		{
			foreach (int index, KOTH_PlayerProfileJson savedProfile : m_playerProfileManager.m_listPlayerProfiles)
			{
				if (savedProfile.m_playerId == playerId) {
					savedProfile.RemoveLevel(10);
					m_playerProfileManager.m_listPlayerProfiles.Set(index, savedProfile);
				}
			}
			m_playerProfileManager.BumpMe();
		}
		
		DbgUI.End();
	}
	
	static void UpdateMenus()
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.KOTH_SHOW_PLAYER_PANEL))
			DrawPlayerPanel();
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.KOTH_SHOW_GAMEMODE_PANEL))
			DrawGameModePanel();
	}
}