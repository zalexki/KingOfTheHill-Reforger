[BaseContainerProps(configRoot:true)]
class KOTH_SCR_ShopAccessoryItem : KOTH_SCR_ShopItem
{
	[Attribute("baguette", desc: "name")]
	string m_itemName;
	
	[Attribute("0", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(AccessoryCategory))]
	AccessoryCategory m_category;
	
	[Attribute(desc: "Prefab of the Arsenal item", params: "et")]
	ResourceName m_itemResource;
	
	[Attribute(desc: "Texture of the Prefab", params: "edds")]
	ResourceName m_textureResource;
		
	[Attribute(desc: "Mask of the Prefab", params: "edds")]
	ResourceName m_maskResource;

	[Attribute("1000", desc: "Price buy once")]
	int m_priceOnce;

	[Attribute("100000", desc: "Price buy permanent")]
	int m_pricePermanent;

	[Attribute("5", desc: "Level required to buy the item")]
	int m_level;
}

enum AccessoryCategory
{
	Optics,
	Muzzle,
	Flashlight
	
}
