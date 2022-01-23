#ifndef  __TYPEDEFS_H__
#define  __TYPEDEFS_H__


#pragma pack(1)


typedef char				CHAR;	// 1 byte
typedef unsigned char		UCHAR;	// 1 byte
typedef short int 			SHORT;	// 2 bytes
typedef unsigned short int	USHORT;	// 2 bytes
typedef int					INT;	// 4 bytes
typedef unsigned int		UINT;	// 4 bytes
typedef short int			BOOL;	// 2 bytes
typedef unsigned int		TIME;	// 4 bytes


typedef struct
{
	UCHAR	Tag;				// NAMED_VARIABLE_TAG(=82)
	UCHAR	FieldLength;		// 3 - 251
	CHAR	*VariableName;		// max size : 33 bytes - null terminated
	CHAR	*VariableValue;		// max size : 211 bytes - null terminated
} S_NAMEDVAR, *PS_NAMEDVAR;

typedef struct
{
	UCHAR	Tag;				// NAMED_ARRAY_TAG(=83)
	UCHAR	FieldLength;		// 4 - 252
	UCHAR	VariableIndex;
	CHAR	*VariableName;		// max size : 33 bytes - null terminated
	CHAR	*VariableValue;		// max size : 211 bytes - null terminated
} S_NAMEDARRARY, *PS_NAMEDARRARY;

typedef struct
{
	INT		TaskGroupHigh;
	INT		TaskGroupLow;
	INT		SequenceNumber;
} S_TASKID, *PS_TASKID;

typedef struct
{
	UCHAR	Tag;				// APP_PATH_ID_TAG(=97)
	INT		AppPathID;
} S_APPPATHID, *PS_APPPATHID;


// Floating Field Format
// typedef struct
// {
//		UCHAR	Tag
//		UCHAR	FieldLength
//		Data...
// }
// Tag, Field Length, Data
// ...
// Tag, Field Length, Data



// Message Header
typedef struct
{
	UINT	MessageLength;		// excludes this header size, 8 bytes
	UINT	MessageType;
} MHDR, *PMHDR;


// FAILURE_CONF - MessageType = 1
typedef struct
{
//	MHDR	MessageHeader;

	// Fixed
	UINT	InvokeID;
	UINT	Status;
} S_FAILURE_CONF, *PS_FAILURE_CONF;

// FAILURE_EVENT - MessageType = 2
typedef struct
{
//	MHDR	MessageHeader;

	// Fixed
	UINT	Status;
} S_FAILURE_EVENT, *PS_FAILURE_EVENT;


#define  MT_OPEN_REQ_FLOATING_SIZE		500
// OPEN_REQ - MessageType = 3
typedef struct
{
	MHDR	MessageHeader;

	// Fixed
	UINT	InvokeID;
	UINT	VersionNumber;	// set to 13
	UINT	IdleTimeout;	// unit:seconds == 4 * heartbeat interval..
							// If the CTI client does not use the HEARTBEAT_REQ message, set this field to 0xFFFFFFFF
	UINT	PeripheralID;	// The Peripheral ID of the ACD whose events are of interest to the client.
							// Required for Client Events service; otherwise, set this field to the special value NULL_PERIPHERAL_ID
	UINT	ServicesRequested;
	UINT	CallMsgMask;
	UINT	AgentStateMask;
	UINT	ConfigMsgMask;
	UINT	Reserved1;
	UINT	Reserved2;
	UINT	Reserved3;

	// Floating
	CHAR	pFloatingAddr[MT_OPEN_REQ_FLOATING_SIZE];
	//CHAR	ClientID[64];			// required
	//CHAR	ClientPassword[64];		// required
	//CHAR	ClientSignature[64];
	//CHAR	AgentExtension[16];
	//CHAR	AgentID[12];
	//CHAR	AgentInstrument[64];
	//INT	ApplicationPathID[4];

} S_OPEN_REQ, *PS_OPEN_REQ;



