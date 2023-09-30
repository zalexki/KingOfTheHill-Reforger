modded class SCR_FactionRequestUIComponent
{
	override void ShowAvailableFactions()
	{
		super.ShowAvailableFactions();
		
		bool isOneOk = false;
		foreach (SCR_DeployButtonBase btn : m_aButtons)
		{
			SCR_FactionButton factionBtn = SCR_FactionButton.Cast(btn);
			if (factionBtn) 
			{
				if (factionBtn.UpdateButtons())
					isOneOk = true;
			}
		}
	}
	override protected void UpdateFactionButtons(Faction faction, int newCount)
	{
		foreach (SCR_DeployButtonBase btn : m_aButtons)
		{
			SCR_FactionButton factionBtn = SCR_FactionButton.Cast(btn);
			if (factionBtn) 
			{
				factionBtn.UpdatePlayerCount();
				factionBtn.UpdateButtons();
			}
		}
	}
}


modded class SCR_FactionButton
{

//	override bool OnFocus(Widget w, int x, int y)
//	{	
//		UpdateButtons();
//		return true;
//	}
	
//	override void SetSelected(bool selected)
//	{
//		UpdateButtons();
//	}
	
	bool UpdateButtons()
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return false;
		
		int bluforPlayers;
		int redforPlayers;
		int greenforPlayers;
		int highestCount = 0;
		
		Faction blu = factionManager.GetFactionByKey(KOTH_Faction.BLUFOR);
		bluforPlayers = factionManager.GetFactionPlayerCount(blu);
		if (highestCount < bluforPlayers)
			highestCount = bluforPlayers;
		
		Faction red = factionManager.GetFactionByKey(KOTH_Faction.OPFOR);
		redforPlayers = factionManager.GetFactionPlayerCount(red);
		if (highestCount < redforPlayers)
			highestCount = redforPlayers;
		
		Faction green = factionManager.GetFactionByKey(KOTH_Faction.INDFOR);
		greenforPlayers = factionManager.GetFactionPlayerCount(green);
		if (highestCount < greenforPlayers)
			highestCount = greenforPlayers;
	
		
		if (m_Faction.GetFactionKey() == KOTH_Faction.BLUFOR)
		{
			if (highestCount < 2)
			{
				SetEnabled(true);
				SetShouldUnlock(true);
				return true;
			}
			if (bluforPlayers >= highestCount)
			{
				if (bluforPlayers == greenforPlayers && bluforPlayers == redforPlayers)
				{
					SetEnabled(true);
					SetShouldUnlock(true);
					return true;
				}
				SetEnabled(false);
				SetShouldUnlock(false);
				return false;
			} else {
				SetEnabled(true);
				SetShouldUnlock(true);
			}
		}
		if (m_Faction.GetFactionKey() == KOTH_Faction.OPFOR)
		{
			if (highestCount < 2)
			{
				SetEnabled(true);
				SetShouldUnlock(true);
				return true;
			}
			if (redforPlayers >= highestCount)
			{
				if (redforPlayers == greenforPlayers && redforPlayers == bluforPlayers)
				{
					SetEnabled(true);
					SetShouldUnlock(true);
					return true;
				}
				SetEnabled(false);
				SetShouldUnlock(false);
				return false;
			} else {
				SetEnabled(true);
				SetShouldUnlock(true);
			}
		}
		if (m_Faction.GetFactionKey() == KOTH_Faction.INDFOR)
		{
			if (highestCount < 2)
			{
				SetEnabled(true);
				SetShouldUnlock(true);
				return true;
			}
			if (greenforPlayers >= highestCount)
			{
				if (greenforPlayers == redforPlayers && greenforPlayers == bluforPlayers)
				{
					SetEnabled(true);
					SetShouldUnlock(true);
					return true;
				}
				SetEnabled(false);
				SetShouldUnlock(false);
				return false;
			} else {
				SetEnabled(true);
				SetShouldUnlock(true);
			}
		}
		
		return true;
	}
}
