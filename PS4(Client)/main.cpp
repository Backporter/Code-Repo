#define LOG_EXAMPLE
#define PRX_EXPORT extern "C" __declspec (dllexport) 

#include <video_out.h>
#include <pthread.h>
#include <cstdio>
#include <stdio.h>
#include <string>
#include <user_service.h>
#include <system_service.h>

#include "Log.h"
Log DebugLog;


int main(){
	DebugLog.Open(Log::Net, "192.168.1.29");
	DebugLog.WriteMessageToLog("Example: Example Runtime = [%x]", 1);
	while (true)
	{
		sceKernelSleep(1);
	}
}
