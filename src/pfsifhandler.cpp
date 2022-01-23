#include  <stdio.h>
#include  <string.h>
#include  <unistd.h>
#include  <errno.h>
#include  <sys/poll.h>

#include  "cfg.h"
#include  "pktdef.h"
#include  "pfsifhandler.h"
#include  "vasifhandler.h"
#include  "vasextra.h"
#include  "log.h"
#include  "cfg.h"


CPfsIfHandler	g_pfsIfHandler[2];

CPfsIfHandler::CPfsIfHandler()
{

}

CPfsIfHandler::~CPfsIfHandler()
{

}

void CPfsIfHandler::SetPfsServerInfo(int index, char cMatePfsPort, char *pServerIp, unsigned short usServerPort)
{
	strcpy(m_szPfsServerIp, pServerIp);
	m_usPfsServerPort = usServerPort;

	m_index = index;
	m_mainIndex = 9;
	m_cMatePfsPort = cMatePfsPort;
}

int CPfsIfHandler::SendVasChange(char vasno)
{
	int	ret;
	PKT_VAS_CHANGE_MSG	msg, rsp;

	MakeVasChange(&msg, vasno);
	ret = Send((char*) &msg, sizeof(PKT_VAS_CHANGE_MSG));
	if(ret <= 0)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "Failed to send a VAS change request to a server, %s:%d.\n", m_szPfsServerIp, m_usPfsServerPort);
		return ret;
	}
	ret = Receive((char*) &rsp, sizeof(PKT_VAS_CHANGE_MSG));
	if(ret <= 0)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "Failed to Receive a VAS change response from a server, %s:%d.\n", m_szPfsServerIp, m_usPfsServerPort);
		return ret;
	}

	return 0;
}

int CPfsIfHandler::SendHeartBeat()
{
	int	ret, size;
	PKT_HEALTH_CHECK_MSG msg, rsp;

	size = sizeof(PKT_HEALTH_CHECK_MSG);

	MakeHealthCheck(&msg);
	ret = Send((char*) &msg, size);
	if(ret <= 0)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "Failed to send a Heartbeat to a server, %s:%d.\n", m_szPfsServerIp, m_usPfsServerPort);
		return ret;
	}
	ret = Receive((char*) &rsp, size);
	if(ret <= 0)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "Failed to Receive a Heartbeat from a server, %s:%d.\n", m_szPfsServerIp, m_usPfsServerPort);
		return ret;
	}
	if(ret != size)
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "The size(%d) of received Heartbeat is not the total size(%d).\n", ret, size);
		return -1;
	}
	if(rsp.cResultCode != 'Y' && rsp.cResultCode != 'y')
	{
		LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "The response of received Heartbeat is (%c).\n", rsp.cResultCode);
		return -1;
	}

	return 1;
}

int CPfsIfHandler::ConnectToPfsServer(char *pServerIp, unsigned short usServerPort)
{
	if(CreateSocket() == -1) return -1;
	LogPrintf(m_mainIndex, m_index, TM_INFO, "Successfully created a socket for a server, %s:%d.\n", pServerIp, usServerPort);

	if(ConnectToServer(pServerIp, usServerPort) == -1) return -1;
	LogPrintf(m_mainIndex, m_index, TM_INFO, "Successfully connected to a server, %s:%d.\n", pServerIp, usServerPort);

	return 0;
}

