#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <errno.h>
#include  <unistd.h>
#include  <sys/socket.h>
#include  <arpa/inet.h>
#include  <sys/poll.h>
#include  <sys/time.h>
#include  <pthread.h>

#include  "typedefs.h"
#include  "cfg.h"
#include  "ctiqueue.h"
#include  "log.h"


#define  SEMAPHORE_NAME_CTI_QUEUE		"ctiqueue"
#define  MAX_CTI_QUEUE_SIZE				200


static PKT_CTI_MSG	g_pkt_cti_msg_queue[MAX_CTI_SERVER_COUNT][MAX_CTI_QUEUE_SIZE];


CCtiQueue	g_ctiQueue[MAX_CTI_SERVER_COUNT];


CCtiQueue::CCtiQueue()
{
	m_mainIndex = 1;
	m_nQueueSize = 0;
	m_nHead = m_nTail = 0;
	m_queueMutex = PTHREAD_MUTEX_INITIALIZER;
}

CCtiQueue::~CCtiQueue()
{
}

int CCtiQueue::CreateQueueInterface(int semIndex)
{
	char	szSem[255];

	m_index = semIndex;
	sprintf(szSem, "%s.%d", SEMAPHORE_NAME_CTI_QUEUE, semIndex);
	CreateSemaphore(szSem, 0);

	if(pthread_mutex_init(&m_queueMutex, NULL) != 0)
	{
		fprintf(stderr, "can't initialize queue mutex!!! %s\n", strerror(errno));
		_exit(1);
	}

	return 0;
}

int CCtiQueue::EnqueueMessage(MHDR mhdr, char *pMsg)
{
	return enqueue(mhdr, pMsg);
}

int CCtiQueue::enqueue(MHDR mhdr, char *pMsg)
{
	int	q_success;

	// lock
	if(pthread_mutex_lock(&m_queueMutex) != 0)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_lock returns an error, %s\n", strerror(errno));
		return -1;
	}

	if(GetQueueSize() < MAX_CTI_QUEUE_SIZE)
	{
		g_pkt_cti_msg_queue[m_index][m_nHead].mhdr = mhdr;
		memcpy(g_pkt_cti_msg_queue[m_index][m_nHead].buf, pMsg, mhdr.MessageLength);

		m_nHead ++;
		if(m_nHead == MAX_CTI_QUEUE_SIZE) m_nHead = 0;
//		m_nQueueSize = (m_nHead - m_nTail + MAX_CTI_QUEUE_SIZE) % MAX_CTI_QUEUE_SIZE;
		m_nQueueSize ++;

		LogPrintf(m_mainIndex, m_index, TM_INFO, "enqueue of CtiQueue: m_nQueueSize=%d\n", m_nQueueSize);
		q_success = 1;
	}
	else
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "enqueue of CtiQueue: Q FULL happened=%d\n", GetQueueSize());
		q_success = 0;
	}

	// unlock
	if(pthread_mutex_unlock(&m_queueMutex) != 0)
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_unlock returns an error, %s\n", strerror(errno));

	if(q_success) PostSemaphore();

	return 0;
}

int CCtiQueue::GetQueueSize()
{
	return m_nQueueSize;
}

int CCtiQueue::DequeueMessage(MHDR *pmhdr, char *pMsg)
{
	return dequeue(pmhdr, pMsg);
}

int CCtiQueue::dequeue(MHDR *pmhdr, char *pMsg)
{
	int		ret = -1;

	// lock
	if(pthread_mutex_lock(&m_queueMutex) != 0)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_lock returns an error, %s\n", strerror(errno));
		return ret;
	}

	if(GetQueueSize() > 0)
	{
		ret = 0;
		*pmhdr = g_pkt_cti_msg_queue[m_index][m_nTail].mhdr;
		memcpy(pMsg, g_pkt_cti_msg_queue[m_index][m_nTail].buf, pmhdr->MessageLength);

		memset(&(g_pkt_cti_msg_queue[m_index][m_nTail]), 0x0, sizeof(g_pkt_cti_msg_queue[m_index][m_nTail]));

		m_nTail ++;
		if(m_nTail == MAX_CTI_QUEUE_SIZE) m_nTail = 0;

//		m_nQueueSize = (m_nHead - m_nTail + MAX_CTI_QUEUE_SIZE) % MAX_CTI_QUEUE_SIZE;
		m_nQueueSize --;
		if(m_nQueueSize < 0) m_nQueueSize = 0;	// just for safety
	}

	LogPrintf(m_mainIndex, m_index, TM_INFO, "dequeue of CtiQueue: m_nQueueSize=%d, ret=%d\n", m_nQueueSize, ret);

	// unlock
	if(pthread_mutex_unlock(&m_queueMutex) != 0)
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_unlock returns an error, %s\n", strerror(errno));

	return ret;
}

int CCtiQueue::DestroyQueueInterface()
{
	char	szSem[255];

	sprintf(szSem, "%s.%d", SEMAPHORE_NAME_CTI_QUEUE, m_index);
	DestroySemaphore(szSem);

	pthread_mutex_destroy(&m_queueMutex);
}