// OPEN_CONF - MessageType = 4
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	InvokeID;
	UINT	ServicesGranted;
	UINT	MonitorID;
	UINT	PGStatus;
	TIME	ICMCentralControllerTime;
	BOOL	PeripheralOnline;
	USHORT	PeripheralType;
	USHORT	agentState;

	// Floating
	CHAR	pFloatingAddr[1];
	//CHAR	AgentExtension[16];
	//CHAR	AgentID[12];
	//CHAR	AgentInstrument[64];
	//USHORT	NumPeripherals;
	//UINT	FltPeripheralID;
	//USHORT	MultilineAgentControl;
} S_OPEN_CONF, *PS_OPEN_CONF;


// HEARTBEAT_REQ - MessageType = 5
typedef struct
{
	MHDR	MessageHeader;

	// Fixed
	UINT	InvokeID;
} S_HEARTBEAT_REQ, *PS_HEARTBEAT_REQ;


// HEARTBEAT_CONF - MessageType = 6
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	InvokeID;
} S_HEARTBEAT_CONF, *PS_HEARTBEAT_CONF;



// CLOSE_REQ - MessageType = 7
typedef struct
{
	MHDR	MessageHeader;

	// Fixed
	UINT	InvokeID;
	UINT	Status;
} S_CLOSE_REQ, *PS_CLOSE_REQ;


// CLOSE_CONF - MessageType = 8
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	InvokeID;
} S_CLOSE_CONF, *PS_CLOSE_CONF;


// AGENT_PRE_CALL_EVENT - MessageType = 105
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	USHORT	NumNamedVariables;
	USHORT	NumNamedArrays;
	UINT	ServiceNumber;
	UINT	ServiceID;
	UINT	SkillGroupNumber;
	UINT	SkillGroupID;
	USHORT	SkillGroupPriority;
	UINT	MRDID;

	// Floating
	CHAR	pFloatingAddr[1];
	// AgentInstrument
	// UINT RouterCallKeyDay
	// UINT	RouterCallKeyCallID
	// UINT RouterCallKeySequenceNumber
	// ANI
	// UserToUserInfo
	// DialedNumber
	// CallerEnteredDigits
	// CallVariable1
	// ...
	// CallVariable10
	// NamedVariable
	// NamedArray
} S_AGENT_PRE_CALL_EVENT, *PS_AGENT_PRE_CALL_EVENT;

// BEGIN_CALL_EVENT - MessageType = 23
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	NumCTIClients;
	USHORT	NumNamedVariables;
	USHORT	NumNamedArrays;
	USHORT	CallType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;
	USHORT	CalledPartyDisposition;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
	// ANI
	// UserToUserInfo
	// DNIS
	// DialedNumber
	// UINT RouterCallKeyDay
	// UINT	RouterCallKeyCallID
	// UINT RouterCallKeySequenceNumber
	// CallVariable1
	// ...
	// CallVariable10
	// CallWrapupData
	// NamedVariable
	// NamedArray
	// CTIClientSignature
	// TIME CTIClientTimestamp
	// CallReferenceID
} S_BEGIN_CALL_EVENT, *PS_BEGIN_CALL_EVENT;

// END_CALL_EVENT - MessageType = 24
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
} S_END_CALL_EVENT, *PS_END_CALL_EVENT;

// CALL_DATA_UPDATE_EVENT - MessageType = 25
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	NumCTIClients;
	USHORT	NumNamedVariables;
	USHORT	NumNamedArrays;
	USHORT	CallType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;
	USHORT	NewConnectionDeviceIDType;
	UINT	NewConnectionCallID;
	USHORT	CalledPartyDisposition;
	UINT	CampaignID;
	UINT	QueryRuleID;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
	// NewConnectionDeviceID
	// ANI
	// UserToUserInfo
	// DNIS
	// DialedNumber
	// CallerEnteredDigits
	// UINT RouterCallKeyDay
	// UINT	RouterCallKeyCallID
	// UINT RouterCallKeySequenceNumber
	// CallVariable1
	// ...
	// CallVariable10
	// CallWrapupData
	// NamedVariable
	// NamedArray
	// CustomerPhoneNumber
	// CustomerAccountNumber
	// CTIClientSignature
	// TIME CTIClientTimestamp
	// CallReferenceID
} S_CALL_DATA_UPDATE_EVENT, *PS_CALL_DATA_UPDATE_EVENT;

