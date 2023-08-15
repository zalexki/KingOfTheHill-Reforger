[ComponentEditorProps(category: "GameScripted/KOTH", description: "map markers.")]
class KOTH_SCR_MapDescriptorComponentClass : SCR_MapDescriptorComponentClass
{
};

class KOTH_SCR_MapDescriptorComponent : SCR_MapDescriptorComponent
{
	protected MapItem m_item;

	void KOTH_SCR_MapDescriptorComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		GetGame().GetCallqueue().CallLater(createMarker, 100, false);
	}

	void createMarker()
	{
		m_item = Item();

		m_item.SetBaseType(EMapDescriptorType.MDT_ICON);
		m_item.SetImageDef("KOTH_Zone");
		m_item.SetPriority(1);

		MapDescriptorProps props = m_item.GetProps();
		props.SetDetail(96);
		props.SetIconSize(256, 1, 1);
		props.SetFrontColor(Color.Gray25);
		props.SetTextVisible(false);
		props.SetIconVisible(true);
		props.Activate(true);
	
		m_item.SetProps(props);
	}
	
	 //------------------------------------------------------------------------------------------------
    void changeMarker(FactionKey factionkey)
    {
        MapDescriptorProps props = m_item.GetProps();
        
        Color color;
        switch (factionkey)
        {
            case "USSR": color = color.DarkRed; break;
            case "US": color = color.DarkBlue; break;
            case "FIA": color = color.DarkGreen; break;
        }
        color.SetA(0.7);
        props.SetFrontColor(color);
    }
};
