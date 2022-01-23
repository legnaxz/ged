#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <errno.h>
#include  <unistd.h>
#include  <sys/socket.h>
#include  <arpa/inet.h>
#include  <sys/poll.h>
#include  <sys/time.h>

#include  <map>
#include  <functional>

#include  "typedefs.h"
#include  "pktdef.h"
#include  "messagetype.h"
#include  "mask.h"
#include  "agentstate.h"
#include  "statuscode.h"
#include  "tagvalue.h"
#include  "cfg.h"
#include  "ctiqueue.h"
#include  "vasifhandler.h"
#include  "ctiifhandler.h"
#include  "log.h"

#include  <iostream>

CCtiIfHandler	*g_ctiIfHandler[MAX_CTI_SERVER_COUNT];

// 2020.05 OB Starts
int CCtiIfHandler::m_nCallKeySeqNo = 0;
pthread_mutex_t CCtiIfHandler::m_callkeyMutex = PTHREAD_MUTEX_INITIALIZER;
// 2020.05 OB Ends

CCtiIfHandler::CCtiIfHandler(int index)
{
	m_mainIndex = 2;
	m_index = index;
	m_bStopFlag = false;

	// 2020.05 OB Starts
	if(pthread_mutex_init(&m_callkeyMutex, NULL) != 0)
	{
		fprintf(stderr, "can't initialize callkey mutex!!! %s\n", strerror(errno));
		_exit(1);
	}
	// 2020.05 OB Ends
}

CCtiIfHandler::~CCtiIfHandler()
{
	// 2020.05 OB Starts
	pthread_mutex_destroy(&m_callkeyMutex);
	// 2020.05 OB Ends
}

void CCtiIfHandler::OnAgentPreCallEvent(void *pData, UINT size)
{
	PS_AGENT_PRE_CALL_EVENT	papce = (PS_AGENT_PRE_CALL_EVENT) pData;

	papce->MonitorID          = ntohl(papce->MonitorID);
	papce->NumNamedVariables  = ntohs(papce->NumNamedVariables);
	papce->NumNamedArrays     = ntohs(papce->NumNamedArrays);
	papce->ServiceNumber      = ntohl(papce->ServiceNumber);
	papce->ServiceID          = ntohl(papce->ServiceID);
	papce->SkillGroupNumber   = ntohl(papce->SkillGroupNumber);
	papce->SkillGroupID       = ntohl(papce->SkillGroupID);
	papce->SkillGroupPriority = ntohs(papce->SkillGroupPriority);
	papce->MRDID              = ntohl(papce->MRDID);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "Agent Precall Event! MonID[%04X],NumNamedVariables[%X],NumNamedArrays[%X],SvcNo[%04X],SvcID[%04X],"
			"SklGrpNo[%04X],SklGrpID[%04X],SklGrpPri[%X],MRDID[%04X]\n",
			papce->MonitorID,
			papce->NumNamedVariables,
			papce->NumNamedArrays,
			papce->ServiceNumber,
			papce->ServiceID,
			papce->SkillGroupNumber,
			papce->SkillGroupID,
			papce->SkillGroupPriority,
			papce->MRDID
			);

	PrintAgentPreCallEventFloatingFields(papce->pFloatingAddr, size - sizeof(S_AGENT_PRE_CALL_EVENT) + sizeof(papce->pFloatingAddr));
}

// 2020.05 OB Starts
void CCtiIfHandler::GenerateCallKey(char *pCallkey)
{
	struct timeval		tv;
	struct timezone		tz;
	struct tm			t;

	// lock
	if(pthread_mutex_lock(&m_callkeyMutex) != 0)
	{
		fprintf(stderr, "pthread_mutex_lock returns an error, %s\n", strerror(errno));
		return;
	}

	gettimeofday(&tv, &tz);
	localtime_r(&tv.tv_sec, &t);

	sprintf(pCallkey, "%s%02d%02d%02d%02d%04d", cfg.szScsCallKeyAreaPrefix, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, m_nCallKeySeqNo);
	m_nCallKeySeqNo ++;
	m_nCallKeySeqNo %= 10000;

	// unlock
	if(pthread_mutex_unlock(&m_callkeyMutex) != 0)
		fprintf(stderr, "pthread_mutex_unlock returns an error, %s\n", strerror(errno));
}
// 2020.05 OB Ends

void CCtiIfHandler::OnBeginCallEvent(void *pData, UINT size)
{
	INFO_FOR_VAS	ifv;
	PS_BEGIN_CALL_EVENT	pbce = (PS_BEGIN_CALL_EVENT) pData;

	pbce->MonitorID              = ntohl(pbce->MonitorID);
	pbce->PeripheralID           = ntohl(pbce->PeripheralID);
	pbce->PeripheralType         = ntohs(pbce->PeripheralType);
	pbce->NumCTIClients          = ntohs(pbce->NumCTIClients);
	pbce->NumNamedVariables      = ntohs(pbce->NumNamedVariables);
	pbce->NumNamedArrays         = ntohs(pbce->NumNamedArrays);
	pbce->CallType               = ntohs(pbce->CallType);
	pbce->ConnectionDeviceIDType = ntohs(pbce->ConnectionDeviceIDType);
	pbce->ConnectionCallID       = ntohl(pbce->ConnectionCallID);
	pbce->CalledPartyDisposition = ntohs(pbce->CalledPartyDisposition);

	memset(&ifv, 0x0, sizeof(ifv));

#if 1
	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is started! CallType[%X],ConnDevIDType[%X],ConnCallID[%04X]\n",
			pbce->CallType, pbce->ConnectionDeviceIDType, pbce->ConnectionCallID
			);
#else
	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is started! MonID[%04X],PeriID[%04X],PeriType[%X],[%X],NumNV[%X],NumNA[%X],CallType[%X],ConnDevIDType[%X],ConnCallID[%04X],[%X]\n",
			pbce->MonitorID,
			pbce->PeripheralID,
			pbce->PeripheralType,
			pbce->NumCTIClients,
			pbce->NumNamedVariables,
			pbce->NumNamedArrays,
			pbce->CallType,
			pbce->ConnectionDeviceIDType,
			pbce->ConnectionCallID,
			pbce->CalledPartyDisposition
			);
#endif

	PrintBeginCallEventFloatingFields(pbce->pFloatingAddr, size - sizeof(S_BEGIN_CALL_EVENT) + sizeof(pbce->pFloatingAddr), &ifv);

	// 2020.05 OB Starts
	ifv.nInboundCall = m_nInboundCall;
	if(m_nInboundCall == 0)
	{
		// Check out whether or NOT both callkey (CallVariable_1) and ANI (CallVariable_3) are NULL
		if(ifv.cCallkey[0] == 0x0)
		{
			GenerateCallKey(ifv.cCallkey);
		}
		// 2020.06 OB Starts
		ifv.nInitialCallEstablished = 1;
		// 2020.06 OB Ends
	}
	// 2020.05 OB Ends

	m_mapCDR.insert(std::make_pair(pbce->ConnectionCallID, ifv));
	LogPrintf(m_mainIndex, m_index, TM_INFO, "Insert ConnCallID[%04X] to a map(size=%d)\n", pbce->ConnectionCallID, m_mapCDR.size());
}

