#include  <stdio.h>
#include  <string.h>
#include  <unistd.h>
#include  <errno.h>
#include  <unistd.h>
#include  <sys/socket.h>
#include  <arpa/inet.h>
#include  <sys/poll.h>
#include  <sys/time.h>

#include  "typedefs.h"
#include  "messagetype.h"
#include  "mask.h"
#include  "agentstate.h"
#include  "statuscode.h"
#include  "pgstatuscode.h"
#include  "tagvalue.h"
#include  "cfg.h"
#include  "log.h"
#include  "ctiqueue.h"
#include  "ctisocketif.h"


#define  NULL_PERIPHERAL_ID		0xFFFFFFFF


CCtiSocketIf	*g_ctiSocketIf[MAX_CTI_SERVER_COUNT];


UINT CCtiSocketIf::m_InvokeID = 1000;
pthread_mutex_t CCtiSocketIf::m_mutex = PTHREAD_MUTEX_INITIALIZER;


CCtiSocketIf::CCtiSocketIf()
{

}

CCtiSocketIf::~CCtiSocketIf()
{
	pthread_mutex_destroy(&m_mutex);
}

void CCtiSocketIf::SetCtiServerInfo(int index, char *pServerIpActive, unsigned short usServerPortActive, char *pServerIpStandby, unsigned short usServerPortStandby)
{
	strcpy(m_szCtiServerIpActive, pServerIpActive);
	m_usCtiServerPortActive = usServerPortActive;

	strcpy(m_szCtiServerIpStandby, pServerIpStandby);
	m_usCtiServerPortStandby = usServerPortStandby;

	m_index = index;
}

void CCtiSocketIf::OnFailureConf(void *pData, UINT size)
{
	PS_FAILURE_CONF	pfc = (PS_FAILURE_CONF) pData;

	pfc->InvokeID = ntohl(pfc->InvokeID);
	pfc->Status = ntohl(pfc->Status);
	LogPrintf(0, m_index, TM_INFO, "Received a FAILURE_CONF with %s\n", strErrorStatus(pfc->Status));
}

void CCtiSocketIf::OnFailureEvent(void *pData, UINT size)
{
	PS_FAILURE_EVENT	pfe = (PS_FAILURE_EVENT) pData;

	pfe->Status = ntohl(pfe->Status);
	LogPrintf(0, m_index, TM_INFO, "Received a FAILURE_EVENT with %s\n", strErrorStatus(pfe->Status));
}

void CCtiSocketIf::OnOpenConf(void *pData, UINT size)
{
	PS_OPEN_CONF	poc = (PS_OPEN_CONF) pData;

	poc->InvokeID                 = ntohl(poc->InvokeID);
	poc->ServicesGranted          = ntohl(poc->ServicesGranted);
	poc->MonitorID                = ntohl(poc->MonitorID);
	poc->PGStatus                 = ntohl(poc->PGStatus);
	poc->ICMCentralControllerTime = ntohl(poc->ICMCentralControllerTime);
	poc->PeripheralOnline         = ntohs(poc->PeripheralOnline);
	poc->PeripheralType           = ntohs(poc->PeripheralType);
	poc->agentState               = ntohs(poc->agentState);

	LogPrintf(0, m_index, TM_INFO, "Open Request has been confirmed! [%04X,%04X,%04X,%04X,%04X,%X,%X,%X]\n",
			poc->InvokeID,
			poc->ServicesGranted,
			poc->MonitorID,
			poc->PGStatus,
			poc->ICMCentralControllerTime,
			poc->PeripheralOnline,
			poc->PeripheralType,
			poc->agentState
			);

	PrintOpenConfEventFloatingFields(poc->pFloatingAddr, size - sizeof(S_OPEN_CONF) + sizeof(poc->pFloatingAddr));
}

