#ifndef  __POSIXSEM_H__
#define  __POSIXSEM_H__


#include  <semaphore.h>


// Posix Semaphore
class CPosixSem
{
public:
	bool	CreateSemaphore(const char *pSemName, int nInitValue);
	bool	PostSemaphore();
	bool	WaitSemaphore();
	void	DestroySemaphore(const char *pSemName);

private:
	bool	GetValue(int &sval);

	sem_t	*m_pSemId;		// semaphore handle
};


#endif	// end of __POSIXSEM_H__
