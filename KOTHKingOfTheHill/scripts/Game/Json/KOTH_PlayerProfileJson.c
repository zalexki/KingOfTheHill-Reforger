class KOTH_PlayerProfileJson : JsonApiStruct
{
    string m_name;
	int m_money = 0;
	int m_level = 1;
	int m_xp = 0;
	
    int m_kills = 0;
    int m_deaths = 0;
    int m_friendlyKills = 0;


    void KOTH_PlayerProfileJson()
	{
		RegV("m_name");
		RegV("m_money");
		RegV("m_level");
		RegV("m_xp");
		
		RegV("m_kills");
		RegV("m_deaths");
		RegV("m_friendlyKills");
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
	
	void BuyStuff(int amount)
	{
	 	m_money = m_money - amount;
	}
	
	void RemoveFriendlyKillXpAndMoney()
	{
		m_money = m_money - 300;
		m_xp = m_xp - 300;
		m_friendlyKills++;
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
		// TODO: up to 1000 for release
		return (m_level + m_level - 1) * 100;
	}
	
	int GetLevel()
	{
		return m_level;
	}

	bool RplSave(ScriptBitWriter writer)
    {
		writer.WriteString(m_name);
        writer.WriteInt(m_money);
		writer.WriteInt(m_level);
		writer.WriteInt(m_xp);
		return true;
    }

    bool RplLoad(ScriptBitReader reader)
    {
		reader.ReadString(m_name);
		reader.ReadInt(m_money);
		reader.ReadInt(m_level);
		reader.ReadInt(m_xp);
		return true;
	}
	
	
	// ## Encode/Decode
	// Encoding snapshot into a packet and decoding snapshot from a packet.
	// Packets need to be as small as possible, so this process tries to reduce the size as much as it can.
	// Knowing what range of values can certain variable have and encoding that range in minimum number of bits required is key.
	// If it is to assume the full range of values is needed, helpers that already implement those for different types can be used.

	static bool Extract(KOTH_PlayerProfileJson instance, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		// Fill a snapshot with values from an instance.
		snapshot.SerializeString(instance.m_name);
		snapshot.SerializeInt(instance.m_money);
		snapshot.SerializeInt(instance.m_level);
		snapshot.SerializeInt(instance.m_xp);

		return true;
	}

	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, KOTH_PlayerProfileJson instance)
	{
		// Fill an instance with values from snapshot.
		snapshot.SerializeString(instance.m_name);
		snapshot.SerializeInt(instance.m_money);
		snapshot.SerializeInt(instance.m_level);
		snapshot.SerializeInt(instance.m_xp);
		return true;
	}

	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet)
	{
		// Read values from snapshot, encode them into smaller representation, then
		// write them into packet.	
		snapshot.EncodeString(packet);
		snapshot.EncodeInt(packet);
		snapshot.EncodeInt(packet);
		snapshot.EncodeInt(packet);
	}

	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot)
	{
		// Read values from packet, decode them into their original representation,
		// then write them into snapshot.
		snapshot.DecodeString(packet);
		snapshot.DecodeInt(packet);
		snapshot.DecodeInt(packet);
		snapshot.DecodeInt(packet);
		return true;
	}

	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx)
	{
		// Compare two snapshots and determine whether they are the same.
		return lhs.CompareSnapshots(rhs, 4 + 4 + 4 + 12);
	}

	static bool PropCompare(KOTH_PlayerProfileJson instance, SSnapSerializerBase snapshot, ScriptCtx ctx)
	{
		// Determine whether current values in instance are sufficiently different from
		// an existing snapshot that it's worth creating new one.
		// For float or vector values, it is possible to use some threshold to avoid creating too
		// many snapshots due to tiny changes in these values.
		return snapshot.CompareInt(instance.m_money)
			&& snapshot.CompareInt(instance.m_level)
			&& snapshot.CompareInt(instance.m_xp)
			&& snapshot.CompareString(instance.m_name);
	}
}