void CPfsIfHandler::Run()
{
	int		sockfd, timeout, nNumberOfFileDesriptors;
	int		ret, i, try_out_count;
	char	buf[200];
	bool	bConnected;
	struct pollfd	poll_events;

	timeout = cfg.nPfsHBPeriod;
	nNumberOfFileDesriptors = 1;

	while(1)
	{
		if(ConnectToPfsServer(m_szPfsServerIp, m_usPfsServerPort) != 0)
		{
			LogPrintf(m_mainIndex, m_index, TM_INFO, "Failed to a PFS server, %s:%d.\n", m_szPfsServerIp, m_usPfsServerPort);
			// sleep 10 seconds
			for(i = 0; i < 20; i ++) usleep(500000);
			continue;
		}

		sockfd = GetSocket();
		poll_events.fd      = sockfd;
		poll_events.events  = POLLIN | POLLERR | POLLHUP;
		poll_events.revents = 0;

		while(true)
		{
			ret = poll((struct pollfd *) &poll_events, nNumberOfFileDesriptors, timeout);
			if(ret < 0)	// Error
			{
				if(errno != EINTR)
				{
					LogPrintf(m_mainIndex, m_index, TM_INFO, "Got unexpected poll socket error, %s\n", strerror(errno));
				}
				continue;
			}
			else if(ret == 0)
			{
				LogPrintf(m_mainIndex, m_index, TM_INFO, "Sending a Heartbeat to PFS server, %s:%d\n", m_szPfsServerIp, m_usPfsServerPort);
				if(SendHeartBeat() <= 0)
				{
					DestroySocket();

					LogErrorPrintf(m_mainIndex, m_index, TM_INFO, "Failed to send a heartbeat to PFS and try to connect again.\n");
					bConnected = false;
					for(try_out_count = 0; try_out_count < 3; try_out_count ++)
					{
						// sleep 0.5 second
						usleep(500000);
						if(ConnectToPfsServer(m_szPfsServerIp, m_usPfsServerPort) == 0)
						{
							bConnected = true;
							break;
						}
					}
					if(try_out_count == 3 && bConnected == false)
					{
						// Send PFS Change request to ALL VAS...
						for(i = 0; i < cfg.nVasServerCount; i ++)
						{
							g_vasIfHandler[i]->PfsChange(m_cMatePfsPort);
							LogPrintf(m_mainIndex, m_index, TM_INFO, "Sending a PFS Change request to VAS IF Handler-%d.\n", i);
							LogErrorPrintf(m_mainIndex, m_index, TM_INFO, "Sending a PFS Change request to VAS IF Handler-%d.\n", i);
						}
						g_vasExtra->PfsChange(m_cMatePfsPort);
						LogPrintf(m_mainIndex, m_index, TM_INFO, "Sending a PFS Change request to VAS EXTRA Handler.\n");
						LogErrorPrintf(m_mainIndex, m_index, TM_INFO, "Sending a PFS Change request to VAS EXTRA Handler.\n");
						break;
					}

					sockfd = GetSocket();
					poll_events.fd      = sockfd;
					poll_events.events  = POLLIN | POLLERR | POLLHUP;
					poll_events.revents = 0;
				}
			}
			else if(ret > 0)
			{
				if(poll_events.revents & POLLIN)
				{
					ret = Receive(buf, sizeof(buf));
					if(ret <= 0)
					{
						LogPrintf(m_mainIndex, m_index, TM_INFO, "Got unexpected socket error from a PFS server(%s:%d), %s\n", m_szPfsServerIp, m_usPfsServerPort, strerror(errno));
						LogErrorPrintf(m_mainIndex, m_index, TM_INFO, "Got unexpected socket error from a PFS server(%s:%d), %s\n", m_szPfsServerIp, m_usPfsServerPort, strerror(errno));
						DestroySocket();

						LogErrorPrintf(m_mainIndex, m_index, TM_INFO, "Unexpected socket error from PFS and try to connect again.\n");
						bConnected = false;
						for(try_out_count = 0; try_out_count < 3; try_out_count ++)
						{
							// sleep 0.5 second
							usleep(500000);
							if(ConnectToPfsServer(m_szPfsServerIp, m_usPfsServerPort) == 0)
							{
								bConnected = true;
								break;
							}
						}
						if(try_out_count == 3 && bConnected == false)
						{
							// Send PFS Change request to ALL VAS...
							for(i = 0; i < cfg.nVasServerCount; i ++)
							{
								g_vasIfHandler[i]->PfsChange(m_cMatePfsPort);
								LogPrintf(m_mainIndex, m_index, TM_INFO, "Sending a PFS Change request to VAS IF Handler-%d.\n", i);
								LogErrorPrintf(m_mainIndex, m_index, TM_INFO, "Sending a PFS Change request to VAS IF Handler-%d.\n", i);
							}
							g_vasExtra->PfsChange(m_cMatePfsPort);
							LogPrintf(m_mainIndex, m_index, TM_INFO, "Sending a PFS Change request to VAS EXTRA Handler.\n");
							LogErrorPrintf(m_mainIndex, m_index, TM_INFO, "Sending a PFS Change request to VAS EXTRA Handler.\n");
							break;
						}

						sockfd = GetSocket();
						poll_events.fd      = sockfd;
						poll_events.events  = POLLIN | POLLERR | POLLHUP;
						poll_events.revents = 0;
					}
				}
			}
		}
	}
}
