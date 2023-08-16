class KOTH_PlayerProfileJson : JsonApiStruct
{
    string m_name;
    int m_money = 0;
	int m_level = 1;
	int m_xp = 0;

    void KOTH_PlayerProfileJson()
	{
		RegV("m_name");
		RegV("m_money");
		RegV("m_level");
		RegV("m_xp");
	}
}