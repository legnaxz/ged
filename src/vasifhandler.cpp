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
#include  "vasifhandler.h"
#include  "vasextra.h"
#include  "pfsifhandler.h"
#include  "log.h"
#include  "util.h"


#define  SEMAPHORE_NAME_VAS_QUEUE		"vasqueue"
#define  MAX_VAS_QUEUE_SIZE				100


static PKT_SCS_EVT_MSG	g_pkt_scs_evt_msg_queue[MAX_VAS_SERVER_COUNT][MAX_VAS_QUEUE_SIZE];
static char g_pfs_change[MAX_VAS_SERVER_COUNT] = { 0, };


CVasIfHandler	*g_vasIfHandler[MAX_VAS_SERVER_COUNT];


CVasIfHandler::CVasIfHandler()
{
	m_usVasServerPort = 0;
	m_nChangeBackRequest = 0;

	m_mainIndex = 3;
	m_nQueueSize = 0;
	m_nHead = m_nTail = 0;
}

CVasIfHandler::~CVasIfHandler()
{
}

void CVasIfHandler::PfsChange(char cPfsPortSet)
{
	g_pfs_change[m_index] = cPfsPortSet;

	PostSemaphore();
}

int CVasIfHandler::CreateQueueInterface(int semIndex)
{
	char	szSem[255];

	m_index = semIndex;
	sprintf(szSem, "%s.%d", SEMAPHORE_NAME_VAS_QUEUE, semIndex);
	CreateSemaphore(szSem, 0);

	if(pthread_mutex_init(&m_queueMutex, NULL) != 0)
	{
		fprintf(stderr, "can't initialize queue mutex!!! %s\n", strerror(errno));
		_exit(1);
	}

	return 0;
}

int CVasIfHandler::EnqueueMessage(void *pmsg)
{
	return enqueue(pmsg);
}

int CVasIfHandler::enqueue(void *pmsg)
{
	int	q_success;

	// lock
	if(pthread_mutex_lock(&m_queueMutex) != 0)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_lock returns an error, %s\n", strerror(errno));
		return -1;
	}

	if(GetQueueSize() < MAX_VAS_QUEUE_SIZE)
	{
		memcpy(&(g_pkt_scs_evt_msg_queue[m_index][m_nHead]), pmsg, sizeof(PKT_SCS_EVT_MSG));

		m_nHead ++;
		if(m_nHead == MAX_VAS_QUEUE_SIZE) m_nHead = 0;
//		m_nQueueSize = (m_nHead - m_nTail + MAX_VAS_QUEUE_SIZE) % MAX_VAS_QUEUE_SIZE;
		m_nQueueSize ++;

		LogPrintf(m_mainIndex, m_index, TM_INFO, "enqueue of VasIfHandler: m_nQueueSize=%d\n", m_nQueueSize);
		q_success = 1;
	}
	else
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "enqueue of VasIfHandler: Q FULL happened=%d\n", GetQueueSize());
		q_success = 0;
	}

	// unlock
	if(pthread_mutex_unlock(&m_queueMutex) != 0)
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_unlock returns an error, %s\n", strerror(errno));

	if(q_success) PostSemaphore();

	return 0;
}

int CVasIfHandler::GetQueueSize()
{
	return m_nQueueSize;
}

int CVasIfHandler::dequeue(void *pmsg)
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

		memcpy(pmsg, &(g_pkt_scs_evt_msg_queue[m_index][m_nTail]), sizeof(PKT_SCS_EVT_MSG));
		memset(&(g_pkt_scs_evt_msg_queue[m_index][m_nTail]), 0x0, sizeof(PKT_SCS_EVT_MSG));

		m_nTail ++;
		if(m_nTail == MAX_VAS_QUEUE_SIZE) m_nTail = 0;

//		m_nQueueSize = (m_nHead - m_nTail + MAX_VAS_QUEUE_SIZE) % MAX_VAS_QUEUE_SIZE;
		m_nQueueSize --;
		if(m_nQueueSize < 0) m_nQueueSize = 0;	// just for a safety
	}

	LogPrintf(m_mainIndex, m_index, TM_INFO, "dequeue of VasIfHandler: m_nQueueSize=%d, ret=%d\n", m_nQueueSize, ret);

	// unlock
	if(pthread_mutex_unlock(&m_queueMutex) != 0)
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "pthread_mutex_unlock returns an error, %s\n", strerror(errno));

	return ret;
}

