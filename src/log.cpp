#include  <ctype.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <fcntl.h>
#include  <stdarg.h>
#ifdef  WIN32
#include  <windows.h>
#include  <winsock.h>
#include  <errno.h>
#define	  _MT
#include  <process.h>
#else
#include  <sys/stat.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <errno.h>
#include  <sys/time.h>
#include  <time.h>
#include  <pthread.h>
#endif


#include  "log.h"


#ifdef  WIN32
#define   DIR_EXPRESSION		'\\'
CRITICAL_SECTION				g_LogCs;
#else
#define   DIR_EXPRESSION		'/'
pthread_mutex_t					g_LogMutex = PTHREAD_MUTEX_INITIALIZER;
#endif


char			g_szBaseLogDir[255], g_szLogFilenameDir[255], g_szLogFilename[1024], g_szErrorLogFilename[1024];
char			g_szAppName[255];
int				g_filecount = 0;
unsigned long	g_sizelimit;
unsigned long	g_ConsoleTraceMask = 0xFFFFFFFF, g_FileTraceMask = 0xFFFFFFFF;


void update_time(int *year, int *month, int *day, char *pTimestr);
void get_log_filename(int year, int month, int day);
void get_error_log_filename(int year, int month, int day);
const char *lookup_tracemask(unsigned long tracemask);


void InitializeLog(const char *pBaseLogDir, unsigned long logFileSizeLimit)
{
	strcpy(g_szBaseLogDir, pBaseLogDir);
	g_sizelimit = logFileSizeLimit;
	
#ifdef WIN32
	InitializeCriticalSection(&g_LogCs);
#else
	if(pthread_mutex_init(&g_LogMutex, NULL) != 0)
	{
		fprintf(stderr, "InitializeLog: can't initialize mutex!!! %s\n", strerror(errno));
	}
#endif
}


void UninitializeLog()
{
#ifdef WIN32
	DeleteCriticalSection(&g_LogCs);
#else
	pthread_mutex_destroy(&g_LogMutex);
#endif
}


int MakeLogBaseDir()
{
	int		nResult;
	char	*pTempDir, *pDir, dirname[255];

	memset(dirname, 0x0, sizeof(dirname));
#ifdef  WIN32
	if(_mkdir(g_szBaseLogDir) < 0)
#else
	if(mkdir(g_szBaseLogDir, S_IFDIR|S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO) < 0)
#endif
	{
		if(errno != 0 && errno != EEXIST)
			fprintf(stderr, "mkdir failed to create the directory (%s) : %s\n", g_szBaseLogDir, strerror(errno));

		pTempDir = g_szBaseLogDir;
		if(*pTempDir== '.')
		{
			if(*(pTempDir + 1) == DIR_EXPRESSION)
			{
				pTempDir += 2;
			}
			else
			{	// .. parent directory...
				fprintf(stderr, "The log base directory must be started with the root directory(/) or the current directory(./).\n");
				return -1;
			}
		}
		else if(*pTempDir == DIR_EXPRESSION)
		{
			pTempDir ++;
		}

		while(*pTempDir != 0)
		{
			pDir = strchr(pTempDir, DIR_EXPRESSION);
			if(pDir == NULL)
			{
				strcpy(dirname, g_szBaseLogDir);
				pTempDir += strlen(g_szBaseLogDir);
			}
			else
			{
				nResult = pDir - g_szBaseLogDir + 1;
				strncpy(dirname, g_szBaseLogDir, nResult);
				pTempDir = g_szBaseLogDir + nResult;
			}
#ifdef  WIN32
			if(_mkdir(dirname) < 0)
#else
			if(mkdir(dirname, S_IFDIR|S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO))
#endif
			{
				if(errno != EEXIST)
				{
					fprintf(stderr, "mkdir failed to create the directory (%s) : %s\n", dirname, strerror(errno));
					return -1;
				}
			}
		}
	}

	return 0;
}


void SetTraceMask(unsigned long console_tm, unsigned long file_tm)
{
	g_ConsoleTraceMask = console_tm;
	g_FileTraceMask    = file_tm;
}


