#ifndef  __VASIF_HANDLER_H__
#define  __VASIF_HANDLER_H__


#include  "mythread.h"
#include  "posixsem.h"


class CVasIfHandler : public CMyThread, CPosixSem
{
public:
	CVasIfHandler();
	~CVasIfHandler();

	void	PfsChange(char cPfsPortSet);
	void	SetVasChangeBack(int nChangeBackRequest=0);
	int		GetVasChangeBack();
	void	SetVasServerInfo(int index, char *pServerIp, unsigned short usServerPort);

	void Run();

	int		CreateQueueInterface(int semIndex);
	int		EnqueueMessage(void *pmsg);
	int		DestroyQueueInterface();
	void	GetSentTime(int *pday, int *pSentTime);
	int		DiffTime(int day, int sentTime);

private:
	int		enqueue(void *pmsg);
	int		GetQueueSize();
	int		dequeue(void *pmsg);

private:
	int		m_nQueueSize;
	int		m_nHead, m_nTail;
	pthread_mutex_t		m_queueMutex;

private:
	int		m_day, m_sentTime;
	int		m_nChangeBackRequest;

private:
	int		m_mainIndex, m_index;
	char	m_szVasServerIp[20];
	unsigned short	m_usVasServerPort;
};

extern CVasIfHandler	*g_vasIfHandler[MAX_VAS_SERVER_COUNT];

#endif		// __VASIF_HANDLER_H__
