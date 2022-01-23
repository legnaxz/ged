#ifndef  __CFG_H__
#define  __CFG_H__

#pragma warning(disable:4103)
#pragma pack(1)		/* In order to get a correct size of the structure */


#define  FILE_PARAMETER						"./ged188.cfg"


#define  PARM_ID_CTI_SERVER_1					1
#define  PARM_ID_CTI_SERVER_2					2
#define  PARM_ID_CTI_SERVER_3					3
#define  PARM_ID_CTI_SERVER_4					4
#define  PARM_ID_CTI_SERVER_5					5
#define  PARM_ID_CTI_SERVER_6					6
#define  PARM_ID_CTI_SERVER_7					7
#define  PARM_ID_CTI_SERVER_8					8
#define  PARM_ID_CTI_SERVER_9					9
#define  PARM_ID_CTI_SERVER_10					10
#define  PARM_ID_HB_MATE_SERVER					20
#define  PARM_ID_HB_PERIOD						21
#define  PARM_ID_HB_LOST						22
#define  PARM_ID_LOG_DIR						100

#define  PARM_ID_PFS_SERVER						200
#define  PARM_ID_PFS_HB_PERIOD					201
#define  PARM_ID_VAS_HB_PERIOD					202

#define  PARM_ID_VAS_SERVER_1					300
#define  PARM_ID_VAS_SERVER_2					301
#define  PARM_ID_VAS_SERVER_3					302
#define  PARM_ID_VAS_SERVER_4					303
#define  PARM_ID_VAS_SERVER_5					304
#define  PARM_ID_VAS_SERVER_6					305
#define  PARM_ID_VAS_SERVER_7					306
#define  PARM_ID_VAS_SERVER_8					307
#define  PARM_ID_VAS_SERVER_EXTRA				308

#define  PARM_ID_RANGE_FILE_1					400
#define  PARM_ID_RANGE_FILE_2					401
#define  PARM_ID_RANGE_FILE_3					402
#define  PARM_ID_RANGE_FILE_4					403
#define  PARM_ID_RANGE_FILE_5					404
#define  PARM_ID_RANGE_FILE_6					405
#define  PARM_ID_RANGE_FILE_7					406
#define  PARM_ID_RANGE_FILE_8					407
#define  PARM_ID_SCS_MODE 						500
#define  PARM_ID_SCS_ACTIVE_SERVER_IP			501
#define  PARM_ID_SCS_ACTIVE_SERVER_PORT			502

// 2020.05 OB Starts
#define  PARM_ID_SCS_CALLKEY_AREA_PREFIX		600
// 2020.05 OB Ends


#define  KEYWORD_CTI_SERVER_1					"CTI_SERVER_1"
#define  KEYWORD_CTI_SERVER_2					"CTI_SERVER_2"
#define  KEYWORD_CTI_SERVER_3					"CTI_SERVER_3"
#define  KEYWORD_CTI_SERVER_4					"CTI_SERVER_4"
#define  KEYWORD_CTI_SERVER_5					"CTI_SERVER_5"
#define  KEYWORD_CTI_SERVER_6					"CTI_SERVER_6"
#define  KEYWORD_CTI_SERVER_7					"CTI_SERVER_7"
#define  KEYWORD_CTI_SERVER_8					"CTI_SERVER_8"
#define  KEYWORD_CTI_SERVER_9					"CTI_SERVER_9"
#define  KEYWORD_CTI_SERVER_10					"CTI_SERVER_10"
#define  KEYWORD_HB_MATE_SERVER					"HB_MATE_SERVER"
#define  KEYWORD_HB_PERIOD						"HB_PERIOD"
#define  KEYWORD_HB_LOST						"HB_LOST"
#define  KEYWORD_LOG_DIR						"LOG_DIR"

#define  KEYWORD_PFS_SERVER						"PFS_SERVER"
#define  KEYWORD_PFS_HB_PERIOD					"PFS_HB_PERIOD"
#define  KEYWORD_VAS_HB_PERIOD					"VAS_HB_PERIOD"

