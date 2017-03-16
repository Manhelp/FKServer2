#include "stdafx.h"

#include "Login.h"
#include "TimeManager.h"
#include "Log.h"
#include "LoginDBManager.h"
#include "Config.h"
#include "ConnectManager.h"
#include "ThreadManager.h"
#include "ProcessManager.h"
#include "ServerManager.h"
#include "PlayerPool.h"
#include "PacketFactoryManager.h"
#include "TurnPlayerQueue.h"
#include "DBThreadManager.h"
#include "CharConfig.h"
//#include "ClientMain.h"

Login	g_Login;


Login::Login() : m_bExited(FALSE)
{

}

Login::~Login()
{
}

BOOL	Login::Init()
{
	__ENTER_FUNCTION

	
	BOOL	bRet = FALSE;

	Log::SaveLog( "./Log/Login.log", "��ʼ���������ļ���ȡ��" ) ;
	bRet = g_Config.Init( ) ;
	if(!bRet)
	{
		Assert(bRet);
		return FALSE;
	}
	Log::SaveLog( "./Log/Login.log", "�����ļ���ȡ��ϡ�" ) ;

	Log::SaveLog( "./Log/Login.log", "����Loginȫ����������" ) ;
	bRet  = NewLogin();
	if(!bRet)
	{
		Assert(bRet);
		return FALSE;
	}
	Log::SaveLog( "./Log/Login.log", "����Loginȫ����������ϡ�" ) ;

	Log::SaveLog( "./Log/Login.log", "��ʼ��Loginȫ����������" ) ;
	bRet = InitLogin();
	if(!bRet)
	{
		Assert(bRet);
		return FALSE;
	}
	Log::SaveLog( "./Log/Login.log", "��ʼ��Loginȫ����������ϡ�" ) ;

	m_bExited = FALSE;
	return TRUE;

	__LEAVE_FUNCTION

	return FALSE;
}

BOOL	Login::Loop()
{
	__ENTER_FUNCTION

	BOOL bRet = FALSE ;

	Log::SaveLog( LOGIN_LOGFILE, "��ʼ���߼�ѭ����" ) ;
	Log::SaveLog( LOGIN_LOGFILE, "=====================================================" ) ;

	
	//���ӿͻ����߳�
	Log::SaveLog( LOGIN_LOGFILE, "�ͻ����߳� ConnectManager ����" ) ;
	g_pConnectManager->start();

	//����World �� BillingSystem ���߳�
	Log::SaveLog( LOGIN_LOGFILE, "���������ڲ��߳� ThreadManager ����" ) ;
	bRet = g_pThreadManager->Start( ) ;
	Assert( bRet ) ;
	
	//���ݿ⴦��
	Log::SaveLog(LOGIN_LOGFILE,"���ݿ��߳� DBThreadManager ����");
	g_pDBThreadManager->Start();

	//���߳���Դ���� ProcessManager
	Log::SaveLog( LOGIN_LOGFILE, "�߼����߳� ProcessManager ����" ) ;
	g_pProcessManager->start();

	//�ػ��߳�
	while(TRUE)
	{
		MySleep(100);
	}

	return bRet;

	__LEAVE_FUNCTION

	return FALSE;
}

