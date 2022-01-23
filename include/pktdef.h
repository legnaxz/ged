#ifndef  __PKTDEF_H__
#define  __PKTDEF_H__


#pragma	pack(1)


#define  SCS_EVENT_VALUE				"1000"
#define  PFS_CHANGE_VALUE				"2000"
#define  VAS_CHANGE_VALUE				"3000"
#define  HEALTH_CHECK_VALUE				"9999"


#define  SCS_EVENT_BODY_SIZE_VALUE		"110"
#define  PFS_CHANGE_BODY_SIZE_VALUE		"19"
#define  VAS_CHANGE_BODY_SIZE_VALUE		"19"
#define  HEALTH_CHECK_BODY_SIZE_VALUE	"19"


#define  PKT_INTERFACE_ID_LEN			4
#define  PKT_BODY_LEN					5

#define  TIMESTAMP_LEN					17
#define  CID_LEN						12
#define  CALLKEY_LEN					20
#define  AGENT_EXT_LEN					5
#define  AGENT_ID_LEN					10
#define  AGENT_IP_LEN					15
#define  RESERVED_LEN					30


typedef struct
{
	char cIfId[PKT_INTERFACE_ID_LEN];
	char cBodySize[PKT_BODY_LEN];
} PKT_HEADER, *PPKT_HEADER;


typedef struct
{
	PKT_HEADER	h;
	char	cTimestamp[TIMESTAMP_LEN];
	char	cEventType;
	char	cCID[CID_LEN];
	char	cCallkey[CALLKEY_LEN];
	char	cAgentExt[AGENT_EXT_LEN];
	char	cAgentId[AGENT_ID_LEN];
	char	cAgentIp[AGENT_IP_LEN];
	char	cReserved[RESERVED_LEN];
} PKT_SCS_EVT_MSG, *PPKT_SCS_EVT_MSG;


#define  PFS_PORT_SET_ACTIVE		'A'
#define  PFS_PORT_SET_STANDBY		'S'


typedef struct
{
	PKT_HEADER	h;
	char	cTimestamp[TIMESTAMP_LEN];
	char	cPfsPortSet;
	char	cResultCode;
} PKT_PFS_CHANGE_MSG, *PPKT_PFS_CHANGE_MSG;


typedef struct
{
	PKT_HEADER	h;
	char	cTimestamp[TIMESTAMP_LEN];
	char	cVasErrorServer;
	char	cResultCode;
} PKT_VAS_CHANGE_MSG, *PPKT_VAS_CHANGE_MSG;


typedef struct
{
	PKT_HEADER	h;
	char	cTimestamp[TIMESTAMP_LEN];
	char	cHealthCheck;
	char	cResultCode;
} PKT_HEALTH_CHECK_MSG, *PPKT_HEALTH_CHECK_MSG;


void MakeScsEvent(PPKT_SCS_EVT_MSG pmsg, char cEvtType, char *pCID, char *pCallKey, char *pAgtExt, char *pAgtId, char *pAgtIP);
void MakePfsChange(PPKT_PFS_CHANGE_MSG pmsg, char cPfsPortSet);
void MakeVasChange(PPKT_VAS_CHANGE_MSG pmsg, char cDeadVasNo);
void MakeHealthCheck(PPKT_HEALTH_CHECK_MSG pmsg);


#endif // __PKTDEF_H__
