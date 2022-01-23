#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <signal.h>
#include  <execinfo.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <fcntl.h>

#include  <map>

#include  "typedefs.h"
#include  "ged188ver.h"
#include  "log.h"
#include  "cfg.h"
#include  "vasifhandler.h"
#include  "vasextra.h"
#include  "ctiifhandler.h"
#include  "pfsifhandler.h"
#include  "ctiqueue.h"
#include  "ctisocketif.h"
#include  "tcpserver.h"
#include  "pktdef.h"
#include  "CKeepAlivedServer.h"
#include  "CKeepAlivedClient.h"


char		*g_pConfigFile;

// Signal handler....
void int_handler(int parm)
{
	LogPrintf(0, 0, TM_FATAL, "INTERUPT signal is invoked. parm = %d\n", parm);
	exit(1);
}

void kill_handler(int parm)
{
	LogPrintf(0, 0, TM_FATAL, "KILL signal is invoked. parm = %d\n", parm);
	// shutdown here
	exit(1);
}

void term_handler(int parm)
{
	LogPrintf(0, 0, TM_FATAL, "TERM signal is invoked. parm = %d\n", parm);
	// shutdown here
	exit(1);
}

void epipe_handler(int parm)
{
	LogPrintf(0, 0, TM_FATAL, "Broken PIPE happened. parm = %d\n", parm);
}

void handler(int sig)
{
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}

int main(int argc, char *argv[])
{
	int			i;
	char		*pConfigFile;
	extern char		g_szAppName[255];

	CTcpServer	*pTcpServer = new CTcpServer;
	CKeepAlivedServer *pKeepAlivedServer = new CKeepAlivedServer;


	if(argc == 2 && argv[1][0] == '-')
	{
		if(argv[1][1] == 'v' || argv[1][1] == 'V')
		{
			printf("=======================================================\n");
			printf("Copyright : %s\n", GED188_COPYRIGHT);
			printf("Version   : %s\n", GED188_VERSION);
			printf("Build date: %s\n", GED188_TIMESTAMP);
			printf("=======================================================\n");
		}
		return 0;
	}
	else if(argc == 2)
	{
		g_pConfigFile = pConfigFile = argv[1];
	}
	else
	{
		printf("usage: %s <config file>\n", argv[0]);
		return 0;
	}

	printf("=======================================================\n");
	printf("Copyright : %s\n", GED188_COPYRIGHT);
	printf("Version   : %s\n", GED188_VERSION);
	printf("Build date: %s\n", GED188_TIMESTAMP);
	printf("=======================================================\n");

	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);

	signal(SIGINT,  int_handler);
	signal(SIGKILL, kill_handler);
	signal(SIGTERM, term_handler);
	signal(SIGPIPE, epipe_handler);

	SetDefaultParms();
	LoadParameters(pConfigFile);

	strcpy(g_szAppName, argv[0]);
	InitializeLog(cfg.szLogDir, 10000000);
	MakeLogBaseDir();

	// 2020.05 OB Starts
	if(cfg.szScsCallKeyAreaPrefix[0] == 0x0)
	{
		printf("SCS Callkey prefix value for an area is not set!\n");
		return 0;
	}
	// 2020.05 OB Ends

	if(pTcpServer->CreateServer(14445) == -1) goto EXIT;
	pTcpServer->Start();

	if(strncmp(cfg.cScsMode, "A", 1) == 0){
		if(pKeepAlivedServer->CreateServer(9001) == -1) goto EXIT;
		LogPrintf(0, 0, TM_INFO, "SCS START MODE [ACTIVE]!!!\n");
		pKeepAlivedServer->Start();
	}
	else{

		CKeepAlivedClient *pKeepAlivedClient = new CKeepAlivedClient(cfg.szScsActiveServerIp, cfg.usScsActiveServerPort);
		while(true){

			if(pKeepAlivedClient->CreateSocket() == 0) {
				LogPrintf(0, 0, TM_INFO, "SCS START MODE [BACKUP]!!! {%d}\n", pKeepAlivedClient->retry);
				if(pKeepAlivedClient->ConnectServer() == 0) {

					while(1){
						if(!pKeepAlivedClient->HealthCheck()){
							pKeepAlivedClient->DestroySocket();
							break;
						}
						pKeepAlivedClient->retry = 0;
						sleep(5);
					}
				}
				else {
					LogPrintf(0, 0, TM_MAJOR, "SCS ACTIVE ConnectServer fail !!! {%d}\n", pKeepAlivedClient->retry);
					pKeepAlivedClient->DestroySocket();
				}
			}
			else {
				LogPrintf(0, 0, TM_MAJOR, "SCS ACTIVE CreateServer fail !!! {%d}\n", pKeepAlivedClient->retry);
				pKeepAlivedClient->DestroySocket();
			}
			pKeepAlivedClient->retry++;
			if (pKeepAlivedClient->retry >= 3){
				LogPrintf(0, 0, TM_MAJOR, "SCS ACTIVE CHANGE!!!\n");
				break;
			}
			sleep(5);
		}
	}



