modded class GameOverScreenInput: ChimeraMenuBase
{
	override void OnMenuOpen() 
	{
		super.OnMenuOpen();
		
		KOTH_ScoringGameModeComponent scoreComp = KOTH_ScoringGameModeComponent.Cast(GetGame().GetGameMode().FindComponent(KOTH_ScoringGameModeComponent));

		Widget newRow = GetGame().GetWorkspace().CreateWidgets("{0AB65604E4B00513}UI/Layouts/HUD/GameOver/EndScreen/EndScreenBonusLine.layout", GetRootWidget());
		
		Widget notif = newRow.FindAnyWidget("NotifContainer");
		if (!notif)
			return;
		
		TextWidget text = TextWidget.Cast(notif.FindAnyWidget("TextNotif"));
		TextWidget money = TextWidget.Cast(notif.FindAnyWidget("MoneyNotif"));
		TextWidget xp = TextWidget.Cast(notif.FindAnyWidget("XpNotif"));
		text.SetText("END GAME BONUS");
		
		Faction playerFaction = SCR_FactionManager.Cast(GetGame().GetFactionManager()).GetPlayerFaction( GetGame().GetPlayerController().GetPlayerId());
		int bonus;
		if (playerFaction.GetFactionKey() == "US") { bonus = scoreComp.m_blueBonus; }
		if (playerFaction.GetFactionKey() == "USSR") { bonus = scoreComp.m_redBonus; }
		if (playerFaction.GetFactionKey() == "FIA") { bonus = scoreComp.m_greenBonus; }
		money.SetText(""+bonus+" $");
		xp.SetText(""+bonus+" xp");
		Log(notif);
	}
}