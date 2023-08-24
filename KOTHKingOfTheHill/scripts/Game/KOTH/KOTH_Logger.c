typedef func Log;
void Log(string log, LogLevel level = LogLevel.NORMAL)
{
	Print("----- KOTH: "+log, level);
}
void Log(int log, LogLevel level = LogLevel.NORMAL)
{
	Print("----- KOTH: "+log, level);
}
// why u no work :(
void Log(typename log, LogLevel level = LogLevel.NORMAL)
{
	Print("----- KOTH: "+log, level);
}