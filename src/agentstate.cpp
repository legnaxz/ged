#include  <stdio.h>

#include  "typedefs.h"
#include  "agentstate.h"


const char *strAgentState(USHORT as)
{
	switch(as)
	{
	case AGENT_STATE_LOGIN          : return "AGENT_STATE_LOGIN";
	case AGENT_STATE_LOGOUT         : return "AGENT_STATE_LOGOUT";
	case AGENT_STATE_NOT_READY      : return "AGENT_STATE_NOT_READY";
	case AGENT_STATE_AVAILABLE      : return "AGENT_STATE_AVAILABLE";
	case AGENT_STATE_TALKING        : return "AGENT_STATE_TALKING";
	case AGENT_STATE_WORK_NOT_READY : return "AGENT_STATE_WORK_NOT_READY";
	case AGENT_STATE_WORK_READY     : return "AGENT_STATE_WORK_READY";
	case AGENT_STATE_BUSY_OTHER     : return "AGENT_STATE_BUSY_OTHER";
	case AGENT_STATE_RESERVED       : return "AGENT_STATE_RESERVED";
	case AGENT_STATE_UNKNOWN        : return "AGENT_STATE_UNKNOWN";
	case AGENT_STATE_HOLD           : return "AGENT_STATE_HOLD";
	case AGENT_STATE_ACTIVE         : return "AGENT_STATE_ACTIVE";
	case AGENT_STATE_PAUSED         : return "AGENT_STATE_PAUSED";
	case AGENT_STATE_INTERRUPTED    : return "AGENT_STATE_INTERRUPTED";
	case AGENT_STATE_NOT_ACTIVE     : return "AGENT_STATE_NOT_ACTIVE";
	default                         : return "UNKNOWN";
	}

	return "";
}