// CALL_ESTABLISHED_EVENT - MessageType = 10
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;
	USHORT	LineHandle;
	USHORT	LineType;
	UINT	ServiceNumber;
	UINT	ServiceID;
	UINT	SkillGroupNumber;
	UINT	SkillGroupID;
	USHORT	SkillGroupPriority;
	USHORT	AnsweringDeviceType;
	USHORT	CallingDeviceType;
	USHORT	CalledDeviceType;
	USHORT	LastRedirectDeviceType;
	USHORT	LocalConnectionState;
	USHORT	EventCause;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
	// AnsweringDeviceID
	// CallingDeviceID
	// CalledDeviceID
	// LastRedirectDeviceID
	// UINT	TrunkNumber
	// UINT	TrunkGroupNumber;
} S_CALL_ESTABLISHED_EVENT, *PS_CALL_ESTABLISHED_EVENT;

// CALL_ORIGINATED_EVENT - MessageType = 15
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;
	USHORT	LineHandle;
	USHORT	LineType;
	UINT	ServiceNumber;
	UINT	ServiceID;
	UINT	SkillGroupNumber;
	UINT	SkillGroupID;
	USHORT	SkillGroupPriority;
	USHORT	CallingDeviceType;
	USHORT	CalledDeviceType;
	USHORT	LocalConnectionState;
	USHORT	EventCause;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
	// CallingDeviceID
	// CalledDeviceID
} S_CALL_ORIGINATED_EVENT, *PS_CALL_ORIGINATED_EVENT;

// CALL_TRANSFERRED_EVENT - MessageType = 18
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	PrimaryDeviceIDType;
	UINT	PrimaryCallID;
	USHORT	LineHandle;
	USHORT	LineType;
	UINT	SkillGroupNumber;
	UINT	SkillGroupID;
	USHORT	SkillGroupPriority;
	USHORT	NumParties;
	USHORT	SecondaryDeviceIDType;
	UINT	SecondaryCallID;
	USHORT	TransferringDeviceType;
	USHORT	TransferredDeviceType;
	USHORT	LocalConnectionState;
	USHORT	EventCause;

	// Floating
	CHAR	pFloatingAddr[1];
	// PrimaryDeviceID
	// SecondaryDeviceID
	// TransferringDeviceID
	// TransferredDeviceID
	// ConnectedPartyCallID			UINT
	// ConnectedPartyDeviceIDType	USHORT
	// ConnectedPartyDeviceID
} S_CALL_TRANSFERRED_EVENT, *PS_CALL_TRANSFERRED_EVENT;

// CALL_HELD_EVENT - MessageType = 11
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;
	USHORT	HoldingDeviceType;
	USHORT	LocalConnectionState;
	USHORT	EventCause;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
	// HoldingDeviceID
} S_CALL_HELD_EVENT, *PS_CALL_HELD_EVENT;

// CALL_CLEARED_EVENT - MessageType = 13
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;
	USHORT	LocalConnectionState;
	USHORT	EventCause;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
} S_CALL_CLEARED_EVENT, *PS_CALL_CLEARED_EVENT;

// CALL_CONNECTION_CLEARED_EVENT - MessageType = 14
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;
	USHORT	ReleasingDeviceType;
	USHORT	LocalConnectionState;
	USHORT	EventCause;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
} S_CALL_CONNECTION_CLEARED_EVENT, *PS_CALL_CONNECTION_CLEARED_EVENT;

// CALL_DELIVERED_EVENT - MessageType = 9
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	USHORT	PeripheralType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;
	USHORT	LineHandle;
	USHORT	LineType;
	UINT	ServiceNumber;
	UINT	ServiceID;
	UINT	SkillGroupNumber;
	UINT	SkillGroupID;
	USHORT	SkillGroupPriority;
	USHORT	AlertingDeviceType;
	USHORT	CallingDeviceType;
	USHORT	CalledDeviceType;
	USHORT	LastRedirectDeviceType;
	USHORT	LocalConnectionState;
	USHORT	EventCause;
	USHORT	NumNamedVariables;
	USHORT	NumNamedArrays;

	// Floating
	CHAR	pFloatingAddr[1];
	// ConnectionDeviceID
	// AlertingDeviceID
	// CallingDeviceID
	// CalledDeviceID
	// LastRedirectDeviceID
	// UINT	TrunkNumber
	// UINT	TrunkGroupNumber;
	// SecondaryConnectionCallID
	// ANI
	// ANI_II
	// UserToUserInfo
	// DNIS
	// DialedNumber
	// CallerEnteredDigits
	// CallVariable1
	// ...
	// CallVariable10
	// CallWrapupData
	// NamedVariable
	// NamedArray
} S_CALL_DELIVERED_EVENT, *PS_CALL_DELIVERED_EVENT;

