#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <strings.h>
#include  <ctype.h>
#include  <errno.h>
#include  <netdb.h>
#include  <unistd.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>

#include  "util.h"
#include  "cfg.h"


void GetIpPort(char *pSrcIpPort, char *pIp, unsigned short *pPort);
int SetRange(char vas_no, char *pFile);
int get_cfgkeyword(char **bufp, argval *pval);
int check_ip(char *pIp);

CONFIG	cfg;

#define  MAX_IP_01		10
static int		g_vas_ip_01[MAX_IP_01];
static short	g_vas_range[MAX_IP_01][256][256];


void SetDefaultParms()
{
	cfg.nCtiServerCount = 0;

	memset(cfg.szCtiServerIpActive,    0x0, sizeof(cfg.szCtiServerIpActive));
	memset(cfg.usCtiServerPortActive,  0x0, sizeof(cfg.usCtiServerPortActive));
	memset(cfg.szCtiServerIpStandby,   0x0, sizeof(cfg.szCtiServerIpStandby));
	memset(cfg.usCtiServerPortStandby, 0x0, sizeof(cfg.usCtiServerPortStandby));

	memset(cfg.szHBMateServerIp, 0x0, sizeof(cfg.szHBMateServerIp));
	cfg.usHBMateServerPort = 5010;
	cfg.nHBPeriod = 200;
	cfg.nHBLost = 5;

	strcpy(cfg.szLogDir, "./log");

	memset(cfg.szPfsServerIpActive, 0x0, sizeof(cfg.szPfsServerIpActive));
	memset(cfg.szPfsServerIpStandby, 0x0, sizeof(cfg.szPfsServerIpStandby));
	cfg.usPfsServerPortActive = cfg.usPfsServerPortStandby = 0;
	cfg.cPfsServerPortActiveMate = cfg.cPfsServerPortStandbyMate = 0;

	cfg.nPfsHBPeriod = 30000;
	cfg.nVasHBPeriod = 30000;

	cfg.nVasServerCount = 0;
	memset(cfg.szVasServerIp,   0x0, sizeof(cfg.szVasServerIp));
	memset(cfg.usVasServerPort, 0x0, sizeof(cfg.usVasServerPort));

	memset(cfg.szVasServerIpExtra, 0x0, sizeof(cfg.szVasServerIpExtra));
	cfg.usVasServerPortExtra = 0;
	memset(cfg.cScsMode, 0x0, sizeof(cfg.cScsMode));
	memset(cfg.szScsActiveServerIp, 0x0, sizeof(cfg.szScsActiveServerIp));
	cfg.usScsActiveServerPort = 0;

	// 2020.05 OB Starts
	memset(cfg.szScsCallKeyAreaPrefix, 0x0, sizeof(cfg.szScsCallKeyAreaPrefix));
	// 2020.05 OB Ends
}