int CVasIfHandler::DestroyQueueInterface()
{
	char	szSem[255];

	sprintf(szSem, "%s.%d", SEMAPHORE_NAME_VAS_QUEUE, m_index);
	DestroySemaphore(szSem);

	pthread_mutex_destroy(&m_queueMutex);
}

void CVasIfHandler::SetVasServerInfo(int index, char *pServerIp, unsigned short usServerPort)
{
	strcpy(m_szVasServerIp, pServerIp);
	m_usVasServerPort = usServerPort;

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
		LogPrintf(mainIndex, index, TM_CRITICAL, "Failed to send a Heartbeat to a VAS server.\n");
		return ret;
	}
	ret = recv(sockfd, (char*) &rsp, size, 0);
	if(ret <= 0)
	{
		LogPrintf(mainIndex, index, TM_CRITICAL, "Failed to Receive a Heartbeat from a VAS server.\n");
		return ret;
	}
	if(ret != size)
	{
		LogPrintf(mainIndex, index, TM_CRITICAL, "The size(%d) of received Heartbeat from VAS is not the total size(%d).\n", ret, size);
		return -1;
	}
	if(rsp.cResultCode != 'Y' && rsp.cResultCode != 'y')
	{
		LogPrintf(mainIndex, index, TM_CRITICAL, "The response of received Heartbeat from VAS is (%c).\n", rsp.cResultCode);
		return -1;
	}

	return 1;
}

typedef struct
{
	int		sockfd, connected, failedToConnect;
	int		mainIndex, index;
	char	szVasServerIp[30];
	unsigned short	usVasServerPort;
} VAS_ARG;

