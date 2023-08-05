/*class KothLogger 
{
	public static void Log(string log)
	{
		Print("KOTH: "+log, LogLevel.NORMAL);
	}
}*/

typedef func Log;
void Log(string log)
{
	Print("KOTH: "+log, LogLevel.NORMAL);
}

/*
public static void LogInformation(string log)
{
    Print("KOTH: "+log, LogLevel.NORMAL);
}

public static void LogError(string log)
{
    Print("KOTH: "+log, LogLevel.ERROR);
}

public static void LogDebug(string log)
{
    Print("KOTH: "+log, LogLevel.DEBUG);
}*/