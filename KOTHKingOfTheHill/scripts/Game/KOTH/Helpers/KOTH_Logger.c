typedef func Log;
void Log(string log, LogLevel level = LogLevel.NORMAL)
{
	Print("----- KOTH: "+log, level);
}
void Log(int log, LogLevel level = LogLevel.NORMAL)
{
	Print("----- KOTH: "+log, level);
}
void Log(Managed log, LogLevel level = LogLevel.NORMAL)
{
	Print("----- KOTH: "+log, level);
}