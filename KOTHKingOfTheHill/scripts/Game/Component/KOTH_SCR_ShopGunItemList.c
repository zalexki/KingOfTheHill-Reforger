[BaseContainerProps(configRoot:true)]
class KOTH_SCR_ShopGunItemList
{
	[Attribute("", UIWidgets.Auto)]
	protected ref array<ref KOTH_SCR_ShopGunItem> m_items;
	
	array<ref KOTH_SCR_ShopGunItem> GetItems()
	{
		return m_items;
	}
};