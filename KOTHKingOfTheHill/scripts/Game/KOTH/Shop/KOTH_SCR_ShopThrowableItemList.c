[BaseContainerProps(configRoot:true)]
class KOTH_SCR_ShopThrowableItemList
{
	[Attribute("", UIWidgets.Auto)]
	protected ref array<ref KOTH_SCR_ShopThrowableItem> m_items;

	array<ref KOTH_SCR_ShopThrowableItem> GetItems()
	{
		return m_items;
	}
}
