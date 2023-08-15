[EntityEditorProps(category: "GameScripted/Koth", description: "Handles Map Markers.")]
class KOTH_SCR_MapMarkerHandlerComponentClass: ScriptComponentClass
{
};

class KOTH_SCR_MapMarkerHandlerComponent : ScriptComponent
{
	protected SCR_PlayerController m_playerController;
	protected SCR_MapEntity m_mapEntity;
	protected RplComponent m_rplComponent;
	protected SCR_BaseGameMode m_gameMode;
	
	protected float m_zoomValue;

	protected bool IsProxy()
	{
		return (m_rplComponent && m_rplComponent.IsProxy());
	}
	
	void ~KOTH_SCR_MapMarkerHandlerComponent()
	{
		m_mapEntity.GetOnMapOpen().Remove(OnPlayerMapOpen);
		m_mapEntity.GetOnMapZoom().Remove(OnPlayerMapZoom);
	}

	override void EOnInit(IEntity owner)
	{
		m_playerController = SCR_PlayerController.Cast(PlayerController.Cast(owner));
		
		if (!m_playerController)
		{
			Print("KOTH_SCR_MapMarkerHandlerComponent must be attached to PlayerController!", LogLevel.ERROR);
			return;
		}
		
		m_rplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
		
		m_mapEntity = SCR_MapEntity.GetMapInstance();

		m_mapEntity.GetOnMapOpen().Insert(OnPlayerMapOpen);
		m_mapEntity.GetOnMapZoom().Insert(OnPlayerMapZoom);
	}
	
	protected void OnPlayerMapOpen()
	{
		m_zoomValue = m_mapEntity.GetCurrentZoom();
	}

	protected void OnPlayerMapZoom()
	{
		float currentZoom = m_mapEntity.GetCurrentZoom(); // 0.0-20.0 ZOOM

		IEntity kothZone = m_gameMode.m_kothTrigger;
		if (!kothZone) 
		{
			Log("missing KOTH_PresenceTriggerEntity named 'koth_zone' in world", LogLevel.ERROR);
			return;
		}
		
		KOTH_SCR_MapDescriptorComponent zone = KOTH_SCR_MapDescriptorComponent.Cast(kothZone.FindComponent(KOTH_SCR_MapDescriptorComponent));
		MapItem item = zone.Item();
		if (item)
		{
			MapDescriptorProps props = item.GetProps();
			//props.SetIconSize(256, -currentZoom * 1.25, -currentZoom * 1.25);
			props.SetIconSize(256, -currentZoom * 0.40, -currentZoom * 0.40);
			item.SetProps(props);
		}
	}
};