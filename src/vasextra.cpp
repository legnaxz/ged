#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <sys/types.h>
#include  <sys/time.h>
#include  <sys/poll.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <errno.h>
#include  <unistd.h>

#include  "cfg.h"
#include  "pktdef.h"
#include  "mysocket.h"
#include  "vasextra.h"
#include  "pfsifhandler.h"
#include  "log.h"
#include  "util.h"


#define  SEMAPHORE_NAME_VAS_EXTRA_QUEUE		"vasextraqueue"
#define  MAX_VAS_EXTRA_QUEUE_SIZE			700


static PKT_SCS_EVT_MSG	g_pkt_scs_evt_msg_queue[MAX_VAS_EXTRA_QUEUE_SIZE];
static char g_pfs_change = 0;


CVasExtra	*g_vasExtra;


CVasExtra::CVasExtra()
{
	m_usVasServerPortExtra = 0;

	m_mainIndex = 7;
	m_nQueueSize = 0;
	m_nHead = m_nTail = 0;
}

CVasExtra::~CVasExtra()
{
}

void CVasExtra::PfsChange(char cPfsPortSet)
{
	g_pfs_change = cPfsPortSet;

	PostSemaphore();
}

int CVasExtra::CreateQueueInterface(int semIndex)
{
	char	szSem[255];

	m_index = semIndex;
	sprintf(szSem, "%s.%d", SEMAPHORE_NAME_VAS_EXTRA_QUEUE, semIndex);
	CreateSemaphore(szSem, 0);

	if(pthread_mutex_init(&m_queueMutex, NULL) != 0)
	{
		fprintf(stderr, "can't initialize queue mutex!!! %s\n", strerror(errno));
		_exit(1);
	}

	return 0;
}

int CVasExtra::EnqueueMessage(void *pmsg)
{
	return enqueue(pmsg);
}

int CVasExtra::enqueue(void *pmsg)
{
	int	q_success;

	// lock
	if(pthread_mutex_lock(&m_queueMutex) != 0)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_lock returns an error, %s\n", strerror(errno));
		return -1;
	}

	if(GetQueueSize() < MAX_VAS_EXTRA_QUEUE_SIZE)
	{
		memcpy(&(g_pkt_scs_evt_msg_queue[m_nHead]), pmsg, sizeof(PKT_SCS_EVT_MSG));

		m_nHead ++;
		if(m_nHead == MAX_VAS_EXTRA_QUEUE_SIZE) m_nHead = 0;
//		m_nQueueSize = (m_nHead - m_nTail + MAX_VAS_EXTRA_QUEUE_SIZE) % MAX_VAS_EXTRA_QUEUE_SIZE;
		m_nQueueSize ++;

		LogPrintf(m_mainIndex, m_index, TM_INFO, "enqueue: m_nQueueSize=%d\n", m_nQueueSize);
		q_success = 1;
	}
	else
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "enqueue: Q FULL happened=%d\n", GetQueueSize());
		q_success = 0;
	}

	// unlock
	if(pthread_mutex_unlock(&m_queueMutex) != 0)
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_unlock returns an error, %s\n", strerror(errno));

	if(q_success) PostSemaphore();

	return 0;
}

int CVasExtra::GetQueueSize()
{
	return m_nQueueSize;
}

int CVasExtra::dequeue(void *pmsg)
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

		memcpy(pmsg, &(g_pkt_scs_evt_msg_queue[m_nTail]), sizeof(PKT_SCS_EVT_MSG));
		memset(&(g_pkt_scs_evt_msg_queue[m_nTail]), 0x0, sizeof(PKT_SCS_EVT_MSG));

		m_nTail ++;
		if(m_nTail == MAX_VAS_EXTRA_QUEUE_SIZE) m_nTail = 0;

//		m_nQueueSize = (m_nHead - m_nTail + MAX_VAS_EXTRA_QUEUE_SIZE) % MAX_VAS_EXTRA_QUEUE_SIZE;
		m_nQueueSize --;
		if(m_nQueueSize < 0) m_nQueueSize = 0;	// just for safety
	}

	LogPrintf(m_mainIndex, m_index, TM_INFO, "dequeue: m_nQueueSize=%d\n", m_nQueueSize);

	// unlock
	if(pthread_mutex_unlock(&m_queueMutex) != 0)
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_unlock returns an error, %s\n", strerror(errno));

	return ret;
}

int CVasExtra::DestroyQueueInterface()
{
	char	szSem[255];

	sprintf(szSem, "%s.%d", SEMAPHORE_NAME_VAS_EXTRA_QUEUE, m_index);
	DestroySemaphore(szSem);

	pthread_mutex_destroy(&m_queueMutex);
}