void CCtiIfHandler::OnCallDataUpdateEvent(void *pData, UINT size)
{
	PS_CALL_DATA_UPDATE_EVENT	pcdue = (PS_CALL_DATA_UPDATE_EVENT) pData;

	pcdue->MonitorID                 = ntohl(pcdue->MonitorID);
	pcdue->PeripheralID              = ntohl(pcdue->PeripheralID);
	pcdue->PeripheralType            = ntohs(pcdue->PeripheralType);
	pcdue->NumCTIClients             = ntohs(pcdue->NumCTIClients);
	pcdue->NumNamedVariables         = ntohs(pcdue->NumNamedVariables);
	pcdue->NumNamedArrays            = ntohs(pcdue->NumNamedArrays);
	pcdue->CallType                  = ntohs(pcdue->CallType);
	pcdue->ConnectionDeviceIDType    = ntohs(pcdue->ConnectionDeviceIDType);
	pcdue->ConnectionCallID          = ntohl(pcdue->ConnectionCallID);
	pcdue->NewConnectionDeviceIDType = ntohs(pcdue->NewConnectionDeviceIDType);
	pcdue->NewConnectionCallID       = ntohl(pcdue->NewConnectionCallID);
	pcdue->CalledPartyDisposition    = ntohs(pcdue->CalledPartyDisposition);
	pcdue->CampaignID                = ntohl(pcdue->CampaignID);
	pcdue->QueryRuleID               = ntohl(pcdue->QueryRuleID);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is updated! MonID[%04X],PeriID[%04X],PeriType[%X],[%X],NumNV[%X],NumNA[%X],CallType[%X],ConnDevIDType[%X],ConnCallID[%04X],NewConnDevIDType[%X],NewConnCallID[%04X],[%X],[%04X],[%04X]\n",
			pcdue->MonitorID,
			pcdue->PeripheralID,
			pcdue->PeripheralType,
			pcdue->NumCTIClients,
			pcdue->NumNamedVariables,
			pcdue->NumNamedArrays,
			pcdue->CallType,
			pcdue->ConnectionDeviceIDType,
			pcdue->ConnectionCallID,
			pcdue->NewConnectionDeviceIDType,
			pcdue->NewConnectionCallID,
			pcdue->CalledPartyDisposition,
			pcdue->CampaignID,
			pcdue->QueryRuleID
			);

	PrintFloating(pcdue->pFloatingAddr, size - sizeof(S_CALL_DATA_UPDATE_EVENT) + sizeof(pcdue->pFloatingAddr));
}

void CCtiIfHandler::OnCallEstablishedEvent(void *pData, UINT size)
{
	// 2020.06 OB Starts - Modification
	char		szCalledDeviceID[255];
	// 2020.06 OB Ends - Modification

	PS_CALL_ESTABLISHED_EVENT	pcee = (PS_CALL_ESTABLISHED_EVENT) pData;

	pcee->MonitorID              = ntohl(pcee->MonitorID);
	pcee->PeripheralID           = ntohl(pcee->PeripheralID);
	pcee->PeripheralType         = ntohs(pcee->PeripheralType);
	pcee->ConnectionDeviceIDType = ntohs(pcee->ConnectionDeviceIDType);
	pcee->ConnectionCallID       = ntohl(pcee->ConnectionCallID);
	pcee->LineHandle             = ntohs(pcee->LineHandle);
	pcee->LineType               = ntohs(pcee->LineType);
	pcee->ServiceNumber          = ntohl(pcee->ServiceNumber);
	pcee->ServiceID              = ntohl(pcee->ServiceID);
	pcee->SkillGroupNumber       = ntohl(pcee->SkillGroupNumber);
	pcee->SkillGroupID           = ntohl(pcee->SkillGroupID);
	pcee->SkillGroupPriority     = ntohs(pcee->SkillGroupPriority);
	pcee->AnsweringDeviceType    = ntohs(pcee->AnsweringDeviceType);
	pcee->CallingDeviceType      = ntohs(pcee->CallingDeviceType);
	pcee->CalledDeviceType       = ntohs(pcee->CalledDeviceType);
	pcee->LastRedirectDeviceType = ntohs(pcee->LastRedirectDeviceType);
	pcee->LocalConnectionState   = ntohs(pcee->LocalConnectionState);
	pcee->EventCause             = ntohs(pcee->EventCause);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is established! MonID[%04X],PeriID[%04X],PeriType[%X],ConnDevIDType[%X],ConnCallID[%04X],[%X],[%X],SvcNo[%04X],SvcID[%04X],SGN[%04X],SGID[%04X],[%X],[%X],[%X],[%X],[%X],[%X],[%X]\n",
			pcee->MonitorID,
			pcee->PeripheralID,
			pcee->PeripheralType,
			pcee->ConnectionDeviceIDType,
			pcee->ConnectionCallID,
			pcee->LineHandle,
			pcee->LineType,
			pcee->ServiceNumber,
			pcee->ServiceID,
			pcee->SkillGroupNumber,
			pcee->SkillGroupID,
			pcee->SkillGroupPriority,
			pcee->AnsweringDeviceType,
			pcee->CallingDeviceType,
			pcee->CalledDeviceType,
			pcee->LastRedirectDeviceType,
			pcee->LocalConnectionState,
			pcee->EventCause
			);

	// 2020.06 OB Starts - Modification
	memset(szCalledDeviceID, 0x0, sizeof(szCalledDeviceID));
	// 2020.06 OB Ends - Modification

	// 2020.06 OB Starts - Modification
	PrintCallEstablishedEventFloatingFields(pcee->pFloatingAddr, size - sizeof(S_CALL_ESTABLISHED_EVENT) + sizeof(pcee->pFloatingAddr), szCalledDeviceID);
	// 2020.06 OB Ends - Modification

	// 2020.05 OB Starts
	std::map<UINT, INFO_FOR_VAS>::iterator it = m_mapCDR.find(pcee->ConnectionCallID);
	if(it != m_mapCDR.end())
	{
		if(it->second.nInboundCall == 0)
		{
			if( GetVasNo(it->second.cAgentIp) > 0 )
			{
				// 2020.06 OB Starts - Modification
				strcpy(it->second.cCID, szCalledDeviceID);
				// 2020.06 OB Ends - Modification
				SendScsEventToVasQueue(pcee->ConnectionCallID, 'S');
			}
			// 2020.06 OB Starts
			it->second.nInitialCallEstablished = 0;
			// 2020.06 OB Ends
		}
	}
	// 2020.05 OB Ends
}

void CCtiIfHandler::OnCallOriginatedEvent(void *pData, UINT size)
{
	PS_CALL_ORIGINATED_EVENT	pcoe = (PS_CALL_ORIGINATED_EVENT) pData;

	pcoe->MonitorID              = ntohl(pcoe->MonitorID);
	pcoe->PeripheralID           = ntohl(pcoe->PeripheralID);
	pcoe->PeripheralType         = ntohs(pcoe->PeripheralType);
	pcoe->ConnectionDeviceIDType = ntohs(pcoe->ConnectionDeviceIDType);
	pcoe->ConnectionCallID       = ntohl(pcoe->ConnectionCallID);
	pcoe->LineHandle             = ntohs(pcoe->LineHandle);
	pcoe->LineType               = ntohs(pcoe->LineType);
	pcoe->ServiceNumber          = ntohl(pcoe->ServiceNumber);
	pcoe->ServiceID              = ntohl(pcoe->ServiceID);
	pcoe->SkillGroupNumber       = ntohl(pcoe->SkillGroupNumber);
	pcoe->SkillGroupID           = ntohl(pcoe->SkillGroupID);
	pcoe->SkillGroupPriority     = ntohs(pcoe->SkillGroupPriority);
	pcoe->CallingDeviceType      = ntohs(pcoe->CallingDeviceType);
	pcoe->CalledDeviceType       = ntohs(pcoe->CalledDeviceType);
	pcoe->LocalConnectionState   = ntohs(pcoe->LocalConnectionState);
	pcoe->EventCause             = ntohs(pcoe->EventCause);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is originated! "
			"MonID[%04X],PeriID[%04X],PeriType[%X],ConnDevIDType[%X],ConnCallID[%04X],LineHd[%X],LineType[%X],SvcNo[%04X],SvcID[%04],"
			"SklGrpNo[%04X],SklGrpID[%04X],SklGrpPri[%X],CallingDevType[%X],CalledDevType[%X],LocalConnState[%X],EvtCause[%X]\n",
			pcoe->MonitorID,
			pcoe->PeripheralID,
			pcoe->PeripheralType,
			pcoe->ConnectionDeviceIDType,
			pcoe->ConnectionCallID,
			pcoe->LineHandle,
			pcoe->LineType,
			pcoe->ServiceNumber,
			pcoe->ServiceID,
			pcoe->SkillGroupNumber,
			pcoe->SkillGroupID,
			pcoe->SkillGroupPriority,
			pcoe->CallingDeviceType,
			pcoe->CalledDeviceType,
			pcoe->LocalConnectionState,
			pcoe->EventCause
			);

	PrintCallOriginatedEventFloatingFields(pcoe->pFloatingAddr, size - sizeof(S_CALL_ORIGINATED_EVENT) + sizeof(pcoe->pFloatingAddr));
}

