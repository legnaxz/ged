#ifndef  __VASEXTRA_H__
#define  __VASEXTRA_H__


#include  "mythread.h"
#include  "posixsem.h"


class CVasExtra: public CMyThread, CPosixSem
{
public:
	CVasExtra();
	~CVasExtra();

	void	PfsChange(char cPfsPortSet);
	void	SetVasExtraServerInfo(int index, char *pServerIpExtra, unsigned short usServerPortExtra);

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

private:
	int		m_mainIndex, m_index;
	char	m_szVasServerIpExtra[20];
	unsigned short	m_usVasServerPortExtra;
};

extern CVasExtra	*g_vasExtra;

#endif // __VASEXTRA_H__
