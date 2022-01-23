#ifndef  __CTISOCKETIF_H__
#define  __CTISOCKETIF_H__


#include  "mythread.h"
#include  "mysocket.h"


class CCtiSocketIf : public CMyThread, CMySocket
{
public:
	CCtiSocketIf();
	~CCtiSocketIf();
	void	SetCtiServerInfo(int index, char *pServerIpActive, unsigned short usServerPortActive, char *pServerIpStandby, unsigned short usServerPortStandby);
	int		ConnectToCtiServer(char *pServerIp, unsigned short usServerPort);
	void	Run();

private :
	void	SetMesssageHeader(MHDR *pmhdr, UINT ml, UINT mt);
	UINT	GetInvokeID();
	void	ClientID(CHAR *pFloatingField, UCHAR *pFloatingFieldTotalSize, CHAR *pID, UCHAR length);
	void	ClientPassword(CHAR *pFloatingField, UCHAR *pFloatingFieldTotalSize, CHAR *pPassword, UCHAR length);
	int		SetOpenReqMsg(PS_OPEN_REQ ps_or);
	int		SetCloseReqMsg(PS_CLOSE_REQ ps_cr);
	int		SetHeartBeatReqMsg(PS_HEARTBEAT_REQ ps_hbr);

	void	OnFailureConf(void *pData, UINT size);
	void	OnFailureEvent(void *pData, UINT size);

	void	OnOpenConf(void *pData, UINT size);
	void	PrintOpenConfEventFloatingFields(void *pData, UINT size);

	void	OnHeartbeatConf(void *pData, UINT size);
	void	OnCloseConf(void *pData, UINT size);

private:
	char			m_szCtiServerIpActive[20], m_szCtiServerIpStandby[20];
	unsigned short	m_usCtiServerPortActive, m_usCtiServerPortStandby;

private:
	int		m_mainIndex, m_index;
	static UINT		m_InvokeID;
	static pthread_mutex_t		m_mutex;
};


extern CCtiSocketIf	*g_ctiSocketIf[MAX_CTI_SERVER_COUNT];


#endif		// end of __CTISOCKETIF_H__