void CCtiIfHandler::OnCallTransferredEvent(void *pData, UINT size)
{
	PS_CALL_TRANSFERRED_EVENT	pcte = (PS_CALL_TRANSFERRED_EVENT) pData;

	pcte->MonitorID              = ntohl(pcte->MonitorID);
	pcte->PeripheralID           = ntohl(pcte->PeripheralID);
	pcte->PeripheralType         = ntohs(pcte->PeripheralType);
	pcte->PrimaryDeviceIDType    = ntohs(pcte->PrimaryDeviceIDType);
	pcte->PrimaryCallID          = ntohl(pcte->PrimaryCallID);
	pcte->LineHandle             = ntohs(pcte->LineHandle);
	pcte->LineType               = ntohs(pcte->LineType);
	pcte->SkillGroupNumber       = ntohl(pcte->SkillGroupNumber);
	pcte->SkillGroupID           = ntohl(pcte->SkillGroupID);
	pcte->SkillGroupPriority     = ntohs(pcte->SkillGroupPriority);
	pcte->NumParties             = ntohs(pcte->NumParties);
	pcte->SecondaryDeviceIDType  = ntohs(pcte->SecondaryDeviceIDType);
	pcte->SecondaryCallID        = ntohl(pcte->SecondaryCallID);
	pcte->TransferringDeviceType = ntohs(pcte->TransferringDeviceType);
	pcte->TransferredDeviceType  = ntohs(pcte->TransferredDeviceType);
	pcte->LocalConnectionState   = ntohs(pcte->LocalConnectionState);
	pcte->EventCause             = ntohs(pcte->EventCause);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is transferred! "
			"MonID[%04X],PeriID[%04X],PeriType[%X],PriDevIDType[%X],PriCallID[%04X],LineHd[%X],LineType[%X],SklGrpNo[%04X],SklGrpID[%04X],"
			"SklGrpPri[%X],NumParties[%X],SecDevIDType[%X],SecCallID[%04X],TrnsfringDevType[%X],TrnsfrredDevType[%X],LclConnState[%X],EvtCause[%X]\n",
			pcte->MonitorID,
			pcte->PeripheralID,
			pcte->PeripheralType,
			pcte->PrimaryDeviceIDType,
			pcte->PrimaryCallID,
			pcte->LineHandle,
			pcte->LineType,
			pcte->SkillGroupNumber,
			pcte->SkillGroupID,
			pcte->SkillGroupPriority,
			pcte->NumParties,
			pcte->SecondaryDeviceIDType,
			pcte->SecondaryCallID,
			pcte->TransferringDeviceType,
			pcte->TransferredDeviceType,
			pcte->LocalConnectionState,
			pcte->EventCause
			);

	PrintCallTransferredEventFloatingFields(pcte->pFloatingAddr, size - sizeof(S_CALL_TRANSFERRED_EVENT) + sizeof(pcte->pFloatingAddr));
}

void CCtiIfHandler::OnCallHeldEvent(void *pData, UINT size)
{
	PS_CALL_HELD_EVENT	pche = (PS_CALL_HELD_EVENT) pData;

	pche->MonitorID              = ntohl(pche->MonitorID);
	pche->PeripheralID           = ntohl(pche->PeripheralID);
	pche->PeripheralType         = ntohs(pche->PeripheralType);
	pche->ConnectionDeviceIDType = ntohs(pche->ConnectionDeviceIDType);
	pche->ConnectionCallID       = ntohl(pche->ConnectionCallID);
	pche->HoldingDeviceType      = ntohs(pche->HoldingDeviceType);
	pche->LocalConnectionState   = ntohs(pche->LocalConnectionState);
	pche->EventCause             = ntohs(pche->EventCause);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is held! MonID[%04X],PeriID[%04X],PeriType[%X],ConnDevIDType[%X],ConnCallID[%04X],HoldingDevType[%X],LocalConnState[%X],EventCause[%X]\n",
			pche->MonitorID,
			pche->PeripheralID,
			pche->PeripheralType,
			pche->ConnectionDeviceIDType,
			pche->ConnectionCallID,
			pche->HoldingDeviceType,
			pche->LocalConnectionState,
			pche->EventCause
			);

	PrintCallHeldEventFloatingFields(pche->pFloatingAddr, size - sizeof(S_CALL_HELD_EVENT) + sizeof(pche->pFloatingAddr));
}

void CCtiIfHandler::OnCallConnectionClearedEvent(void *pData, UINT size)
{
	PS_CALL_CONNECTION_CLEARED_EVENT	pccce = (PS_CALL_CONNECTION_CLEARED_EVENT) pData;

	pccce->MonitorID              = ntohl(pccce->MonitorID);
	pccce->PeripheralID           = ntohl(pccce->PeripheralID);
	pccce->PeripheralType         = ntohs(pccce->PeripheralType);
	pccce->ConnectionDeviceIDType = ntohs(pccce->ConnectionDeviceIDType);
	pccce->ConnectionCallID       = ntohl(pccce->ConnectionCallID);
	pccce->ReleasingDeviceType    = ntohs(pccce->ReleasingDeviceType);
	pccce->LocalConnectionState   = ntohs(pccce->LocalConnectionState);
	pccce->EventCause             = ntohs(pccce->EventCause);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is cleared! MonID[%04X],PeriID[%04X],PeriType[%X],ConnDevIDType[%X],ConnCallID[%04X],RelDevType[%X],LocalConnState[%X],EventCause[%X]\n",
			pccce->MonitorID,
			pccce->PeripheralID,
			pccce->PeripheralType,
			pccce->ConnectionDeviceIDType,
			pccce->ConnectionCallID,
			pccce->ReleasingDeviceType,
			pccce->LocalConnectionState,
			pccce->EventCause
			);

	PrintCallConnectionClearedEventFloatingFields(pccce->pFloatingAddr, size - sizeof(S_CALL_CONNECTION_CLEARED_EVENT) + sizeof(pccce->pFloatingAddr));
}

void CCtiIfHandler::OnCallClearedEvent(void *pData, UINT size)
{
	PS_CALL_CLEARED_EVENT	pcce = (PS_CALL_CLEARED_EVENT) pData;

	pcce->MonitorID              = ntohl(pcce->MonitorID);
	pcce->PeripheralID           = ntohl(pcce->PeripheralID);
	pcce->PeripheralType         = ntohs(pcce->PeripheralType);
	pcce->ConnectionDeviceIDType = ntohs(pcce->ConnectionDeviceIDType);
	pcce->ConnectionCallID       = ntohl(pcce->ConnectionCallID);
	pcce->LocalConnectionState   = ntohs(pcce->LocalConnectionState);
	pcce->EventCause             = ntohs(pcce->EventCause);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is cleared! MonID[%04X],PeriID[%04X],PeriType[%X],ConnDevIDType[%X],ConnCallID[%04X],LocalConnState[%X],EventCause[%X]\n",
			pcce->MonitorID,
			pcce->PeripheralID,
			pcce->PeripheralType,
			pcce->ConnectionDeviceIDType,
			pcce->ConnectionCallID,
			pcce->LocalConnectionState,
			pcce->EventCause
			);

	PrintCallClearedEventFloatingFields(pcce->pFloatingAddr, size - sizeof(S_CALL_CLEARED_EVENT) + sizeof(pcce->pFloatingAddr));

	// remove from the map
	std::map<UINT, INFO_FOR_VAS>::iterator it = m_mapCDR.find(pcce->ConnectionCallID);
	if(it != m_mapCDR.end())
	{
		m_mapCDR.erase(it);
	}
}

