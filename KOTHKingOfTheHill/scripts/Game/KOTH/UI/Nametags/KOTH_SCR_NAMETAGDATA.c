modded enum ENameTagEntityState
{
	RED 	= 1<<7,
	BLUE 	= 1<<8,
	GREEN 	= 1<<9,
	LVL = 1<<10,
}

modded class SCR_NameTagData
{
	override protected void InitData(SCR_NameTagConfig config)
	{
		super.InitData(config);
		if (!m_Entity)
			return;

		FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(m_Entity.FindComponent(FactionAffiliationComponent));
		if (!targetFactionComp)
			return;

		FactionKey factionkey = targetFactionComp.GetAffiliatedFaction().GetFactionName();
		switch (factionkey)
		{
			case "OPFOR": ActivateEntityState(ENameTagEntityState.RED); break;
			case "BLUFOR": ActivateEntityState(ENameTagEntityState.BLUE); break;
			case "INDFOR": ActivateEntityState(ENameTagEntityState.GREEN); break;
		}
	}
}