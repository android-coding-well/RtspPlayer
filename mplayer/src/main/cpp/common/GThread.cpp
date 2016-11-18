#include "GThread.h"

CGThread::CGThread(void)
{
	pThreadData=NULL;
	m_bIsExit=false;
}

CGThread::~CGThread(void)
{
}


#ifdef _WIN32
#include <process.h>
#endif


#ifndef _WIN32 
int	 THREAD_PR_CondTimeWait(
			pthread_cond_t*						in_pCond,					 
			pthread_mutex_t*					in_pMutex,
			int								in_iTimeout
	)
{
	struct timeval now;
	struct timespec outtime;

	gettimeofday(&now, NULL);

	outtime.tv_sec = now.tv_sec + ((now.tv_usec + in_iTimeout * 1000) / (1000 * 1000));
	outtime.tv_nsec = ((now.tv_usec + in_iTimeout * 1000) % (1000 * 1000)) * 1000;

	return pthread_cond_timedwait(
		in_pCond, 
		in_pMutex, 
		&outtime);
}
#endif


//------------------------------------------------------------------------------------------------
//�����߳�
#ifdef _WIN32
ULONG THREAD_ThreadWork(
							  		void*							in_pParameter				//����ָ��
									)
#else
void THREAD_ThreadWork(
							 		void*							in_pParameter				//����ָ��
							 )
#endif
{
	PGXX_S_THREAD_DATA							pThreadData;

	if ( in_pParameter )
	{
		pThreadData = (PGXX_S_THREAD_DATA)in_pParameter;

		if ( pThreadData->pThreadProc )
		{
			pThreadData->pThreadProc(pThreadData->pThreadParam);
		}
	}

#ifdef _WIN32
	_endthreadex(0);
	return 0;
#else
	pthread_mutex_lock(&pThreadData->t_mutex_lock);
	pThreadData->bIfCreated = false;
	pthread_cond_signal(&pThreadData->cond_exit_notify);
	pthread_mutex_unlock(&pThreadData->t_mutex_lock);
#endif
}

//--------------------------------------------------------------------------------
//�����߳�
bool	CGThread::CreateThread(
			fGXX_F_THREAD_StartProc			in_pWorkProc,			//�̹߳������
			void*						in_pInParam				//�������
	)
{
	bool								bRet;
	bRet = true;
	pThreadData = NULL;

	//CODE AREA
	{
		//�����ڴ�
		pThreadData = (PGXX_S_THREAD_DATA)malloc(sizeof(GXX_S_THREAD_DATA));
		if ( pThreadData == NULL )
		{
			bRet = false;
			goto _GXX_CLEAR;
		}
		memset(pThreadData, 0, sizeof(GXX_S_THREAD_DATA));

		pThreadData->bRunFlag = true;
		m_bIsExit=true;
		pThreadData->pThreadProc = in_pWorkProc;
		pThreadData->pThreadParam = in_pInParam;
		

#ifdef _WIN32
		//�����߳�
		pThreadData->hThread = (HANDLE)_beginthreadex(
			NULL, 
			0, 
			(unsigned int (__stdcall *)(void *))THREAD_ThreadWork,
			pThreadData, 
			0, 
			(unsigned int *)&(pThreadData->dwThreadId));
		if ( pThreadData->hThread == NULL )
		{
			bRet = FALSE;
			goto _GXX_CLEAR;
		} 
#else
		//��ʼ��������
		if ( pthread_mutex_init(&pThreadData->t_mutex_lock, NULL ) != 0 )
		{
			bRet = false;
			goto _GXX_CLEAR;
		}
		pThreadData->bIfInitMutex = true;

		//��ʼ����������
		if ( pthread_cond_init(&pThreadData->cond_exit_notify, NULL) != 0 )
		{
			bRet = false;
			goto _GXX_CLEAR;
		}
		pThreadData->bIfInitCond = true;

		//�����߳�
		if ( pthread_create(
			&pThreadData->t_thread_data, 
			0, 
			(void*(*)(void*))THREAD_ThreadWork,
			(void*)pThreadData) != 0 )
		{
			bRet = false;
			goto _GXX_CLEAR;
		}

		pThreadData->bIfCreated = true;
#endif
	}

	//CLEAR AREA
_GXX_CLEAR:
	{
		if ( !bRet && pThreadData )
		{
			//�ȴ��˳��߳�
			WaitExitThread( 
				0								//��ʱʱ�������룩
				);
			pThreadData = NULL;
		}
	}

	return bRet;
}



//�ȴ��˳��߳�
void	CGThread::WaitExitThread( 
			int							in_iTimeout				//��ʱʱ�������룩
	)
{
	//PGXX_S_THREAD_DATA						pThreadData;

	//pThreadData = (PGXX_S_THREAD_DATA)in_pThread;
	m_bIsExit=false;
	if ( pThreadData == NULL )
		return;

	if ( in_iTimeout == 0 )
	{
		in_iTimeout = 5000;
	} 
	pThreadData->bRunFlag = false;
	

#ifdef _WIN32
	if ( pThreadData->hThread ) 
	{
		if ( WaitForSingleObject(pThreadData->hThread, in_iTimeout) != WAIT_OBJECT_0 )
			TerminateThread(pThreadData->hThread, 0);

		CloseHandle(pThreadData->hThread);
		pThreadData->hThread = NULL;
	}
#else
	if ( pThreadData->bIfInitMutex )
		pthread_mutex_lock(&pThreadData->t_mutex_lock);

	if ( pThreadData->bIfCreated && pThreadData->bIfInitCond )
	{
		//���������ȴ�
		if ( THREAD_PR_CondTimeWait(
			&pThreadData->cond_exit_notify,			//��������ָ��
			&pThreadData->t_mutex_lock,				//������ָ��
			in_iTimeout								//��ʱʱ�������룩
			) != 0 )
		{
			pThreadData->bIfCreated = false;
			pthread_kill(pThreadData->t_thread_data,0);
			//pthread_cancel(pThreadData->t_thread_data);
		}
	}

	if ( pThreadData->bIfInitMutex )
		pthread_mutex_unlock(&pThreadData->t_mutex_lock);

	if ( pThreadData->bIfInitMutex )
	{
		pthread_mutex_destroy(&pThreadData->t_mutex_lock);
		pThreadData->bIfInitMutex = false;
	}

	if ( pThreadData->bIfInitCond )
	{
		pthread_cond_destroy(&pThreadData->cond_exit_notify);
		pThreadData->bIfInitCond = false;
	}
#endif

	free(pThreadData);
	pThreadData = NULL;
}

bool CGThread::TestExit()
{
	return m_bIsExit;

}
