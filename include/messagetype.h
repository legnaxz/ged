#ifndef  __MESSAGETYPE_H__
#define  __MESSAGETYPE_H__


/////////////////////////////////////////////////////////////////
//	Message Type (MT)
/////////////////////////////////////////////////////////////////

#define  MT_FAILURE_CONF					1	// Negative confirmation; may be sent in response to any request.
#define  MT_FAILURE_EVENT					2	// Unsolicited notification of a failure or error.
#define  MT_OPEN_REQ						3	// Communication session establishment request.
#define  MT_OPEN_CONF						4	// Communication session establishment confirmation.
#define  MT_HEARTBEAT_REQ					5	// Communication session maintenance request.
#define  MT_HEARTBEAT_CONF					6	// Communication session maintenance confirmation.
#define  MT_CLOSE_REQ						7	// Communication session termination request.
#define  MT_CLOSE_CONF						8	// Communication session termination confirmation.
#define  MT_CALL_DELIVERED_EVENT			9	// Notification of inbound call arrival.
#define  MT_CALL_ESTABLISHED_EVENT			10	// Notification of answering of inbound call.
#define  MT_CALL_HELD_EVENT					11	// Notification of call placed on hold.
#define  MT_CALL_RETRIEVED_EVENT			12	// Notification of call taken off hold.
#define  MT_CALL_CLEARED_EVENT				13	// Notification of call termination.
#define  MT_CALL_CONNECTION_CLEARED_EVENT	14	// Notification of the termination of a conference party connection.
#define  MT_CALL_ORIGINATED_EVENT			15	// Notification of outbound call initiation.
#define  MT_CALL_FAILED_EVENT				16	// Notification of inability to complete call.
#define  MT_CALL_CONFERENCED_EVENT			17	// Notification of tandem connection of two calls.
#define  MT_CALL_TRANSFERRED_EVENT			18	// Notification of call transfer.
#define  MT_CALL_DIVERTED_EVENT				19	// Notification of call changing to a different service.
#define  MT_CALL_SERVICE_INITIATED_EVENT	20	// Notification of the initiation of telecommunications service at a device (¡°dial-tone¡±).
#define  MT_CALL_QUEUED_EVENT				21	// Notification of call being placed in a queue pending the availability of some resource.
#define  MT_CALL_TRANSLATION_ROUTE_EVENT	22	// Notification of call context data for a call that has been routed to the peripheral via a translation route.
#define  MT_BEGIN_CALL_EVENT				23	// Notification that a call has been associated with the CTI client.
#define  MT_END_CALL_EVENT					24	// Notification that a call is no longer associated with a CTI client.
#define  MT_CALL_DATA_UPDATE_EVENT			25	// Notification of a change in a call¡¯s context data.
#define  MT_SET_CALL_DATA_REQ				26	// Request to update one or more call variables or call wrap-up data.
#define  MT_SET_CALL_DATA_CONF				27	// Response confirming a previous SET_CALL_DATA request.
#define  MT_RELEASE_CALL_REQ				28	// Notification that all call data updates are complete.
#define  MT_RELEASE_CALL_CONF				29	// Response confirming a previous RELEASE_CALL request.
#define  MT_AGENT_STATE_EVENT				30	// Notification of new agent state.
#define  MT_SYSTEM_EVENT					31	// Notification of a PG Status change.
#define  MT_CLIENT_EVENT_REPORT_REQ			32	// Request to report a CTI client event.
#define  MT_CLIENT_EVENT_REPORT_CONF		33	// Response confirming a previous CLIENT_EVENT_REPORT request.
#define  MT_CALL_REACHED_NETWORK_EVENT		34	// Notification of outbound call being connected to the network.
#define  MT_CONTROL_FAILURE_CONF			35	// Response indicating the failure of a proceeding control request.
#define  MT_QUERY_AGENT_STATE_REQ			36	// Request to obtain the current state of an agent position.
#define  MT_QUERY_AGENT_STATE_CONF			37	// Response to a QUERY_AGENT_ STATE request.
#define  MT_SET_AGENT_STATE_REQ				38	// Request to alter the current state of an agent position.
#define  MT_SET_AGENT_STATE_CONF			39	// Response confirming a previous SET_AGENT_STATE request.
#define  MT_ALTERNATE_CALL_REQ				40	// Request to alternate between a held and an active call.
#define  MT_ALTERNATE_CALL_CONF				41	// Response confirming a previous ALTERNATE_CALL request.
#define  MT_ANSWER_CALL_REQ					42	// Request to answer an alerting call.
#define  MT_ANSWER_CALL_CONF				43	// Response confirming a previous ANSWER_CALL request.
#define  MT_CLEAR_CALL_REQ					44	// Request to release all devices from a call.
#define  MT_CLEAR_CALL_CONF					45	// Response confirming a previous CLEAR_CALL request.
#define  MT_CLEAR_CONNECTION_REQ			46	// Request to release a single device from a call.
#define  MT_CLEAR_CONNECTION_CONF			47	// Response confirming a previous CLEAR_CONNECTION request.
#define  MT_CONFERENCE_CALL_REQ				48	// Request to conference a held call with an active call.
#define  MT_CONFERENCE_CALL_CONF			49	// Response confirming a previous CONFERENCE_CALL request.
#define  MT_CONSULTATION_CALL_REQ			50	// Request to hold an active call and initiate a new call.
#define  MT_CONSULTATION_CALL_CONF			51	// Response confirming a previous CONSULTATION_CALL request.
#define  MT_DEFLECT_CALL_REQ				52	// Request to move an alerting call to a different device.
#define  MT_DEFLECT_CALL_CONF				53	// Response confirming a previous DEFLECT_CALL request.
#define  MT_HOLD_CALL_REQ					54	// Request to place a call connection in the held state.
#define  MT_HOLD_CALL_CONF					55	// Response confirming a previous HOLD_CALL request.
#define  MT_MAKE_CALL_REQ					56	// Request to initiate a new call between two devices.
#define  MT_MAKE_CALL_CONF					57	// Response confirming a previous MAKE_CALL request.
#define  MT_MAKE_PREDICTIVE_CALL_REQ		58	// Request to initiate a new predictive call.
#define  MT_MAKE_PREDICTIVE_CALL_CONF		59	// Response confirming a previous MAKE_PREDICTIVE_CALL request.
#define  MT_RECONNECT_CALL_REQ				60	// Request to clear a connection and retrieve a held call.
#define  MT_RECONNECT_CALL_CONF				61	// Response confirming a previous RECONNECT_CALL request.
#define  MT_RETRIEVE_CALL_REQ				62	// Request to reconnect a held call.
#define  MT_RETRIEVE_CALL_CONF				63	// Response confirming a previous RETRIEVE_CALL request.
#define  MT_TRANSFER_CALL_REQ				64	// Request to transfer a held call to an active call.
#define  MT_TRANSFER_CALL_CONF				65	// Response confirming a previous TRANSFER_CALL request.