void *ThreadVasSocketWatcher(void *parg)
{
	char	buf[5000];
	bool	bConnected;
	int		sockfd, index, mainIndex;
	int		ret, timeout, nNumberOfFileDesriptors;
	int		day, sentTime, try_out_count;
	struct pollfd	poll_events;
	CMySocket		my_socket;
	VAS_ARG			*pva = (VAS_ARG *) parg;

	index = pva->index;
	mainIndex = pva->mainIndex;

	timeout = cfg.nVasHBPeriod;
	nNumberOfFileDesriptors = 1;

	while(true)
	{
		pva->connected = 0;
		pva->failedToConnect = 0;

		if(my_socket.CreateSocket() == -1)
		{
			usleep(1000000);
			continue;
		}
		LogPrintf(mainIndex, index, TM_INFO, "Successfully created a socket for a server, %s:%d.\n", pva->szVasServerIp, pva->usVasServerPort);

		if(my_socket.ConnectToServer(pva->szVasServerIp, pva->usVasServerPort) == -1)
		{
			usleep(1000000);
			continue;
		}
		LogPrintf(mainIndex, index, TM_INFO, "Successfully connected to a server, %s:%d.\n", pva->szVasServerIp, pva->usVasServerPort);

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
					LogPrintf(mainIndex, index, TM_INFO, "Got unexpected poll socket error, %s from a VAS.\n", strerror(errno));
				}
				continue;
			}
			else if(ret == 0)
			{
				LogPrintf(mainIndex, index, TM_INFO, "Sending a Heartbeat to VAS server\n");

				g_vasIfHandler[index]->GetSentTime(&day, &sentTime);
				if((ret = g_vasIfHandler[index]->DiffTime(day, sentTime)) >= cfg.nVasHBPeriod / 1000)
				{
					if(SendHeartBeat(sockfd, mainIndex, index) <= 0)
					{
						pva->connected = 0;
						pva->failedToConnect = 0;
						LogPrintf(mainIndex, index, TM_INFO, "Got unexpected socket error from a VAS server, %s\n", strerror(errno));
						my_socket.DestroySocket();

						bConnected = false;
						for(try_out_count = 0; try_out_count < 3; try_out_count ++)
						{
							// sleep 0.5 second
							usleep(500000);
							if(my_socket.CreateSocket() == 0)
							{
								if(my_socket.ConnectToServer(pva->szVasServerIp, pva->usVasServerPort) == 0)
								{
									bConnected = true;
									break;
								}
							}
						}
						if(try_out_count == 3 && bConnected == false)
						{
							// request the VAS change to PFS...
							g_pfsIfHandler[0].SendVasChange('1' + index);
							g_pfsIfHandler[1].SendVasChange('1' + index);

							LogPrintf(mainIndex, index, TM_INFO, "VAS(%c) change message was sent to a PFS server!\n", '1' + index);
							LogErrorPrintf(mainIndex, index, TM_INFO, "VAS(%c) change message was sent to a PFS server!\n", '1' + index);
							break;
						}

						sockfd = my_socket.GetSocket();

						pva->sockfd = sockfd;

						poll_events.fd      = sockfd;
						poll_events.events  = POLLIN | POLLERR | POLLHUP;
						poll_events.revents = 0;

						pva->connected = 1;
					}
				}
	//			printf("ret = %d, cfg.nVasHBPeriod=%d\n", ret, cfg.nVasHBPeriod);
			}
			else if(ret > 0)
			{
				if(poll_events.revents & POLLIN)
				{
					ret = recv(sockfd, buf, sizeof(buf), 0);
					if(ret <= 0)
					{
						pva->connected = 0;
						pva->failedToConnect = 0;
						LogPrintf(mainIndex, index, TM_INFO, "Got unexpected socket error from a VAS server, %s\n", strerror(errno));
						my_socket.DestroySocket();

						bConnected = false;
						for(try_out_count = 0; try_out_count < 3; try_out_count ++)
						{
							// sleep 0.5 second
							usleep(500000);
							if(my_socket.CreateSocket() == 0)
							{
								if(my_socket.ConnectToServer(pva->szVasServerIp, pva->usVasServerPort) == 0)
								{
									bConnected = true;
									break;
								}
							}
						}
						if(try_out_count == 3 && bConnected == false)
						{
							// request the VAS change to PFS...
							g_pfsIfHandler[0].SendVasChange('1' + index);
							g_pfsIfHandler[1].SendVasChange('1' + index);

							LogPrintf(mainIndex, index, TM_INFO, "VAS(%c) change message was sent to a PFS server!\n", '1' + index);
							LogErrorPrintf(mainIndex, index, TM_INFO, "VAS(%c) change message was sent to a PFS server!\n", '1' + index);
							break;
						}

						sockfd = my_socket.GetSocket();

						pva->sockfd = sockfd;

						poll_events.fd      = sockfd;
						poll_events.events  = POLLIN | POLLERR | POLLHUP;
						poll_events.revents = 0;

						pva->connected = 1;
					}
					else
					{
						PKT_PFS_CHANGE_MSG *p = (PKT_PFS_CHANGE_MSG *) buf;
						LogPrintf(mainIndex, index, TM_INFO, "received from VAS(%c):%s\n", '1' + index, buf);
					}
				}
			}
		}

		pva->failedToConnect = 1;
		LogErrorPrintf(mainIndex, index, TM_INFO, "VAS(%c) is disconnected!\n", '1' + index);

		// Waiting for a switchback request...
		while(g_vasIfHandler[index]->GetVasChangeBack() == 0)
		{
			usleep(1000000);
		}
		g_vasIfHandler[index]->SetVasChangeBack();
	}
}

void CVasIfHandler::SetVasChangeBack(int nChangeBackRequest)
{
	m_nChangeBackRequest = nChangeBackRequest;
}

int CVasIfHandler::GetVasChangeBack()
{
	return m_nChangeBackRequest;
}

void CVasIfHandler::GetSentTime(int *pday, int *pSentTime)
{
	struct timeval tv;
	struct tm	lt;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &lt);
	*pday = lt.tm_mday;
	*pSentTime = lt.tm_hour * 3600 + lt.tm_min * 60 + lt.tm_sec;
}

