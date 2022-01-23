#include  <stdio.h>
#include  <string.h>

#include  "typedefs.h"
#include  "statuscode.h"


const char *strErrorStatus(UINT e)
{
	switch(e)
	{
	case E_CTI_NO_ERROR                       : return "NO_ERROR";
	case E_CTI_INVALID_VERSION                : return "INVALID_VERSION";
	case E_CTI_INVALID_MESSAGE_TYPE           : return "INVALID_MESSAGE_TYPE";
	case E_CTI_INVALID_FIELD_TAG              : return "INVALID_FIELD_TAG";
	case E_CTI_SESSION_NOT_OPEN               : return "SESSION_NOT_OPEN";
	case E_CTI_SESSION_ALREADY_OPEN           : return "SESSION_ALREADY_OPEN";
	case E_CTI_REQUIRED_DATA_MISSING          : return "REQUIRED_DATA_MISSING";
	case E_CTI_INVALID_PERIPHERAL_ID          : return "INVALID_PERIPHERAL_ID";
	case E_CTI_INVALID_AGENT_DATA             : return "INVALID_AGENT_DATA";
	case E_CTI_AGENT_NOT_LOGGED_ON            : return "AGENT_NOT_LOGGED_ON";
	case E_CTI_DEVICE_IN_USE                  : return "DEVICE_IN_USE";
	case E_CTI_NEW_SESSION_OPENED             : return "NEW_SESSION_OPENED";
	case E_CTI_FUNCTION_NOT_AVAILABLE         : return "FUNCTION_NOT_AVAILABLE";
	case E_CTI_INVALID_CALLID                 : return "INVALID_CALLID";
	case E_CTI_PROTECTED_VARIABLE             : return "PROTECTED_VARIABLE";
	case E_CTI_CTI_SERVER_OFFLINE             : return "CTI_SERVER_OFFLINE";
	case E_CTI_TIMEOUT                        : return "TIMEOUT";
	case E_CTI_UNSPECIFIED_FAILURE            : return "UNSPECIFIED_FAILURE";
	case E_CTI_INVALID_TIMEOUT                : return "INVALID_TIMEOUT";
	case E_CTI_INVALID_SERVICE_MASK           : return "INVALID_SERVICE_MASK";
	case E_CTI_INVALID_CALL_MSG_MASK          : return "INVALID_CALL_MSG_MASK";
	case E_CTI_INVALID_AGENT_STATE_MASK       : return "INVALID_AGENT_STATE_MASK";
	case E_CTI_INVALID_RESERVED_FIELD         : return "INVALID_RESERVED_FIELD";
	case E_CTI_INVALID_FIELD_LENGTH           : return "INVALID_FIELD_LENGTH";
	case E_CTI_INVALID_DIGITS                 : return "INVALID_DIGITS";
	case E_CTI_BAD_MESSAGE_FORMAT             : return "BAD_MESSAGE_FORMAT";
	case E_CTI_INVALID_TAG_FOR_MSG_TYPE       : return "INVALID_TAG_FOR_MSG_TYPE";
	case E_CTI_INVALID_DEVICE_ID_TYPE         : return "INVALID_DEVICE_ID_TYPE";
	case E_CTI_INVALID_LCL_CONN_STATE         : return "INVALID_LCL_CONN_STATE";
	case E_CTI_INVALID_EVENT_CAUSE            : return "INVALID_EVENT_CAUSE";
	case E_CTI_INVALID_NUM_PARTIES            : return "INVALID_NUM_PARTIES";
	case E_CTI_INVALID_SYS_EVENT_ID           : return "INVALID_SYS_EVENT_ID";
	case E_CTI_INCONSISTENT_AGENT_DATA        : return "INCONSISTENT_AGENT_DATA";
	case E_CTI_INVALID_CONNECTION_ID_TYPE     : return "INVALID_CONNECTION_ID_TYPE";
	case E_CTI_INVALID_CALL_TYPE              : return "INVALID_CALL_TYPE";
	case E_CTI_NOT_CALL_PARTY                 : return "NOT_CALL_PARTY";
	case E_CTI_INVALID_PASSWORD               : return "INVALID_PASSWORD";
	case E_CTI_CLIENT_DISCONNECTED            : return "CLIENT_DISCONNECTED";
	case E_CTI_INVALID_OBJECT_STATE           : return "INVALID_OBJECT_STATE";
	case E_CTI_INVALID_NUM_SKILL_GROUPS       : return "INVALID_NUM_SKILL_GROUPS";
	case E_CTI_INVALID_NUM_LINES              : return "INVALID_NUM_LINES";
	case E_CTI_INVALID_LINE_TYPE              : return "INVALID_LINE_TYPE";
	case E_CTI_INVALID_ALLOCATION_STATE       : return "INVALID_ALLOCATION_STATE";
	case E_CTI_INVALID_ANSWERING_MACHINE      : return "INVALID_ANSWERING_MACHINE";
	case E_CTI_INVALID_CALL_MANNER_TYPE       : return "INVALID_CALL_MANNER_TYPE";
	case E_CTI_INVALID_CALL_PLACEMENT_TYPE    : return "INVALID_CALL_PLACEMENT_TYPE";
	case E_CTI_INVALID_CONSULT_TYPE           : return "INVALID_CONSULT_TYPE";
	case E_CTI_INVALID_FACILITY_TYPE          : return "INVALID_FACILITY_TYPE";
	case E_CTI_INVALID_MSG_TYPE_FOR_VERSION   : return "INVALID_MSG_TYPE_FOR_VERSION";
	case E_CTI_INVALID_TAG_FOR_VERSION        : return "INVALID_TAG_FOR_VERSION";
	case E_CTI_INVALID_AGENT_WORK_MODE        : return "INVALID_AGENT_WORK_MODE";
	case E_CTI_INVALID_CALL_OPTION            : return "INVALID_CALL_OPTION";
	case E_CTI_INVALID_DESTINATION_COUNTRY    : return "INVALID_DESTINATION_COUNTRY";
	case E_CTI_INVALID_ANSWER_DETECT_MODE     : return "INVALID_ANSWER_DETECT_MODE";
	case E_CTI_MUTUALLY_EXCLUS_DEVICEID_TYPES : return "MUTUALLY_EXCLUS_DEVICEID_TYPES";
	case E_CTI_INVALID_MONITORID              : return "INVALID_MONITORID";
	case E_CTI_SESSION_MONITOR_ALREADY_EXISTS : return "SESSION_MONITOR_ALREADY_EXISTS";
	case E_CTI_SESSION_MONITOR_IS_CLIENTS     : return "SESSION_MONITOR_IS_CLIENTS";
	case E_CTI_INVALID_CALL_CONTROL_MASK      : return "INVALID_CALL_CONTROL_MASK";
	case E_CTI_INVALID_FEATURE_MASK           : return "INVALID_FEATURE_MASK";
	case E_CTI_INVALID_TRANSFER_CONFERENCE_SETUP_MASK: return "INVALID_TRANSFER_CONFERENCE_SETUP_MASK";
	case E_CTI_INVALID_ARRAY_INDEX            : return "INVALID_ARRAY_INDEX";
	case E_CTI_INVALID_CHARACTER              : return "INVALID_CHARACTER";
	case E_CTI_CLIENT_NOT_FOUND               : return "CLIENT_NOT_FOUND";
	case E_CTI_SUPERVISOR_NOT_FOUND           : return "SUPERVISOR_NOT_FOUND";
	case E_CTI_TEAM_NOT_FOUND                 : return "TEAM_NOT_FOUND";
	case E_CTI_NO_CALL_ACTIVE                 : return "NO_CALL_ACTIVE";
	case E_CTI_NAMED_VARIABLE_NOT_CONFIGURED  : return "NAMED_VARIABLE_NOT_CONFIGURED";
	case E_CTI_NAMED_ARRAY_NOT_CONFIGURED     : return "NAMED_ARRAY_NOT_CONFIGURED";
	case E_CTI_INVALID_CALL_VARIABLE_MASK     : return "INVALID_CALL_VARIABLE_MASK";
	case E_CTI_ELEMENT_NOT_FOUND              : return "ELEMENT_NOT_FOUND";
	case E_CTI_INVALID_DISTRIBUTION_TYPE      : return "INVALID_DISTRIBUTION_TYPE";
	case E_CTI_INVALID_SKILL_GROUP            : return "INVALID_SKILL_GROUP";
	case E_CTI_TOO_MUCH_DATA                  : return "TOO_MUCH_DATA";
	case E_CTI_VALUE_TOO_LONG                 : return "VALUE_TOO_LONG";
	case E_CTI_SCALAR_FUNCTION_ON_ARRAY       : return "SCALAR_FUNCTION_ON_ARRAY";
	case E_CTI_ARRAY_FUNCTION_ON_SCALAR       : return "ARRAY_FUNCTION_ON_SCALAR";
	case E_CTI_INVALID_NUM_NAMED_VARIABLES    : return "INVALID_NUM_NAMED_VARIABLES";
	case E_CTI_INVALID_NUM_NAMED_ARRAYS       : return "INVALID_NUM_NAMED_ARRAYS";
	case E_CTI_INVALID_RTP_DIRECTION          : return "INVALID_RTP_DIRECTION";
	case E_CTI_INVALID_RTP_TYPE               : return "INVALID_RTP_TYPE";
	case E_CTI_CALLED_PARTY_DISPOSITION       : return "CALLED_PARTY_DISPOSITION";
	case E_CTI_INVALID_SUPERVISORY_ACTION     : return "INVALID_SUPERVISORY_ACTION";
	case E_CTI_AGENT_TEAM_MONITOR_ALREADY_EXISTS: return "AGENT_TEAM_MONITOR_ALREADY_EXISTS";
	case E_CTI_INVALID_SERVICE                : return "INVALID_SERVICE";
	case E_CTI_SERVICE_CONFLICT               : return "SERVICE_CONFLICT";
	case E_CTI_SKILL_GROUP_CONFLICT           : return "SKILL_GROUP_CONFLICT";
	case E_CTI_INVALID_DEVICE                 : return "INVALID_DEVICE";
	case E_CTI_INVALID_MR_DOMAIN              : return "INVALID_MR_DOMAIN";
	case E_CTI_MONITOR_ALREADY_EXISTS         : return "MONITOR_ALREADY_EXISTS";
	case E_CTI_MONITOR_TERMINATED             : return "MONITOR_TERMINATED";
	case E_CTI_INVALID_TASK_MSG_MASK          : return "INVALID_TASK_MSG_MASK";
	case E_CTI_SERVER_NOT_MASTER              : return "SERVER_NOT_MASTER";
	case E_CTI_INVALID_CSD                    : return "INVALID_CSD";
	case E_CTI_JTAPI_CCM_PROBLEM              : return "JTAPI_CCM_PROBLEM";
	case E_INVALID_CONFIG_MSG_MASK            : return "INVALID_CONFIG_MSG_MASK";
	case E_CTI_AUTO_CONFIG_RESET              : return "AUTO_CONFIG_RESET";
	case E_CTI_INVALID_MONITOR_STATUS         : return "INVALID_MONITOR_STATUS";
	case E_CTI_INVALID_REQUEST_TYPE           : return "INVALID_REQUEST_TYPE";
	default                                   : return "UNKNOWN!";
	}
	return "";
}
