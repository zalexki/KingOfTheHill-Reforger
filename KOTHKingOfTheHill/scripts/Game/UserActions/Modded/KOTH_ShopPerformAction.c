// Script File
modded enum ChimeraMenuPreset 
{
    	KOTH_ShopGun
};

class KOTH_ShopPerformAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.KOTH_ShopGun);
	}
};