int LoadParameters(char *pFile)
{
	FILE		*pConfigFile;
	char		buf[1024], szTemp[1024], szOption[1024];
	char		*p = buf;
	argval		value;
	int			keyword, i;
	std::vector<std::string>	vecTokens;
	std::vector<std::string>::iterator it;

	if((pConfigFile = fopen(pFile, "r")) == NULL)
	{
		printf("Failed to load a file, %s\n", pFile);
		return -1;
	}

	memset(g_vas_ip_01, -1, sizeof(g_vas_ip_01));
	memset(g_vas_range, -1, sizeof(g_vas_range));

	for(;;)
	{
		/* Read a line from config file; break on EOF or error. */
		if(fgets(buf, sizeof buf, pConfigFile) == NULL) break;

		/* Strip comments, quotes, and newline. */
		strip_comments("#", (char *)buf);

		/* First token is keyword; some may be followed by a value or range */
		p = buf;
		keyword = get_cfgkeyword(&p, &value);
		switch(keyword)
		{
			case -2:
			case -1:
			case 0:
				continue;

			case PARM_ID_CTI_SERVER_1:
				cfg.nCtiServerCount = 0;
			case PARM_ID_CTI_SERVER_2:
			case PARM_ID_CTI_SERVER_3:
			case PARM_ID_CTI_SERVER_4:
			case PARM_ID_CTI_SERVER_5:
			case PARM_ID_CTI_SERVER_6:
			case PARM_ID_CTI_SERVER_7:
			case PARM_ID_CTI_SERVER_8:
			case PARM_ID_CTI_SERVER_9:
			case PARM_ID_CTI_SERVER_10:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				tokenize(value.strval, ",", vecTokens);
				if(vecTokens.size() != 2)
				{
					printf("Invalid CTI Server format: %s\n", szTemp);
					exit(0);
				}
				it = vecTokens.begin();
				GetIpPort((char*)it->c_str(), cfg.szCtiServerIpActive[cfg.nCtiServerCount], &cfg.usCtiServerPortActive[cfg.nCtiServerCount]);
				if(check_ip(cfg.szCtiServerIpActive[cfg.nCtiServerCount]) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szCtiServerIpActive[cfg.nCtiServerCount]);
					exit(0);
				}
				it ++;
				GetIpPort((char*)it->c_str(), cfg.szCtiServerIpStandby[cfg.nCtiServerCount], &cfg.usCtiServerPortStandby[cfg.nCtiServerCount]);
				if(check_ip(cfg.szCtiServerIpStandby[cfg.nCtiServerCount]) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szCtiServerIpStandby[cfg.nCtiServerCount]);
					exit(0);
				}
				cfg.nCtiServerCount ++;
				break;

			case PARM_ID_HB_MATE_SERVER:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				GetIpPort(value.strval, cfg.szHBMateServerIp, &cfg.usHBMateServerPort);
				if(check_ip(cfg.szHBMateServerIp) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szHBMateServerIp);
					exit(0);
				}
				break;

			case PARM_ID_HB_PERIOD:
				if(get_args(p, &value, NUMVALUE) != 1) continue;
				cfg.nHBPeriod = value.numval;
				break;

			case PARM_ID_HB_LOST:
				if(get_args(p, &value, NUMVALUE) != 1) continue;
				cfg.nHBLost = value.numval;
				break;

			case PARM_ID_LOG_DIR:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				strcpy(cfg.szLogDir, value.strval);
				break;

			case PARM_ID_PFS_SERVER:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				tokenize(value.strval, ",", vecTokens);
				if(vecTokens.size() != 4)
				{
					printf("Invalid PFS Server format: %s\n", szTemp);
					exit(0);
				}
				it = vecTokens.begin();
				GetIpPort((char*)it->c_str(), cfg.szPfsServerIpActive, &cfg.usPfsServerPortActive);
				if(check_ip(cfg.szPfsServerIpActive) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szPfsServerIpActive);
					exit(0);
				}
				it ++;
				cfg.cPfsServerPortActiveMate = *(it->c_str());
				it ++;
				GetIpPort((char*)it->c_str(), cfg.szPfsServerIpStandby, &cfg.usPfsServerPortStandby);
				if(check_ip(cfg.szPfsServerIpStandby) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szPfsServerIpStandby);
					exit(0);
				}
				it ++;
				cfg.cPfsServerPortStandbyMate = *(it->c_str());
				break;

			case PARM_ID_PFS_HB_PERIOD:
				if(get_args(p, &value, NUMVALUE) != 1) continue;
				cfg.nPfsHBPeriod = value.numval;
				break;

			case PARM_ID_VAS_HB_PERIOD:
				if(get_args(p, &value, NUMVALUE) != 1) continue;
				cfg.nVasHBPeriod = value.numval;
				break;

			case PARM_ID_VAS_SERVER_1:
				cfg.nVasServerCount = 0;
			case PARM_ID_VAS_SERVER_2:
			case PARM_ID_VAS_SERVER_3:
			case PARM_ID_VAS_SERVER_4:
			case PARM_ID_VAS_SERVER_5:
			case PARM_ID_VAS_SERVER_6:
			case PARM_ID_VAS_SERVER_7:
			case PARM_ID_VAS_SERVER_8:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				GetIpPort(value.strval, cfg.szVasServerIp[cfg.nVasServerCount], &cfg.usVasServerPort[cfg.nVasServerCount]);
				if(check_ip(cfg.szVasServerIp[cfg.nVasServerCount]) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szVasServerIp[cfg.nVasServerCount]);
					exit(0);
				}
				printf("VAS %d - %s:%d\n", cfg.nVasServerCount+1, cfg.szVasServerIp[cfg.nVasServerCount], cfg.usVasServerPort[cfg.nVasServerCount]);
				cfg.nVasServerCount ++;
				break;

			case PARM_ID_VAS_SERVER_EXTRA:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				GetIpPort(value.strval, cfg.szVasServerIpExtra, &cfg.usVasServerPortExtra);
				if(check_ip(cfg.szVasServerIpExtra) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szVasServerIpExtra);
					exit(0);
				}
				break;

			case PARM_ID_RANGE_FILE_1:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(1, value.strval);
				break;

			case PARM_ID_RANGE_FILE_2:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(2, value.strval);
				break;

			case PARM_ID_RANGE_FILE_3:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(3, value.strval);
				break;

			case PARM_ID_RANGE_FILE_4:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(4, value.strval);
				break;

			case PARM_ID_RANGE_FILE_5:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(5, value.strval);
				break;

			case PARM_ID_RANGE_FILE_6:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(6, value.strval);
				break;

			case PARM_ID_RANGE_FILE_7:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(7, value.strval);
				break;

			case PARM_ID_RANGE_FILE_8:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(8, value.strval);
				break;

			case PARM_ID_SCS_MODE:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				sprintf(cfg.cScsMode, "%-1.1s", &value.strval);
				break;
			case PARM_ID_SCS_ACTIVE_SERVER_IP:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				//sprintf(cfg.szScsActiveServerIp, "%-20.20s", &value.strval);
				strcpy(cfg.szScsActiveServerIp, value.strval);
				break;
			case PARM_ID_SCS_ACTIVE_SERVER_PORT:
				if(get_args(p, &value, NUMVALUE) != 1) continue;
				cfg.usScsActiveServerPort = value.numval;
				break;
			// 2020.05 OB Starts
			case PARM_ID_SCS_CALLKEY_AREA_PREFIX:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				strcpy(cfg.szScsCallKeyAreaPrefix, value.strval);
				break;
			// 2020.05 OB Ends

			default:
				continue;
		}
	}

	fclose(pConfigFile);
	return 0;
}