BOOL	Login::Exit()
{
	__ENTER_FUNCTION

	
	Log::SaveLog( LOGIN_LOGFILE, "Begin delete..." ) ;

	SAFE_DELETE(g_pDataBaseManager);
	Log::SaveLog( LOGIN_LOGFILE, "g_pDataBaseManager delete...OK" ) ;

	SAFE_DELETE(g_pProcessManager);
	Log::SaveLog( LOGIN_LOGFILE, "g_pProcessManager delete...OK" ) ;

	SAFE_DELETE(g_pPlayerPool);
	Log::SaveLog( LOGIN_LOGFILE, "g_pPlayerPool delete...OK" ) ;
	

	SAFE_DELETE(g_pPacketFactoryManager);
	Log::SaveLog( LOGIN_LOGFILE, "g_pPacketFactoryManager delete...OK" ) ;

	SAFE_DELETE(g_pProcessPlayerQueue);
	Log::SaveLog( LOGIN_LOGFILE, "g_pProcessPlayerQueue delete...OK" ) ;
	
	SAFE_DELETE(g_pWorldPlayerQueue);
	Log::SaveLog( LOGIN_LOGFILE, "g_pWorldPlayerQueue delete...OK" ) ;
	
	SAFE_DELETE(g_pThreadManager);
	Log::SaveLog( LOGIN_LOGFILE, "g_pThreadManager delete...OK" ) ;
	
	SAFE_DELETE(g_pServerManager);
	Log::SaveLog( LOGIN_LOGFILE, "g_pServerManager delete...OK" ) ;
	
	SAFE_DELETE(g_pConnectManager);
	Log::SaveLog( LOGIN_LOGFILE, "g_pConnectManager delete...OK" ) ;
	
	SAFE_DELETE(g_pDBThreadManager);
	Log::SaveLog(LOGIN_LOGFILE,"g_pDBThreadManager delete ... OK");

	Log::SaveLog( LOGIN_LOGFILE, "End delete..." ) ;
	
	m_bExited = TRUE;

	return TRUE;

	__LEAVE_FUNCTION

	return FALSE;
}


/*
*	����ռ����
*/
BOOL	Login::NewLogin()
{
	__ENTER_FUNCTION

	g_pDataBaseManager	=	 new	LoginDBManager();
	AssertEx(g_pDataBaseManager,"�������ݿ������ʧ��!");
	Log::SaveLog(LOGIN_LOGFILE,"����LoginDBManager�ɹ�");

	g_pProcessManager	=	 new  ProcessManager();
	AssertEx( g_pProcessManager,"����g_pProcessManager ʧ��!");
	Log::SaveLog( LOGIN_LOGFILE, "����ProcessManager�ɹ�" ) ;
	
	g_pPlayerPool		=	new	  PlayerPool ;
	AssertEx( g_pPlayerPool,"����g_pPlayerPool ʧ��!");
	Log::SaveLog( LOGIN_LOGFILE, "����PlayerPool�ɹ�" ) ;

	g_pPacketFactoryManager = new PacketFactoryManager ;
	AssertEx( g_pPacketFactoryManager,"����g_pFacketFactoryManager ʧ��!") ;
	Log::SaveLog( LOGIN_LOGFILE, "����PacketFactoryManager�ɹ�" ) ;
	
	g_pProcessPlayerQueue	= new TurnPlayerQueue;
	AssertEx(g_pProcessPlayerQueue,"�����ŶӶ���ʧ��!");
	Log::SaveLog(LOGIN_LOGFILE,"����TurnPlayerQueue�ɹ�");

	g_pWorldPlayerQueue	= new WorldPlayerQueue;
	AssertEx(g_pWorldPlayerQueue,"�����ŶӶ���ʧ��!");
	Log::SaveLog(LOGIN_LOGFILE,"����WorldPlayerQueue�ɹ�");

	g_pThreadManager	=	 new ThreadManager();
	AssertEx( g_pThreadManager,"����g_pThreadManager ʧ��!");
	Log::SaveLog( LOGIN_LOGFILE, "����ThreadManager�ɹ�" ) ;

	g_pServerManager    =	 new ServerManager ;
	AssertEx( g_pServerManager,"����g_pServerManager ʧ��!" ) ;
	Log::SaveLog( LOGIN_LOGFILE, "����ServerManager�ɹ�" ) ;

	g_pConnectManager   =    new ConnectManager ;
	AssertEx( g_pConnectManager,"����g_pConnectManager ʧ��!");
	Log::SaveLog( LOGIN_LOGFILE, "����ConnectManager�ɹ�" ) ;

	g_pDBThreadManager	=	new DBThreadManager;
	AssertEx(g_pDBThreadManager,"����g_pDBThreadManager ʧ��!");
	Log::SaveLog( LOGIN_LOGFILE, "����DBThreadManager�ɹ�" );

	return TRUE;

	__LEAVE_FUNCTION

	return FALSE;
}