void CCtiIfHandler::OnCallDeliveredEvent(void *pData, UINT size)
{
	PS_CALL_DELIVERED_EVENT	pcde = (PS_CALL_DELIVERED_EVENT) pData;

	pcde->MonitorID              = ntohl(pcde->MonitorID);
	pcde->PeripheralID           = ntohl(pcde->PeripheralID);
	pcde->PeripheralType         = ntohs(pcde->PeripheralType);
	pcde->ConnectionDeviceIDType = ntohs(pcde->ConnectionDeviceIDType);
	pcde->ConnectionCallID       = ntohl(pcde->ConnectionCallID);
	pcde->LineHandle             = ntohs(pcde->LineHandle);
	pcde->LineType               = ntohs(pcde->LineType);
	pcde->ServiceNumber          = ntohl(pcde->ServiceNumber);
	pcde->ServiceID              = ntohl(pcde->ServiceID);
	pcde->SkillGroupNumber       = ntohl(pcde->SkillGroupNumber);
	pcde->SkillGroupID           = ntohl(pcde->SkillGroupID);
	pcde->SkillGroupPriority     = ntohs(pcde->SkillGroupPriority);
	pcde->AlertingDeviceType     = ntohs(pcde->AlertingDeviceType);
	pcde->CallingDeviceType      = ntohs(pcde->CallingDeviceType);
	pcde->CalledDeviceType       = ntohs(pcde->CalledDeviceType);
	pcde->LastRedirectDeviceType = ntohs(pcde->LastRedirectDeviceType);
	pcde->LocalConnectionState   = ntohs(pcde->LocalConnectionState);
	pcde->EventCause             = ntohs(pcde->EventCause);
	pcde->NumNamedVariables      = ntohs(pcde->NumNamedVariables);
	pcde->NumNamedArrays         = ntohs(pcde->NumNamedArrays);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call is delivered!"
			"MonID[%04X],PeriID[%04X],PeriType[%X],ConnDevIDType[%X],ConnCallID[%04X],LineHd[%X],LineType[%X],SvcNo[%04X],SvcID[%04X],SklGrpNo[%04X],SklGrpID[%04X],SklGrpPri[%X],"
			"AlertingDevType[%X],CallingDevType[%X],CalledDevType[%X],LastRedirectDevType[%X],LocalConnState[%X],EvtCause[%X],NumNamedVar[%X],NumNamedArrays[%X]\n",
			pcde->MonitorID,
			pcde->PeripheralID,
			pcde->PeripheralType,
			pcde->ConnectionDeviceIDType,
			pcde->ConnectionCallID,
			pcde->LineHandle,
			pcde->LineType,
			pcde->ServiceNumber,
			pcde->ServiceID,
			pcde->SkillGroupNumber,
			pcde->SkillGroupID,
			pcde->SkillGroupPriority,
			pcde->AlertingDeviceType,
			pcde->CallingDeviceType,
			pcde->CalledDeviceType,
			pcde->LastRedirectDeviceType,
			pcde->LocalConnectionState,
			pcde->EventCause,
			pcde->NumNamedVariables,
			pcde->NumNamedArrays
			);

	PrintCallDeliveredEventFloatingFields(pcde->pFloatingAddr, size - sizeof(S_CALL_DELIVERED_EVENT) + sizeof(pcde->pFloatingAddr));
}

void CCtiIfHandler::OnAgentStateEvent(void *pData, UINT size)
{
	PS_AGENT_STATE_EVENT	pase = (PS_AGENT_STATE_EVENT) pData;

	pase->MonitorID               = ntohl(pase->MonitorID);
	pase->PeripheralID            = ntohl(pase->PeripheralID);
	pase->SessionID               = ntohl(pase->SessionID);
	pase->PeripheralType          = ntohs(pase->PeripheralType);
	pase->SkillGroupState         = ntohs(pase->SkillGroupState);
	pase->StateDuration           = ntohl(pase->StateDuration);
	pase->SkillGroupNumber        = ntohl(pase->SkillGroupNumber);
	pase->SkillGroupID            = ntohl(pase->SkillGroupID);
	pase->SkillGroupPriority      = ntohs(pase->SkillGroupPriority);
	pase->AgentState              = ntohs(pase->AgentState);
	pase->EventReasonCode         = ntohs(pase->EventReasonCode);
	pase->MRDID                   = ntohl(pase->MRDID);
	pase->NumTasks                = ntohl(pase->NumTasks);
	pase->AgentMode               = ntohs(pase->AgentMode);
	pase->MaxTaskLimit            = ntohl(pase->MaxTaskLimit);
	pase->ICMAgentID              = ntohl(pase->ICMAgentID);
	pase->AgentAvailabilityStatus = ntohl(pase->AgentAvailabilityStatus);
	pase->NumFltSkillGroups       = ntohs(pase->NumFltSkillGroups);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "Agent State: MonID[%04X],PeriID[%04X],SessionID[%04X],PeriType[%X],SklGrpState[%X],StateDur[%04X],SklGrpNo[%04X],SklGrpID[%04X],SklGrpPri[%X],AgtState[%X](%s),"
			"EvtRsnCd[%X],MRDID[%04X],NumTasks[%04X],AgtMd[%X],MaxTaskLmt[%04X],ICMAgtID[%04X],AgtAvailStatus[%04X],NumFltSklGrps[%X]\n",
			pase->MonitorID,
			pase->PeripheralID,
			pase->SessionID,
			pase->PeripheralType,
			pase->SkillGroupState,
			pase->StateDuration,
			pase->SkillGroupNumber,
			pase->SkillGroupID,
			pase->SkillGroupPriority,
			pase->AgentState,
			strAgentState(pase->AgentState),
			pase->EventReasonCode,
			pase->MRDID,
			pase->NumTasks,
			pase->AgentMode,
			pase->MaxTaskLimit,
			pase->ICMAgentID,
			pase->AgentAvailabilityStatus,
			pase->NumFltSkillGroups
			);

	PrintAgentStateEventFloatingFields(pase->pFloatingAddr, size - sizeof(S_AGENT_STATE_EVENT) + sizeof(pase->pFloatingAddr));
}

void CCtiIfHandler::OnEndCallEvent(void *pData, UINT size)
{
	PS_END_CALL_EVENT	pece = (PS_END_CALL_EVENT) pData;

	pece->MonitorID              = ntohl(pece->MonitorID);
	pece->PeripheralID           = ntohl(pece->PeripheralID);
	pece->PeripheralType         = ntohs(pece->PeripheralType);
	pece->ConnectionDeviceIDType = ntohs(pece->ConnectionDeviceIDType);
	pece->ConnectionCallID       = ntohl(pece->ConnectionCallID);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "A call ended! MonID[%04X],PeriID[%04X],PeriType[%X],ConnDevIDType[%X],ConnCallID[%04X]\n",
			pece->MonitorID,
			pece->PeripheralID,
			pece->PeripheralType,
			pece->ConnectionDeviceIDType,
			pece->ConnectionCallID
			);

	PrintEndCallEventFloatingFields(pece->pFloatingAddr, size - sizeof(S_END_CALL_EVENT) + sizeof(pece->pFloatingAddr));
}

