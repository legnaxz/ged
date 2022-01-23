#ifndef  __MASK_H__
#define  __MASK_H__


/////////////////////////////////////////////////////////////////
// CTI Service Mask
/////////////////////////////////////////////////////////////////

#define  CTI_SERVICE_DEBUG					0x80000000	// Causes all messages exchanged during the current session to be captured to a file for later analysis.
#define  CTI_SERVICE_CLIENT_EVENTS			0x00000001	// Client receives call and agent state change events associated with a specific ACD phone.
#define  CTI_SERVICE_CALL_DATA_UPDATE		0x00000002	// Client may modify call context data.
#define  CTI_SERVICE_CLIENT_CONTROL			0x00000004	// Client may control calls and agent states associated with a specific ACD phone.
#define  CTI_SERVICE_CONNECTION_MONITOR		0x00000008	// Establishment and termination of this session cause corresponding Unified ICM Alarm events to be generated.
#define  CTI_SERVICE_ALL_EVENTS				0x00000010	// Client receives all call and agent state change events (associated with any ACD phone).
#define  CTI_SERVICE_PERIPHERAL_MONITOR		0x00000020	// Client may dynamically add and remove devices and/or calls that it wishes to receive call and agent state events for.
#define  CTI_SERVICE_CLIENT_MONITOR			0x00000040	// Client receives notification when all other CTI client sessions are opened and closed, and may monitor the activity of other CTI client sessions.
#define  CTI_SERVICE_SUPERVISOR				0x00000080	// Client may request supervisor services.
#define  CTI_SERVICE_SERVER					0x00000100	// Client identify itself as server application.
#define  CTI_SERVICE_AGENT_REPORTING		0x00000400	// Client may reporting/routing ARM(Agent Reporting And Management) messages.
#define  CTI_SERVICE_ALL_TASK_EVENTS		0x00000800	// Client receives all task events.
#define  CTI_SERVICE_TASK_MONITOR			0x00001000	// Client receives monitored task events.
#define  CTI_AGENT_STATE_CONTROL_ONLY		0x00002000	// Client can change agent state only. Call control is not allowed. If a client requests for CTI_SERVICE_ CLIENT_CONTROL, the server may grant this flag to indicate that only agent state change is allowed.

// Unused	0x00004000

#define  CTI_DEVICE_STATE_CONTROL			0x00008000	// The client/server wishes to register and get resource state change requests
#define  CTI_SERVICE_UPDATE_EVENTS			0x00080000	// Requests that this client receive update notification events. (No data)
#define  CTI_SERVICE_IGNORE_DUPLICATE_AGENT_EVENTS	0x00100000	// Request to suppress duplicate agent state events.
#define  CTI_SERVICE_IGNORE_CONF			0x00200000	// Do not send confirmations for third party requests.
#define  CTI_SERVICE_ACD_LINE_ONLY			0x00400000	// Request that events for non-ACD lines not be sent. (Unified CCE only)


/////////////////////////////////////////////////////////////////
// Unsolicited Call Event Message Mask
/////////////////////////////////////////////////////////////////

