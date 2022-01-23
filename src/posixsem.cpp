#include  <stdio.h>
#include  <string.h>
#include  <errno.h>
#include  <fcntl.h>
#include  "posixsem.h"
#include  "log.h"


bool CPosixSem::CreateSemaphore(const char *pSemName, int nInitValue)
{
	sem_unlink(pSemName);
	if(errno != 0 && ENOENT != errno)
	{
		if(errno != EEXIST)
			LogPrintf(0, 0, TM_APIERROR, "sem_unlink returns an error, %s in %s\n", strerror(errno), __FUNCTION__);
	}

	m_pSemId = sem_open(pSemName, O_CREAT, S_IRUSR | S_IWUSR, nInitValue);	// | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, nInitValue);
	if(m_pSemId == SEM_FAILED)
	{
		LogPrintf(0, 0, TM_APIERROR, "sem_open returns an error, %s\n", strerror(errno));
		return false;
	}

	return true;
}

bool CPosixSem::GetValue(int &sval)
{
	if(sem_getvalue(m_pSemId, &sval) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "sem_getvalue returns an error, %s\n", strerror(errno));
		return false;
	}
	return true;
}

bool CPosixSem::PostSemaphore()
{
	if(sem_post(m_pSemId) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "sem_post returns an error, %s\n", strerror(errno));
		return false;
	}
	return true;
}

bool CPosixSem::WaitSemaphore()
{
	if(sem_wait(m_pSemId) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "sem_wait returns an error, %s\n", strerror(errno));
		return false;
	}
	return true;
}

void CPosixSem::DestroySemaphore(const char *pSemName)
{
	if(sem_unlink(pSemName) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "sem_unlink returns an error, %s\n", strerror(errno));
	}
}