int CVasIfHandler::DiffTime(int day, int sentTime)
{
	printf("day=%d, sentTime=%d, m_day=%d, m_sentTime=%d\n", day, sentTime, m_day, m_sentTime);

	if(m_day == day)				return abs(sentTime - m_sentTime);
	else if(m_sentTime > sentTime)	return (24 * 3600 + sentTime - m_sentTime);
	else if(m_sentTime < sentTime)	return (24 * 3600 + m_sentTime - sentTime);
	else							return 1;
}

void CVasIfHandler::Run()
{
	bool	bQuit = false;
	int		size, ret;
	pthread_t		tid;
	VAS_ARG			va;
	PKT_SCS_EVT_MSG	msg;

	strcpy(va.szVasServerIp, m_szVasServerIp);
	va.connected = 0;
	va.failedToConnect = 0;
	va.usVasServerPort = m_usVasServerPort;
	va.index = m_index;
	va.mainIndex = m_mainIndex;

	size = sizeof(PKT_SCS_EVT_MSG);

	if(0 != pthread_create(&tid, NULL, ThreadVasSocketWatcher, &va))
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
		if(g_pfs_change[m_index] != 0)
		{
			PKT_PFS_CHANGE_MSG change, rsp;

			MakePfsChange(&change, g_pfs_change[m_index]);
			if(send(va.sockfd, (char*)&change, sizeof(PKT_PFS_CHANGE_MSG), 0) == -1)
			{
#if 0
				// wait

				// request the VAS change to PFS...
				g_pfsIfHandler[0].SendVasChange('1' + m_index);
				g_pfsIfHandler[1].SendVasChange('1' + m_index);

				LogPrintf(m_mainIndex, m_index, TM_INFO, "VAS(%c) change message was sent to a PFS server before a PFS change request to a VAS server!\n", '1' + m_index);
#endif
				LogPrintf(m_mainIndex, m_index, TM_INFO, "Failed to send PFS change message to a VAS server because of a failure of send() to VAS!\n");
			}
			else
			{
				LogPrintf(m_mainIndex, m_index, TM_INFO, "Successfully sent PFS change message to a VAS server!\n");
			}
			GetSentTime(&m_day, &m_sentTime);

			g_pfs_change[m_index] = 0x0;
		}
		else
		{
			ret = dequeue(&msg);
			if(ret != 0)
			{
				LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "dequeuing has an error or an empty queue!\n");
				continue;
			}

			LogPrintf(m_mainIndex, m_index, TM_INFO, "Sending to VAS: [%s]\n", (char*)&msg);

			// If my VAS is dead, forward the message request to an EXTRA VAS server.
			if(va.connected == 0 && va.failedToConnect == 1)
			{
				g_vasExtra->EnqueueMessage(&msg);
			}
			else if(va.connected == 0 && va.failedToConnect == 0)
			{
				// wait
				while(1)
				{
					if(va.connected == 0 && va.failedToConnect == 0) usleep(500000);
					if(va.connected == 1)
					{
						if(send(va.sockfd, (char*)&msg, size, 0) == -1)
						{
							LogPrintf(m_mainIndex, m_index, TM_INFO, "Failed to send a message to a VAS server because of a failure of send()!\n");
						}
						GetSentTime(&m_day, &m_sentTime);
						break;
					}
					if(va.connected == 0 && va.failedToConnect == 1)
					{
						g_vasExtra->EnqueueMessage(&msg);
						break;
					}
				}
			}
			else
			{
				if(send(va.sockfd, (char*)&msg, size, 0) == -1)
				{
#if 0
					// wait...

					// request the VAS change to PFS...
					g_pfsIfHandler[0].SendVasChange('1' + m_index);
					g_pfsIfHandler[1].SendVasChange('1' + m_index);

					LogPrintf(m_mainIndex, m_index, TM_INFO, "VAS(%c) change message was sent to a PFS server!\n", '1' + m_index);
#endif
					LogPrintf(m_mainIndex, m_index, TM_INFO, "Failed to send a message to a VAS server because of a failure of send()!\n");
				}

				GetSentTime(&m_day, &m_sentTime);
			}
		}
	}
}
