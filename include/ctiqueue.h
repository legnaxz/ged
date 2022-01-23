#ifndef  __CTIQUEUE_H__
#define  __CTIQUEUE_H__


#include  "posixsem.h"


typedef struct
{
	MHDR	mhdr;
	char	buf[5000];
} PKT_CTI_MSG, *PPKT_CTI_MSG;


class CCtiQueue : public CPosixSem
{
public:
	CCtiQueue();
	~CCtiQueue();
	int		CreateQueueInterface(int semIndex);
	int		EnqueueMessage(MHDR mhdr, char *pMsg);
	int		DequeueMessage(MHDR *pmhdr, char *pMsg);
	int		DestroyQueueInterface();

private:
	int		enqueue(MHDR mhdr, char *pMsg);
	int		GetQueueSize();
	int		dequeue(MHDR *pmhdr, char *pMsg);

private:
	int		m_mainIndex, m_index;
	int		m_nQueueSize;
	int		m_nHead, m_nTail;
	pthread_mutex_t		m_queueMutex;
};


extern CCtiQueue	g_ctiQueue[MAX_CTI_SERVER_COUNT];


#endif		// end of __CTIQUEUE_H__
