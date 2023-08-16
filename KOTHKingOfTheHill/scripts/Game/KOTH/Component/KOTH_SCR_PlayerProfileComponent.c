class KOTH_SCR_PlayerProfileComponentClass : ScriptComponentClass {}

class KOTH_SCR_PlayerProfileComponent : ScriptComponent
{
	private int m_money = 0;
	private int m_level = 1;
	private int m_xp = 0;
	
	void AddInZoneXpAndMoney()
	{
		AddXp(10);
		AddMoney(10);
	}
	
	void AddKillXpAndMoney()
	{
		AddXp(100);
		AddMoney(100);
	}
	
	int GetMoney()
	{
		return m_money;
	}
	
	int AddMoney(int amount)
	{
		m_money = m_money + amount;
		return m_money;
	}

	int AddXp(int amount)
	{
		m_xp = m_xp + amount;
		
		if (m_xp >= GetXpNextLevel()) {
			m_level++;
			m_xp = 0;
		}
		
		return m_xp;
	}
	
	int GetXp()
	{
		return m_xp;
	}
	
	int GetXpNextLevel()
	{
		if (m_level == 0) {
			return 100;
		}
		
		return (m_level + m_level - 1) * 100;
	}
	
	int GetLevel()
	{
		return m_level;
	}
}