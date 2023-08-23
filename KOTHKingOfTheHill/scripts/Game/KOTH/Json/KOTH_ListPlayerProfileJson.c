class KOTH_ListPlayerProfileJson : JsonApiStruct
{
	ref array<ref KOTH_PlayerProfileJson> m_list = new array<ref KOTH_PlayerProfileJson>;

	void KOTH_ListPlayerProfileJson()
	{
		RegV("m_list");
	}
}
