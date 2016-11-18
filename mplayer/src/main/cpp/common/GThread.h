#ifndef TGHREAD_H
#define TGHREAD_H

#include <pthread.h>
 
typedef void (*fGXX_F_THREAD_StartProc)(void* pInParam);
 
typedef struct _GXX_S_THREAD_DATA
{
	bool						bRunFlag;

#ifdef _WIN32
	ULONG						dwThreadId;						 
	HANDLE							hThread;						 
#else
	pthread_t						t_thread_data;
	bool						bIfCreated;
	pthread_mutex_t					t_mutex_lock;
	bool						bIfInitMutex;
	pthread_cond_t					cond_exit_notify;
	bool						bIfInitCond;
#endif

	fGXX_F_THREAD_StartProc			pThreadProc;					 
	void*						pThreadParam;					 

} GXX_S_THREAD_DATA, *PGXX_S_THREAD_DATA;

class CGThread
{
public:
	CGThread(void);
	~CGThread(void);
 
	typedef void (*fGXX_F_THREAD_StartProc)(void* pInParam);


	bool	CreateThread(
				fGXX_F_THREAD_StartProc			in_pWorkProc,			 
				void*						in_pInParam				 
		);
 
	void	WaitExitThread( 
				int							in_iTimeout
		);

	bool TestExit();

private:
	PGXX_S_THREAD_DATA						pThreadData;
	bool									m_bIsExit;
 
};


#endif
