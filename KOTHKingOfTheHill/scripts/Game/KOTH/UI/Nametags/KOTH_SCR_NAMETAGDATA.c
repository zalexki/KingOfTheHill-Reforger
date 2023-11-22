modded enum ENameTagEntityState
{
	RED 	= 1<<7,
	BLUE 	= 1<<8,
	GREEN 	= 1<<9,
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

		Faction faction = targetFactionComp.GetAffiliatedFaction();
		if (!faction)
			return;
		
		switch (faction.GetFactionName())
		{
			case KOTH_Faction.OPFOR: ActivateEntityState(ENameTagEntityState.RED); break;
			case KOTH_Faction.BLUFOR: ActivateEntityState(ENameTagEntityState.BLUE); break;
			case KOTH_Faction.INDFOR: ActivateEntityState(ENameTagEntityState.GREEN); break;
		}
	}
}