void CCtiIfHandler::OnRTPStartedEvent(void *pData, UINT size)
{
	// 2020-02-11 starts here
	char			szIp[30];
	// 2020-02-11 ends here
	// 2020.05 OB Starts
	char			szAgentExt[255];
	// 2020.05 OB Ends

	INFO_FOR_VAS	ifv;
	PS_RTP_STARTED_EVENT	prse = (PS_RTP_STARTED_EVENT) pData;

	memset(&ifv, 0x0, sizeof(ifv));

	prse->MonitorID              = ntohl(prse->MonitorID);
	prse->PeripheralID           = ntohl(prse->PeripheralID);
	prse->ClientPort             = ntohl(prse->ClientPort);
	prse->Direction              = ntohs(prse->Direction);
	prse->RTPType                = ntohs(prse->RTPType);
	prse->BitRate                = ntohl(prse->BitRate);
	prse->EchoCancellation       = ntohs(prse->EchoCancellation);
	prse->PacketSize             = ntohl(prse->PacketSize);
	prse->PayloadType            = ntohs(prse->PayloadType);
	prse->ConnectionDeviceIDType = ntohs(prse->ConnectionDeviceIDType);
	prse->ConnectionCallID       = ntohl(prse->ConnectionCallID);

#if 1
	LogPrintf(m_mainIndex, m_index, TM_INFO, "RTP started! ClientPort[%04X,%d],Direction[%X],ConnDevIDType[%X],ConnCallID[%04X]\n",
			prse->ClientPort, prse->ClientPort,
			prse->Direction,
			prse->ConnectionDeviceIDType,
			prse->ConnectionCallID
			);
#else
	LogPrintf(m_mainIndex, m_index, TM_INFO, "RTP started! MonID[%04X],PeriID[%04X],ClientPort[%04X],Direction[%X],RTPType[%X],BR[%04X],EC[%X],PktSize[%04X],PT[%X],ConnDevIDType[%X],ConnCallID[%04X]\n",
			prse->MonitorID,
			prse->PeripheralID,
			prse->ClientPort,
			prse->Direction,
			prse->RTPType,
			prse->BitRate,
			prse->EchoCancellation,
			prse->PacketSize,
			prse->PayloadType,
			prse->ConnectionDeviceIDType,
			prse->ConnectionCallID
			);
#endif

	// 2020.05 OB Starts
	memset(szAgentExt, 0x0, sizeof(szAgentExt));
	// 2020-02-11 starts here
	memset(szIp, 0x0, sizeof(szIp));
	PrintRTPStartedEventFloatingFields(prse->pFloatingAddr, size - sizeof(S_RTP_STARTED_EVENT) + sizeof(prse->pFloatingAddr), &ifv, prse->Direction, szIp, szAgentExt);
	// 2020-02-11 ends here
	// 2020.05 OB Ends

	std::map<UINT, INFO_FOR_VAS>::iterator it = m_mapCDR.find(prse->ConnectionCallID);
	if(it != m_mapCDR.end())
	{
		it->second.nTwoRtpStarted ++;
		// 2020.05 OB Starts
		if(it->second.nInboundCall == 1)
		{
		// 2020.05 OB Ends

			if(prse->Direction == 0)
			{
				strcpy(ifv.cCID, it->second.cCID);
				strcpy(ifv.cCallkey, it->second.cCallkey);
				strcpy(ifv.cAgentExt, it->second.cAgentExt);
				ifv.nTwoRtpStarted = it->second.nTwoRtpStarted;
				ifv.nInboundCall = 1;
				it->second = ifv;
			}

			// 2020-02-11 starts here
			if(it->second.nTwoRtpStarted >= 1)
			{
				if( GetVasNo(szIp) > 0 )
				{
					SendScsEventToVasQueue(prse->ConnectionCallID, 'S');
					it->second.nTwoRtpStarted = 0;
				}
			}
			// 2020-02-11 ends here

		// 2020.05 OB Starts
		}
		else
		{
			if(prse->Direction == 0)
			{
				strcpy(it->second.cAgentIp, ifv.cAgentIp);
				strcpy(it->second.cAgentId, ifv.cAgentId);
				strcpy(it->second.cAgentExt, szAgentExt);
			}

			// 2020.06 OB Starts
			if(it->second.nInitialCallEstablished == 0)
			{
				if(it->second.nTwoRtpStarted >= 1)
				{
					if( GetVasNo(szIp) > 0 )
					{
						SendScsEventToVasQueue(prse->ConnectionCallID, 'S');
						it->second.nTwoRtpStarted = 0;
					}
				}
			}
			// 2020.06 OB Ends

		}
		// 2020.05 OB Ends

		it->second = it->second;
	}
	else
	{
		LogPrintf(m_mainIndex, m_index, TM_WARNING, "Could not find data from map, key=%X to insert data to map!\n", prse->ConnectionCallID);
	}
}

void CCtiIfHandler::OnRTPStoppedEvent(void *pData, UINT size)
{
	// 2020-02-11 starts here
	char			szIp[30];
	// 2020-02-11 ends here
	PS_RTP_STOPPED_EVENT	prse = (PS_RTP_STOPPED_EVENT) pData;

	prse->MonitorID              = ntohl(prse->MonitorID);
	prse->PeripheralID           = ntohl(prse->PeripheralID);
	prse->ClientPort             = ntohl(prse->ClientPort);
	prse->Direction              = ntohs(prse->Direction);
	prse->ConnectionDeviceIDType = ntohs(prse->ConnectionDeviceIDType);
	prse->ConnectionCallID       = ntohl(prse->ConnectionCallID);

#if 1
	LogPrintf(m_mainIndex, m_index, TM_INFO, "RTP stopped! ClientPort[%04X, %d],Direction[%X],ConnDevIDType[%X],ConnCallID[%04X]\n",
			prse->ClientPort, prse->ClientPort,
			prse->Direction,
			prse->ConnectionDeviceIDType,
			prse->ConnectionCallID
			);
#else
	LogPrintf(m_mainIndex, m_index, TM_INFO, "RTP stopped! MonID[%04X],PeriID[%04X],ClientPort[%04X],Direction[%X],ConnDevIDType[%X],ConnCallID[%04X]\n",
			prse->MonitorID,
			prse->PeripheralID,
			prse->ClientPort,
			prse->Direction,
			prse->ConnectionDeviceIDType,
			prse->ConnectionCallID
			);
#endif

	// 2020-02-11 starts here
	memset(szIp, 0x0, sizeof(szIp));
	PrintRTPStoppedEventFloatingFields(prse->pFloatingAddr, size - sizeof(S_RTP_STOPPED_EVENT) + sizeof(prse->pFloatingAddr), szIp);
	// 2020-02-11 ends here

	std::map<UINT, INFO_FOR_VAS>::iterator it = m_mapCDR.find(prse->ConnectionCallID);
	if(it != m_mapCDR.end())
	{
		it->second.nTwoRtpStopped ++;
		// 2020-02-11 starts here
		if(it->second.nTwoRtpStopped >= 1)
		{
			if( GetVasNo(szIp) > 0 )
			{
				SendScsEventToVasQueue(prse->ConnectionCallID, 'E');
				it->second.nTwoRtpStopped = 0;
			}
		}
		// 2020-02-11 ends here
		it->second = it->second;
	}
	else
	{
		LogPrintf(m_mainIndex, m_index, TM_WARNING, "Could not find data from map, key=%X to insert data to map!\n", prse->ConnectionCallID);
	}
}

