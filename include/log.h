#ifndef  __LOG_H__
#define  __LOG_H__


/* Trace mask */
#define  TM_FATAL			0x00000001
#define  TM_CRITICAL		0x00000002
#define  TM_MAJOR			0x00000004
#define  TM_MINOR			0x00000008
#define  TM_WARNING			0x00000010
#define  TM_INFO			0x00000020
#define  TM_FNBEGIN			0x00000040
#define  TM_FNEND			0x00000080
#define  TM_APIERROR		0x00000100
#define  TM_STAT			0x00000200


void InitializeLog(const char *pBaseLogDir, unsigned long logFileSizeLimit);
void UninitializeLog();
int MakeLogBaseDir();
void SetTraceMask(unsigned long console_tm, unsigned long file_tm);
void LogPrintf(int main_session, int sub_session, unsigned long message_type, const char *format, ... );
void LogErrorPrintf(int main_session, int sub_session, unsigned long trace_mask, const char *format, ... );

void LogTCP(int main_session, int sub_session, const char *sent_rcvd, char *buf, int length);

#endif  /* end of __LOG_H__ */
