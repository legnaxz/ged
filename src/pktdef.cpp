#include  <stdio.h>
#include  <string.h>
#include  <sys/time.h>
#include  <time.h>

#include  "pktdef.h"

static void GetTimestamp(char *pTimestamp)
{
	struct timeval tv;
	struct tm	lt;
	int milli;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &lt);
	milli = tv.tv_usec / 1000;
	sprintf(pTimestamp, "%04d%02d%02d%02d%02d%02d%03d", lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, milli);

}

// 2019.11.14 - a change starts here
static void Make444(char *pOrg, char *pTgt)
{
	char	szLocalOrg[30];

	int len = strlen(pOrg);

	if(len >= 12)
	{
		strncpy(pTgt, pOrg, 12);
		return;
	}

	memset(szLocalOrg, 0x0, sizeof(szLocalOrg));
	if(pOrg[0] != '0')
	{
		szLocalOrg[0] = '0';
		strcpy(szLocalOrg + 1, pOrg);
	}
	else
	{
		strcpy(szLocalOrg, pOrg);
	}

	// len == 11
	// 3:4:4 --> ex) 01090895228
	// 3:4:4 --> ex) 03187569876

	// len == 10 is changed to len == 11 after '0' is inserted.
	// 2:4:4 --> ex)  1090895228 ==> 3:4:4 --> ex) 01090895228
	// 2:4:4 --> ex)  3122265228 ==> 3:4:4 --> ex) 03122265228



	// len == 10
	// 3:3:4 --> ex) 0113334444
	// 3:3:4 --> ex) 0316729568
	// 2:4:4 --> ex)  0222265228

	// len == 9 is changed to len == 10 after '0' is inserted.
	// 1:4:4 --> ex)   222265228 ==> 2:4:4 --> ex) 0222265228
	// 2:3:4 --> ex)  31 7729568 ==> 3:3:4 --> ex) 031 7729568



	// len == 9
	// 2:3:4 --> ex)  02 4445758

	// len == 8 is changed to len == 9 after '0' is inserted.
	// 1:3:4 --> ex)   2 4455678 ==> 2:3:4 --> ex) 02 4445758


	len = strlen(szLocalOrg);
	if(len >= 12)
	{
		strncpy(pTgt, szLocalOrg, 12);
		return;
	}
	else if(len == 11)
	{
		pTgt[0] = '0';
		strncpy(pTgt + 1, szLocalOrg, 11);
		return;
	}
	else if(len == 10)
	{
		if(szLocalOrg[0] == '0' && szLocalOrg[1] != '2')
		{
			strncpy(pTgt, "0", 1);
			strncpy(pTgt + 1, szLocalOrg, 3);
			strncpy(pTgt + 4, "0", 1);
			strncpy(pTgt + 5, szLocalOrg + 3, 7);
		}
  		else
		{
			strncpy(pTgt, "00", 2);
			strncpy(pTgt + 2, szLocalOrg, 10);
		}
	}
	else if(len == 9)
	{
		strncpy(pTgt, "00", 2);
		strncpy(pTgt + 2, szLocalOrg, 2);
		strncpy(pTgt + 4, "0", 1);
		strncpy(pTgt + 5, szLocalOrg + 2, 7);
	}
	else
	{
		memset(pTgt, '0', CID_LEN - len);
		strncpy(pTgt + CID_LEN - len, szLocalOrg, len);
//		sprintf(pTgt + CID_LEN - len, "%*s", CID_LEN - len, szLocalOrg);
	}
}
// 2019.11.14 - a change ends here

