#ifndef __MYTHREAD_H__
#define __MYTHREAD_H__


#include  <pthread.h>


class CRunnable
{
public:
	virtual void Run() = 0;
};


class CMyThread : public CRunnable
{
public:
	CMyThread();
	CMyThread(CRunnable* pRunnable);
	void Start();
	void Wait();
	virtual void Run();		// virtual function implementation. Define the main logical function to be executed by a thread

private:
	pthread_t	tid;
	CRunnable*	runnable;

	static void* Main(void* pInst);
};


#endif	// end of __MYTHREAD_H__