void CVasExtra::SetVasExtraServerInfo(int index, char *pServerIpExtra, unsigned short usServerPortExtra)
{
	strcpy(m_szVasServerIpExtra, pServerIpExtra);
	m_usVasServerPortExtra = usServerPortExtra;

	m_index = index;
}

static int SendHeartBeat(int sockfd, int mainIndex, int index)
{
	int	ret, size;
	PKT_HEALTH_CHECK_MSG msg, rsp;

	size = sizeof(PKT_HEALTH_CHECK_MSG);

	MakeHealthCheck(&msg);
	ret = send(sockfd, (char*) &msg, size, 0);
	if(ret <= 0)
	{
		LogPrintf(mainIndex, index, TM_CRITICAL, "Failed to send a Heartbeat to a VAS EXTRA server.\n");
		return ret;
	}
	ret = recv(sockfd, (char*) &rsp, size, 0);
	if(ret <= 0)
	{
		LogPrintf(mainIndex, index, TM_CRITICAL, "Failed to Receive a Heartbeat from a VAS EXTRA server.\n");
		return ret;
	}
	if(ret != size)
	{
		LogPrintf(mainIndex, index, TM_CRITICAL, "The size(%d) of received Heartbeat from VAS EXTRA is not the total size(%d).\n", ret, size);
		return -1;
	}
	if(rsp.cResultCode != 'Y' && rsp.cResultCode != 'y')
	{
		LogPrintf(mainIndex, index, TM_CRITICAL, "The response of received Heartbeat from VAS EXTRA is (%c).\n", rsp.cResultCode);
		return -1;
	}

	return 1;
}

typedef struct
{
	int		sockfd, connected;
	int		mainIndex, index;
	char	szVasExtraServerIp[30];
	unsigned short	usVasExtraServerPort;
} VAS_EXTRA_ARG;

void *ThreadVasExtraSocketWatcher(void *parg)
{
	char	buf[5000];
	int		sockfd, index, mainIndex;
	int		ret, timeout, nNumberOfFileDesriptors;
	int		day, sentTime;
	struct pollfd	poll_events;
	CMySocket		my_socket;
	VAS_EXTRA_ARG	*pva = (VAS_EXTRA_ARG *) parg;

	pva->connected = 0;

	index = pva->index;
	mainIndex = pva->mainIndex;

	timeout = cfg.nVasHBPeriod;
	nNumberOfFileDesriptors = 1;

	while(true)
	{
		if(my_socket.CreateSocket() == -1)
		{
			usleep(1000000);
			continue;
		}
		LogPrintf(mainIndex, index, TM_INFO, "Successfully created a socket for a server, %s:%d.\n", pva->szVasExtraServerIp, pva->usVasExtraServerPort);

		if(my_socket.ConnectToServer(pva->szVasExtraServerIp, pva->usVasExtraServerPort) == -1)
		{
			usleep(1000000);
			continue;
		}
		LogPrintf(mainIndex, index, TM_INFO, "Successfully connected to a server, %s:%d.\n", pva->szVasExtraServerIp, pva->usVasExtraServerPort);

		sockfd = my_socket.GetSocket();

		pva->sockfd = sockfd;
		pva->connected = 1;

		poll_events.fd      = sockfd;
		poll_events.events  = POLLIN | POLLERR | POLLHUP;
		poll_events.revents = 0;

		while(pva->connected == 1)
		{
			ret = poll((struct pollfd *) &poll_events, nNumberOfFileDesriptors, timeout);
			if(ret < 0)	// Error
			{
				if(errno != EINTR)
				{
					LogPrintf(mainIndex, index, TM_INFO, "Got unexpected poll socket error, %s from a VAS \n", strerror(errno));
				}
				continue;
			}
			else if(ret == 0)
			{
				LogPrintf(mainIndex, index, TM_INFO, "Sending a Heartbeat to VAS EXTRA server\n");

				g_vasExtra->GetSentTime(&day, &sentTime);
				if((ret = g_vasExtra->DiffTime(day, sentTime)) >= cfg.nVasHBPeriod / 1000)
				{
					if(SendHeartBeat(sockfd, mainIndex, index) <= 0)
					{
						pva->connected = 0;
						LogPrintf(mainIndex, index, TM_INFO, "Got unexpected socket error from a VAS server, %s\n", strerror(errno));
						my_socket.DestroySocket();
						break;
					}
				}
			}
			else if(ret > 0)
			{
				if(poll_events.revents & POLLIN)
				{
					ret = recv(sockfd, buf, sizeof(buf), 0);
					if(ret <= 0)
					{
						pva->connected = 0;
						LogPrintf(mainIndex, index, TM_INFO, "Got unexpected socket error from a VAS server, %s\n", strerror(errno));
						my_socket.DestroySocket();
						break;
					}
					PKT_PFS_CHANGE_MSG *p = (PKT_PFS_CHANGE_MSG *) buf;

					LogPrintf(mainIndex, index, TM_INFO, "received from VAS EXTRA (%c):%s\n", '1' + index, buf);
				}
			}
		}

		LogErrorPrintf(mainIndex, index, TM_INFO, "VAS EXTRA(%c) is disconnected!\n", '1' + index);
		usleep(1000000);
	}
}

