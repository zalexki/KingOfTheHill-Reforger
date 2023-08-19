class KOTH_ShopGunClass : ChimeraMenuBase {

	protected Widget m_wRoot;  
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		Widget HorizontalLayout1 = m_wRoot.FindAnyWidget("HorizontalLayout1");
		
		SCR_NavigationButtonComponent back = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Cancel", HorizontalLayout1);
		if (back)
			back.m_OnActivated.Insert(OnBack);

	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnBack() {
		
		GetGame().GetMenuManager().CloseMenu(this); 
		
	}
};