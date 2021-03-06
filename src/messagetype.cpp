#include  <stdio.h>
#include  "typedefs.h"
#include  "messagetype.h"

const char *strMessageType(UINT mt)
{
	switch(mt)
	{
	case MT_FAILURE_CONF                    : return "FAILURE_CONF";
	case MT_FAILURE_EVENT                   : return "FAILURE_EVENT";
	case MT_OPEN_REQ                        : return "OPEN_REQ";
	case MT_OPEN_CONF                       : return "OPEN_CONF";
	case MT_HEARTBEAT_REQ                   : return "HEARTBEAT_REQ";
	case MT_HEARTBEAT_CONF                  : return "HEARTBEAT_CONF";
	case MT_CLOSE_REQ                       : return "CLOSE_REQ";
	case MT_CLOSE_CONF                      : return "CLOSE_CONF";
	case MT_CALL_DELIVERED_EVENT            : return "CALL_DELIVERED_EVENT";
	case MT_CALL_ESTABLISHED_EVENT          : return "CALL_ESTABLISHED_EVENT";
	case MT_CALL_HELD_EVENT                 : return "CALL_HELD_EVENT";
	case MT_CALL_RETRIEVED_EVENT            : return "CALL_RETRIEVED_EVENT";
	case MT_CALL_CLEARED_EVENT              : return "CALL_CLEARED_EVENT";
	case MT_CALL_CONNECTION_CLEARED_EVENT   : return "CALL_CONNECTION_CLEARED_EVENT";
	case MT_CALL_ORIGINATED_EVENT           : return "CALL_ORIGINATED_EVENT";
	case MT_CALL_FAILED_EVENT               : return "CALL_FAILED_EVENT";
	case MT_CALL_CONFERENCED_EVENT          : return "CALL_CONFERENCED_EVENT";
	case MT_CALL_TRANSFERRED_EVENT          : return "CALL_TRANSFERRED_EVENT";
	case MT_CALL_DIVERTED_EVENT             : return "CALL_DIVERTED_EVENT";
	case MT_CALL_SERVICE_INITIATED_EVENT    : return "CALL_SERVICE_INITIATED_EVENT";
	case MT_CALL_QUEUED_EVENT               : return "CALL_QUEUED_EVENT";
	case MT_CALL_TRANSLATION_ROUTE_EVENT    : return "CALL_TRANSLATION_ROUTE_EVENT";
	case MT_BEGIN_CALL_EVENT                : return "BEGIN_CALL_EVENT";
	case MT_END_CALL_EVENT                  : return "END_CALL_EVENT";
	case MT_CALL_DATA_UPDATE_EVENT          : return "CALL_DATA_UPDATE_EVENT";
	case MT_SET_CALL_DATA_REQ               : return "SET_CALL_DATA_REQ";
	case MT_SET_CALL_DATA_CONF              : return "SET_CALL_DATA_CONF";
	case MT_RELEASE_CALL_REQ                : return "RELEASE_CALL_REQ";
	case MT_RELEASE_CALL_CONF               : return "RELEASE_CALL_CONF";
	case MT_AGENT_STATE_EVENT               : return "AGENT_STATE_EVENT";
	case MT_SYSTEM_EVENT                    : return "SYSTEM_EVENT";
	case MT_CLIENT_EVENT_REPORT_REQ         : return "CLIENT_EVENT_REPORT_REQ";
	case MT_CLIENT_EVENT_REPORT_CONF        : return "CLIENT_EVENT_REPORT_CONF";
	case MT_CALL_REACHED_NETWORK_EVENT      : return "CALL_REACHED_NETWORK_EVENT";
	case MT_CONTROL_FAILURE_CONF            : return "CONTROL_FAILURE_CONF";
	case MT_QUERY_AGENT_STATE_REQ           : return "QUERY_AGENT_STATE_REQ";
	case MT_QUERY_AGENT_STATE_CONF          : return "QUERY_AGENT_STATE_CONF";
	case MT_SET_AGENT_STATE_REQ             : return "SET_AGENT_STATE_REQ";
	case MT_SET_AGENT_STATE_CONF            : return "SET_AGENT_STATE_CONF";
	case MT_ALTERNATE_CALL_REQ              : return "ALTERNATE_CALL_REQ";
	case MT_ALTERNATE_CALL_CONF             : return "ALTERNATE_CALL_CONF";
	case MT_ANSWER_CALL_REQ                 : return "ANSWER_CALL_REQ";
	case MT_ANSWER_CALL_CONF                : return "ANSWER_CALL_CONF";
	case MT_CLEAR_CALL_REQ                  : return "CLEAR_CALL_REQ";
	case MT_CLEAR_CALL_CONF                 : return "CLEAR_CALL_CONF";
	case MT_CLEAR_CONNECTION_REQ            : return "CLEAR_CONNECTION_REQ";
	case MT_CLEAR_CONNECTION_CONF           : return "CLEAR_CONNECTION_CONF";
	case MT_CONFERENCE_CALL_REQ             : return "CONFERENCE_CALL_REQ";
	case MT_CONFERENCE_CALL_CONF            : return "CONFERENCE_CALL_CONF";
	case MT_CONSULTATION_CALL_REQ           : return "CONSULTATION_CALL_REQ";
	case MT_CONSULTATION_CALL_CONF          : return "CONSULTATION_CALL_CONF";
	case MT_DEFLECT_CALL_REQ                : return "DEFLECT_CALL_REQ";
	case MT_DEFLECT_CALL_CONF               : return "DEFLECT_CALL_CONF";
	case MT_HOLD_CALL_REQ                   : return "HOLD_CALL_REQ";
	case MT_HOLD_CALL_CONF                  : return "HOLD_CALL_CONF";
	case MT_MAKE_CALL_REQ                   : return "MAKE_CALL_REQ";
	case MT_MAKE_CALL_CONF                  : return "MAKE_CALL_CONF";
	case MT_MAKE_PREDICTIVE_CALL_REQ        : return "MAKE_PREDICTIVE_CALL_REQ";
	case MT_MAKE_PREDICTIVE_CALL_CONF       : return "MAKE_PREDICTIVE_CALL_CONF";
	case MT_RECONNECT_CALL_REQ              : return "RECONNECT_CALL_REQ";
	case MT_RECONNECT_CALL_CONF             : return "RECONNECT_CALL_CONF";
	case MT_RETRIEVE_CALL_REQ               : return "RETRIEVE_CALL_REQ";
	case MT_RETRIEVE_CALL_CONF              : return "RETRIEVE_CALL_CONF";
	case MT_TRANSFER_CALL_REQ               : return "TRANSFER_CALL_REQ";
	case MT_TRANSFER_CALL_CONF              : return "TRANSFER_CALL_CONF";
	case MT_QUERY_DEVICE_INFO_REQ           : return "QUERY_DEVICE_INFO_REQ";
	case MT_QUERY_DEVICE_INFO_CONF          : return "QUERY_DEVICE_INFO_CONF";
	case MT_SNAPSHOT_CALL_REQ               : return "SNAPSHOT_CALL_REQ";
	case MT_SNAPSHOT_CALL_CONF              : return "SNAPSHOT_CALL_CONF";
	case MT_SNAPSHOT_DEVICE_REQ             : return "SNAPSHOT_DEVICE_REQ";
	case MT_SNAPSHOT_DEVICE_CONF            : return "SNAPSHOT_DEVICE_CONF";
	case MT_CALL_DEQUEUED_EVENT             : return "CALL_DEQUEUED_EVENT";
	case MT_SEND_DTMF_SIGNAL_REQ            : return "SEND_DTMF_SIGNAL_REQ";
	case MT_SEND_DTMF_SIGNAL_CONF           : return "SEND_DTMF_SIGNAL_CONF";
	case MT_MONITOR_START_REQ               : return "MONITOR_START_REQ";
	case MT_MONITOR_START_CONF              : return "MONITOR_START_CONF";
	case MT_MONITOR_STOP_REQ                : return "MONITOR_STOP_REQ";
	case MT_MONITOR_STOP_CONF               : return "MONITOR_STOP_CONF";
	case MT_CHANGE_MONITOR_MASK_REQ         : return "CHANGE_MONITOR_MASK_REQ";
	case MT_CHANGE_MONITOR_MASK_CONF        : return "CHANGE_MONITOR_MASK_CONF";
	case MT_CLIENT_SESSION_OPENED_EVENT     : return "CLIENT_SESSION_OPENED_EVENT";
	case MT_CLIENT_SESSION_CLOSED_EVENT     : return "CLIENT_SESSION_CLOSED_EVENT";
	case MT_SESSION_MONITOR_START_REQ       : return "SESSION_MONITOR_START_REQ";
	case MT_SESSION_MONITOR_START_CONF      : return "SESSION_MONITOR_START_CONF";
	case MT_SESSION_MONITOR_STOP_REQ        : return "SESSION_MONITOR_STOP_REQ";
	case MT_SESSION_MONITOR_STOP_CONF       : return "SESSION_MONITOR_STOP_CONF";
	case MT_AGENT_PRE_CALL_EVENT            : return "AGENT_PRE_CALL_EVENT";
	case MT_AGENT_PRE_CALL_ABORT_EVENT      : return "AGENT_PRE_CALL_ABORT_EVENT";
	case MT_USER_MESSAGE_REQ                : return "USER_MESSAGE_REQ";
	case MT_USER_MESSAGE_CONF               : return "USER_MESSAGE_CONF";
	case MT_USER_MESSAGE_EVENT              : return "USER_MESSAGE_EVENT";
	case MT_REGISTER_VARIABLES_REQ          : return "REGISTER_VARIABLES_REQ";
	case MT_REGISTER_VARIABLES_CONF         : return "REGISTER_VARIABLES_CONF";
	case MT_QUERY_AGENT_STATISTICS_REQ      : return "QUERY_AGENT_STATISTICS_REQ";
	case MT_QUERY_AGENT_STATISTICS_CONF     : return "QUERY_AGENT_STATISTICS_CONF";
	case MT_QUERY_SKILL_GROUP_STATISTICS_REQ : return "QUERY_SKILL_GROUP_STATISTICS_REQ";
	case MT_QUERY_SKILL_GROUP_STATISTICS_CONF: return "QUERY_SKILL_GROUP_STATISTICS_CONF";
	case MT_RTP_STARTED_EVENT               : return "RTP_STARTED_EVENT";
	case MT_RTP_STOPPED_EVENT               : return "RTP_STOPPED_EVENT";
	case MT_SUPERVISOR_ASSIST_REQ           : return "SUPERVISOR_ASSIST_REQ";
	case MT_SUPERVISOR_ASSIST_CONF          : return "SUPERVISOR_ASSIST_CONF";
	case MT_SUPERVISOR_ASSIST_EVENT         : return "SUPERVISOR_ASSIST_EVENT";
	case MT_EMERGENCY_CALL_REQ              : return "EMERGENCY_CALL_REQ";
	case MT_EMERGENCY_CALL_CONF             : return "EMERGENCY_CALL_CONF";
	case MT_EMERGENCY_CALL_EVENT            : return "EMERGENCY_CALL_EVENT";
	case MT_SUPERVISE_CALL_REQ              : return "SUPERVISE_CALL_REQ";
	case MT_SUPERVISE_CALL_CONF             : return "SUPERVISE_CALL_CONF";
	case MT_AGENT_TEAM_CONFIG_REQ           : return "AGENT_TEAM_CONFIG_REQ";
	case MT_AGENT_TEAM_CONFIG_CONF          : return "AGENT_TEAM_CONFIG_CONF";
	case MT_AGENT_TEAM_CONFIG_EVENT         : return "AGENT_TEAM_CONFIG_EVENT";
	case MT_SET_APP_DATA_REQ                : return "SET_APP_DATA_REQ";
	case MT_SET_APP_DATA_CONF               : return "SET_APP_DATA_CONF";
	case MT_AGENT_DESK_SETTINGS_REQ         : return "AGENT_DESK_SETTINGS_REQ";
	case MT_AGENT_DESK_SETTINGS_CONF        : return "AGENT_DESK_SETTINGS_CONF";
	case MT_LIST_AGENT_TEAM_REQ             : return "LIST_AGENT_TEAM_REQ";
	case MT_LIST_AGENT_TEAM_CONF            : return "LIST_AGENT_TEAM_CONF";
	case MT_MONITOR_AGENT_TEAM_START_REQ    : return "MONITOR_AGENT_TEAM_START_REQ";
	case MT_MONITOR_AGENT_TEAM_START_CONF   : return "MONITOR_AGENT_TEAM_START_CONF";
	case MT_MONITOR_AGENT_TEAM_STOP_REQ     : return "MONITOR_AGENT_TEAM_STOP_REQ";
	case MT_MONITOR_AGENT_TEAM_STOP_CONF    : return "MONITOR_AGENT_TEAM_STOP_CONF";
	case MT_BAD_CALL_REQ                    : return "BAD_CALL_REQ";
	case MT_BAD_CALL_CONF                   : return "BAD_CALL_CONF";
	case MT_SET_DEVICE_ATTRIBUTES_REQ       : return "SET_DEVICE_ATTRIBUTES_REQ";
	case MT_SET_DEVICE_ATTRIBUTES_CONF      : return "SET_DEVICE_ATTRIBUTES_CONF";
	case MT_REGISTER_SERVICE_REQ            : return "REGISTER_SERVICE_REQ";
	case MT_REGISTER_SERVICE_CONF           : return "REGISTER_SERVICE_CONF";
	case MT_UNREGISTER_SERVICE_REQ          : return "UNREGISTER_SERVICE_REQ";
	case MT_UNREGISTER_SERVICE_CONF         : return "UNREGISTER_SERVICE_CONF";
	case MT_START_RECORDING_REQ             : return "START_RECORDING_REQ";
	case MT_START_RECORDING_CONF            : return "START_RECORDING_CONF";
	case MT_STOP_RECORDING_REQ              : return "STOP_RECORDING_REQ";
	case MT_STOP_RECORDING_CONF             : return "STOP_RECORDING_CONF";
	case MT_MEDIA_LOGIN_REQ                 : return "MEDIA_LOGIN_REQ";
	case MT_MEDIA_LOGIN_RESP                : return "MEDIA_LOGIN_RESP";
	case MT_MEDIA_LOGOUT_IND                : return "MEDIA_LOGOUT_IND";
	case MT_MAKE_AGENT_ROUTABLE_IND         : return "MAKE_AGENT_ROUTABLE_IND";
	case MT_MAKE_AGENT_NOT_ROUTABLE_REQ     : return "MAKE_AGENT_NOT_ROUTABLE_REQ";
	case MT_MAKE_AGENT_NOT_ROUTABLE_RESP    : return "MAKE_AGENT_NOT_ROUTABLE_RESP";
	case MT_MAKE_AGENT_READY_IND            : return "MAKE_AGENT_READY_IND";
	case MT_MAKE_AGENT_NOT_READY_REQ        : return "MAKE_AGENT_NOT_READY_REQ";
	case MT_MAKE_AGENT_NOT_READY_RESP       : return "MAKE_AGENT_NOT_READY_RESP";
	case MT_OFFER_TASK_IND                  : return "OFFER_TASK_IND";
	case MT_OFFER_APPLICATION_TASK_REQ      : return "OFFER_APPLICATION_TASK_REQ";
	case MT_OFFER_APPLICATION_TASK_RESP     : return "OFFER_APPLICATION_TASK_RESP";
	case MT_START_TASK_IND                  : return "START_TASK_IND";
	case MT_START_APPLICATION_TASK_REQ      : return "START_APPLICATION_TASK_REQ";
	case MT_START_APPLICATION_TASK_RESP     : return "START_APPLICATION_TASK_RESP";
	case MT_PAUSE_TASK_IND                  : return "PAUSE_TASK_IND";
	case MT_RESUME_TASK_IND                 : return "RESUME_TASK_IND";
	case MT_WRAPUP_TASK_IND                 : return "WRAPUP_TASK_IND";
	case MT_END_TASK_IND                    : return "END_TASK_IND";
	case MT_AGENT_MADE_NOT_ROUTABLE_EVENT   : return "AGENT_MADE_NOT_ROUTABLE_EVENT";
	case MT_AGENT_INTERRUPT_ADVISORY_EVENT  : return "AGENT_INTERRUPT_ADVISORY_EVENT";
	case MT_AGENT_INTERRUPT_ACCEPTED_IND    : return "AGENT_INTERRUPT_ACCEPTED_IND";
	case MT_AGENT_INTERRUPT_UNACCEPTED_IND  : return "AGENT_INTERRUPT_UNACCEPTED_IND";
	case MT_AGENT_INTERRUPT_DONE_ADVISORY_EVENT: return "AGENT_INTERRUPT_DONE_ADVISORY_EVENT";
	case MT_AGENT_INTERRUPT_DONE_ACCEPTED_IND  : return "AGENT_INTERRUPT_DONE_ACCEPTED_IND";
	case MT_CHANGE_MAX_TASK_LIMIT_REQ       : return "CHANGE_MAX_TASK_LIMIT_REQ";
	case MT_CHANGE_MAX_TASK_LIMIT_RESP      : return "CHANGE_MAX_TASK_LIMIT_RESP";
	case MT_OVERRIDE_LIMIT_REQ              : return "OVERRIDE_LIMIT_REQ";
	case MT_OVERRIDE_LIMIT_RESP             : return "OVERRIDE_LIMIT_RESP";
	case MT_UPDATE_TASK_CONTEXT_IND         : return "UPDATE_TASK_CONTEXT_IND";
	case MT_BEGIN_AGENT_INIT_IND            : return "BEGIN_AGENT_INIT_IND";
	case MT_AGENT_INIT_REQ                  : return "AGENT_INIT_REQ";
	case MT_AGENT_INIT_RESP                 : return "AGENT_INIT_RESP";
	case MT_END_AGENT_INIT_IND              : return "END_AGENT_INIT_IND";
	case MT_TASK_INIT_IND                   : return "TASK_INIT_IND";
	case MT_AGENT_INIT_READY_EVENT          : return "AGENT_INIT_READY_EVENT";
	case MT_GET_PRECALL_MESSAGES_REQ        : return "GET_PRECALL_MESSAGES_REQ";
	case MT_GET_PRECALL_MESSAGES_RESP       : return "GET_PRECALL_MESSAGES_RESP";
	case MT_AGENT_LEGACY_PRE_CALL_EVENT     : return "AGENT_LEGACY_PRE_CALL_EVENT";
	case MT_FAILURE_RESP                    : return "FAILURE_RESP";
	case MT_BEGIN_TASK_EVENT                : return "BEGIN_TASK_EVENT";
	case MT_QUEUED_TASK_EVENT               : return "QUEUED_TASK_EVENT";
	case MT_DEQUEUED_TASK_EVENT             : return "DEQUEUED_TASK_EVENT";
	case MT_OFFER_TASK_EVENT                : return "OFFER_TASK_EVENT";
	case MT_START_TASK_EVENT                : return "START_TASK_EVENT";
	case MT_PAUSE_TASK_EVENT                : return "PAUSE_TASK_EVENT";
	case MT_RESUME_TASK_EVENT               : return "RESUME_TASK_EVENT";
	case MT_WRAPUP_TASK_EVENT               : return "WRAPUP_TASK_EVENT";
	case MT_END_TASK_EVENT                  : return "END_TASK_EVENT";
	case MT_TASK_DATA_UPDATE_EVENT          : return "TASK_DATA_UPDATE_EVENT";
	case MT_TASK_MONITOR_START_REQ          : return "TASK_MONITOR_START_REQ";
	case MT_TASK_MONITOR_START_CONF         : return "TASK_MONITOR_START_CONF";
	case MT_TASK_MONITOR_STOP_REQ           : return "TASK_MONITOR_STOP_REQ";
	case MT_TASK_MONITOR_STOP_CONF          : return "TASK_MONITOR_STOP_CONF";
	case MT_CHANGE_TASK_MONITOR_MASK_REQ    : return "CHANGE_TASK_MONITOR_MASK_REQ";
	case MT_CHANGE_TASK_MONITOR_MASK_CONF   : return "CHANGE_TASK_MONITOR_MASK_CONF";
	case MT_MAX_TASK_LIFETIME_EXCEEDED_EVENT: return "MAX_TASK_LIFETIME_EXCEEDED_EVENT";
	case MT_SET_APP_PATH_DATA_IND           : return "SET_APP_PATH_DATA_IND";
	case MT_TASK_INIT_REQ                   : return "TASK_INIT_REQ";
	case MT_TASK_INIT_RESP                  : return "TASK_INIT_RESP";
	case MT_ROUTE_REGISTER_EVENT            : return "ROUTE_REGISTER_EVENT";
	case MT_ROUTE_REGISTER_REPLY_EVENT      : return "ROUTE_REGISTER_REPLY_EVENT";
	case MT_ROUTE_REQUEST_EVENT             : return "ROUTE_REQUEST_EVENT";
	case MT_ROUTE_SELECT                    : return "ROUTE_SELECT";
	case MT_ROUTE_END                       : return "ROUTE_END";
	case MT_TEAM_CONFIG_REQ                 : return "TEAM_CONFIG_REQ";
	case MT_TEAM_CONFIG_EVENT               : return "TEAM_CONFIG_EVENT";
	case MT_TEAM_CONFIG_CONF                : return "TEAM_CONFIG_CONF";
	case MT_CALL_ATTRIBUTE_CHANGE_EVENT     : return "CALL_ATTRIBUTE_CHANGE_EVENT";
	case MT_CALL_TERMINATION_EVENT          : return "CALL_TERMINATION_EVENT";
	}
	return "UNKNOWN";
}
