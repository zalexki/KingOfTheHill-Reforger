modded enum ChimeraMenuPreset 
{
    	KOTH_ShopGun
};

class KOTH_ShopPerformAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (!scrPlayerController || pUserEntity != scrPlayerController.GetMainEntity()) 
            return;
		
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.KOTH_ShopGun);
	}
};