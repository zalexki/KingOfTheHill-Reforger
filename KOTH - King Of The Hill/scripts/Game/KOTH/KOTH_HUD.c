class KOTH_HUD : SCR_InfoDisplay
{
	TextWidget m_blueforPointsText;
	TextWidget m_greenforPointsText;
	TextWidget m_redforPointsText;
	
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		
		Widget root = GetRootWidget();
		if (root) {
			
	        FrameWidget frameWidget = FrameWidget.Cast(root.FindWidget("KOTH_HUB"));
			
	        m_blueforPointsText = TextWidget.Cast(frameWidget.FindWidget("HorizontalLayout0.BlueforPoints"));
			m_greenforPointsText = TextWidget.Cast(frameWidget.FindWidget("HorizontalLayout0.GreenforPoints"));
			m_redforPointsText = TextWidget.Cast(frameWidget.FindWidget("HorizontalLayout0.RedforPoints"));
			
			m_blueforPointsText.SetText("0");
			m_greenforPointsText.SetText("0");
			m_redforPointsText.SetText("0");
		}
	}
	
	protected override event void UpdateValues(IEntity owner, float timeSlice)
	{
		super.UpdateValues(owner, timeSlice);
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		m_blueforPointsText.SetText(gameMode.m_blueforPoints.ToString());
		m_greenforPointsText.SetText(gameMode.m_greenforPoints.ToString());
		m_redforPointsText.SetText(gameMode.m_redforPoints.ToString());
	}
}