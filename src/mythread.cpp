#include  <stdio.h>
#include  <errno.h>
#include  <string.h>
#include  <stdio.h>
#include  <stdlib.h>

#include  "mythread.h"

CMyThread::CMyThread() : tid(0), runnable(0)
{
}

CMyThread::CMyThread(CRunnable* pRunnable) : tid(0), runnable(pRunnable)
{
}

void CMyThread::Start()
{
#ifdef  DEBUG
	printf("%s called.\n", __FUNCTION__);
#endif

	if(0 != pthread_create(&tid, NULL, &CMyThread::Main, this))
	{
		printf("pthread_create returns an error, %s\n", strerror(errno));
		exit(0);
	}
}

void CMyThread::Wait()
{
#ifdef  DEBUG
	printf("%s called.\n", __FUNCTION__);
#endif
	int nr = pthread_join(tid, NULL);
}

void CMyThread::Run()
{
	if(runnable != 0)
	{
#ifdef  DEBUG
		printf("%s calling runnable.\n", __FUNCTION__);
#endif
		runnable->Run();
	}
}

void* CMyThread::Main(void* pInst)
{
#ifdef  DEBUG
	printf("%s is called.\n", __FUNCTION__);
#endif
	CMyThread* pmt = static_cast<CMyThread *>(pInst);
	pmt->Run();
}

