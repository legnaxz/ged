#ifndef  __PFSIF_HANDLER_H__
#define  __PFSIF_HANDLER_H__


#include  "mythread.h"
#include  "mysocket.h"


class CPfsIfHandler : public CMyThread, CMySocket
{
public:
	CPfsIfHandler();
	~CPfsIfHandler();
	void	SetPfsServerInfo(int index, char cMatePfsPort, char *pServerIp, unsigned short usServerPort);
	void	Run();
	int		SendVasChange(char vasno);

private:
	int		ConnectToPfsServer(char *pServerIp, unsigned short usServerPort);
	int		SendHeartBeat();

private:
	char	m_cMatePfsPort;
	char	m_szPfsServerIp[20];
	unsigned short	m_usPfsServerPort;

private:
	int		m_mainIndex, m_index;
};


extern CPfsIfHandler	g_pfsIfHandler[2];


#endif		// end of __PFSIF_HANDLER_H__