void LogPrintf(int main_session, int sub_session, unsigned long trace_mask, const char *format, ... )
{
	FILE	*fp;
	int		year, month, day;
	long	lFilesize;
	char	szTemp[2048], buff[2048], szTimestr[200];
	va_list		other_args;

#ifdef WIN32
	EnterCriticalSection(&g_LogCs);
#else
	if(pthread_mutex_lock(&g_LogMutex) != 0)
		perror("pthread_mutex_lock returns an error\n");
#endif

	update_time(&year, &month, &day, szTimestr);
	get_log_filename(year, month, day);


try_again:
	if(g_FileTraceMask & trace_mask)
	{
		fp = fopen(g_szLogFilename, "a");
		if(fp != NULL)
		{
			fseek(fp, 0L, SEEK_END);
			lFilesize = ftell(fp);
			if(lFilesize > 50000000)	// 50 MB
			{
				g_filecount ++;
				sprintf(g_szLogFilename, "%s%s.%02d%02d-%02d", g_szLogFilenameDir, g_szAppName, month, day, g_filecount);
				fclose(fp);
				goto try_again;
			}
			sprintf(buff, "%s> %s [M%d S%03d] %s", szTimestr, lookup_tracemask(trace_mask), main_session, sub_session, format);
			va_start(other_args, format);
			vfprintf(fp, buff, other_args);
			va_end(other_args);
			fclose(fp);
		}
	}

	if(g_ConsoleTraceMask & trace_mask)
	{
		sprintf(buff, "%-15s%s", lookup_tracemask(trace_mask), format);
		va_start(other_args, format);
		vsprintf(szTemp, format, other_args);
		va_end(other_args);
		printf("%s", szTemp);
	}

#ifdef WIN32
	LeaveCriticalSection(&g_LogCs);
#else
	if(pthread_mutex_unlock(&g_LogMutex) != 0)
		perror("pthread_mutex_unlock returns an error\n");
#endif
}


void LogErrorPrintf(int main_session, int sub_session, unsigned long trace_mask, const char *format, ... )
{
	FILE	*fp;
	int		year, month, day;
	long	lFilesize;
	char	szTemp[2048], buff[2048], szTimestr[200];
	va_list		other_args;

#ifdef WIN32
	EnterCriticalSection(&g_LogCs);
#else
	if(pthread_mutex_lock(&g_LogMutex) != 0)
		perror("pthread_mutex_lock returns an error\n");
#endif

	update_time(&year, &month, &day, szTimestr);
	get_error_log_filename(year, month, day);

	if(g_FileTraceMask & trace_mask)
	{
		fp = fopen(g_szErrorLogFilename, "a");
		if(fp != NULL)
		{
			fseek(fp, 0L, SEEK_END);
			sprintf(buff, "%s> %s [M%d S%03d] %s", szTimestr, lookup_tracemask(trace_mask), main_session, sub_session, format);
			va_start(other_args, format);
			vfprintf(fp, buff, other_args);
			va_end(other_args);
			fclose(fp);
		}
	}

#ifdef WIN32
	LeaveCriticalSection(&g_LogCs);
#else
	if(pthread_mutex_unlock(&g_LogMutex) != 0)
		perror("pthread_mutex_unlock returns an error\n");
#endif
}


/* Beginning of internal codes */

const char *lookup_tracemask(unsigned long tracemask)
{
	switch(tracemask)
	{
		case TM_FATAL:		return "FATL";
		case TM_CRITICAL:	return "CRIT";
		case TM_MAJOR:		return "MAJR";
		case TM_MINOR:		return "MINR";
		case TM_WARNING:	return "WARN";
		case TM_INFO:		return "INFO";
		case TM_FNBEGIN:	return "FBEG";
		case TM_FNEND:		return "FEND";
		case TM_APIERROR:	return "ERRO";
		case TM_STAT:		return "STAT";
	}

	return "UNWN";
}

void update_time(int *year, int *month, int *day, char *pTimestr)
{
	struct timeval		tv;
	struct timezone		tz;
	struct tm			*tptr, t;

	gettimeofday(&tv, &tz);
	localtime_r(&tv.tv_sec, &t);
	tptr = &t;

	sprintf(pTimestr, "%02d:%02d:%02d.%03d", tptr->tm_hour, tptr->tm_min, tptr->tm_sec, tv.tv_usec/1000);
	*year  = 1900 + tptr->tm_year;
	*month = tptr->tm_mon + 1;
	*day   = tptr->tm_mday;
}

