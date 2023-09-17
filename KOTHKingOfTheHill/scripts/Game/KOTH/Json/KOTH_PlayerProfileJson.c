class KOTH_PlayerProfileJson : JsonApiStruct
{
	string m_playerUID;
	string m_playerName;
	int m_playerId;
	
	protected int m_money = 1000;
	protected int m_level = 1;
	protected int m_xp = 0;

	int m_kills = 0;
	int m_deaths = 0;
	int m_friendlyKills = 0;
	
	ref array<string> m_unlockedItems = {};

	void KOTH_PlayerProfileJson()
	{
		RegV("m_playerUID");
		RegV("m_playerName");
		RegV("m_money");
		RegV("m_level");
		RegV("m_xp");

		RegV("m_kills");
		RegV("m_deaths");
		RegV("m_friendlyKills");
		RegV("m_unlockedItems");
	}

	void AddInZoneXpAndMoney()
	{
		AddXp(10);
		AddMoney(10);
	}

	void AddKillXpAndMoney()
	{
		AddXp(100);
		AddMoney(100);
		m_kills++;
	}
	
	void AddEndGameBonusXpAndMoney(int bonus)
	{
		AddXp(bonus);
		AddMoney(bonus);
	}

	void Buy(int price)
	{
		m_money -= price;
	}
	
	void Refund(int price)
	{
		m_money += price;
	}

	void RemoveFriendlyKillXpAndMoney()
	{
		m_money = m_money - 300;
		m_xp = m_xp - 300;
		
		if (m_xp < 0)
			m_xp = 0;
		
		m_friendlyKills++;
	}

	int GetMoney()
	{
		return m_money;
	}
	
	int GetXp()
	{
		return m_xp;
	}

	int GetXpNextLevel()
	{
		// TODO: up to 1000 for release
		return (m_level + m_level - 1) * 100;
	}

	int GetLevel()
	{
		return m_level;
	}

	int AddMoney(int amount)
	{
		m_money += amount;
		
		return m_money;
	}
	
	int RemoveMoney(int amount)
	{
		m_money -= amount;
		
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
	
	int AddLevel(int amount)
	{
		m_level += amount;
		
		return m_level;
	}
	
	int RemoveLevel(int amount)
	{
		m_level -= amount;
		
		return m_level;
	}
}
