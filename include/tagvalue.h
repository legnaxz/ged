#ifndef  __TAGVALUE_H__
#define  __TAGVALUE_H__


/////////////////////////////////////////////////////////////////
//	Tag Value for Floating
/////////////////////////////////////////////////////////////////

#define  CLIENT_ID_TAG						1
#define  CLIENT_PASSWORD_TAG				2
#define  CLIENT_SIGNATURE_TAG				3
#define  AGENT_EXTENSION_TAG				4
#define  AGENT_ID_TAG						5
#define  AGENT_INSTRUMENT_TAG				6
#define  TEXT_TAG							7
#define  ANI_TAG							8
#define  UUI_TAG							9
#define  DNIS_TAG							10
#define  DIALED_NUMBER_TAG					11
#define  CED_TAG							12
#define  CALL_VAR_1_TAG						13
#define  CALL_VAR_2_TAG						14
#define  CALL_VAR_3_TAG						15
#define  CALL_VAR_4_TAG						16
#define  CALL_VAR_5_TAG						17
#define  CALL_VAR_6_TAG						18
#define  CALL_VAR_7_TAG						19
#define  CALL_VAR_8_TAG						20
#define  CALL_VAR_9_TAG						21
#define  CALL_VAR_10_TAG					22
#define  CTI_CLIENT_SIGNATURE_TAG			23
#define  CTI_CLIENT_TIMESTAMP_TAG			24
#define  CONNECTION_DEVID_TAG				25
#define  ALERTING_DEVID_TAG					26
#define  CALLING_DEVID_TAG					27
#define  CALLED_DEVID_TAG					28
#define  LAST_REDIRECT_DEVID_TAG			29
#define  ANSWERING_DEVID_TAG				30
#define  HOLDING_DEVID_TAG					31
#define  RETRIEVING_DEVID_TAG				32
#define  RELEASING_DEVID_TAG				33
#define  FAILING_DEVID_TAG					34
#define  PRIMARY_DEVID_TAG					35
#define  SECONDARY_DEVID_TAG				36
#define  CONTROLLER_DEVID_TAG				37
#define  ADDED_PARTY_DEVID_TAG				38
#define  PARTY_CALLID_TAG					39
#define  PARTY_DEVID_TYPE_TAG				40
#define  PARTY_DEVID_TAG					41
#define  TRANSFERRING_DEVID_TAG				42
#define  TRANSFERRED_DEVID_TAG				43
#define  DIVERTING_DEVID_TAG				44
#define  QUEUE_DEVID_TAG					45
#define  CALL_WRAPUP_DATA_TAG				46
#define  NEW_CONNECTION_DEVID_TAG			47
#define  TRUNK_USED_DEVID_TAG				48
#define  AGENT_PASSWORD_TAG					49
#define  ACTIVE_CONN_DEVID_TAG				50
#define  FACILITY_CODE_TAG					51
#define  OTHER_CONN_DEVID_TAG				52
#define  HELD_CONN_DEVID_TAG				53

// (reserved)	54-55

#define  CALL_CONN_CALLID_TAG				56
#define  CALL_CONN_DEVID_TYPE_TAG			57
#define  CALL_CONN_DEVID_TAG				58
#define  CALL_DEVID_TYPE_TAG				59
#define  CALL_DEVID_TAG						60            
#define  CALL_DEV_CONN_STATE_TAG			61
#define  SKILL_GROUP_NUMBER_TAG				62
#define  SKILL_GROUP_ID_TAG					63
#define  SKILL_GROUP_PRIORITY_TAG			64
#define  SKILL_GROUP_STATE_TAG				65
#define  OBJECT_NAME_TAG					66
#define  DTMF_STRING_TAG					67
#define  POSITION_ID_TAG					68
#define  SUPERVISOR_ID_TAG					69
#define  LINE_HANDLE_TAG					70
#define  LINE_TYPE_TAG						71   
#define  ROUTER_CALL_KEY_DAY_TAG			72
#define  ROUTER_CALL_KEY_CALLID_TAG			73
//#define  ROUTER_CALL_KEY_SEQUENCE_NUM_TAG	110

// (reserved)	74

#define  CALL_STATE_TAG						75
#define  MONITORED_DEVID_TAG				76
#define  AUTHORIZATION_CODE_TAG				77
#define  ACCOUNT_CODE_TAG					78
#define  ORIGINATING_DEVID_TAG				79
#define  ORIGINATING_LINE_ID_TAG			80
#define  CLIENT_ADDRESS_TAG					81
#define  NAMED_VARIABLE_TAG					82
#define  NAMED_ARRAY_TAG					83
#define  CALL_CONTROL_TABLE_TAG				84
#define  SUPERVISOR_INSTRUMENT_TAG			85
#define  ATC_AGENT_ID_TAG					86
#define  AGENT_FLAGS_TAG					87
#define  ATC_AGENT_STATE_TAG				88
#define  ATC_STATE_DURATION_TAG				89
#define  AGENT_CONNECTION_DEVID_TAG			90
#define  SUPERVISOR_CONNECTION_DEVID_TAG	91
#define  LIST_TEAM_ID_TAG					92
#define  DEFAULT_DEVICE_PORT_ADDRESS_TAG	93
#define  SERVICE_NAME_TAG					94
#define  CUSTOMER_PHONE_NUMBER_TAG			95
#define  CUSTOMER_ACCOUNT_NUMBER_TAG		96
#define  APP_PATH_ID_TAG					97
#define  ROUTER_CALL_KEY_SEQUENCE_NUM_TAG	110
#define  TRUNK_NUMBER_TAG					121
#define  TRUNK_GROUP_NUMBER_TAG				122
#define  EXT_AGENT_STATE_TAG				123
#define  DEQUEUE_TYPE_TAG					124
#define  SENDING_ADDRESS_TAG				125
#define  SENDING_PORT_TAG					126