/*
*	���ݸ�ֵ���
*/
BOOL	Login::InitLogin()
{
	__ENTER_FUNCTION

	BOOL	bRet = FALSE;
	INT		nTemp = 0;

	//DB �ĳ�ʼ��,�������ݿ�
	Assert(g_pDataBaseManager);
	bRet = g_pDataBaseManager->Init();
	Assert(bRet);
	Log::SaveLog(LOGIN_LOGFILE,"DataBaseManager ��ʼ�����");

	if( g_Config.m_ConfigInfo.m_SystemModel == 0 )
	{
		nTemp = 5;
	}
	else
	{
		nTemp = MAX_POOL_SIZE;
	}
	bRet = g_pPlayerPool->Init( nTemp ) ;
	Assert( bRet ) ;
	Log::SaveLog( LOGIN_LOGFILE, "PlayerPool ��ʼ�����" ) ;

	bRet = g_pPacketFactoryManager->Init( ) ;
	Assert( bRet ) ;
	Log::SaveLog( LOGIN_LOGFILE, "PacketFactoryManager ��ʼ�����" ) ;
	
	bRet = g_pProcessPlayerQueue->Init();
	Assert(bRet);
	Log::SaveLog(LOGIN_LOGFILE,"DataBaseManager ��ʼ�����");

	bRet = g_pWorldPlayerQueue->Init();
	Assert(bRet);
	Log::SaveLog(LOGIN_LOGFILE,"WorldPlayerQueue ��ʼ�����");

	//�Կͻ������������ĳ�ʼ��
	bRet = g_pConnectManager->Init( ) ;
	Assert( bRet ) ;
	Log::SaveLog( LOGIN_LOGFILE, "ConnectManager ��ʼ�����" ) ;
	
	// ��¼���̹�����
	bRet	= g_pProcessManager->Init();
	Assert(bRet);
	Log::SaveLog( LOGIN_LOGFILE, "ProcessManager ��ʼ�����" ) ;

	// �̹߳�����
	bRet = g_pThreadManager->Init( ) ;
	Assert( bRet ) ;
	Log::SaveLog( LOGIN_LOGFILE, "ThreadManager ��ʼ�����" ) ;

	bRet = g_pDBThreadManager->Init();
	Assert(bRet);
	Log::SaveLog( LOGIN_LOGFILE, "DBThreadManager ��ʼ�����" ) ;

	bRet = g_CharConfig.Init();
	Assert(bRet);
	Log::SaveLog( LOGIN_LOGFILE, "CharConfig ��ʼ�����" ) ;

	//if (g_Config.m_LoginInfo.m_AskAuthType != _LOGIN_INFO::USE_BILLING)
	//{
		//bRet = g_ClientMain.InIt(g_Config.m_LoginInfo.m_ClientMainPath);
		//Assert(bRet);
		//Log::SaveLog( LOGIN_LOGFILE, "g_ClientMain->InIt()...OK" ) ;
	//}


	return TRUE;

	__LEAVE_FUNCTION

	return FALSE;
}

VOID	Login::Stop()
{
	__ENTER_FUNCTION

		if( g_pConnectManager )
		{
			g_pConnectManager->stop( ) ;
		}

	__LEAVE_FUNCTION
}


VOID INTHandler(INT)
{
	DumpStack("INT exception:\r\n");
	exit(0);
}
VOID TERMHandler(INT)
{
	DumpStack("TERM exception:\r\n");
	exit(0);

}
VOID ABORTHandler(INT)
{
	DumpStack("ABORT exception:\r\n");
	exit(0);
}

VOID ILLHandler(INT)
{
	DumpStack("ILL exception:\r\n");
	exit(0);
}

VOID FPEHandler(INT)
{
	DumpStack("FPE exception:\r\n");
	exit(0);
}

VOID SEGHandler(INT)
{
	DumpStack("SEG exception:\r\n");
	exit(0);
}
VOID XFSZHandler(INT)
{
	DumpStack("XFSZ exception:\r\n");
	exit(0);
}

LoginExceptionHandler::LoginExceptionHandler()
{
#ifdef __LINUX__
	signal(SIGSEGV, SEGHandler);
	signal(SIGFPE,  FPEHandler);
	signal(SIGILL,  ILLHandler);
	signal(SIGINT,  INTHandler);  
	signal(SIGTERM, TERMHandler); 
	signal(SIGABRT, ABORTHandler);
	signal(SIGXFSZ, XFSZHandler);
#endif
}

LoginExceptionHandler g_LoginExceptionHandler;
