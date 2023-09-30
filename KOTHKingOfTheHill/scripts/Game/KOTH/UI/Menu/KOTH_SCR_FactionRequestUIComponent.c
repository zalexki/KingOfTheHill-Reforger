modded class SCR_FactionRequestUIComponent
{
	override void ShowAvailableFactions()
	{
		super.ShowAvailableFactions();
		foreach (SCR_DeployButtonBase btn : m_aButtons)
		{
			SCR_FactionButton factionBtn = SCR_FactionButton.Cast(btn);
			if (factionBtn) 
			{
				factionBtn.UpdateButtons();
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
			if (bluforPlayers >= highestCount - 1)
			{
				if (bluforPlayers == greenforPlayers && highestCount != bluforPlayers || bluforPlayers == redforPlayers && highestCount != bluforPlayers)
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
			if (redforPlayers >= highestCount - 1)
			{
				if (redforPlayers == greenforPlayers && highestCount != redforPlayers || redforPlayers == bluforPlayers && highestCount != redforPlayers)
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
			if (greenforPlayers >= highestCount - 1)
			{
				if (greenforPlayers == redforPlayers && highestCount != greenforPlayers || greenforPlayers == bluforPlayers && highestCount != greenforPlayers)
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