void CCtiIfHandler::SendScsEventToVasQueue(UINT ConnectionCallID, char cEventType)
{
	int	vasno;
	PKT_SCS_EVT_MSG	msg;
	std::map<UINT, INFO_FOR_VAS>::iterator it = m_mapCDR.find(ConnectionCallID);
	if(it != m_mapCDR.end())
	{
		vasno = GetVasNo(it->second.cAgentIp);
		if(vasno == -1)
		{
			LogPrintf(m_mainIndex, m_index, TM_WARNING, "DATA TO DISCARD:%c-[%s][%s][%s][%s][%s] - no VASNO about the following IP (%s)\n",
					cEventType, it->second.cCID, it->second.cCallkey, it->second.cAgentId, it->second.cAgentExt, it->second.cAgentIp, it->second.cAgentIp);
		}
		else
		{
			LogPrintf(m_mainIndex, m_index, TM_INFO, "DATA TO SEND:%c-[%s][%s][%s][%s][%s]\n", cEventType, it->second.cCID, it->second.cCallkey, it->second.cAgentId, it->second.cAgentExt, it->second.cAgentIp);
			MakeScsEvent(&msg, cEventType, it->second.cCID, it->second.cCallkey, it->second.cAgentExt, it->second.cAgentId, it->second.cAgentIp);
			g_vasIfHandler[vasno - 1]->EnqueueMessage(&msg);
		}
	}
	else
	{
		LogPrintf(m_mainIndex, m_index, TM_WARNING, "Could not find data from map, key=%X to send an event to VAS!\n", ConnectionCallID);
	}
}
void CCtiIfHandler::PrintAgentPreCallEventFloatingFields(void *pData, UINT size)
{
	UINT	offset, RouterCallKeyDay, RouterCallKeyCallID, RouterCallKeySequenceNum;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case AGENT_INSTRUMENT_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentInstrument with a value, %s\n", buf);
			break;
		case ROUTER_CALL_KEY_DAY_TAG:			// UINT
			memcpy(&RouterCallKeyDay, p + offset, ff_size);
			RouterCallKeyDay = ntohl(RouterCallKeyDay);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "RouterCallKeyDay with a value, %X(%d)\n", RouterCallKeyDay, RouterCallKeyDay);
			break;
		case ROUTER_CALL_KEY_CALLID_TAG:		// UINT
			memcpy(&RouterCallKeyCallID, p + offset, ff_size);
			RouterCallKeyCallID = ntohl(RouterCallKeyCallID);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "RouterCallKeyCallID with a value, %X(%d)\n", RouterCallKeyCallID, RouterCallKeyCallID);
			break;
		case ROUTER_CALL_KEY_SEQUENCE_NUM_TAG:	// UINT
			memcpy(&RouterCallKeySequenceNum, p + offset, ff_size);
			RouterCallKeySequenceNum = ntohl(RouterCallKeySequenceNum);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "RouterCallKeySequenceNum with a value, %X(%d)\n", RouterCallKeySequenceNum, RouterCallKeySequenceNum);
			break;
		case ANI_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ANI with a value, %s\n", buf);
			break;
		case UUI_TAG:	// UNSPEC
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "UUI with a value, %s\n", buf);
			break;
		case DIALED_NUMBER_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "DailedNumber with a value, %s\n", buf);
			break;
		// CallerEnteredDigits .... ?????
		case CALL_VAR_1_TAG:
		case CALL_VAR_2_TAG:
		case CALL_VAR_3_TAG:
		case CALL_VAR_4_TAG:
		case CALL_VAR_5_TAG:
		case CALL_VAR_6_TAG:
		case CALL_VAR_7_TAG:
		case CALL_VAR_8_TAG:
		case CALL_VAR_9_TAG:
		case CALL_VAR_10_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallVariable with a value, %s\n", buf);
			break;
		case NAMED_VARIABLE_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "NamedVariable with a value, %s\n", buf);
			break;
		case NAMED_ARRAY_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "NamedArray with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::PrintBeginCallEventFloatingFields(void *pData, UINT size, INFO_FOR_VAS *pifv)
{
	UINT	offset, RouterCallKeyDay, RouterCallKeyCallID, RouterCallKeySequenceNum;
	TIME	CTIClientTimestamp;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	// 2020.05 OB Starts
	CHAR	szConnectionId[255], szANI[255];
	memset(szConnectionId, 0x0, 255);
	memset(szANI, 0x0, 255);
	// 2020.05 OB Ends

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			// 2020.05 OB Starts
			strcpy(szConnectionId, buf);
			// 2020.05 OB Ends
			break;
		case ANI_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ANI with a value, %s\n", buf);
			// 2020.05 OB Starts
			strcpy(szANI, buf);
			// 2020.05 OB Ends
			break;
		case UUI_TAG:	// UNSPEC
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "UUI with a value, %s\n", buf);
			break;
		case DNIS_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "DNIS with a value, %s\n", buf);
			strcpy(pifv->cAgentExt, buf);
			break;
		case DIALED_NUMBER_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "DailedNumber with a value, %s\n", buf);
			break;
		// CallerEnteredDigits .... ?????
		case ROUTER_CALL_KEY_DAY_TAG:			// UINT
			memcpy(&RouterCallKeyDay, p + offset, ff_size);
			RouterCallKeyDay = ntohl(RouterCallKeyDay);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "RouterCallKeyDay with a value, %X(%d)\n", RouterCallKeyDay, RouterCallKeyDay);
			break;
		case ROUTER_CALL_KEY_CALLID_TAG:		// UINT
			memcpy(&RouterCallKeyCallID, p + offset, ff_size);
			RouterCallKeyCallID = ntohl(RouterCallKeyCallID);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "RouterCallKeyCallID with a value, %X(%d)\n", RouterCallKeyCallID, RouterCallKeyCallID);
			break;
		case ROUTER_CALL_KEY_SEQUENCE_NUM_TAG:	// UINT
			memcpy(&RouterCallKeySequenceNum, p + offset, ff_size);
			RouterCallKeySequenceNum = ntohl(RouterCallKeySequenceNum);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "RouterCallKeySequenceNum with a value, %X(%d)\n", RouterCallKeySequenceNum, RouterCallKeySequenceNum);
			break;
		case CALL_VAR_1_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallVariable with a value, %s\n", buf);
			strcpy(pifv->cCallkey, buf);
			break;
		case CALL_VAR_2_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallVariable with a value, %s\n", buf);
			break;
		case CALL_VAR_3_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallVariable with a value, %s\n", buf);
			strcpy(pifv->cCID, buf);
			break;
		case CALL_VAR_4_TAG:
		case CALL_VAR_5_TAG:
		case CALL_VAR_6_TAG:
		case CALL_VAR_7_TAG:
		case CALL_VAR_8_TAG:
		case CALL_VAR_9_TAG:
		case CALL_VAR_10_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallVariable with a value, %s\n", buf);
			break;
		case CALL_WRAPUP_DATA_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallWrapupData with a value, %s\n", buf);
			break;
		case NAMED_VARIABLE_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "NamedVariable with a value, %s\n", buf);
			break;
		case NAMED_ARRAY_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "NamedArray with a value, %s\n", buf);
			break;
		case CLIENT_SIGNATURE_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ClientSignature with a value, %s\n", buf);
			break;
		case CTI_CLIENT_TIMESTAMP_TAG:	// TIME
			memcpy(&CTIClientTimestamp, p + offset, ff_size);
			CTIClientTimestamp = ntohl(CTIClientTimestamp);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CTIClientTimestamp with a value, %X(%d)\n", CTIClientTimestamp, CTIClientTimestamp);
			break;
		case CALL_REFERENCE_ID_TAG:		// UNSPEC
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallReferenceID with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}

	// 2020.05 OB Starts
	m_nInboundCall = 1;
	if( strlen(szConnectionId) > 0 && strlen(szANI) > 0 )
	{
		if(strcmp(szConnectionId, szANI) == 0)
		{
			if( strlen(szConnectionId) == 5 && strlen(szANI) == 5 )
			{
				m_nInboundCall = 0;
			}
		}
	}
	// 2020.05 OB Ends
}

