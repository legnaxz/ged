#ifndef  __STATUSCODE_H__
#define  __STATUSCODE_H__


/////////////////////////////////////////////////////////////////
//	Status Code for FAILURE_CONF and FAILURE_EVENT
/////////////////////////////////////////////////////////////////

#define  E_CTI_NO_ERROR							0	// No error occurred.
#define  E_CTI_INVALID_VERSION					1	// The CTI Server does not support the protocol version number requested by the CTI client.
#define  E_CTI_INVALID_MESSAGE_TYPE				2	// A message with an invalid message type field was received.
#define  E_CTI_INVALID_FIELD_TAG				3	// A message with an invalid floating field tag was received.
#define  E_CTI_SESSION_NOT_OPEN					4	// No session is currently open on the connection.
#define  E_CTI_SESSION_ALREADY_OPEN				5	// A session is already open on the connection.
#define  E_CTI_REQUIRED_DATA_MISSING			6	// The request did not include one or more floating items that are required.
#define  E_CTI_INVALID_PERIPHERAL_ID			7	// A message with an invalid PeripheralID value was received.
#define  E_CTI_INVALID_AGENT_DATA				8	// The provided agent data item(s) are invalid.
#define  E_CTI_AGENT_NOT_LOGGED_ON				9	// The indicated agent is not currently logged on.
#define  E_CTI_DEVICE_IN_USE					10	// The indicated agent teleset is already associated with a different CTI client.
#define  E_CTI_NEW_SESSION_OPENED				11	// This session is being terminated due to a new session open request from the client.
#define  E_CTI_FUNCTION_NOT_AVAILABLE			12	// A request message was received for a function or service that was not granted to the client.
#define  E_CTI_INVALID_CALLID					13	// A request message was received with an invalid CallID value.
#define  E_CTI_PROTECTED_VARIABLE				14	// The CTI client may not update the requested variable.
#define  E_CTI_CTI_SERVER_OFFLINE				15	// The CTI Server is not able to function normally. The CTI client should close the session upon receipt of this error.
#define  E_CTI_TIMEOUT							16	// The CTI Server failed to respond to a request message within the time-out period, or no messages have been received from the CTI client within the IdleTimeout period.
#define  E_CTI_UNSPECIFIED_FAILURE				17	// An unspecified error occurred.
#define  E_CTI_INVALID_TIMEOUT					18	// The IdleTimeout field contains a value that is less than 20 seconds (4 times the minimum heartbeat interval of 5 seconds).
#define  E_CTI_INVALID_SERVICE_MASK				19	// The ServicesRequested field has unused bits set. All unused bit positions must be zero.
#define  E_CTI_INVALID_CALL_MSG_MASK			20	// The CallMsgMask field has unused bits set. All unused bit positions must be zero.
#define  E_CTI_INVALID_AGENT_STATE_MASK			21	// The AgentStateMask field has unused bits set. All unused bit positions must be zero.
#define  E_CTI_INVALID_RESERVED_FIELD			22	// A Reserved field has a non-zero value.
#define  E_CTI_INVALID_FIELD_LENGTH				23	// A floating field exceeds the allowable length for that field type.
#define  E_CTI_INVALID_DIGITS					24	// A STRING field contains characters that are not digits (¡°0¡± through ¡°9¡±).
#define  E_CTI_BAD_MESSAGE_FORMAT				25	// The message is improperly constructed. This may be caused by omitted or incorrectly sized fixed message fields.
#define  E_CTI_INVALID_TAG_FOR_MSG_TYPE			26	// A floating field tag is present that specifies a field that does not belong in this message type.
#define  E_CTI_INVALID_DEVICE_ID_TYPE			27	// A DeviceIDType field contains a value that is not in Table 6-11.
#define  E_CTI_INVALID_LCL_CONN_STATE			28	// A LocalConnectionState field contains a value that is not in Table 6-8.
#define  E_CTI_INVALID_EVENT_CAUSE				29	// An EventCause field contains a value that is not in Table 6-9.
#define  E_CTI_INVALID_NUM_PARTIES				30	// The NumParties field contains a value that exceeds the maximum (16).
#define  E_CTI_INVALID_SYS_EVENT_ID				31	// The SystemEventID field contains a value that is not in Table 6-2.
#define  E_CTI_INCONSISTENT_AGENT_DATA			32	// The provided agent extension, agent id, and/or agent instrument values are inconsistent with each other.
#define  E_CTI_INVALID_CONNECTION_ID_TYPE		33	// A ConnectionDeviceIDType field contains a value that is not in Table 6-13.
#define  E_CTI_INVALID_CALL_TYPE				34	// The CallType field contains a value that is not in Table 6-12.
#define  E_CTI_NOT_CALL_PARTY					35	// A CallDataUpdate or Release Call request specified a call that the client is not a party to.
#define  E_CTI_INVALID_PASSWORD					36	// The ClientID and Client Password provided in an OPEN_REQ message is incorrect.
#define  E_CTI_CLIENT_DISCONNECTED				37	// The client TCP/IP connection was disconnected without a CLOSE_REQ.
#define  E_CTI_INVALID_OBJECT_STATE				38	// An invalid object state value was provided.
#define  E_CTI_INVALID_NUM_SKILL_GROUPS			39	// An invalid NumSkillGroups value was provided.
#define  E_CTI_INVALID_NUM_LINES				40	// An invalid NumLines value was provided.
#define  E_CTI_INVALID_LINE_TYPE				41	// An invalid LineType value was provided.
#define  E_CTI_INVALID_ALLOCATION_STATE			42	// An invalid AllocationState value was provided.
#define  E_CTI_INVALID_ANSWERING_MACHINE		43	// An invalid AnsweringMachine value was provided.
#define  E_CTI_INVALID_CALL_MANNER_TYPE			44	// An invalid CallMannerType value was provided.
#define  E_CTI_INVALID_CALL_PLACEMENT_TYPE		45	// An invalid CallPlacementType value was provided.
#define  E_CTI_INVALID_CONSULT_TYPE				46	// An invalid ConsultType value was provided.
#define  E_CTI_INVALID_FACILITY_TYPE			47	// An invalid FacilityType value was provided.
#define  E_CTI_INVALID_MSG_TYPE_FOR_VERSION		48	// The provided MessageType is invalid for the opened protocol version.
#define  E_CTI_INVALID_TAG_FOR_VERSION			49	// A floating field tag value is invalid for the opened protocol version.
#define  E_CTI_INVALID_AGENT_WORK_MODE			50	// An invalid AgentWorkMode value was provided.
#define  E_CTI_INVALID_CALL_OPTION				51	// An invalid call option value was provided.
#define  E_CTI_INVALID_DESTINATION_COUNTRY		52	// An invalid destination country value was provided.
#define  E_CTI_INVALID_ANSWER_DETECT_MODE		53	// An invalid answer detect mode value was provided.
#define  E_CTI_MUTUALLY_EXCLUS_DEVICEID_TYPES	54	// A peripheral monitor request may not specify both a call and a device.
#define  E_CTI_INVALID_MONITORID				55	// An invalid monitorID value was provided.
#define  E_CTI_SESSION_MONITOR_ALREADY_EXISTS	56	// A requested session monitor was already created.
#define  E_CTI_SESSION_MONITOR_IS_CLIENTS		57	// A client may not monitor its own session.
#define  E_CTI_INVALID_CALL_CONTROL_MASK		58	// An invalid call control mask value was provided.
#define  E_CTI_INVALID_FEATURE_MASK				59	// An invalid feature mask value was provided.
#define  E_CTI_INVALID_TRANSFER_CONFERENCE_SETUP_MASK	60	// An invalid transfer conference setup mask value was provided.
#define  E_CTI_INVALID_ARRAY_INDEX				61	// An invalid named array index value was provided.
#define  E_CTI_INVALID_CHARACTER				62	// An invalid character value was provided.
#define  E_CTI_CLIENT_NOT_FOUND					63	// There is no open session with a matching ClientID.
#define  E_CTI_SUPERVISOR_NOT_FOUND				64	// The agent¡¯s supervisor is unknown or does not have an open CTI session.
#define  E_CTI_TEAM_NOT_FOUND					65	// The agent is not a member of an agent team.
#define  E_CTI_NO_CALL_ACTIVE					66	// The specified agent does not have an active call.
#define  E_CTI_NAMED_VARIABLE_NOT_CONFIGURED	67	// The specified named variable is not configured in the Unified ICM.
#define  E_CTI_NAMED_ARRAY_NOT_CONFIGURED		68	// The specified named array is not configured in the Unified ICM.
#define  E_CTI_INVALID_CALL_VARIABLE_MASK		69	// The specified call variable mask in not valid.
#define  E_CTI_ELEMENT_NOT_FOUND				70	// An internal error occurred manipulating a named variable or named array element.
#define  E_CTI_INVALID_DISTRIBUTION_TYPE		71	// The specified distribution type is invalid.
#define  E_CTI_INVALID_SKILL_GROUP				72	// The specified skill group is invalid.
#define  E_CTI_TOO_MUCH_DATA					73	// The total combined size of named variables and named arrays may not exceed the limit of 2000 bytes.
#define  E_CTI_VALUE_TOO_LONG					74	// The value of the specified named variable or named array element exceeds the maximum permissible length.
#define  E_CTI_SCALAR_FUNCTION_ON_ARRAY			75	// A NamedArray was specified with a NamedVariable tag.
#define  E_CTI_ARRAY_FUNCTION_ON_SCALAR			76	// A NamedVariable was specified with a NamedArray tag.
#define  E_CTI_INVALID_NUM_NAMED_VARIABLES		77	// The value in the NumNamedVariables field is different than the number of NamedVariable floating fields in the message.
#define  E_CTI_INVALID_NUM_NAMED_ARRAYS			78	// The value in the NumNamedArrays field is different than the number of NamedArray floating fields in the message.
#define  E_CTI_INVALID_RTP_DIRECTION			79	// The RTP direction value is invalid.
#define  E_CTI_INVALID_RTP_TYPE					80	// The RTP type value is invalid.
#define  E_CTI_CALLED_PARTY_DISPOSITION			81	// The called party disposition is invalid.
#define  E_CTI_INVALID_SUPERVISORY_ACTION		82	// The supervisory action is invalid.
#define  E_CTI_AGENT_TEAM_MONITOR_ALREADY_EXISTS	83	// The agent team monitor already exists.
#define  E_CTI_INVALID_SERVICE					84	// The ServiceNumber or ServiceID value is invalid.
#define  E_CTI_SERVICE_CONFLICT					85	// The ServiceNumber and ServiceID values given represent different services.
#define  E_CTI_SKILL_GROUP_CONFLICT				86	// The SkillGroupNumber/SkillGroupPriority and SkillGroupID values given represent different skill groups.
#define  E_CTI_INVALID_DEVICE					87	// The specified device is invalid.
#define  E_CTI_INVALID_MR_DOMAIN				88	// Media Routing Domain is invalid.
#define  E_CTI_MONITOR_ALREADY_EXISTS			89	// Monitor already exists.
#define  E_CTI_MONITOR_TERMINATED				90	// Monitor has terminated.
#define  E_CTI_INVALID_TASK_MSG_MASK			91	// The task msg mask is invalid.
#define  E_CTI_SERVER_NOT_MASTER				92	// The server is a standby server.
#define  E_CTI_INVALID_CSD						93	// The CSD Specified is invalid (Unified CCX Only).
#define  E_CTI_JTAPI_CCM_PROBLEM				94	// Indicates a JTAPI or Unified CM problem.
#define  E_INVALID_CONFIG_MSG_MASK				95	// Indicates a bad config mask in OPEN_REQ.
#define  E_CTI_AUTO_CONFIG_RESET				96	// Indicates a configuration change (Unified CCX only).
#define  E_CTI_INVALID_MONITOR_STATUS			97	// Indicates an invalid monitor.
#define  E_CTI_INVALID_REQUEST_TYPE				98	// Indicates an invalid request ID type.


const char *strErrorStatus(UINT e);


#endif		// end of __STATUSCODE_H__