void get_log_filename(int year, int month, int day)
{
#if 1
	static int	curyear = 0;
	static int	curmon = 0;
	static int	curday = 0;

	// Make Directory
	// Directory Architecture
	// base directory/YYYY/
	if(curyear != year)
	{
		if(g_szBaseLogDir[strlen(g_szBaseLogDir) - 1] != DIR_EXPRESSION)
		{
			sprintf(g_szLogFilenameDir, "%s%c%04d%c", g_szBaseLogDir, DIR_EXPRESSION, year, DIR_EXPRESSION);
		}
		else
		{
			sprintf(g_szLogFilenameDir, "%s%04d%c", g_szBaseLogDir, year, DIR_EXPRESSION);
		}
		mkdir(g_szLogFilenameDir, S_IFDIR|S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO);

		curyear = year;
	}

	if(curmon != month)
	{
		if(g_szBaseLogDir[strlen(g_szBaseLogDir) - 1] != DIR_EXPRESSION)
		{
			sprintf(g_szLogFilenameDir, "%s%c%04d%c%02d%c", g_szBaseLogDir, DIR_EXPRESSION, year, DIR_EXPRESSION, month, DIR_EXPRESSION);
		}
		else
		{
			sprintf(g_szLogFilenameDir, "%s%04d%c%02d%c", g_szBaseLogDir, year, DIR_EXPRESSION, month, DIR_EXPRESSION);
		}
		mkdir(g_szLogFilenameDir, S_IFDIR|S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO);
		curmon = month;
	}

	// Filename
	// Filename %s.MMDD-xx where DD is day and xx is the number starting from 00
	if(curday != day)
	{
		g_filecount = 0;
		sprintf(g_szLogFilename, "%s%s.%02d%02d-%02d", g_szLogFilenameDir, g_szAppName, month, day, g_filecount);
		curday = day;
	}

#else
	if(g_szBaseLogDir[strlen(g_szBaseLogDir) - 1] != DIR_EXPRESSION)
	{
		sprintf(g_szLogFilename, "%s%c%s.%02d%02d", g_szBaseLogDir, DIR_EXPRESSION, g_szAppName, month, day);
	}
	else
	{
		sprintf(g_szLogFilename, "%s%s.%02d%02d", g_szBaseLogDir, g_szAppName, month, day);
	}
#endif
}

void get_error_log_filename(int year, int month, int day)
{
	if(g_szBaseLogDir[strlen(g_szBaseLogDir) - 1] != DIR_EXPRESSION)
	{
		sprintf(g_szErrorLogFilename, "%s%c%s_err.%02d%02d", g_szBaseLogDir, DIR_EXPRESSION, g_szAppName, month, day);
	}
	else
	{
		sprintf(g_szErrorLogFilename, "%s%s_err.%02d%02d", g_szBaseLogDir, g_szAppName, month, day);
	}
}

// Debugging purpose only
void LogTCP(int main_session, int sub_session, const char *sent_rcvd, char *buf, int length)
{
	FILE	*fp;
	int		year, month, day, i;
	long	lFilesize;
	char	szTemp[10], szHex[255], szChar[255], szTimestr[200];

#ifdef WIN32
	EnterCriticalSection(&g_LogCs);
#else
	if(pthread_mutex_lock(&g_LogMutex) != 0)
		perror("pthread_mutex_lock returns an error\n");
#endif

	update_time(&year, &month, &day, szTimestr);
	get_log_filename(year, month, day);

	fp = fopen(g_szLogFilename, "a");
	if(fp != NULL)
	{
		fseek(fp, 0L, SEEK_END);
		fprintf(fp, "%s> TCPL [M%d S%03d] TCP: %s", szTimestr,  main_session, sub_session, sent_rcvd);
#if 0
		memset(szHex, 0x0, sizeof(szHex));
		memset(szChar, 0x0, sizeof(szChar));
		for(i = 0; i < length; i ++)
		{
			sprintf(szTemp, "%02X ", 0x0FF & buf[i]);
			strcat(szHex, szTemp);
			if(buf[i] >= 32 && buf[i] <= 126) sprintf(szTemp, "%c ", buf[i]);
			else strcpy(szTemp, ". ");
			strcat(szChar, szTemp);

			if((i + 1) % 16 == 0)
			{
				fprintf(fp, "|%-49s| %s\n", szHex, szChar);
				memset(szHex, 0x0, sizeof(szHex));
				memset(szChar, 0x0, sizeof(szChar));
			}
			else if((i + 1) % 8 == 0)
			{
				strcat(szHex, " ");
				strcat(szChar, " ");
			}
		}
		// The following statement can be extra if it is multiple times of 16
		fprintf(fp, "|%-49s| %s\n", szHex, szChar);
#endif
		fclose(fp);
	}

#ifdef WIN32
	LeaveCriticalSection(&g_LogCs);
#else
	if(pthread_mutex_unlock(&g_LogMutex) != 0)
		perror("pthread_mutex_unlock returns an error\n");
#endif
}