void CCtiIfHandler::PrintEndCallEventFloatingFields(void *pData, UINT size)
{
	UINT	offset;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::PrintCallOriginatedEventFloatingFields(void *pData, UINT size)
{
	UINT	offset;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			break;
		case CALLING_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallingDeviceID with a value, %s\n", buf);
			break;
		case CALLED_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CalledDeviceID with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::PrintCallTransferredEventFloatingFields(void *pData, UINT size)
{
	UINT	offset, PartyCallID;
	USHORT	ParyDevIDType;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case PRIMARY_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "PrimaryDeviceID with a value, %s\n", buf);
			break;
		case SECONDARY_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "SEcondaryDeviceID with a value, %s\n", buf);
			break;
		case TRANSFERRING_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "TransferringDeviceID with a value, %s\n", buf);
			break;
		case TRANSFERRED_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "TransferredDeviceID with a value, %s\n", buf);
			break;
		case PARTY_CALLID_TAG:			// UINT
			memcpy(&PartyCallID, p + offset, ff_size);
			PartyCallID = ntohl(PartyCallID);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectedPartyCallID with a value, %X(%d)\n", PartyCallID, PartyCallID);
			break;
		case PARTY_DEVID_TYPE_TAG:		// USHORT
			memcpy(&ParyDevIDType, p + offset, ff_size);
			ParyDevIDType = ntohs(ParyDevIDType);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectedPartyDeviceIDType with a value, %X(%d)\n", ParyDevIDType, ParyDevIDType);
			break;
		case PARTY_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectedPartyDeviceID with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::PrintCallHeldEventFloatingFields(void *pData, UINT size)
{
	UINT	offset;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			break;
		case HOLDING_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "HoldingDeviceID with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

// 2020.06 OB Starts - Modification
void CCtiIfHandler::PrintCallEstablishedEventFloatingFields(void *pData, UINT size, char *pCalledDeviceID)
// 2020.06 OB Ends - Modification
{
	UINT	offset, TrunkNumber, TrunkGroupNumber;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			// 2020.05 OB Starts -- removed
			//strcpy(pConnectionDeviceID, buf);
			// 2020.05 OB Ends
			break;
		case ANSWERING_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AnsweringDeviceID with a value, %s\n", buf);
			break;
		case CALLING_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallingDeviceID with a value, %s\n", buf);
			break;
		case CALLED_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CalledDeviceID with a value, %s\n", buf);
			// 2020.06 OB Starts
			strcpy(pCalledDeviceID, buf);
			// 2020.06 OB Ends
			break;
		case LAST_REDIRECT_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "LastRedirectDeviceID with a value, %s\n", buf);
			break;
		case TRUNK_NUMBER_TAG:			// UINT
			memcpy(&TrunkNumber, p + offset, ff_size);
			TrunkNumber = ntohl(TrunkNumber);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "TrunkNumber with a value, %X(%d)\n", TrunkNumber, TrunkNumber);
			break;
		case TRUNK_GROUP_NUMBER_TAG:	// UINT
			memcpy(&TrunkGroupNumber, p + offset, ff_size);
			TrunkGroupNumber = ntohl(TrunkGroupNumber);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "TrunkGroupNumber with a value, %X(%d)\n", TrunkGroupNumber, TrunkGroupNumber);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::PrintCallClearedEventFloatingFields(void *pData, UINT size)
{
	UINT	offset;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::PrintCallConnectionClearedEventFloatingFields(void *pData, UINT size)
{
	UINT	offset;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			break;
		case RELEASING_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ReleasingDeviceID with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::PrintCallDeliveredEventFloatingFields(void *pData, UINT size)
{
	UINT	offset, TrunkNumber, TrunkGroupNumber;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			break;
		case ALERTING_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AlertingDeviceID with a value, %s\n", buf);
			break;
		case CALLING_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallingDeviceID with a value, %s\n", buf);
			break;
		case CALLED_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CalledDeviceID with a value, %s\n", buf);
			break;
		case LAST_REDIRECT_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "LastRedirectDeviceID with a value, %s\n", buf);
			break;
		case TRUNK_NUMBER_TAG:				// UINT
			memcpy(&TrunkNumber, p + offset, ff_size);
			TrunkNumber = ntohl(TrunkNumber);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "TrunkNumber with a value, %X(%d)\n", TrunkNumber, TrunkNumber);
			break;
		case TRUNK_GROUP_NUMBER_TAG:		// UINT
			memcpy(&TrunkGroupNumber, p + offset, ff_size);
			TrunkGroupNumber = ntohl(TrunkGroupNumber);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "TrunkGroupNumber with a value, %X(%d)\n", TrunkGroupNumber, TrunkGroupNumber);
			break;
		case SECONDARY_CONNECTION_CALL_ID:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "SecondaryConnectionCallID with a value, %s\n", buf);
			break;
		case ANI_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ANI with a value, %s\n", buf);
			break;
		case ANI_II_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ANI_II with a value, %s\n", buf);
			break;
		case UUI_TAG:	// UNSPEC
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "UUI with a value, %s\n", buf);
			break;
		case DNIS_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "DNIS with a value, %s\n", buf);
			break;
		case DIALED_NUMBER_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "DailedNumber with a value, %s\n", buf);
			break;
		// CallerEnteredDigits ...???
		case CALL_VAR_1_TAG:
		case CALL_VAR_2_TAG:
		case CALL_VAR_3_TAG:
		case CALL_VAR_4_TAG:
		case CALL_VAR_5_TAG:
		case CALL_VAR_6_TAG:
		case CALL_VAR_7_TAG:
		case CALL_VAR_8_TAG:
		case CALL_VAR_9_TAG:
		case CALL_VAR_10_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallVariable with a value, %s\n", buf);
			break;
		case CALL_WRAPUP_DATA_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "CallWrapupData with a value, %s\n", buf);
			break;
		case NAMED_VARIABLE_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "NamedVariable with a value, %s\n", buf);
			break;
		case NAMED_ARRAY_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "NamedArray with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::PrintAgentStateEventFloatingFields(void *pData, UINT size)
{
	UINT	offset, Duration, FltSkillGroupID;
	USHORT	NextAgentState, FltSkillGroupPriority, FltSkillGroupState;
	INT		FltSkillGroupNumber;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CLIENT_SIGNATURE_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ClientSignature with a value, %s\n", buf);
			break;
		case AGENT_EXTENSION_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentExtension with a value, %s\n", buf);
			break;
		case AGENT_ID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentID with a value, %s\n", buf);
			break;
		case AGENT_INSTRUMENT_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentInstrument with a value, %s\n", buf);
			break;
		case DURATION_TAG:
			memcpy(&Duration, p + offset, ff_size);
			Duration = ntohl(Duration);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "Duration with a value, %s\n", Duration);
			break;
		case EXT_AGENT_STATE_TAG:
			memcpy(&NextAgentState, p + offset, ff_size);
			NextAgentState = ntohs(NextAgentState);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "NextAgentState with a value, %d\n", NextAgentState);
			break;
		case SKILL_GROUP_NUMBER_TAG:
			memcpy(&FltSkillGroupNumber, p + offset, ff_size);
			FltSkillGroupNumber = ntohl(FltSkillGroupNumber);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "FltSkillGroupNumber with a value, %d\n", FltSkillGroupNumber);
			break;
		case SKILL_GROUP_ID_TAG:
			memcpy(&FltSkillGroupID, p + offset, ff_size);
			FltSkillGroupID = ntohl(FltSkillGroupID);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "FltSkillGroupID with a value, %d\n", FltSkillGroupID);
			break;
		case SKILL_GROUP_PRIORITY_TAG:
			memcpy(&FltSkillGroupPriority, p + offset, ff_size);
			FltSkillGroupPriority = ntohs(FltSkillGroupPriority);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "FltSkillGroupPriority with a value, %d\n", FltSkillGroupPriority);
			break;
		case SKILL_GROUP_STATE_TAG:
			memcpy(&FltSkillGroupState, p + offset, ff_size);
			FltSkillGroupState = ntohs(FltSkillGroupState);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "FltSkillGroupState with a value, %d\n", FltSkillGroupState);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

// 2020.05 OB Starts - Modification
// 2020-02-11 starts here
void CCtiIfHandler::PrintRTPStartedEventFloatingFields(void *pData, UINT size, INFO_FOR_VAS *pifv, USHORT Direction, char *pip, char *pAgentExt)
// 2020-02-11 ends here
// 2020.05 OB Ends - Modification
{
	UINT	offset, SendingPort;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			break;
		case CLIENT_ADDRESS_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ClientAddress with a value, %s\n", buf);
			/*
				direction of event: 0 - input (Agent)
				direction of event: 1 - output
				direction of event: 2 - bi-directional
			*/
			// 2020-02-11 starts here
			if(Direction == 0)
			{
				strcpy(pifv->cAgentIp, buf);
				strcpy(pip, buf);
			}
			// 2020-02-11 ends here
			break;
		case AGENT_EXTENSION_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentExtension with a value, %s\n", buf);
			// 2020.05 OB Starts
			if(Direction == 0) strcpy(pAgentExt, buf);
			// 2020.05 OB Ends
			break;
		case AGENT_ID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentID with a value, %s\n", buf);
			if(Direction == 0) strcpy(pifv->cAgentId, buf);
			break;
		case AGENT_INSTRUMENT_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentInstrument with a value, %s\n", buf);
			break;
		case SENDING_ADDRESS_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "SendingAddress with a value, %s\n", buf);
			break;
		case SENDING_PORT_TAG:		// UINT
			memcpy(&SendingPort, p + offset, ff_size);
			SendingPort = ntohl(SendingPort);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "SendingPort with a value, %d\n", SendingPort);
			break;
		case CLIENT_ADDRESS_IPV6_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ClientAddressIPv6 with a value, %s\n", buf);
			break;
		case SENDING_ADDRESS_IPV6_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "SendingAddressIPv6 with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

