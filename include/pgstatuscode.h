#ifndef  __PGSTATUSCODE_H__
#define  __PGSTATUSCODE_H__


#define  PGS_OPC_DOWN				0x00000001
#define  PGS_CC_DOWN				0x00000002
#define  PGS_PERIPHERAL_OFFLINE		0x00000004
#define  PGS_CTI_SERVER_OFFLINE		0x00000008
#define  PGS_LIMITED_FUNCTION		0x00000010


const char *strPGStatusCode(UINT pgsc);


#define  SYS_CENTRAL_CONTROLLER_ONLINE		1
#define  SYS_CENTRAL_CONTROLLER_OFFLINE		2
#define  SYS_PERIPHERAL_ONLINE				3
#define  SYS_PERIPHERAL_OFFLINE				4
#define  SYS_TEXT_FYI						5
#define  SYS_PERIPHERAL_GATEWAY_OFFLINE		6
#define  SYS_CTI_SERVER_OFFLINE				7
#define  SYS_CTI_SERVER_ONLINE				8
#define  SYS_HALF_HOUR_CHANGE				9
#define  SYS_INSTRUMENT_OUT_OF_SERVICE		10
#define  SYS_INSTRUMENT_BACK_IN_SERVICE		11


const char *strSystemEventId(UINT sei);


#endif		// end of __PGSTATUSCODE_H__
