class KOTH_ScoringGameModeComponentClass : SCR_BaseGameModeComponentClass {}
class KOTH_ScoringGameModeComponent : SCR_BaseGameModeComponent
{
	[RplProp()]
    int m_blueforPoints = 0;

	[RplProp()]
    int m_redforPoints = 0;

	[RplProp()]
    int m_greenforPoints = 0;
	
	void BumpMe()
	{
		Replication.BumpMe();
	}
	
	override bool RplSave(ScriptBitWriter writer)
    {
		super.RplSave(writer);
		
        writer.WriteInt(m_blueforPoints);
        writer.WriteInt(m_redforPoints);
        writer.WriteInt(m_greenforPoints);

		return true;
    }
	
	override bool RplLoad(ScriptBitReader reader)
    {
		reader.ReadInt(m_blueforPoints);
		reader.ReadInt(m_redforPoints);
		reader.ReadInt(m_greenforPoints);
        return true;
    }
}