/*
static void Make444(char *pOrg, char *pTgt)
{
	int len = strlen(pOrg);

	// len == 11
	// 3:4:4 --> ex) 01090895228
	// 3:4:4 --> ex) 03187569876
	// len == 10
	// 3:3:4 --> ex) 011 3334444
	// 3:3:4 --> ex) 031 6729568
	// 2:4:4 --> ex)  1090895228
	// 2:4:4 --> ex)  0222265228
	// 2:4:4 --> ex)  3122265228
	// len == 9
	// 1:4:4 --> ex)   222265228
	// 2:3:4 --> ex)  02 4445758
	// 2:3:4 --> ex)  31 7729568
	// len == 8
	// 1:3:4 --> ex)   2 4455678
	// ====>
	if(len >= 12)
	{
		strncpy(pTgt, pOrg, 12);
		return;
	}
	else if(len == 11)
	{
		pTgt[0] = '0';
		strncpy(pTgt + 1, pOrg, 11);
		return;
	}
	else if(len == 10)
	{
		if(pOrg[0] == '0' && pOrg[1] == '1')
		{
			strncpy(pTgt, "0", 1);
			strncpy(pTgt + 1, pOrg, 3);
			strncpy(pTgt + 4, "0", 1);
			strncpy(pTgt + 5, pOrg + 3, 7);
		}
		else if(pOrg[0] == '0' && pOrg[1] != '2')
		{
			strncpy(pTgt, "0", 1);
			strncpy(pTgt + 1, pOrg, 3);
			strncpy(pTgt + 4, "0", 1);
			strncpy(pTgt + 5, pOrg + 3, 7);
		}
		else
		{
			strncpy(pTgt, "00", 2);
			strncpy(pTgt + 2, pOrg, 10);
		}
	}
	else if(len == 9)
	{
		if(pOrg[0] == '2')
		{
			strncpy(pTgt, "000", 3);
			strncpy(pTgt + 3, pOrg, 9);
		}
		else
		{
			strncpy(pTgt, "00", 2);
			strncpy(pTgt + 2, pOrg, 2);
			strncpy(pTgt + 4, "0", 1);
			strncpy(pTgt + 5, pOrg + 2, 7);
		}
	}
	else
	{
		memset(pTgt, '0', CID_LEN - len);
		sprintf(pTgt + CID_LEN - len, "%*s", CID_LEN - len, pOrg);
	}
}
*/
void MakeScsEvent(PPKT_SCS_EVT_MSG pmsg, char cEvtType, char *pCID, char *pCallKey, char *pAgtExt, char *pAgtId, char *pAgtIP)
{
	char	target[CID_LEN];

	strncpy(pmsg->h.cIfId, SCS_EVENT_VALUE, strlen(SCS_EVENT_VALUE));
	sprintf(pmsg->h.cBodySize, "%*s", PKT_BODY_LEN, SCS_EVENT_BODY_SIZE_VALUE);
	GetTimestamp(pmsg->cTimestamp);
	pmsg->cEventType = cEvtType;
	Make444(pCID, target);
	sprintf(pmsg->cCID, "%*s", CID_LEN, target);
	sprintf(pmsg->cCallkey, "%-*s", CALLKEY_LEN, pCallKey);
	sprintf(pmsg->cAgentExt, "%-*s", AGENT_EXT_LEN, pAgtExt);
	sprintf(pmsg->cAgentId, "%-*s", AGENT_ID_LEN, pAgtId);
	sprintf(pmsg->cAgentIp, "%-*s", AGENT_IP_LEN, pAgtIP);
	sprintf(pmsg->cReserved, "%*s", RESERVED_LEN, " ");
}

void MakePfsChange(PPKT_PFS_CHANGE_MSG pmsg, char cPfsPortSet)
{
	strncpy(pmsg->h.cIfId, PFS_CHANGE_VALUE, strlen(PFS_CHANGE_VALUE));
	sprintf(pmsg->h.cBodySize, "%*s", PKT_BODY_LEN, PFS_CHANGE_BODY_SIZE_VALUE);
	GetTimestamp(pmsg->cTimestamp);
	pmsg->cPfsPortSet = cPfsPortSet;
	pmsg->cResultCode = ' ';
}

void MakeVasChange(PPKT_VAS_CHANGE_MSG pmsg, char cDeadVasNo)
{
	strncpy(pmsg->h.cIfId, VAS_CHANGE_VALUE, strlen(VAS_CHANGE_VALUE));
	sprintf(pmsg->h.cBodySize, "%*s", PKT_BODY_LEN, VAS_CHANGE_BODY_SIZE_VALUE);
	GetTimestamp(pmsg->cTimestamp);
	pmsg->cVasErrorServer = cDeadVasNo;
	pmsg->cResultCode = ' ';
}

void MakeHealthCheck(PPKT_HEALTH_CHECK_MSG pmsg)
{
	strncpy(pmsg->h.cIfId, HEALTH_CHECK_VALUE, strlen(HEALTH_CHECK_VALUE));
	sprintf(pmsg->h.cBodySize, "%*s", PKT_BODY_LEN, HEALTH_CHECK_BODY_SIZE_VALUE);
	GetTimestamp(pmsg->cTimestamp);
	pmsg->cHealthCheck = 'H';
	pmsg->cResultCode = ' ';
}
