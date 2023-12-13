class KOTH_ListPlayerProfileJson : JsonApiStruct
{
	ref array<ref KOTH_PlayerProfileJson> m_list = {};

	void KOTH_ListPlayerProfileJson()
	{
		RegV("m_list");
	}
}
