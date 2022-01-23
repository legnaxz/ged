#ifndef  __CTIIF_HANDLER_H__
#define  __CTIIF_HANDLER_H__


#include  "mythread.h"


typedef struct
{
	// ConnectionCallID
	// ConnectionDeviceID;

	/******************************************/
	/* Information to be delivered to the VAS */
	/******************************************/
//	PKT_VAS_MSG		pkt_vas_msg;
	// CallKey
	// ANI
	// DNIS
	// Agent ID
	// Agent Extension
	// Agent IP address
	// Caller IP address, which is normally CUBE IP address and port

} INFO_TO_VAS;


typedef struct
{
	char	cEventType;
	char	cCID[30];
	char	cCallkey[30];
	char	cAgentId[30];
	char	cAgentExt[30];
	char	cAgentIp[30];
	int		nTwoRtpStarted, nTwoRtpStopped;

	// 2020.05 OB Starts
	int		nInboundCall;
	// 2020.05 OB Ends
	// 2020.06 OB Starts
	int		nInitialCallEstablished;
	// 2020.06 OB Ends

} INFO_FOR_VAS;

class CCtiIfHandler : public CMyThread
{
public:
	CCtiIfHandler(int index);
	~CCtiIfHandler();
	void	Run();

private:
	// 2020.05 OB Starts
	int		m_nInboundCall;
	static void GenerateCallKey(char *pCallkey);
	static int m_nCallKeySeqNo;
	static pthread_mutex_t		m_callkeyMutex;
	// 2020.05 OB Ends

private:
	bool	m_bStopFlag;
	int		m_mainIndex, m_index;
	std::map<UINT, INFO_FOR_VAS>	m_mapCDR;
//	INFO_FOR_VAS	m_ifv;
	void	SendScsEventToVasQueue(UINT ConnectionCallID, char cEventType);

private:
	void	OnFailureConf(void *pData, UINT size);
	void	OnFailureEvent(void *pData, UINT size);
	void	OnCloseConf(void *pData, UINT size);

private:
	void	PrintFloating(void *pData, UINT size);

	void	PrintOpenConfEventFloatingFields(void *pData, UINT size);
	void	PrintAgentPreCallEventFloatingFields(void *pData, UINT size);
	void	PrintBeginCallEventFloatingFields(void *pData, UINT size, INFO_FOR_VAS *pifv);
	void	PrintEndCallEventFloatingFields(void *pData, UINT size);
	void	PrintCallOriginatedEventFloatingFields(void *pData, UINT size);
	void	PrintCallTransferredEventFloatingFields(void *pData, UINT size);
	void	PrintCallHeldEventFloatingFields(void *pData, UINT size);
	// 2020.06 OB Starts - Modification
	void	PrintCallEstablishedEventFloatingFields(void *pData, UINT size, char *pCalledDeviceID=NULL);
	// 2020.06 OB Ends - Modification
	void	PrintCallConnectionClearedEventFloatingFields(void *pData, UINT size);
	void	PrintCallClearedEventFloatingFields(void *pData, UINT size);
	void	PrintCallDeliveredEventFloatingFields(void *pData, UINT size);
	void	PrintAgentStateEventFloatingFields(void *pData, UINT size);
	// 2020-02-11 starts here
	// 2020.05 OB Starts - Modification
	void	PrintRTPStartedEventFloatingFields(void *pData, UINT size, INFO_FOR_VAS *pifv, USHORT Direction, char *pip, char *pAgentExt);
	// 2020.05 OB Ends - Modification
	void	PrintRTPStoppedEventFloatingFields(void *pData, UINT size, char *pip);
	// 2020-02-11 starts here

private:
	void	SetMesssageHeader(MHDR *pmhdr, UINT ml, UINT mt);
	void	Parse(UINT mt, void *pData, UINT size);

private:
	void	OnAgentPreCallEvent(void *pData, UINT size);
	void	OnBeginCallEvent(void *pData, UINT size);
	void	OnCallDataUpdateEvent(void *pData, UINT size);
	void	OnCallEstablishedEvent(void *pData, UINT size);
	void	OnCallOriginatedEvent(void *pData, UINT size);
	void	OnCallTransferredEvent(void *pData, UINT size);
	void	OnCallHeldEvent(void *pData, UINT size);
	void	OnCallConnectionClearedEvent(void *pData, UINT size);
	void	OnCallClearedEvent(void *pData, UINT size);
	void	OnCallDeliveredEvent(void *pData, UINT size);
	void	OnAgentStateEvent(void *pData, UINT size);
	void	OnEndCallEvent(void *pData, UINT size);
	void	OnRTPStartedEvent(void *pData, UINT size);
	void	OnRTPStoppedEvent(void *pData, UINT size);
	void	OnSystemEvent(void *pData, UINT size);
};


extern CCtiIfHandler	*g_ctiIfHandler[MAX_CTI_SERVER_COUNT];


#endif		// end of __CTIIF_HANDLER_H__
