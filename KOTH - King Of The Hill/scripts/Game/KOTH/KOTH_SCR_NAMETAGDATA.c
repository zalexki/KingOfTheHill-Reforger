modded class SCR_NameTagData
{

	override protected void InitData(SCR_NameTagConfig config)
	{
		super.InitData(config);
		if (m_Entity)
		{
			FactionAffiliationComponent targetFactionComp = FactionAffiliationComponent.Cast(m_Entity.FindComponent(FactionAffiliationComponent));
			if (targetFactionComp) 
			{
				Faction faction = targetFactionComp.GetAffiliatedFaction();
				Print(faction.GetFactionKey());
				
				ActivateEntityState(ENameTagEntityState.RED);
			}
		}
	}
}