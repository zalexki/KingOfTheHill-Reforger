[BaseContainerProps(configRoot:true)]
class KOTH_ShopItemList
{
	[Attribute("", UIWidgets.Auto)]
	protected ref array<ref KOTH_ShopItem> m_items;

	array<ref KOTH_ShopItem> GetItems()
	{
		return m_items;
	}
}