// 2020-02-11 starts here
void CCtiIfHandler::PrintRTPStoppedEventFloatingFields(void *pData, UINT size, char *pip)
// 2020-02-11 ends here
{
	UINT	offset, SendingPort;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		switch(tag)
		{
		case CONNECTION_DEVID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ConnectionDeviceID with a value, %s\n", buf);
			break;
		case CLIENT_ADDRESS_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ClientAddress with a value, %s\n", buf);
			// 2020-02-11 starts here
			strcpy(pip, buf);
			// 2020-02-11 ends here
			break;
		case AGENT_EXTENSION_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentExtension with a value, %s\n", buf);
			break;
		case AGENT_ID_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentID with a value, %s\n", buf);
			break;
		case AGENT_INSTRUMENT_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "AgentInstrument with a value, %s\n", buf);
			break;
		case SENDING_ADDRESS_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "SendingAddress with a value, %s\n", buf);
			break;
		case SENDING_PORT_TAG:		// UINT
			memcpy(&SendingPort, p + offset, ff_size);
			SendingPort = ntohl(SendingPort);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "SendingPort with a value, %d\n", SendingPort);
			break;
		case CLIENT_ADDRESS_IPV6_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "ClientAddressIPv6 with a value, %s\n", buf);
			break;
		case SENDING_ADDRESS_IPV6_TAG:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "SendingAddressIPv6 with a value, %s\n", buf);
			break;
		default:
			memcpy(buf, p + offset, ff_size);
			LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
			break;
		}
		offset += ff_size;
	}
}

void CCtiIfHandler::OnSystemEvent(void *pData, UINT size)
{
	PS_SYSTEM_EVENT	pse = (PS_SYSTEM_EVENT) pData;

	pse->PGStatus                 = ntohl(pse->PGStatus);
	pse->ICMCentralControllerTime = ntohl(pse->ICMCentralControllerTime);
	pse->SystemEventID            = ntohl(pse->SystemEventID);
	pse->SystemEventArg1          = ntohl(pse->SystemEventArg1);
	pse->SystemEventArg2          = ntohl(pse->SystemEventArg2);
	pse->SystemEventArg3          = ntohl(pse->SystemEventArg3);
	pse->EventDeviceType          = ntohs(pse->EventDeviceType);

	LogPrintf(m_mainIndex, m_index, TM_INFO, "RTP stopped! PGStatus[%04X],ICMCentralCtrllerTime[%04X],SysEvtID[%04X],SysEvtArg1[%X],SysEvtArg2[%X],SysEvtArg3[%04X],EvtDevType[%x]\n",
			pse->PGStatus,
			pse->ICMCentralControllerTime,
			pse->SystemEventID,
			pse->SystemEventArg1,
			pse->SystemEventArg2,
			pse->SystemEventArg3,
			pse->EventDeviceType
			);

	PrintFloating(pse->pFloatingAddr, size - sizeof(S_SYSTEM_EVENT) + sizeof(pse->pFloatingAddr));
}

void CCtiIfHandler::PrintFloating(void *pData, UINT size)
{
	UINT	offset;
	UCHAR	tag, ff_size;
	CHAR	buf[300];
	CHAR	*p = (CHAR *) pData;

	offset = 0;
	while(offset < size)
	{
		tag = p[offset++];
		ff_size = p[offset++];
		memset(buf, 0x0, sizeof(buf));
		memcpy(buf, p + offset, ff_size);
		LogPrintf(m_mainIndex, m_index, TM_INFO, "%s, %d with a value, %s\n", strTagValue(tag), ff_size, buf);
		offset += ff_size;
	}
}
void CCtiIfHandler::OnFailureConf(void *pData, UINT size)
{

}
void CCtiIfHandler::OnFailureEvent(void *pData, UINT size)
{

}
void CCtiIfHandler::OnCloseConf(void *pData, UINT size)
{
}

void CCtiIfHandler::Parse(UINT mt, void *pData, UINT size)
{
	switch(mt)
	{
	case MT_FAILURE_CONF:
		OnFailureConf(pData, size);
		break;
	case MT_FAILURE_EVENT:
		OnFailureEvent(pData, size);
		break;
	case MT_CLOSE_CONF:
		OnCloseConf(pData, size);
		break;
	case MT_AGENT_PRE_CALL_EVENT:
		OnAgentPreCallEvent(pData, size);
		break;
	case MT_BEGIN_CALL_EVENT:
		OnBeginCallEvent(pData, size);
		break;
	case MT_CALL_DATA_UPDATE_EVENT:
		OnCallDataUpdateEvent(pData, size);
		break;
	case MT_CALL_ESTABLISHED_EVENT:
		OnCallEstablishedEvent(pData, size);
		break;
	case MT_CALL_ORIGINATED_EVENT:
		OnCallOriginatedEvent(pData, size);
		break;
	case MT_CALL_TRANSFERRED_EVENT:
		OnCallTransferredEvent(pData, size);
		break;
	case MT_CALL_HELD_EVENT:
		OnCallHeldEvent(pData, size);
		break;
	case MT_CALL_CONNECTION_CLEARED_EVENT:
		OnCallConnectionClearedEvent(pData, size);
		break;
	case MT_CALL_CLEARED_EVENT:
		OnCallClearedEvent(pData, size);
		break;
	case MT_CALL_DELIVERED_EVENT:
		OnCallDeliveredEvent(pData, size);
		break;
	case MT_AGENT_STATE_EVENT:
		OnAgentStateEvent(pData, size);
		break;
	case MT_END_CALL_EVENT:
		OnEndCallEvent(pData, size);
		break;
	case MT_RTP_STARTED_EVENT:
		OnRTPStartedEvent(pData, size);
		break;
	case MT_RTP_STOPPED_EVENT:
		OnRTPStoppedEvent(pData, size);
		break;
	default:
		LogPrintf(m_mainIndex, m_index, TM_INFO, "OTHER EVENT which can't parse right now is received!!!!\n");
		break;
	}
}


void CCtiIfHandler::Run()
{
	int		ret;
	MHDR	mhdr;
	char	buf[8000];
#ifdef  MY_TCP_DEBUG
	char	szTcpLog[5000];
#endif

	LogPrintf(m_mainIndex, m_index, TM_INFO, "CTI_IF_Handler is starting, stop_flag=%d!\n", m_bStopFlag);
	while(m_bStopFlag == false)
	{
		// Wait
		g_ctiQueue[m_index].WaitSemaphore();

		// dequeue
		ret = g_ctiQueue[m_index].DequeueMessage(&mhdr, buf);
		if(ret != 0)
		{
			LogPrintf(m_mainIndex, m_index, TM_CRITICAL, "DequeueMessage has an error or an empty queue!\n");
			continue;
		}

#ifdef  MY_TCP_DEBUG
		sprintf(szTcpLog, "MHDR: MessageType(%s-%d) with length, %d is received.\n", strMessageType(mhdr.MessageType), mhdr.MessageType, mhdr.MessageLength);
		LogTCP(m_mainIndex, m_index, szTcpLog, buf, mhdr.MessageLength);
#endif
		Parse(mhdr.MessageType, buf, mhdr.MessageLength);

		// START & STOP...
	}
}

