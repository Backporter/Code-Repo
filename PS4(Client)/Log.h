#include <cstdio>
#include <stdio.h>
#include <kernel.h>
#include <stdint.h>
#include <scetypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net.h>
#include <libnetctl.h>
#include <libhttp.h>
#include <libsysmodule.h>
#include <libnet\socket.h>
#include <libnet\sockinfo.h>


#include "Notify.h"

#ifndef LOG_H
#define LOG_H
class Log
{
public:
	Log();
	~Log();

	void Open(int FolderID, const char* RelitivePath);
	__hidden void OpenNet(const char* PCIP);
	void WriteMessageToLog(const char* MessageFMT, ...);
	enum 
	{
		App,
		Download,
		USB,
		Data,
		SaveData,
		Net,
	};

private:
	static FILE * LogFileDis;
	static SceNetId SERV_Socket;
	static bool UseNet;
	static int ret;
};
#endif