#if 0
	for(;;)
	{
		usleep(500000);	// 500 ms sleep
	}
#endif

	// PFS
#ifdef  PFS_ENABLED
	g_pfsIfHandler[0].SetPfsServerInfo(0, cfg.cPfsServerPortActiveMate, cfg.szPfsServerIpActive, cfg.usPfsServerPortActive);
	g_pfsIfHandler[0].Start();
	g_pfsIfHandler[1].SetPfsServerInfo(1, cfg.cPfsServerPortStandbyMate, cfg.szPfsServerIpStandby, cfg.usPfsServerPortStandby);
	g_pfsIfHandler[1].Start();
#endif

	// CTI Server
#ifdef  CTI_ENABLED
	for(i = 0; i < cfg.nCtiServerCount; i ++)
	{
		g_ctiQueue[i].CreateQueueInterface(i);


		g_ctiSocketIf[i] = new CCtiSocketIf;
		g_ctiSocketIf[i]->SetCtiServerInfo(i, cfg.szCtiServerIpActive[i], cfg.usCtiServerPortActive[i], cfg.szCtiServerIpStandby[i], cfg.usCtiServerPortStandby[i]);
		g_ctiSocketIf[i]->Start();

		g_ctiIfHandler[i] = new CCtiIfHandler(i);
		g_ctiIfHandler[i]->Start();
	}
#endif

	// VAS
#ifdef  VAS_ENABLED
	for(i = 0; i < cfg.nVasServerCount; i ++)
	{
		g_vasIfHandler[i] = new CVasIfHandler;
		g_vasIfHandler[i]->CreateQueueInterface(i);
		g_vasIfHandler[i]->SetVasServerInfo(i, cfg.szVasServerIp[i], cfg.usVasServerPort[i]);
		g_vasIfHandler[i]->Start();
	}

	// VAS Extra
	g_vasExtra = new CVasExtra;
	if(cfg.usVasServerPortExtra != 0)
	{
		g_vasExtra->CreateQueueInterface(0);
		g_vasExtra->SetVasExtraServerInfo(i, cfg.szVasServerIpExtra, cfg.usVasServerPortExtra);
		g_vasExtra->Start();
	}
#endif

#ifdef CTI_ENABLED
	g_ctiSocketIf[0]->Wait();
#endif
	pTcpServer->Wait();

EXIT:
	pTcpServer->DestroyServer();

#ifdef CTI_ENABLED
	for(i = 0; i < cfg.nCtiServerCount; i ++)
	{
		g_ctiQueue[i].DestroyQueueInterface();
	}
#endif

	#ifdef  VAS_ENABLED
	for(i = 0; i < cfg.nVasServerCount; i ++)
	{
		g_vasIfHandler[i]->DestroyQueueInterface();
	}

	delete g_vasExtra;
	delete [] g_vasIfHandler;
#endif

#ifdef CTI_ENABLED
	delete [] g_ctiSocketIf;
	delete [] g_ctiIfHandler;
#endif

	delete pTcpServer;

	return 0;
}