void ReadVasConfig(char *pFile)
{
	FILE		*pConfigFile;
	char		buf[1024], szTemp[1024], szOption[1024];
	char		*p = buf;
	argval		value;
	int			keyword, i;
	std::vector<std::string>	vecTokens;
	std::vector<std::string>::iterator it;

	if((pConfigFile = fopen(pFile, "r")) == NULL)
	{
		return;
	}

	for(;;)
	{
		/* Read a line from config file; break on EOF or error. */
		if(fgets(buf, sizeof buf, pConfigFile) == NULL) break;

		/* Strip comments, quotes, and newline. */
		strip_comments("#", (char *)buf);

		/* First token is keyword; some may be followed by a value or range */
		p = buf;
		keyword = get_cfgkeyword(&p, &value);
		switch(keyword)
		{
			case -2:
			case -1:
			case 0:
				continue;

			case PARM_ID_VAS_SERVER_1:
				cfg.nVasServerCount = 0;
			case PARM_ID_VAS_SERVER_2:
			case PARM_ID_VAS_SERVER_3:
			case PARM_ID_VAS_SERVER_4:
			case PARM_ID_VAS_SERVER_5:
			case PARM_ID_VAS_SERVER_6:
			case PARM_ID_VAS_SERVER_7:
			case PARM_ID_VAS_SERVER_8:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				GetIpPort(value.strval, cfg.szVasServerIp[cfg.nVasServerCount], &cfg.usVasServerPort[cfg.nVasServerCount]);
				if(check_ip(cfg.szVasServerIp[cfg.nVasServerCount]) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szVasServerIp[cfg.nVasServerCount]);
					exit(0);
				}
				printf("VAS %d - %s:%d\n", cfg.nVasServerCount+1, cfg.szVasServerIp[cfg.nVasServerCount], cfg.usVasServerPort[cfg.nVasServerCount]);
				cfg.nVasServerCount ++;
				break;

			case PARM_ID_VAS_SERVER_EXTRA:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				GetIpPort(value.strval, cfg.szVasServerIpExtra, &cfg.usVasServerPortExtra);
				if(check_ip(cfg.szVasServerIpExtra) != 1)
				{
					printf("Invalid IP address: %s\n", cfg.szVasServerIpExtra);
					exit(0);
				}
				break;

			case PARM_ID_RANGE_FILE_1:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				// Check out the file has been changed and if changed, then read it again.
				SetRange(1, value.strval);
				break;

			case PARM_ID_RANGE_FILE_2:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(2, value.strval);
				break;

			case PARM_ID_RANGE_FILE_3:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(3, value.strval);
				break;

			case PARM_ID_RANGE_FILE_4:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(4, value.strval);
				break;

			case PARM_ID_RANGE_FILE_5:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(5, value.strval);
				break;

			case PARM_ID_RANGE_FILE_6:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(6, value.strval);
				break;

			case PARM_ID_RANGE_FILE_7:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(7, value.strval);
				break;

			case PARM_ID_RANGE_FILE_8:
				if(get_args(p, &value, STRINGVALUE) != 1) continue;
				SetRange(8, value.strval);
				break;

			default:
				continue;
		}
	}

	fclose(pConfigFile);
}