// (reserved)	66- 77

#define  MT_QUERY_DEVICE_INFO_REQ			78	// Request to obtain general device information.
#define  MT_QUERY_DEVICE_INFO_CONF			79	// Response to a previous QUERY_DEVICE_INFO request.

// (reserved)	80-81

#define  MT_SNAPSHOT_CALL_REQ				82	// Request to obtain information about a specified call.
#define  MT_SNAPSHOT_CALL_CONF				83	// Response to a previous SNAPSHOT_CALL request.
#define  MT_SNAPSHOT_DEVICE_REQ				84	// Request to obtain information about a specified device.
#define  MT_SNAPSHOT_DEVICE_CONF			85	// Response to a previous SNAPSHOT_DEVICE request.
#define  MT_CALL_DEQUEUED_EVENT				86	// Notification of call being removed from a queue.

// (reserved)	87- 90

#define  MT_SEND_DTMF_SIGNAL_REQ			91	// Request to transmit a sequence of DTMF tones.
#define  MT_SEND_DTMF_SIGNAL_CONF			92	// Response to a previous SEND_ DTMF_SIGNAL_REQ request.
#define  MT_MONITOR_START_REQ				93	// Request to initiate monitoring of a given call or device.
#define  MT_MONITOR_START_CONF				94	// Response to a previous MONITOR_START request.
#define  MT_MONITOR_STOP_REQ				95	// Request to terminate monitoring of a given call or device.
#define  MT_MONITOR_STOP_CONF				96	// Response to a previous MONITOR_STOP request.
#define  MT_CHANGE_MONITOR_MASK_REQ			97	// Request to change the message masks of a given call or device monitor.
#define  MT_CHANGE_MONITOR_MASK_CONF		98	// Response to a previous CHANGE_ MONITOR_MASK request.
#define  MT_CLIENT_SESSION_OPENED_EVENT		99	// Notification that a new CTI Client session has been opened.
#define  MT_CLIENT_SESSION_CLOSED_EVENT		100	// Notification that a CTI Client session has been terminated.
#define  MT_SESSION_MONITOR_START_REQ		101	// Request to initiate monitoring of a given CTI Client session.
#define  MT_SESSION_MONITOR_START_CONF		102	// Response to a previous SESSION_ MONITOR_START request.
#define  MT_SESSION_MONITOR_STOP_REQ		103	// Request to terminate monitoring of a given CTI Client session.
#define  MT_SESSION_MONITOR_STOP_CONF		104	// Response to a previous SESSION_ MONITOR_STOP request.
#define  MT_AGENT_PRE_CALL_EVENT			105	// Advance notification of a call routed to an Enterprise Agent.
#define  MT_AGENT_PRE_CALL_ABORT_EVENT		106	// Cancellation of advance notification of a call routed to an Enterprise Agent.
#define  MT_USER_MESSAGE_REQ				107	// Request to send a message to other CTI Server client(s).
#define  MT_USER_MESSAGE_CONF				108	// Response to a previous USER_MESSAGE_REQ request.
#define  MT_USER_MESSAGE_EVENT				109	// Notification of a message sent by another CTI Server client.
#define  MT_REGISTER_VARIABLES_REQ			110	// Request to register call context variables used by application.
#define  MT_REGISTER_VARIABLES_CONF			111	// Response to a previous REGISTER_VARIABLES_REQ request.
#define  MT_QUERY_AGENT_STATISTICS_REQ		112	// Request for current agent call handling statistics.
#define  MT_QUERY_AGENT_STATISTICS_CONF		113	// Response to a previous QUERY_AGENT_STATISTICS_REQ request.
#define  MT_QUERY_SKILL_GROUP_STATISTICS_REQ	114	// Request for current skill group call handling statistics.
#define  MT_QUERY_SKILL_GROUP_STATISTICS_CONF	115	// Response to a previous QUERY_SKILL_GROUP_ STATISTICS_REQ request.
#define  MT_RTP_STARTED_EVENT				116	// Indicates that a RTP input has been started
#define  MT_RTP_STOPPED_EVENT				117	// Indicates that a RTP input has been stopped
#define  MT_SUPERVISOR_ASSIST_REQ			118	// An agent requests for assistance to their supervisor.
#define  MT_SUPERVISOR_ASSIST_CONF			119	// Response to a previous SUPERVISOR_ASSIST_REQ request.
#define  MT_SUPERVISOR_ASSIST_EVENT			120	// Notification of a supervisor assist request sent by a CTI Server client.
#define  MT_EMERGENCY_CALL_REQ				121	// An agent declaring an emergency situation to their supervisor.
#define  MT_EMERGENCY_CALL_CONF				122	// Response to a previous EMERGENCY_CALL_REQ request.
#define  MT_EMERGENCY_CALL_EVENT			123	// Notification of an emergency call request sent by a CTI Server client.
#define  MT_SUPERVISE_CALL_REQ				124	// A supervisor request to perform monitor or barge-in operations.
#define  MT_SUPERVISE_CALL_CONF				125	// Response to a previous SUPERVISE_CALL_REQ request.
#define  MT_AGENT_TEAM_CONFIG_REQ			126	// Request to change temporary agent team configuration.
#define  MT_AGENT_TEAM_CONFIG_CONF			127	// Response to a previous AGENT_TEAM_CONFIG_REQ request.
#define  MT_AGENT_TEAM_CONFIG_EVENT			128	// Notification of passing the team member list
#define  MT_SET_APP_DATA_REQ				129	// Request to update one or more application variables.
#define  MT_SET_APP_DATA_CONF				130	// Response confirming a previous SET_APP_DATA request.
#define  MT_AGENT_DESK_SETTINGS_REQ			131	// Request to obtain Agent Desk Settings.
#define  MT_AGENT_DESK_SETTINGS_CONF		132	// Response to a previous AGENT_DESK_SETTINGS_REQ request.
#define  MT_LIST_AGENT_TEAM_REQ				133	// Request to obtain a list of Agent Teams.
#define  MT_LIST_AGENT_TEAM_CONF			134	// Response to a previous LIST_AGENT_TEAM_REQ request.
#define  MT_MONITOR_AGENT_TEAM_START_REQ	135	// Request to start monitoring an Agent Team.
#define  MT_MONITOR_AGENT_TEAM_START_CONF	136	// Response to a previous MONITOR_ AGENT_TEAM_START_REQ request.
#define  MT_MONITOR_AGENT_TEAM_STOP_REQ		137	// Request to stop monitoring an Agent Team.
#define  MT_MONITOR_AGENT_TEAM_STOP_CONF	138	// Response to a previous MONITOR_ AGENT_TEAM_STOP_REQ request.
#define  MT_BAD_CALL_REQ					139	// Request to mark a call as having poor voice quality.
#define  MT_BAD_CALL_CONF					140	// Response to a previous BAD_CALL_REQ request.
#define  MT_SET_DEVICE_ATTRIBUTES_REQ		141	// Request to set the default attributes of a calling device.
#define  MT_SET_DEVICE_ATTRIBUTES_CONF		142	// Response to a previous SET_DEVICE_ ATTRIBUTES_REQ request.
#define  MT_REGISTER_SERVICE_REQ			143	// Request to register service for the server application.
#define  MT_REGISTER_SERVICE_CONF			144	// Response to a previous REGISTER_SERVICE_REQ request.
#define  MT_UNREGISTER_SERVICE_REQ			145	// Request to unregister service for the server application.
#define  MT_UNREGISTER_SERVICE_CONF			146	// Response to a previous UNREGISTER_SERVICE_REQ request.
#define  MT_START_RECORDING_REQ				147	// Request to start recording.
#define  MT_START_RECORDING_CONF			148	// Response to a previous START_RECORDING_REQ request.
#define  MT_STOP_RECORDING_REQ				149	// Request to stop recording
#define  MT_STOP_RECORDING_CONF				150	// Response to a previous STOP_RECORDING_REQ request.
#define  MT_MEDIA_LOGIN_REQ					151	// Report agent login to MRD.
#define  MT_MEDIA_LOGIN_RESP				152	// Response to MEDIA_LOGIN_REQ.
#define  MT_MEDIA_LOGOUT_IND				153	// Report agent logout from MRD.
#define  MT_MAKE_AGENT_ROUTABLE_IND			154	// Make agent routable for MRD request.
#define  MT_MAKE_AGENT_NOT_ROUTABLE_REQ		155	// Make agent not routable for MRD request.
#define  MT_MAKE_AGENT_NOT_ROUTABLE_RESP	156	// Response to MAKE_AGENT_NOT_ROUTABLE_REQ.
#define  MT_MAKE_AGENT_READY_IND			157	// Report agent made ready.
#define  MT_MAKE_AGENT_NOT_READY_REQ		158	// Report agent made not ready.
#define  MT_MAKE_AGENT_NOT_READY_RESP		159	// Response to MAKE_AGENT_NOT_READY_REQ.
#define  MT_OFFER_TASK_IND					160	// Report agent has been offered task, agent selected by Unified ICM.
#define  MT_OFFER_APPLICATION_TASK_REQ		161	// Report agent has been offered task, agent not selected by Unified ICM.
#define  MT_OFFER_APPLICATION_TASK_RESP		162	// Response to OFFER_APPLICATION_TASK_REQ.
#define  MT_START_TASK_IND					163	// Report agent has begun task, agent selected by Unified ICM.
#define  MT_START_APPLICATION_TASK_REQ		164	// Report agent has begun task, agent not selected by Unified ICM.
#define  MT_START_APPLICATION_TASK_RESP		165	// Response to START_APPLICATION_TASK_REQ.
#define  MT_PAUSE_TASK_IND					166	// Report agent has paused task.
#define  MT_RESUME_TASK_IND					167	// Report agent has resumed task.
#define  MT_WRAPUP_TASK_IND					168	// Report agent has entered wrapup for task.
#define  MT_END_TASK_IND					169	// Report agent has ended task.
#define  MT_AGENT_MADE_NOT_ROUTABLE_EVENT	170	// Notify client that agent made not routable for MRD.
#define  MT_AGENT_INTERRUPT_ADVISORY_EVENT	171	// Notify client that agent has been interrupted by non-interruptible task.
#define  MT_AGENT_INTERRUPT_ACCEPTED_IND	172	// Report acceptance of the interrupt.
#define  MT_AGENT_INTERRUPT_UNACCEPTED_IND	173	// Report non-acceptance of the interrupt.
#define  MT_AGENT_INTERRUPT_DONE_ADVISORY_EVENT	174	// Notify client that interrupt has been ended.
#define  MT_AGENT_INTERRUPT_DONE_ACCEPTED_IND	175	// Report acceptance of interrupt end.
#define  MT_CHANGE_MAX_TASK_LIMIT_REQ		176	// Change the maximum number of simultaneous tasks for the agent MRD combination.
#define  MT_CHANGE_MAX_TASK_LIMIT_RESP		177	// Response to CHANGE_MAX_TASK_LIMIT_REQ.
#define  MT_OVERRIDE_LIMIT_REQ				178	// Request a task assignment even though it would exceed agent¡¯s maximum number of simultaneous tasks for the MRD.
#define  MT_OVERRIDE_LIMIT_RESP				179	// Response to OVERRIDE_LIMIT_REQ.
#define  MT_UPDATE_TASK_CONTEXT_IND			180	// Update Unified ICM task context.
#define  MT_BEGIN_AGENT_INIT_IND			181	// Report begin agent and task resynchronization.
#define  MT_AGENT_INIT_REQ					182	// Report agent¡¯s current state.
#define  MT_AGENT_INIT_RESP					183	// Response to AGENT_INIT_REQ.
#define  MT_END_AGENT_INIT_IND				184	// Report end of agent and task resynchronization.
#define  MT_TASK_INIT_IND					185	// Report task¡¯s state.
#define  MT_AGENT_INIT_READY_EVENT			186	// Notify client that Unified ICM is ready to receive agent and task resynchronization messages.
#define  MT_GET_PRECALL_MESSAGES_REQ		187	// Request any pending PRE-CALL messages.
#define  MT_GET_PRECALL_MESSAGES_RESP		188	// Response to GET_PRECALL_MESSAGES_REQ.
#define  MT_AGENT_LEGACY_PRE_CALL_EVENT		189	// Current task context.
#define  MT_FAILURE_RESP					190	// Failure response to ARM indication messages.
#define  MT_BEGIN_TASK_EVENT				191	// Indicates the specified task has entered the system, either queued, offered, or begun.
#define  MT_QUEUED_TASK_EVENT				192	// Indicate the specified task has been queued in the router.
#define  MT_DEQUEUED_TASK_EVENT				193	// Indicate the specified task has been dequeued from the router.
#define  MT_OFFER_TASK_EVENT				194	// Indicates the specified agent has been reserved to handle the specified task
#define  MT_START_TASK_EVENT				195	// Indicates the specified agent has started handling the task.
#define  MT_PAUSE_TASK_EVENT				196	// Indicates the specified agent has temporarily suspended handling of the specified task.
#define  MT_RESUME_TASK_EVENT				197	// Indicates the specified agent has resumed handling of the specified task after having previously sent a Pause Task message.
#define  MT_WRAPUP_TASK_EVENT				198	// Indicates the specified agent is no longer actively handling the task but is doing followup work related to the task.
#define  MT_END_TASK_EVENT					199	// Indicates the specified agent has ended handling of the specified task.
#define  MT_TASK_DATA_UPDATE_EVENT			200	// Update task context for the specified task.
#define  MT_TASK_MONITOR_START_REQ			201	// Request to start the task monitor with the task mask in the request message.
#define  MT_TASK_MONITOR_START_CONF			202	// Response to TASK_ MONITOR_START_REQ.
#define  MT_TASK_MONITOR_STOP_REQ			203	// Request to stop the task monitor with the monitor ID in the request message.
#define  MT_TASK_MONITOR_STOP_CONF			204	// Response to TASK_MONITOR_STOP_REQ.
#define  MT_CHANGE_TASK_MONITOR_MASK_REQ	205	// Request to change the task monitor mask with the new mask in the request message.
#define  MT_CHANGE_TASK_MONITOR_MASK_CONF	206	// Response to CHANGE_ TASK_MONITOR_MASK_REQ.
#define  MT_MAX_TASK_LIFETIME_EXCEEDED_EVENT	207	// Unified ICM terminated a task which had exceeded its configured maximum lifetime ? the result is equivalent to the task ending due to an end task but with a special reason code in the Termination Call Detail record.
#define  MT_SET_APP_PATH_DATA_IND			208	// Set or update the application path-specific data variables available to routing scripts.
#define  MT_TASK_INIT_REQ					209	// Report task¡¯s state ? used when an Unified ICM taskID has not yet been assigned to the task because the task began when the ARM client interface was down.
#define  MT_TASK_INIT_RESP					210	// Response to the TASK_INIT_REQ message.
#define  MT_ROUTE_REGISTER_EVENT			211	// Register to receive route requests
#define  MT_ROUTE_REGISTER_REPLY_EVENT		212	// Reply to registration message
#define  MT_ROUTE_REQUEST_EVENT				213	// Route request for a destination for a call
#define  MT_ROUTE_SELECT					214	// Supplies a route destination for a route request
#define  MT_ROUTE_END						215	// End Routing dialog

// Reserved	216 - 235

#define  MT_TEAM_CONFIG_REQ					236
#define  MT_TEAM_CONFIG_EVENT				237
#define  MT_TEAM_CONFIG_CONF				238

// Reserved	239-247   Register to receive route requests

#define  MT_CALL_ATTRIBUTE_CHANGE_EVENT		240	// Reply to registration message

// Reserved	241- 246

#define  MT_CALL_TERMINATION_EVENT			247	// Call Termination Summary Info (not supported by CTI-Server)


const char *strMessageType(UINT mt);


#endif		// end of __MESSAGETYPE_H__
