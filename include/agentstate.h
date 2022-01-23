#ifndef  __AGENTSTATE_H__
#define  __AGENTSTATE_H__


#define  AGENT_STATE_LOGIN				0	// The agent has logged on to the ACD. It does not necessarily indicate that the agent is ready to accept calls.
#define  AGENT_STATE_LOGOUT				1	// The agent has logged out of the ACD and cannot accept any additional calls.
#define  AGENT_STATE_NOT_READY			2	// The agent is unavailable for any call work.
#define  AGENT_STATE_AVAILABLE			3	// The agent is ready to accept a call.
#define  AGENT_STATE_TALKING			4	// The agent is currently talking on a call (inbound, outbound, or inside).
#define  AGENT_STATE_WORK_NOT_READY		5	// The agent is performing after call work, but will not be ready to receive a call when completed.
#define  AGENT_STATE_WORK_READY			6	// The agent is performing after call work, and will be ready to receive a call when completed.
#define  AGENT_STATE_BUSY_OTHER			7	// The agent is busy performing a task associated with another active SkillGroup.
#define  AGENT_STATE_RESERVED			8	// The agent is reserved for a call that will arrive at the ACD shortly.
#define  AGENT_STATE_UNKNOWN			9	// The agent state is currently unknown.
#define  AGENT_STATE_HOLD				10	// The agent currently has all calls on hold.
#define  AGENT_STATE_ACTIVE				11	// The agent state is currently active
#define  AGENT_STATE_PAUSED				12	// The agent state is currently paused
#define  AGENT_STATE_INTERRUPTED		13	// The agent state is currently interrupted
#define  AGENT_STATE_NOT_ACTIVE			14	// The agent state is currently not active


const char *strAgentState(USHORT as);


#endif		// end of __AGENTSTATE_H__
