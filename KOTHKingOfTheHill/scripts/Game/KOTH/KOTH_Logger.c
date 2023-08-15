typedef func Log;
void Log(string log, LogLevel level = LogLevel.NORMAL)
{
	Print("KOTH: "+log, level);
}