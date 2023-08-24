[BaseContainerProps(configRoot:true)]
class KOTH_SCR_ShopAccessoryItemList
{
	[Attribute("", UIWidgets.Auto)]
	protected ref array<ref KOTH_SCR_ShopAccessoryItem> m_items;

	array<ref KOTH_SCR_ShopAccessoryItem> GetItems()
	{
		return m_items;
	}
}