void GetIpPort(char *pSrcIpPort, char *pIp, unsigned short *pPort)
{
	char *pColon = strstr(pSrcIpPort, ":");
	if(pColon == NULL)
	{
		printf("ERROR - The format should b IP:PORT\n");
		exit(0);
	}
	strncpy(pIp, pSrcIpPort, pColon - pSrcIpPort);
	*pPort = atoi(pColon + 1);
}

int check_ip(char *pIp)
{
	int  count;
	char *pip = pIp;

	count = 0;
	while(*pip != NULL)
	{
		if(*pip >= '0' && *pip <= '9')
		{
			pip ++;
			continue;
		}
		if(*pip == '.')
		{
			pip ++, count ++;
			continue;
		}
		break;
	}
	count ++;
	if(count == 4) return 1;	// okay
	return 0;					// not okay
}


int get_cfgkeyword(char **bufp, argval *pval)
{
	static struct {
		char *keytxt;
		int   keyid;
	} table [] =
	{
		{ KEYWORD_CTI_SERVER_1,				PARM_ID_CTI_SERVER_1            },
		{ KEYWORD_CTI_SERVER_2,				PARM_ID_CTI_SERVER_2            },
		{ KEYWORD_CTI_SERVER_3,				PARM_ID_CTI_SERVER_3            },
		{ KEYWORD_CTI_SERVER_4,				PARM_ID_CTI_SERVER_4            },
		{ KEYWORD_CTI_SERVER_5,				PARM_ID_CTI_SERVER_5            },
		{ KEYWORD_CTI_SERVER_6,				PARM_ID_CTI_SERVER_6            },
		{ KEYWORD_CTI_SERVER_7,				PARM_ID_CTI_SERVER_7            },
		{ KEYWORD_CTI_SERVER_8,				PARM_ID_CTI_SERVER_8            },
		{ KEYWORD_CTI_SERVER_9,				PARM_ID_CTI_SERVER_9            },
		{ KEYWORD_CTI_SERVER_10,			PARM_ID_CTI_SERVER_10           },
		{ KEYWORD_HB_MATE_SERVER,			PARM_ID_HB_MATE_SERVER          },
		{ KEYWORD_HB_PERIOD,				PARM_ID_HB_PERIOD               },
		{ KEYWORD_HB_LOST,					PARM_ID_HB_LOST                 },
		{ KEYWORD_LOG_DIR,					PARM_ID_LOG_DIR                 },
		{ KEYWORD_PFS_SERVER,				PARM_ID_PFS_SERVER              },
		{ KEYWORD_PFS_HB_PERIOD,			PARM_ID_PFS_HB_PERIOD           },
		{ KEYWORD_VAS_HB_PERIOD,			PARM_ID_VAS_HB_PERIOD           },
		{ KEYWORD_VAS_SERVER_1,				PARM_ID_VAS_SERVER_1            },
		{ KEYWORD_VAS_SERVER_2,				PARM_ID_VAS_SERVER_2            },
		{ KEYWORD_VAS_SERVER_3,				PARM_ID_VAS_SERVER_3            },
		{ KEYWORD_VAS_SERVER_4,				PARM_ID_VAS_SERVER_4            },
		{ KEYWORD_VAS_SERVER_5,				PARM_ID_VAS_SERVER_5            },
		{ KEYWORD_VAS_SERVER_6,				PARM_ID_VAS_SERVER_6            },
		{ KEYWORD_VAS_SERVER_7,				PARM_ID_VAS_SERVER_7            },
		{ KEYWORD_VAS_SERVER_8,				PARM_ID_VAS_SERVER_7            },
		{ KEYWORD_VAS_SERVER_EXTRA,			PARM_ID_VAS_SERVER_EXTRA        },
		{ KEYWORD_RANGE_FILE_1,				PARM_ID_RANGE_FILE_1            },
		{ KEYWORD_RANGE_FILE_2,				PARM_ID_RANGE_FILE_2            },
		{ KEYWORD_RANGE_FILE_3,				PARM_ID_RANGE_FILE_3            },
		{ KEYWORD_RANGE_FILE_4,				PARM_ID_RANGE_FILE_4            },
		{ KEYWORD_RANGE_FILE_5,				PARM_ID_RANGE_FILE_5            },
		{ KEYWORD_RANGE_FILE_6,				PARM_ID_RANGE_FILE_6            },
		{ KEYWORD_RANGE_FILE_7,				PARM_ID_RANGE_FILE_7            },
		{ KEYWORD_RANGE_FILE_8,				PARM_ID_RANGE_FILE_8            },
		{ KEYWORD_SCS_MODE,					PARM_ID_SCS_MODE                },
		{ KEYWORD_SCS_ACTIVE_SERVER_IP,		PARM_ID_SCS_ACTIVE_SERVER_IP    },
		{ KEYWORD_SCS_ACTIVE_SERVER_PORT,	PARM_ID_SCS_ACTIVE_SERVER_PORT  },
		// 2020.05 OB Starts
		{ KEYWORD_SCS_CALLKEY_AREA_PREFIX,	PARM_ID_SCS_CALLKEY_AREA_PREFIX },
		// 2020.05 OB Ends
	};
	int i;
	char *p = *bufp;
	char *ptoken;
	int size = 0;
	int keyid = -2;
	char keywordtxt[1024];

	/* Skip leading spaces; done if eol or if rest is comment */
	SKIPSPACE(p);
	if(*p == '\0') return 0;
	ptoken = p;

	/* Skip to end of keyword */
	while(isalnum(*p) || *p == '_' || *p == '.')
	{
		++p;
		++size;
	}
	if(size == 0) return -1;

	/* Scan table to validate keyword */
	strncpy(keywordtxt, ptoken, size);
	keywordtxt[size] = '\0';
	for(i = 0; i < sizeof(table) / sizeof(table[0]); i ++)
	{
		if(strcasecmp(keywordtxt, table[i].keytxt) == 0)
		{
			keyid = table[i].keyid;
			break;
		}
	}
	*bufp = p;

	return keyid;
}

