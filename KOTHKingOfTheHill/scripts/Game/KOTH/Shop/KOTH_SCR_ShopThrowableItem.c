[BaseContainerProps(configRoot:true)]
class KOTH_SCR_ShopThrowableItem : KOTH_SCR_ShopItem
{
	[Attribute("baguette", desc: "name")]
	string m_itemName;
	
	[Attribute("0", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(ThrowableCategory))]
	ThrowableCategory m_category;
	
	[Attribute(desc: "Prefab of the Arsenal item", params: "et")]
	ResourceName m_itemResource;
	
	[Attribute(desc: "Texture of the Prefab", params: "edds")]
	ResourceName m_textureResource;
		
	[Attribute(desc: "Mask of the Prefab", params: "edds")]
	ResourceName m_maskResource;
	
	[Attribute("1", desc: "Number of item added")]
	int m_itemNumber;

	[Attribute("1000", desc: "Price buy once")]
	int m_priceOnce;

	[Attribute("1", desc: "Level required to buy the item")]
	int m_level;
}

enum ThrowableCategory
{
	Grenade,
	Smoke,
	Mine
}