void CCtiSocketIf::PrintOpenConfEventFloatingFields(void *pData, UINT size)
{
	UINT	offset, PeripheralID;
	UCHAR	tag, ff_size;
	USHORT	NumPeripherals, MultiLineAgentControl;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = (UCHAR) *(p + offset);
		offset ++;
		ff_size = (UCHAR) *(p + offset);
		offset ++;
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case AGENT_EXTENSION_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(0, m_index, TM_INFO, "AgentExtension with a value, %s\n", buf);
			break;
		case AGENT_ID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(0, m_index, TM_INFO, "AgentID with a value, %s\n", buf);
			break;
		case AGENT_INSTRUMENT_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(0, m_index, TM_INFO, "AgentInstrument with a value, %s\n", buf);
			break;
		case NUM_PERIPHERALS_TAG:			// USHORT
			memcpy(&NumPeripherals, p + offset, ff_size);
			NumPeripherals = ntohs(NumPeripherals);
			LogPrintf(0, m_index, TM_INFO, "NumPeripherals with a value, %X(%d)\n", NumPeripherals, NumPeripherals);
			break;
		case MULTI_LINE_AGENT_CONTROL_TAG:	// USHORT
			memcpy(&MultiLineAgentControl, p + offset, ff_size);
			MultiLineAgentControl = ntohs(MultiLineAgentControl);
			LogPrintf(0, m_index, TM_INFO, "MultiLineAgentControl with a value, %X(%d)\n", MultiLineAgentControl, MultiLineAgentControl);
			break;
		case PERIPHERAL_ID_TAG:				// UINT
			memcpy(&PeripheralID, p + offset, ff_size);
			PeripheralID = ntohl(PeripheralID);
			LogPrintf(0, m_index, TM_INFO, "PeripheralID with a value, %X(%d)\n", PeripheralID, PeripheralID);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(0, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiSocketIf::OnHeartbeatConf(void *pData, UINT size)
{
}

void CCtiSocketIf::OnCloseConf(void *pData, UINT size)
{
}

int CCtiSocketIf::ConnectToCtiServer(char *pServerIp, unsigned short usServerPort)
{
	char	buf[8000];
	MHDR	mhdr;
	S_OPEN_REQ		s_or;
	size_t	length, sockact_length, total_rcvd_bytes;

	if(CreateSocket() == -1) return -1;
	LogPrintf(0, m_index, TM_INFO, "Successfully created a socket for a server, %s:%d.\n", pServerIp, usServerPort);

	if(ConnectToServer(pServerIp, usServerPort) == -1) return -1;
	LogPrintf(0, m_index, TM_INFO, "Successfully connected to a server, %s:%d.\n", pServerIp, usServerPort);

	// Send a Open Request
	length = SetOpenReqMsg(&s_or);
	sockact_length = Send((char *)&s_or, length);

#ifdef  MY_TCP_DEBUG
	char szTcpLog[255];
	sprintf(szTcpLog, "OPEN REQ sent with a total length, %d bytes.\n", length);
	LogTCP(0, m_index, szTcpLog, (char *)&s_or, length);
#endif

	if(length == sockact_length)
	{
		LogPrintf(0, m_index, TM_INFO, "Sent OPEN_REQ(%d bytes) successfully to a server, %s:%d.\n", length, pServerIp, usServerPort);
	}
	else
	{
		LogPrintf(0, m_index, TM_INFO, "Failed to send OPEN_REQ(%d bytes, actual %d bytes) Successfully connected to a server, %s:%d - %s.\n", length, sockact_length, pServerIp, usServerPort, strerror(errno));
		return 0;
	}

	if((sockact_length = Receive((char *)&mhdr, sizeof(mhdr))) <= 0)
	{
		LogPrintf(0, m_index, TM_INFO, "Failed to receive OPEN_CONF(%d bytes) from a server, %s:%d - %s.\n", sockact_length, pServerIp, usServerPort, strerror(errno));
		return 0;
	}

	LogPrintf(0, m_index, TM_INFO, "Received MHDR successfully from a server, %s:%d.\n", pServerIp, usServerPort);
	mhdr.MessageLength = ntohl(mhdr.MessageLength);		// excludes this header size, 8 bytes
	mhdr.MessageType = ntohl(mhdr.MessageType);

	total_rcvd_bytes = 0;
	memset(buf, 0x0, sizeof(buf));
	while(total_rcvd_bytes < mhdr.MessageLength)
	{
		if((sockact_length = Receive(buf + total_rcvd_bytes, mhdr.MessageLength - total_rcvd_bytes)) <= 0)
		{
			LogPrintf(0, m_index, TM_INFO, "Failed to receive OPEN_CONF(%d bytes) from a server, %s:%d - %s.\n", sockact_length, pServerIp, usServerPort, strerror(errno));
			return 0;
		}
		total_rcvd_bytes += sockact_length;
	}


#ifdef  MY_TCP_DEBUG
	sprintf(szTcpLog, "A message(type=%s-%d,msg_length=%d) received about OPEN REQ. Print data except for MHDR.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, mhdr.MessageLength);
	LogTCP(0, m_index, szTcpLog, buf, total_rcvd_bytes);
#endif

	if(mhdr.MessageType == MT_OPEN_CONF)
	{
		LogPrintf(0, m_index, TM_INFO, "OPEN_CONF is received!\n");
		OnOpenConf(buf, mhdr.MessageLength);
		return 1;
	}
	else
	{
		if(mhdr.MessageType == MT_FAILURE_CONF)
		{
			OnFailureConf(buf, mhdr.MessageLength);
		}
		else
		{
			LogPrintf(0, m_index, TM_INFO, "MHDR: MessageType(%s-%d) with length, %d is received!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, mhdr.MessageLength);
		}
	}

	// Check Out the OPEN_CNF
	return 0;
}

int CCtiSocketIf::SetCloseReqMsg(PS_CLOSE_REQ ps_cr)
{
	UINT	msg_length;

	ps_cr->InvokeID = htonl(GetInvokeID());
	ps_cr->Status = htonl(E_CTI_NO_ERROR);

	msg_length = sizeof(S_CLOSE_REQ) - sizeof(MHDR);
	SetMesssageHeader(&(ps_cr->MessageHeader), msg_length, MT_CLOSE_REQ);

	return (msg_length + sizeof(MHDR));
}

int CCtiSocketIf::SetHeartBeatReqMsg(PS_HEARTBEAT_REQ ps_hbr)
{
	UINT	msg_length;

	ps_hbr->InvokeID = htonl(GetInvokeID());

	msg_length = sizeof(S_HEARTBEAT_REQ) - sizeof(MHDR);
	SetMesssageHeader(&(ps_hbr->MessageHeader), msg_length, MT_HEARTBEAT_REQ);

	return (msg_length + sizeof(MHDR));
}

void CCtiSocketIf::SetMesssageHeader(MHDR *pmhdr, UINT ml, UINT mt)
{
	pmhdr->MessageLength = htonl(ml);
	pmhdr->MessageType   = htonl(mt);
}

UINT CCtiSocketIf::GetInvokeID()
{
	UINT	invokeID;

	pthread_mutex_lock(&m_mutex);
	invokeID = m_InvokeID ++;
	pthread_mutex_unlock(&m_mutex);

	// 2019.11.14 - a change starts here
	if(m_InvokeID > 1000000)
	{
		m_InvokeID = 1000;
	}
	// 2019.11.14 - a change ends here

	return invokeID;
}

void CCtiSocketIf::ClientID(CHAR *pFloatingField, UCHAR *pFloatingFieldTotalSize, CHAR *pID, UCHAR length)
{
	pFloatingField[0] = CLIENT_ID_TAG;
	if(length > 0)
	{
		pFloatingField[1] = length + 1;
		memcpy(pFloatingField + 2, pID, length);
		pFloatingField[2 + length] = 0x0;			// MUST have a NULL BYTE
		*pFloatingFieldTotalSize = 2 + length + 1;	// MUST have a NULL BYTE count.
	}
	else
	{
		pFloatingField[1] = 1;
		pFloatingField[2] = 0x0;		// NULL BYTE
		*pFloatingFieldTotalSize = 3;
	}
}

void CCtiSocketIf::ClientPassword(CHAR *pFloatingField, UCHAR *pFloatingFieldTotalSize, CHAR *pPassword, UCHAR length)
{
	pFloatingField[0] = CLIENT_PASSWORD_TAG;
	if(length > 0)
	{
		pFloatingField[1] = length + 1;
		memcpy(pFloatingField + 2, pPassword, length);
		*pFloatingFieldTotalSize = 2 + length;	// NO NEED to include NULL BYTE?
	}
	else
	{
		pFloatingField[1] = 1;
		pFloatingField[2] = 0x0;		// NULL BYTE
		*pFloatingFieldTotalSize = 3;
	}
}

int CCtiSocketIf::SetOpenReqMsg(PS_OPEN_REQ ps_or)
{
	UINT	msg_length, call_msg_mask, agent_mask, config_mask;
	UCHAR	OneFloatingFieldSize, FloatingFieldOffset;

#define  WORKING
	// Fixed
#ifdef WORKING
	ps_or->InvokeID = htonl(1);
	ps_or->VersionNumber = htonl(14);
	ps_or->IdleTimeout = 0xFFFFFFFF; // htonl(60);
	ps_or->PeripheralID = NULL_PERIPHERAL_ID;
	ps_or->ServicesRequested = htonl(CTI_SERVICE_ALL_EVENTS | CTI_SERVICE_CALL_DATA_UPDATE);

#else
	ps_or->InvokeID = htonl(GetInvokeID());
	ps_or->VersionNumber = htonl(13);	// set to 13
	ps_or->IdleTimeout = 0xFFFFFFFF;
	ps_or->PeripheralID = NULL_PERIPHERAL_ID;
	ps_or->ServicesRequested = htonl(CTI_SERVICE_ALL_EVENTS);

#endif

#ifdef WORKING
	call_msg_mask =
#if 0
			0x03FFFFFF
#else
#if 1
			CALL_ESTABLISHED_MASK |
			CALL_HELD_MASK |
			CALL_RETRIEVED_MASK |
			CALL_CLEARED_MASK |
			CALL_CONNECTION_CLEARED_MASK |
			CALL_CONFERENCED_MASK |
			CALL_TRANSFERRED_MASK |
			BEGIN_CALL_MASK |
			END_CALL_MASK |
			RTP_STARTED_MASK |
			RTP_STOPPED_MASK
#else
			CALL_DELIVERED_MASK | CALL_QUEUED_MASK |
			CALL_ESTABLISHED_MASK | CALL_HELD_MASK | CALL_RETRIEVED_MASK | CALL_CLEARED_MASK | CALL_CONNECTION_CLEARED_MASK |
			CALL_ORIGINATED_MASK | CALL_CONFERENCED_MASK | CALL_TRANSFERRED_MASK | CALL_DIVERTED_MASK |
			CALL_SERVICE_INITIATED_MASK | CALL_TRANSLATION_ROUTE_MASK |
			BEGIN_CALL_MASK | END_CALL_MASK |
			CALL_DATA_UPDATE_MASK | CALL_FAILED_MASK | CALL_REACHED_NETWORK_MASK | CALL_DEQUEUED_MASK
#endif
#endif
			;
	agent_mask =
#if 1
			AGENT_AVAILABLE_MASK | AGENT_TALKING_MASK
#else
			AGENT_LOGIN_MASK | AGENT_LOGOUT_MASK |
			AGENT_NOT_READY_MASK | AGENT_AVAILABLE_MASK | AGENT_TALKING_MASK |
			AGENT_WORK_NOT_READY_MASK | AGENT_WORK_READY_MASK |
			AGENT_BUSY_OTHER_MASK | AGENT_RESERVED_MASK | AGENT_HOLD_MASK
#endif
			;
	config_mask = 0x1F;
#else
	call_msg_mask = BEGIN_CALL_MASK | END_CALL_MASK |
			CALL_ESTABLISHED_MASK | CALL_DATA_UPDATE_MASK | CALL_CLEARED_MASK | CALL_CONNECTION_CLEARED_MASK |
			RTP_STARTED_MASK | RTP_STOPPED_MASK;
	agent_mask = 0;
	config_mask = 0;
#endif

	ps_or->CallMsgMask     = htonl(call_msg_mask);
	ps_or->AgentStateMask  = htonl(agent_mask);
	ps_or->ConfigMsgMask   = htonl(config_mask);

	ps_or->Reserved1 = 0;
	ps_or->Reserved2 = 0;
	ps_or->Reserved3 = 0;

	msg_length = sizeof(S_OPEN_REQ) - MT_OPEN_REQ_FLOATING_SIZE - sizeof(MHDR);
	FloatingFieldOffset = 0;

	// Floating
	ClientID(ps_or->pFloatingAddr, &OneFloatingFieldSize, "RTP", strlen("RTP"));
	msg_length += OneFloatingFieldSize;
	FloatingFieldOffset += OneFloatingFieldSize;

	ClientPassword(ps_or->pFloatingAddr + FloatingFieldOffset, &OneFloatingFieldSize, NULL, 0);
	msg_length += OneFloatingFieldSize;

	SetMesssageHeader(&(ps_or->MessageHeader), msg_length, MT_OPEN_REQ);

	return (msg_length + sizeof(MHDR));
}

void CCtiSocketIf::Run()
{
	MHDR	mhdr;
	char	buf[8000];
	int		sockfd, timeout, nNumberOfFileDesriptors, confirmed;
	int		ret, total_rcvd_bytes, next_server;
	struct pollfd	poll_events;
#ifdef  MY_TCP_DEBUG
	char	szTcpLog[5000];
#endif

	// 2019.01.31 - a change starts here
	int reconnect;
	// 2019.01.31 - a change ends here

	// 2019.11.14 - a change starts here
	int	cont_timeout_count;
	S_HEARTBEAT_REQ		hb_req;
	// 2019.11.14 - a change ends here

	PS_SYSTEM_EVENT		pSystemEvent;


	timeout = 5000;					// 5 sec
	nNumberOfFileDesriptors = 1;

	next_server = 0;
	while(true)
	{
		while(true)
		{
			if(next_server == 0)
			{
				next_server = 1;
				confirmed = ConnectToCtiServer(m_szCtiServerIpActive, m_usCtiServerPortActive);
				if(confirmed == 0)		DestroySocket();
				else if(confirmed == 1)
				{
					// 2019.01.31 - a change starts here
					next_server = 0;
					// 2019.01.31 - a change ends here
					break;
				}
			}

			// total 1 second
			for(ret = 0; ret < 2; ret ++) usleep(500000);	// 500 msec

			if(next_server == 1)
			{
				next_server = 0;
				confirmed = ConnectToCtiServer(m_szCtiServerIpStandby, m_usCtiServerPortStandby);
				if(confirmed == 0)			DestroySocket();
				else if(confirmed == 1)
				{
					// 2019.01.31 - a change starts here
					next_server = 1;
					// 2019.01.31 - a change ends here
					break;
				}
			}

			// total 1 second
			for(ret = 0; ret < 2; ret ++) usleep(500000);	// 500 msec
		}

		sockfd = GetSocket();
		poll_events.fd      = sockfd;
		poll_events.events  = POLLIN | POLLERR | POLLHUP;
		poll_events.revents = 0;

		// 2019.01.31 - a change starts here
		reconnect = 0;
		// 2019.01.31 - a change ends here
		// 2019.11.14 - a change starts here
		cont_timeout_count = 0;
		// 2019.11.14 - a change ends here
		while(true)
		{
			ret = poll((struct pollfd *) &poll_events, nNumberOfFileDesriptors, timeout);
			if(ret < 0)	// Error
			{
				if(errno != EINTR)
					LogPrintf(0, m_index, TM_INFO, "Got unexpected poll socket error, %s\n", strerror(errno));
				continue;
			}
			else if(ret == 0)
			{
				ret = recv(sockfd, &mhdr, sizeof(mhdr), MSG_PEEK | MSG_DONTWAIT);
				if(ret > 0)
				{
					mhdr.MessageLength = ntohl(mhdr.MessageLength);
					mhdr.MessageType   = ntohl(mhdr.MessageType);
					LogPrintf(0, m_index, TM_INFO, "TIMEOUT expired (%d,%d).\n", mhdr.MessageLength, mhdr.MessageType);
				}
				// 2019.11.14 - a change starts here
				else
				{
					cont_timeout_count ++;
					if(cont_timeout_count >= 180)	// 900 seconds == 15 minutes
					{
						cont_timeout_count = 0;

						// Send a Heartbeat to a cti server.
						int len = SetHeartBeatReqMsg(&hb_req);
						if(send(sockfd, &hb_req, len, 0) < 0)
						{
							LogPrintf(0, m_index, TM_INFO, "Failed to send a HEARTBEAT_REQ to a cti server.\n");
							DestroySocket();
							break;
						}
						else
						{
							LogPrintf(0, m_index, TM_INFO, "Successfully sent a HEARTBEAT_REQ to a cti server.\n");
						}
					}
				}
				// 2019.11.14 - a change ends here
			}
			else if(ret > 0)
			{
				if(poll_events.revents & POLLIN)
				{
					// 2019.11.14 - a change starts here
						cont_timeout_count = 0;
					// 2019.11.14 - a change ends here
					ret = recv(sockfd, &mhdr, sizeof(mhdr), 0);
					if(ret <= 0)
					{

						LogPrintf(0, m_index, TM_INFO, "Got unexpected socket error from a server, %s\n", strerror(errno));
						DestroySocket();
						break;
					}

					mhdr.MessageLength = ntohl(mhdr.MessageLength);		// excludes this header size, 8 bytes
					mhdr.MessageType   = ntohl(mhdr.MessageType);

					total_rcvd_bytes = 0;
					memset(buf, 0x0, sizeof(buf));
					while(total_rcvd_bytes < mhdr.MessageLength)
					{
						ret = recv(sockfd, buf + total_rcvd_bytes, mhdr.MessageLength - total_rcvd_bytes, 0);
						total_rcvd_bytes += ret;
					}
					switch(mhdr.MessageType)
					{
					// Insert message to queue
					case MT_BEGIN_CALL_EVENT:
						{
							// 2020.01.20 - debugging purpose only to check out the message delay issue
							PS_BEGIN_CALL_EVENT	pbce = (PS_BEGIN_CALL_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "Enqueue (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pbce->ConnectionCallID), mhdr.MessageLength);
							if(g_ctiQueue[m_index].EnqueueMessage(mhdr, buf) != 0)
							{
								LogPrintf(0, m_index, TM_CRITICAL, "Enqueuing (%s-%d), ConnCallID[%04X] to a CTI queue has an error.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pbce->ConnectionCallID));
							}
							else
							{
								LogPrintf(0, m_index, TM_INFO, "Enqueued (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pbce->ConnectionCallID), mhdr.MessageLength);
							}
						}
						break;
					case MT_CALL_CLEARED_EVENT:
						{
							// 2020.01.20 - debugging purpose only to check out the message delay issue
							PS_CALL_CLEARED_EVENT	pcce = (PS_CALL_CLEARED_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "Enqueue (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcce->ConnectionCallID), mhdr.MessageLength);
							if(g_ctiQueue[m_index].EnqueueMessage(mhdr, buf) != 0)
							{
								LogPrintf(0, m_index, TM_CRITICAL, "Enqueuing (%s-%d), ConnCallID[%04X] to a CTI queue has an error.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcce->ConnectionCallID));
							}
							else
							{
								LogPrintf(0, m_index, TM_INFO, "Enqueued (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcce->ConnectionCallID), mhdr.MessageLength);
							}
						}
						break;
					case MT_RTP_STARTED_EVENT:
						{
							// 2020.01.20 - debugging purpose only to check out the message delay issue
							PS_RTP_STARTED_EVENT	prse = (PS_RTP_STARTED_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "Enqueue (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(prse->ConnectionCallID), mhdr.MessageLength);
							if(g_ctiQueue[m_index].EnqueueMessage(mhdr, buf) != 0)
							{
								LogPrintf(0, m_index, TM_CRITICAL, "Enqueuing (%s-%d), ConnCallID[%04X] to a CTI queue has an error.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(prse->ConnectionCallID));
							}
							else
							{
								LogPrintf(0, m_index, TM_INFO, "Enqueued (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(prse->ConnectionCallID), mhdr.MessageLength);
							}
						}
						break;
					case MT_RTP_STOPPED_EVENT:
						{
							// 2020.01.20 - debugging purpose only to check out the message delay issue
							PS_RTP_STOPPED_EVENT	prse = (PS_RTP_STOPPED_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "Enqueue (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(prse->ConnectionCallID), mhdr.MessageLength);
							if(g_ctiQueue[m_index].EnqueueMessage(mhdr, buf) != 0)
							{
								LogPrintf(0, m_index, TM_CRITICAL, "Enqueuing (%s-%d), ConnCallID[%04X] to a CTI queue has an error.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(prse->ConnectionCallID));
							}
							else
							{
								LogPrintf(0, m_index, TM_INFO, "Enqueued (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(prse->ConnectionCallID), mhdr.MessageLength);
							}
						}
						break;

					case MT_CALL_ESTABLISHED_EVENT:
						{
							PS_CALL_ESTABLISHED_EVENT	pcee = (PS_CALL_ESTABLISHED_EVENT) buf;
							// 2020.05 OB Starts
							LogPrintf(0, m_index, TM_INFO, "Enqueue (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcee->ConnectionCallID), mhdr.MessageLength);
							if(g_ctiQueue[m_index].EnqueueMessage(mhdr, buf) != 0)
							{
								LogPrintf(0, m_index, TM_CRITICAL, "Enqueuing (%s-%d), ConnCallID[%04X] to a CTI queue has an error.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcee->ConnectionCallID));
							}
							else
							{
								LogPrintf(0, m_index, TM_INFO, "Enqueued (%s-%d), ConnCallID[%04X] with a length, %d to a CTI queue.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcee->ConnectionCallID), mhdr.MessageLength);
							}
							// 2020.05 OB Ends
						}
						break;
					case MT_CALL_CONNECTION_CLEARED_EVENT:
						{
							PS_CALL_CONNECTION_CLEARED_EVENT	pccce = (PS_CALL_CONNECTION_CLEARED_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "MessageType(%s-%d), ConnCallID[%04X] with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pccce->ConnectionCallID), mhdr.MessageLength);
						}
						break;
					case MT_CALL_DELIVERED_EVENT:
						{
							PS_CALL_DELIVERED_EVENT	pcde = (PS_CALL_DELIVERED_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "MessageType(%s-%d), ConnCallID[%04X] with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcde->ConnectionCallID), mhdr.MessageLength);
						}
						break;
					case MT_END_CALL_EVENT:
						{
							PS_END_CALL_EVENT	pece = (PS_END_CALL_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "MessageType(%s-%d), ConnCallID[%04X] with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pece->ConnectionCallID), mhdr.MessageLength);
						}
						break;
					case MT_CALL_HELD_EVENT:
						{
							PS_CALL_HELD_EVENT	pche = (PS_CALL_HELD_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "MessageType(%s-%d), ConnCallID[%04X] with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pche->ConnectionCallID), mhdr.MessageLength);
						}
						break;
					case MT_CALL_ORIGINATED_EVENT:
						{
							PS_CALL_ORIGINATED_EVENT	pcoe = (PS_CALL_ORIGINATED_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "MessageType(%s-%d), ConnCallID[%04X] with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcoe->ConnectionCallID), mhdr.MessageLength);
						}
						break;
					case MT_CALL_TRANSFERRED_EVENT:
						{
							PS_CALL_TRANSFERRED_EVENT	pcte = (PS_CALL_TRANSFERRED_EVENT) buf;
							LogPrintf(0, m_index, TM_INFO, "MessageType(%s-%d), priCallID[%04X] with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcte->PrimaryCallID), mhdr.MessageLength);
						}
						break;
					case MT_CALL_DATA_UPDATE_EVENT:
						{
							PS_CALL_DATA_UPDATE_EVENT	pcdue = (PS_CALL_DATA_UPDATE_EVENT) buf;
							pcdue->ConnectionCallID          = ntohl(pcdue->ConnectionCallID);
							LogPrintf(0, m_index, TM_INFO, "MessageType(%s-%d), ConnCallID[%04X] with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, ntohl(pcdue->ConnectionCallID), mhdr.MessageLength);
						}
						break;
					case MT_AGENT_PRE_CALL_EVENT:
					case MT_AGENT_STATE_EVENT:
						LogPrintf(0, m_index, TM_INFO, "MessageType(%s-%d) with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, mhdr.MessageLength);
						break;
					case MT_FAILURE_EVENT:
						LogPrintf(0, m_index, TM_INFO, "MHDR(FAILURE_EVENT): MessageType(%s-%d) with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, mhdr.MessageLength);
						break;

					case MT_SYSTEM_EVENT:
						pSystemEvent = (PS_SYSTEM_EVENT) buf;
						pSystemEvent->PGStatus = ntohl(pSystemEvent->PGStatus);
						pSystemEvent->SystemEventID = ntohl(pSystemEvent->SystemEventID);
						LogPrintf(0, m_index, TM_INFO, "MHDR(SYSTEM_EVENT-%d) with length, %d is received and PG Status(%s), SystemEventID(%s)\n", mhdr.MessageType, mhdr.MessageLength,
								strPGStatusCode(pSystemEvent->PGStatus), strSystemEventId(pSystemEvent->SystemEventID));
						break;

					// discard every message
					default:
#if 1
#ifdef  MY_TCP_DEBUG
						sprintf(szTcpLog, "MHDR: MessageType(%s-%d) with length, %d is received but will be discarded.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, mhdr.MessageLength);
						LogTCP(0, m_index, szTcpLog, buf, mhdr.MessageLength);
#else
						// 2020.01.20 - debugging purpose only to check out the message delay issue
						LogPrintf(0, m_index, TM_INFO, "MHDR: MessageType(%s-%d) with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, mhdr.MessageLength);
#endif
#else
						LogPrintf(0, m_index, TM_INFO, "MHDR: MessageType(%s-%d) with length, %d is received but discarded!\n", strMessageType(mhdr.MessageType), mhdr.MessageType, mhdr.MessageLength);
#endif
						// 2019.01.31 - a change starts here
						// A wrong message type is sometimes received. In this case, disconnect the socket and reconnect it.
						if(mhdr.MessageType <= 0 || mhdr.MessageType > MT_CALL_TERMINATION_EVENT)
						{
							// Something wrong in the socket. So disconnect it and reconnect it.
							DestroySocket();
							reconnect = 1;
						}
						// 2019.01.31 - a change ends here
						break;
					}
				}
				if(poll_events.revents & POLLERR)
				{
					LogPrintf(0, m_index, TM_CRITICAL, "POLLERR happened!!!\n");
				}
				if(poll_events.revents & POLLHUP)
				{
					LogPrintf(0, m_index, TM_CRITICAL, "POLLHUP happened!!!\n");
				}

				// 2019.01.31 - a change starts here
				if(reconnect == 1) break;
				// 2019.01.31 - a change ends here
			}
		}
	}
}
