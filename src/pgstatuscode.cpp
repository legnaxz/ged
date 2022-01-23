#include  <stdio.h>

#include  "typedefs.h"
#include  "pgstatuscode.h"


const char *strPGStatusCode(UINT pgsc)
{
	switch(pgsc)
	{
	case 0                      : return "NORMAL";
	case PGS_OPC_DOWN           : return "PGS_OPC_DOWN";
	case PGS_CC_DOWN            : return "PGS_CC_DOWN";
	case PGS_PERIPHERAL_OFFLINE : return "PGS_PERIPHERAL_OFFLINE";
	case PGS_CTI_SERVER_OFFLINE : return "PGS_CTI_SERVER_OFFLINE";
	case PGS_LIMITED_FUNCTION   : return "PGS_LIMITED_FUNCTION";
	default                     : return "UNKNOWN";
	}

	return "";
}


const char *strSystemEventId(UINT sei)
{
	switch(sei)
	{
	case SYS_CENTRAL_CONTROLLER_ONLINE  : return "SYS_CENTRAL_CONTROLLER_ONLINE";
	case SYS_CENTRAL_CONTROLLER_OFFLINE : return "SYS_CENTRAL_CONTROLLER_OFFLINE";
	case SYS_PERIPHERAL_ONLINE          : return "SYS_PERIPHERAL_ONLINE";
	case SYS_PERIPHERAL_OFFLINE         : return "SYS_PERIPHERAL_OFFLINE";
	case SYS_TEXT_FYI                   : return "SYS_TEXT_FYI";
	case SYS_PERIPHERAL_GATEWAY_OFFLINE : return "SYS_PERIPHERAL_GATEWAY_OFFLINE";
	case SYS_CTI_SERVER_OFFLINE         : return "SYS_CTI_SERVER_OFFLINE";
	case SYS_CTI_SERVER_ONLINE          : return "SYS_CTI_SERVER_ONLINE";
	case SYS_HALF_HOUR_CHANGE           : return "SYS_HALF_HOUR_CHANGE";
	case SYS_INSTRUMENT_OUT_OF_SERVICE  : return "SYS_INSTRUMENT_OUT_OF_SERVICE";
	case SYS_INSTRUMENT_BACK_IN_SERVICE : return "SYS_INSTRUMENT_BACK_IN_SERVICE";
	default                             : return "UNKNOWN";
	}

	return "";
}
