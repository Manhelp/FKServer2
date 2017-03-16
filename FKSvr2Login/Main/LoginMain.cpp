#include "stdafx.h"

#include "LoginMain.h"
#include "TimeManager.h"
#include "Log.h"
#include "Config.h"
#include "Login.h"






INT main(INT argc, CHAR* argv[])
{
#if defined(__WINDOWS__)
	_CrtSetDbgFlag(_CrtSetDbgFlag(0) | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	__ENTER_FUNCTION
	
		if( argc>1 )
		{
			for( int i=1; i<argc; i++ )
			{
				if( strcmp(argv[i],"-ignoreassert")==0 )
				{
					g_Command_Assert=1 ;
				}
				else if( strcmp(argv[i],"-retryassert")==0 )
				{
					g_Command_Assert=2 ;
				}

				if( strcmp(argv[i],"-ignoremessagebox")==0 )
				{
					g_Command_IgnoreMessageBox=TRUE ;
				}
			}
		}	
		
		
	//ʱ�������
	g_pTimeManager	=	new TimeManager;
	Assert( g_pTimeManager ) ;
	if( g_pTimeManager->Init() )
	{
		Log::SaveLog( "./Log/Login.log", "ʱ���������ʼ����ϡ�" );
	}
	else
	{
		Log::SaveLog( "./Log/Login.log", "ʱ���������ʼ��ʧ�ܡ�" );
		return 1;
	}
	g_pTimeManager->SetTime() ;
	Log::SaveLog( "./Log/Login.log", "=====================================" );
	Log::SaveLog( "./Log/Login.log", "����ʱ�䣺 %.10d",g_pTimeManager->Time2DWORD() );
	Log::SaveLog( "./Log/Login.log", "=====================================" );
		
	Log::SaveLog( "./Log/Login.log", "����Login����������ǰʱ�� = %.10d ��ʼʱ����� = %d",
	g_pTimeManager->Time2DWORD(),
	g_pTimeManager->StartTime() );

	srand(g_pTimeManager->CurrentTime());
	BOOL	bRet  =		g_Login.Init();
	Assert(bRet);
		
	bRet	=			g_Login.Loop();	
	Assert(bRet);

	bRet	=			g_Login.Exit();
	Assert(bRet);

	return	0;

	__LEAVE_FUNCTION

	return -1;
}