// Unused       127-128

#define  MAX_QUEUED_TAG						129
#define  QUEUE_ID_TAG						130
#define  CUSTOMER_ID_TAG					131
#define  SERVICE_SKILL_TARGET_ID_TAG		132
#define  PERIPHERAL_NAME_TAG				133
#define  DESCRIPTION_TAG					134
#define  SERVICE_MEMBER_ID_TAG				135
#define  SERVICE_MEMBER_PRIORITY_TAG		136
#define  FIRST_NAME_TAG						137
#define  LAST_NAME_TAG						138
#define  SKILL_GROUP_TAG					139
#define  AGENT_SKILL_TARGET_ID_TAG			141
#define  SERVICE_TAG						142

// Reserved	143-149

#define  DURATION_TAG						150

// Reserved	151-172

#define  EXTENSION_TAG						173
#define  SERVICE_LEVEL_THRESHOLD_TAG		174
#define  SERVICE_LEVEL_TYPE_TAG				175
#define  CONFIG_PARAM_TAG					176
#define  SERVICE_CONFIG_KEY_TAG				177
#define  SKILL_GROUP_CONFIG_KEY_TAG			178
#define  AGENT_CONFIG_KEY_TAG				179
#define  DEVICE_CONFIG_KEY_TAG				180

// Unused		181-182

#define  RECORD_TYPE_TAG					183
#define  PERIPHERAL_NUMBER_TAG				184
#define  AGENT_SKILL_TARGET_ID_TAG_			185
#define  NUM_SERVICE_MEMBERS_TAG			186
#define  SERVICE_MEMBER_TAG					187
#define  SERVICE_PRIORITY_TAG				188
#define  AGENT_TYPE_TAG						189
#define  LOGIN_ID_TAG						190
#define  NUM_SKILLS_TAG						191
#define  SKILL_GROUP_SKILL_TARGET_ID_TAG	192
#define  SERVICE_ID_TAG						193
#define  AGENT_ID_LONG_TAG					194
#define  DEVICE_TYPE_TAG					195

// Unused	196-197

#define  ENABLE_TAG							198
#define  DEVICEID_TAG						199
#define  TIMEOUT_TAG						200
#define  CURRENT_ROUTE_TAG					201
#define  SECONDARY_CONNECTION_CALL_ID		202
#define  PRIORITY_QUEUE_NUMBER_TAG			203
#define  TEAM_NAME_TAG						204
#define  MEMBER_TYPE_TAG					205
#define  EVENT_DEVICE_ID_TAG				206
#define  LOGIN_NAME_TAG 					207		// (V11)
#define  PERIPHERAL_ID_TAG					208		// (V11)
#define  CALL_TYPE_KEY_CONFIG_TAG			209		// (V11)
#define  CALL_TYPE_ID_TAG					210		// (V11)
#define  CUSTOMER_DEFINITION_ID_TAG			211		// (V11)
#define  ENTERPRISE_NAME_TAG				212		// (V11)
#define  CUR_PERIPHERAL_NUMBER_TAG			213
#define  CUR_LOGIN_ID						214
#define  ANI_II_TAG							215
#define  MR_DOMAIN_ID_TAG					216
#define  CTIOS_CIL_CLIENT_ID_TAG			217
#define  SILENT_MONITOR_STATUS_TAG			218
#define  REQUESTING_DEVICE_ID_TAG			219
#define  REQUESTING_DEVICE_ID_TYPE_TAG		220
#define  PRE_CALL_INVOKE_ID_TAG				221
#define  ENTERPRISE_QUEUE_TIME				222
#define  CALL_REFERENCE_ID_TAG				223
#define  MULTI_LINE_AGENT_CONTROL_TAG		224
#define  NETWORK_CONTROLLED_TAG				225
#define  CLIENT_ADDRESS_IPV6_TAG			226
#define  SENDING_ADDRESS_IPV6_TAG			227
#define  NUM_PERIPHERALS_TAG				228
#define  COC_CONNECTION_CALL_ID_TAG			229
#define  COC_CONNECTION_DEVICE_ID_TYPE_TAG	230
#define  COC_CONNECTION_DEVICE_ID_TAG		231
#define  CALL_ORIGINATED_FROM_TAG			232
#define  SET_APPDATA_CALLID_TAG				233
#define  CLIENT_SHARE_KEY_TAG				234


const char *strTagValue(UCHAR t);


#endif		// end of __TAGVALUE_H__
