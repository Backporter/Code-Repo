#include "Log.h"

SceNetId Log::SERV_Socket = NULL;
std::FILE * Log::LogFileDis = NULL;
bool Log::UseNet = false;
int Log::ret = 0;

int GetFolderPath(int FolderID, const char* RelitivePath, char* Buffer) {
	char savedata[] = "/savedata0/";
	char app0[] = "/app0/";
	char download0[] = "/download0/";
	char usb0[] = "/mnt/usb0/";
	char datalog[] = "/data/Logs/";

	switch (FolderID) {
	case Log::App:
		// pszPath = "/app0/";
		memcpy(Buffer, app0, sizeof(app0));
		if (!Buffer) {
			return -1;
		}
		return 0;
		break;
	case Log::Download:
		// pszPath = "/download0/";
		memcpy(Buffer, download0, sizeof(download0));
		if (!Buffer) {
			return -1;
		}
		break;
	case Log::USB:
		// pszPath = "/mnt/usb0/";
		memcpy(Buffer, usb0, sizeof(usb0));
		if (!Buffer) {
			return -1;
		}
		return 0;
		break;
	case Log::Data:
		// pszPath = "/data/Log/";
		memcpy(Buffer, datalog, sizeof(datalog));
		if (!Buffer) {
			return -1;
		}
		return 0;
		break;
	case Log::SaveData:
		// pszPath = "/savedata0/";
		memcpy(Buffer, savedata, sizeof(savedata));
		if (!Buffer) {
			return -1;
		}
		return 0;
		break;
	}
	return 0;
}

std::string getPathName(const std::string& s) {

	char sep = '/';
	size_t i = s.rfind(sep, s.length());
	if (i != std::string::npos) {
		return(s.substr(0, i + 1));
	}

	return("ERRROR");
}

Log::Log() {
	// 
}

void Log::Open(int FolderID, const char* RelitivePath) {

	if (FolderID == Log::Net)
	{
		UseNet = true;
		return OpenNet(RelitivePath);
	}

	char path[260];

	int err = GetFolderPath(FolderID, RelitivePath, path);
	if (err != 0) {
		Notify("GetFolderPath Failed To Copy Path To Buffer!");
	}
	
	strcat_s(path, sizeof(path), RelitivePath);

	// EX FULL PATH: /data/Logs/Jack/jack.log
	// remove "jack.log" and you got the top most directory we need to create

	const char* dir = getPathName(std::string(path)).c_str();

	sceKernelUnlink(dir);
	sceKernelMkdir(dir, 0000777);

	remove(path);
	LogFileDis = fopen(path, "w");
	if (!LogFileDis) {
		Notify("Failed to open path");
	}
}

void Log::OpenNet(const char* PCIP) {
	sceNetInit();

	int opt = 1;
	int ret;
	const char* ConnectionMessage = "PS4 Connected!";
	SceNetSockaddrIn serverAddr;

	if ((SERV_Socket = sceNetSocket("netdbg", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0)) < 0) {
		Notify("Socket creation error [%x]", SERV_Socket);
		return;
	}

	// Forcefully attaching socket to the port 8080
	if ((ret = sceNetSetsockopt(SERV_Socket, SCE_NET_SOL_SOCKET, SCE_NET_SO_KEEPALIVE, (char*)&opt, sizeof(int))) != 0)
	{
		Notify("Failed To Set SocketOPT! [%x]", ret);
	}

	memset(&serverAddr, 0, sizeof(SceNetSockaddrIn));
	serverAddr.sin_family = SCE_NET_AF_INET;
	serverAddr.sin_port = sceNetHtons(8081);

	if ((ret = sceNetInetPton(SCE_NET_AF_INET, PCIP, &serverAddr.sin_addr)) <= 0) {
		Notify("Invalid address/Address not supported [%x]", ret);
		return;
	}

	if ((ret = sceNetConnect(SERV_Socket, (SceNetSockaddr *)&serverAddr, sizeof(serverAddr))) < 0) {
		Notify("Invalid addressConnection Failed [%x]", ret);
		return;
	}

	if ((ret = sceNetSend(SERV_Socket, ConnectionMessage, strlen(ConnectionMessage), SCE_NET_MSG_DONTWAIT)) != strlen(ConnectionMessage)) {
		Notify("Failed to Send Connection Message! [%x]", ret);
		return;
	}
}

void Log::WriteMessageToLog(const char* MessageFMT, ...) {

	if (UseNet) {
		if (!SERV_Socket) {
			return;
		}
		char buffer[256] = { 0 };
		va_list args;
		va_start(args, MessageFMT);
		vsprintf(buffer, MessageFMT, args);
		va_end(args);
		if ((ret = sceNetSend(SERV_Socket, buffer, strlen(buffer), SCE_NET_MSG_DONTWAIT)) != strlen(buffer)) {
			Notify("Failed to Send Message. [%x]", ret);
		}
	}
	else {
		if (!LogFileDis) {
			return;
		}
		char buffer[256];
		va_list args;
		va_start(args, MessageFMT);
		vsprintf(buffer, MessageFMT, args);
		Notify(buffer);
		va_end(args);
		if (fputs(buffer, LogFileDis) >= 0) {
			fflush(LogFileDis);
		}
		if (fputs("\n", LogFileDis) >= 0) {
			fflush(LogFileDis);
		}
	}
}

Log::~Log() {
	if (LogFileDis) {
		fclose(LogFileDis);
	}

	if (UseNet && SERV_Socket) {
		sceNetSocketClose(SERV_Socket);
	}
}