[ComponentEditorProps(category: "GameScripted/KOTH", description: "map markers")]
class KOTH_SCR_MapDescriptorComponentClass : SCR_MapDescriptorComponentClass{};
class KOTH_SCR_MapDescriptorComponent : SCR_MapDescriptorComponent
{
	[RplProp(onRplName: "OnAreaUpdated")]
	private string m_areaState = "none";
	
	protected MapItem m_item;

	void KOTH_SCR_MapDescriptorComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		GetGame().GetCallqueue().CallLater(CreateMarker, 100, false);
	}

	void CreateMarker()
	{
		m_item = Item();

		m_item.SetBaseType(EMapDescriptorType.MDT_ICON);
		m_item.SetImageDef("KOTH_Zone");
		m_item.SetPriority(2);

		MapDescriptorProps props = m_item.GetProps();
		props.SetDetail(96);
		props.SetIconSize(256, 1, 1);
		props.SetFrontColor(Color.Gray75);
		props.SetTextVisible(false);
		props.SetIconVisible(true);
		props.Activate(true);

		m_item.SetProps(props);
	}
	
	void SetState(string state)
	{
		m_areaState = state;
		Replication.BumpMe();
	}

	string GetState()
	{
		return m_areaState;
	}

	void OnAreaUpdated()
	{
		ChangeMarker(m_areaState);
	}
	
	void ChangeMarker(string factionName)
	{
		MapDescriptorProps props = m_item.GetProps();

		Color color;
		switch (factionName)
		{
			case "none": color = color.Gray75; break;
			case "contested": color = color.Violet; break;
			case KOTH_Faction.OPFOR: color = color.DarkRed; break;
			case KOTH_Faction.BLUFOR: color = color.DarkBlue; break;
			case KOTH_Faction.INDFOR: color = color.DarkGreen; break;
		}
		color.SetA(0.6);
		props.SetFrontColor(color);
	}
}