// AGENT_STATE_EVENT - MessageType = 30
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	UINT	SessionID;
	USHORT	PeripheralType;
	USHORT	SkillGroupState;
	UINT	StateDuration;
	UINT	SkillGroupNumber;
	UINT	SkillGroupID;
	USHORT	SkillGroupPriority;
	USHORT	AgentState;
	USHORT	EventReasonCode;
	INT		MRDID;
	UINT	NumTasks;
	USHORT	AgentMode;
	UINT	MaxTaskLimit;
	INT		ICMAgentID;
	UINT	AgentAvailabilityStatus;
	USHORT	NumFltSkillGroups;

	// Floating
	CHAR	pFloatingAddr[1];
	// CTIClientSignature[64]
	// AgentID[12]
	// AgentExtension[16]
	// AgentInstrument[64]
	// Duration				UINT
	// NextAgentState		USHORT
	// FltSkillGroupNumber	INT
	// FltSkillGroupID		UINT
	// FltSkillGroupPriority	USHORT
	// FltSkillGroupState		USHORT
} S_AGENT_STATE_EVENT, *PS_AGENT_STATE_EVENT;

// RTP_STARTED_EVENT - MessageType = 116
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	UINT	ClientPort;
	USHORT	Direction;
	USHORT	RTPType;
	UINT	BitRate;
	USHORT	EchoCancellation;
	UINT	PacketSize;
	USHORT	PayloadType;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;

	// Floating
	CHAR	pFloatingAddr[1];
	//CHAR	ConnectionDeviceID[64];
	//CHAR	ClientAddress[16];
	//CHAR	AgentID[12];			// optional
	//CHAR	AgentExtension[16];		// optional
	//CHAR	AgentInstrument[64];	// optional
	//CHAR	SendingAddress[16];
	//UINT	SendingPort;
	//CHAR	ClientAddressIPV6[16];
	//CHAR	SendingAddressIPV6[16];
} S_RTP_STARTED_EVENT, *PS_RTP_STARTED_EVENT;

// RTP_STOPPED_EVENT - MessageType = 117
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	MonitorID;
	UINT	PeripheralID;
	UINT	ClientPort;
	USHORT	Direction;
	USHORT	ConnectionDeviceIDType;
	UINT	ConnectionCallID;

	// Floating
	CHAR	pFloatingAddr[1];
	//CHAR	ConnectionDeviceID[64];
	//CHAR	ClientAddress[16];
	//CHAR	AgentID[12];			// optional
	//CHAR	AgentExtension[16];		// optional
	//CHAR	AgentInstrument[64];	// optional
	//CHAR	SendingAddress[16];
	//UINT	SendingPort;
	//CHAR	ClientAddressIPV6[16];
	//CHAR	SendingAddressIPV6[16];
} S_RTP_STOPPED_EVENT, *PS_RTP_STOPPED_EVENT;

// SYSTEM_EVENT - MessageType = 31
typedef struct
{
	// MHDR	MessageHeader;

	// Fixed
	UINT	PGStatus;
	TIME	ICMCentralControllerTime;
	UINT	SystemEventID;
	UINT	SystemEventArg1;
	UINT	SystemEventArg2;
	UINT	SystemEventArg3;
	USHORT	EventDeviceType;

	// Floating
	CHAR	pFloatingAddr[1];
	//Text[255]
	//EventDeviceID[64]
} S_SYSTEM_EVENT, *PS_SYSTEM_EVENT;


#endif		// end of __TYPEDEFS_H__