#define  CALL_DELIVERED_MASK				0x00000001	// Set when client wishes to receive CALL_DELIVERED_MASK EVENT messages.
#define  CALL_QUEUED_MASK					0x00000002	// Set when client wishes to receive CALL_QUEUED_MASK EVENT messages.
#define  CALL_ESTABLISHED_MASK				0x00000004	// Set when client wishes to receive CALL_ESTABLISHED_MASK EVENT messages.
#define  CALL_HELD_MASK						0x00000008	// Set when client wishes to receive CALL_HELD_MASK EVENT messages.
#define  CALL_RETRIEVED_MASK				0x00000010	// Set when client wishes to receive CALL_RETRIEVED_MASK EVENT messages.
#define  CALL_CLEARED_MASK					0x00000020	// Set when client wishes to receive CALL_CLEARED_MASK EVENT messages.
#define  CALL_CONNECTION_CLEARED_MASK		0x00000040	// Set when client wishes to receive CALL_CONNECTION_CLEARED_MASK EVENT messages.
#define  CALL_ORIGINATED_MASK				0x00000080	// Set when client wishes to receive CALL_ ORIGINATED_MASK EVENT messages.
#define  CALL_CONFERENCED_MASK				0x00000100	// Set when client wishes to receive CALL_CONFERENCED_MASK EVENT messages.
#define  CALL_TRANSFERRED_MASK				0x00000200	// Set when client wishes to receive CALL_ TRANSFERRED_MASK EVENT messages.
#define  CALL_DIVERTED_MASK					0x00000400	// Set when client wishes to receive CALL_DIVERTED_MASK EVENT messages.
#define  CALL_SERVICE_INITIATED_MASK		0x00000800	// Set when client wishes to receive CALL_SERVICE_INITIATED_MASK EVENT messages.
#define  CALL_TRANSLATION_ROUTE_MASK		0x00001000	// Set when client wishes to receive CALL_ TRANSLATION_ROUTE_MASK EVENT messages.
#define  BEGIN_CALL_MASK					0x00002000	// Set when client wishes to receive BEGIN_CALL_MASK EVENT messages.
#define  END_CALL_MASK						0x00004000	// Set when client wishes to receive END_CALL_MASK EVENT messages.
#define  CALL_DATA_UPDATE_MASK				0x00008000	// Set when client wishes to receive CALL_DATA_UPDATE_MASK EVENT messages.
#define  CALL_FAILED_MASK					0x00010000	// Set when client wishes to receive CALL_FAILED_MASK EVENT messages.
#define  CALL_REACHED_NETWORK_MASK			0x00020000	// Set when client wishes to receive CALL_REACHED_NETWORK_MASK EVENT messages.
#define  CALL_DEQUEUED_MASK					0x00040000	// Set when client wished to receive CALL_DEQUEUED_MASK EVENT messages.
#define  AGENT_PRE_CALL_MASK				0x00080000	// Set when client wished to receive AGENT_PRE_CALL_MASK EVENT messages.
#define  AGENT_PRE_CALL_ABORT_MASK			0x00100000	// Set when client wished to receive AGENT_PRE_CALL_ABORT_MASK EVENT messages.
#define  RTP_STARTED_MASK					0x00200000	// Set when client wished to receive RTP_STARTED_MASK EVENT messages.
#define  RTP_STOPPED_MASK					0x00400000	// Set when client wished to receive RTP_STOPPED_MASK EVENT messages.
#define  AGENT_TEAM_CONFIG_MASK				0x00800000	// Set when client wished to receive AGENT_TEAM_CONFIG_MASK EVENT messages.
#define  AGENT_LEGACY_PRE_CALL_MASK			0x01000000	// Set when client wished to receive AGENT_LEGACY_PRE_CALL_MASK EVENT messages.
#define  CALL_ATTRIBUTE_CHANGE_MASK			0x02000000	// CALL_ATTRIBUTE_CHANGE_MASK EVENT messages.


/////////////////////////////////////////////////////////////////
// Agent State Mask
/////////////////////////////////////////////////////////////////

#define  AGENT_LOGIN_MASK					0x00000001	// Set when client wishes to receive ¡°login¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_LOGOUT_MASK					0x00000002	// Set when client wishes to receive ¡°logout¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_NOT_READY_MASK				0x00000004	// Set when client wishes to receive ¡°not ready¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_AVAILABLE_MASK				0x00000008	// Set when client wishes to receive ¡°available¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_TALKING_MASK					0x00000010	// Set when client wishes to receive ¡°talking¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_WORK_NOT_READY_MASK			0x00000020	// Set when client wishes to receive ¡°work not ready¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_WORK_READY_MASK				0x00000040	// Set when client wishes to receive ¡°work ready¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_BUSY_OTHER_MASK				0x00000080	// Set when client wishes to receive ¡°busy other¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_RESERVED_MASK				0x00000100	// Set when client wishes to receive ¡°reserved¡± AGENT_STATE_MASK EVENT messages.
#define  AGENT_HOLD_MASK					0x00000200	// Set when client wishes to receive ¡°hold¡± AGENT_STATE_MASK EVENT messages.


#endif		// end of __MASK_H__