int SetRange(char vas_no, char *pFile)
{
	FILE	*fp;
	char	buf[1024];
	int		i, ip_01_index, ip_01_good, ip_01_value;
	int		length, ip_0, ip_1, ip_2, ip_3;
	std::vector<std::string>	vecTokens;

	if((fp = fopen(pFile, "rt")) == NULL)
	{
		printf("File, %s does not exist!, so exiting...\n", pFile);
		exit(0);
	}

	while(!feof(fp))
	{
		/* Read a line from config file; break on EOF or error. */
		if(fgets(buf, sizeof buf, fp) == NULL) break;
		length = strlen(buf);
		if(buf[length - 1] == '\n') buf[length - 1] = 0x0;
		if(buf[length - 2] == '\r') buf[length - 2] = 0x0;

		/* Strip comments, quotes, and newline. */
		strip_comments_nocr("#", (char *)buf);
		printf("buf=[%s]\n", buf);
		if(strlen(buf) < 3) continue;

		tokenize(buf, ".", vecTokens);
		if(vecTokens.size() != 4)
		{
			printf("IP range has an error!, %s\n", buf);
			fclose(fp);
			return -1;
		}

		ip_0 = atoi(vecTokens.at(0).c_str());
		ip_1 = atoi(vecTokens.at(1).c_str());
		if(ip_0 < 0 || ip_0 > 255 || ip_1 < 0 || ip_1 > 255)
		{
			printf("IP range has an error!, %s\n", buf);
			fclose(fp);
			return -1;
		}

		ip_01_good = 0;
		ip_01_value = ip_0 * 256 + ip_1;
		for(i = 0; i < MAX_IP_01; i ++)
		{
			if(g_vas_ip_01[i] == -1)
			{
				g_vas_ip_01[i] = ip_01_value;
				ip_01_index = i;
				ip_01_good = 1;
				break;
			}
			else if(g_vas_ip_01[i] == ip_01_value)
			{
				ip_01_index = i;
				ip_01_good = 1;
				break;
			}
		}

		if(ip_01_good == 0)
		{
			printf("IP address format A.B.C.D: The total count of A.B is more than %d\n", MAX_IP_01);
			fclose(fp);
			return -1;
		}
		printf("index=%d, value=%d\n", ip_01_index, g_vas_ip_01[ip_01_index]);

		ip_2 = atoi(vecTokens.at(2).c_str());
		ip_3 = atoi(vecTokens.at(3).c_str());

		if(ip_2 < 0 || ip_2 > 255 || ip_3 < 0 || ip_3 > 255)
		{
			printf("IP range has an error!, %s\n", buf);
			fclose(fp);
			return -1;
		}

#if 1
		g_vas_range[ip_01_index][ip_2][ip_3] = vas_no;
#else
		if(g_vas_range[ip_01_index][ip_2][ip_3] == -1)
		{
			g_vas_range[ip_01_index][ip_2][ip_3] = vas_no;
		}
		else
		{
			printf("The IP address, %s is already existing in the VAS group, %d!\n", buf, g_vas_range[ip_01_index][ip_2][ip_3]);
			fclose(fp);
			return -1;
		}
#endif
	}

	fclose(fp);
	return 0;
}

int GetVasNo(char *pIp)
{
	int i, ip_01_index, ip_01_value;
	int	vas_no, ip_0, ip_1, ip_2, ip_3;
	std::vector<std::string>	vecTokens;

	tokenize(pIp, ".", vecTokens);
	if(vecTokens.size() != 4) return -1;

	ip_0 = atoi(vecTokens.at(0).c_str());
	ip_1 = atoi(vecTokens.at(1).c_str());
	ip_2 = atoi(vecTokens.at(2).c_str());
	ip_3 = atoi(vecTokens.at(3).c_str());

	ip_01_index = -1;
	ip_01_value = ip_0 * 256 + ip_1;
	for(i = 0; i < MAX_IP_01; i ++)
	{
		if(ip_01_value == g_vas_ip_01[i])
		{
			ip_01_index = i;
			break;
		}
	}

	if(ip_01_index == -1) return -1;
	if(ip_2 < 0 || ip_2 > 255) return -1;
	if(ip_3 < 0 || ip_3 > 255) return -1;

	vas_no = g_vas_range[ip_01_index][ip_2][ip_3];

	return vas_no;
}