#define  KEYWORD_VAS_SERVER_1					"VAS_SERVER_1"
#define  KEYWORD_VAS_SERVER_2					"VAS_SERVER_2"
#define  KEYWORD_VAS_SERVER_3					"VAS_SERVER_3"
#define  KEYWORD_VAS_SERVER_4					"VAS_SERVER_4"
#define  KEYWORD_VAS_SERVER_5					"VAS_SERVER_5"
#define  KEYWORD_VAS_SERVER_6					"VAS_SERVER_6"
#define  KEYWORD_VAS_SERVER_7					"VAS_SERVER_7"
#define  KEYWORD_VAS_SERVER_8					"VAS_SERVER_8"
#define  KEYWORD_VAS_SERVER_EXTRA				"VAS_SERVER_EXTRA"

#define  KEYWORD_RANGE_FILE_1					"RANGE_FILE_1"
#define  KEYWORD_RANGE_FILE_2					"RANGE_FILE_2"
#define  KEYWORD_RANGE_FILE_3					"RANGE_FILE_3"
#define  KEYWORD_RANGE_FILE_4					"RANGE_FILE_4"
#define  KEYWORD_RANGE_FILE_5					"RANGE_FILE_5"
#define  KEYWORD_RANGE_FILE_6					"RANGE_FILE_6"
#define  KEYWORD_RANGE_FILE_7					"RANGE_FILE_7"
#define  KEYWORD_RANGE_FILE_8					"RANGE_FILE_8"
#define  KEYWORD_SCS_MODE						"SCS_MODE"
#define  KEYWORD_SCS_ACTIVE_SERVER_IP			"SCS_ACTIVE_SERVER_IP"
#define  KEYWORD_SCS_ACTIVE_SERVER_PORT			"SCS_ACTIVE_SERVER_PORT"

// 2020.05 OB Starts
#define  KEYWORD_SCS_CALLKEY_AREA_PREFIX		"SCS_CALLKEY_AREA_PREFIX"
// 2020.05 OB Ends


#define  MAX_CTI_SERVER_COUNT					10
#define  MAX_VAS_SERVER_COUNT					8


typedef struct
{
	int				nCtiServerCount;
	char			szCtiServerIpActive[MAX_CTI_SERVER_COUNT][20];
	unsigned short	usCtiServerPortActive[MAX_CTI_SERVER_COUNT];
	char			szCtiServerIpStandby[MAX_CTI_SERVER_COUNT][20];
	unsigned short	usCtiServerPortStandby[MAX_CTI_SERVER_COUNT];

	char			szHBMateServerIp[20];
	unsigned short	usHBMateServerPort;
	int				nHBPeriod;
	int				nHBLost;
	char			szLogDir[1024];

	char			szPfsServerIpActive[20], szPfsServerIpStandby[20];
	unsigned short	usPfsServerPortActive, usPfsServerPortStandby;
	char			cPfsServerPortActiveMate, cPfsServerPortStandbyMate;

	int				nPfsHBPeriod;
	int				nVasHBPeriod;

	int				nVasServerCount;
	char			szVasServerIp[MAX_VAS_SERVER_COUNT][20];
	unsigned short	usVasServerPort[MAX_VAS_SERVER_COUNT];

	char			szVasServerIpExtra[20];
	unsigned short	usVasServerPortExtra;
	char			cScsMode[2];
	char      szScsActiveServerIp[20];
	unsigned short  usScsActiveServerPort;

	// 2020.05 OB Starts
	char			szScsCallKeyAreaPrefix[10];
	// 2020.05 OB Ends

} CONFIG;

extern CONFIG	cfg;


void	SetDefaultParms();
int		LoadParameters(char *pFile=NULL);

void	ReadVasConfig(char *pFile);
int		GetVasNo(char *pIp);

#endif  // end of __CFG_H__