void CVasExtra::GetSentTime(int *pday, int *pSentTime)
{
	struct timeval tv;
	struct tm	lt;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &lt);
	*pday = lt.tm_mday;
	*pSentTime = lt.tm_hour * 3600 + lt.tm_min * 60 + lt.tm_sec;
}

int CVasExtra::DiffTime(int day, int sentTime)
{
	printf("day=%d, sentTime=%d, m_day=%d, m_sentTime=%d\n", day, sentTime, m_day, m_sentTime);

	if(m_day == day)				return abs(sentTime - m_sentTime);
	else if(m_sentTime > sentTime)	return (24 * 3600 + sentTime - m_sentTime);
	else if(m_sentTime < sentTime)	return (24 * 3600 + m_sentTime - sentTime);
	else							return 1;
}

void CVasExtra::Run()
{
	bool	bQuit = false;
	int		size, ret;
	pthread_t		tid;
	VAS_EXTRA_ARG	va;
	PKT_SCS_EVT_MSG	msg;

	size = sizeof(PKT_SCS_EVT_MSG);

	strcpy(va.szVasExtraServerIp, m_szVasServerIpExtra);
	va.connected = 0;
	va.usVasExtraServerPort = m_usVasServerPortExtra;
	va.index = m_index;
	va.mainIndex = m_mainIndex;
	if(0 != pthread_create(&tid, NULL, ThreadVasExtraSocketWatcher, &va))
	{
		printf("pthread_create returns an error, %s\n", strerror(errno));
		exit(0);
	}
	pthread_detach(tid);

	while(va.connected == 0) usleep(500000);

	while (!bQuit)
	{
		WaitSemaphore();

		// dequeue
		if(g_pfs_change != 0)
		{
			PKT_PFS_CHANGE_MSG change, rsp;

			MakePfsChange(&change, g_pfs_change);
			if(send(va.sockfd, (char*)&change, sizeof(PKT_PFS_CHANGE_MSG), 0) == -1)
			{
				// request to VAS change to PFS...
				g_pfsIfHandler[0].SendVasChange('1' + m_index);
				g_pfsIfHandler[1].SendVasChange('1' + m_index);

				LogPrintf(m_mainIndex, m_index, TM_INFO, "VAS(%c) change message was sent to a PFS server before a PFS change request to a VAS server!\n", '1' + m_index);
			}
/*
			else
			{
				// This must be changed
				if(recv(va.sockfd, (char*)&rsp, sizeof(PKT_PFS_CHANGE_MSG), 0) == -1)
				{
					// request to VAS change to PFS...
					g_pfsIfHandler[0].SendVasChange('1' + m_index);
					g_pfsIfHandler[1].SendVasChange('1' + m_index);

					LogPrintf(m_mainIndex, m_index, TM_INFO, "VAS(%c) change message was sent to a PFS server before a PFS change request to a VAS server!\n", '1' + m_index);
				}
			}
*/
			GetSentTime(&m_day, &m_sentTime);

			LogPrintf(m_mainIndex, m_index, TM_INFO, "PFS change message was sent to a VAS EXTRA server!\n");
			g_pfs_change = 0x0;
		}
		else
		{
			ret = dequeue(&msg);
			if(ret != 0)
			{
				LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "dequeue has an error or an empty queue!\n");
				continue;
			}

			if(va.connected == 1)
			{
				if(send(va.sockfd, (char*)&msg, size, 0) == -1)
				{
					// request to VAS change to PFS...
					g_pfsIfHandler[0].SendVasChange('1' + m_index);
					g_pfsIfHandler[1].SendVasChange('1' + m_index);

					LogPrintf(m_mainIndex, m_index, TM_INFO, "VAS(%c) change message was sent to a PFS server!\n", '1' + m_index);
				}

				GetSentTime(&m_day, &m_sentTime);
			}
			else
			{
				LogPrintf(m_mainIndex, m_index, TM_INFO, "VAS EXTRA is not connected. Discard a dequeued msg!\n");
			}
		}
	}
}
