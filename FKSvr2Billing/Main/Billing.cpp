#include "stdafx.h"
#include "Billing.h"
#include "TimeManager.h"
#include "Log.h"
#include "Config.h"
#include "ServerManager.h"
#include "WebPlayer.h"
#include "PacketFactoryManager.h"
#include "PlayerPool.h"
#include "UserDBManager.h"



Billing			g_Billing ;

int main(int argc, char* argv[])
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
				if( strcmp(argv[i],"-retryassert")==0 )
				{
					g_Command_Assert=2 ;
				}
				else if( strcmp(argv[i],"-ignoremessagebox")==0 )
				{
					g_Command_IgnoreMessageBox=TRUE ;
				}
			}
		}


		BOOL bRet ;

		//ʱ��ģ����Ҫ�ʼ��ʱ������
		g_pTimeManager = new TimeManager ;
		Assert( g_pTimeManager ) ;
		if( g_pTimeManager->Init() )
		{
			Log::SaveLog( "./Log/Billing.log", "ʱ���������ʼ����ϡ�" ) ;
		}
		else
		{
			Log::SaveLog( "./Log/Billing.log", "ʱ���������ʼ��ʧ�ܡ�" ) ;
			return 1;
		}
		Log::SaveLog( "./Log/Billing.log", "=====================================" );
		Log::SaveLog( "./Log/Billing.log", "����ʱ�䣺 %.10d",g_pTimeManager->Time2DWORD() );
		Log::SaveLog( "./Log/Billing.log", "=====================================" );

		Log::SaveLog( "./Log/Billing.log", "����Billing������" ) ;

		bRet = g_Billing.Init( ) ;
		if( bRet==FALSE )
		{
			Assert(FALSE) ;
			return 1 ;
		}
		else
		{
			Log::SaveLog( "./Log/Billing.log", "Billing��ʼ����ϡ�" ) ;
		}

		bRet = g_Billing.Loop( ) ;
		if( bRet==FALSE )
		{
			Assert(FALSE) ;
			return 1 ;
		}

		bRet = g_Billing.Exit( ) ;
		if( bRet==FALSE )
		{
			Assert(FALSE) ;
			return 1 ;
		}

		__LEAVE_FUNCTION

			return 0 ;
}

Billing::Billing( )
{
	__ENTER_FUNCTION

#if defined(__WINDOWS__)
		WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData ); 
#endif


	__LEAVE_FUNCTION
}

Billing::~Billing( )
{
	__ENTER_FUNCTION

#if defined(__WINDOWS__)
		WSACleanup();
#endif

	__LEAVE_FUNCTION
}

BOOL Billing::Init( )
{
	__ENTER_FUNCTION

	BOOL bRet ;

	Log::SaveLog( "./Log/Billing.log", "��ʼ���������ļ���ȡ��" ) ;
	bRet = g_Config.Init( ) ;
	Assert( bRet ) ;
	Log::SaveLog( "./Log/Billing.log", "�����ļ���ȡ��ϡ�" ) ;

	Log::SaveLog( "./Log/Billing.log", "������̬��������" ) ;
	bRet = NewStaticManager( ) ;
	Assert( bRet ) ;
	Log::SaveLog( "./Log/Billing.log", "������̬��������ϡ�" ) ;

	Log::SaveLog( "./Log/Billing.log", "��ʼ����̬��������" ) ;
	bRet = InitStaticManager( ) ;
	Assert( bRet ) ;
	Log::SaveLog( "./Log/Billing.log", "��ʼ����̬��������ϡ�" ) ;

	return TRUE ;

	__LEAVE_FUNCTION

	return FALSE ;
}

BOOL Billing::Loop( )
{
	__ENTER_FUNCTION

	Log::SaveLog( "./Log/Billing.log", "��ʼ���߼�ѭ����" ) ;
	Log::SaveLog( "./Log/Billing.log", "=====================================================" ) ;
	g_pServerManager->Loop( ) ;

	__LEAVE_FUNCTION

		return TRUE ;
}

BOOL Billing::Exit( )
{
	__ENTER_FUNCTION

	BOOL bRet ;

	bRet = DelStaticManager( ) ;
	Assert( bRet ) ;

	__LEAVE_FUNCTION

		return TRUE ;
}

BOOL Billing::NewStaticManager( )
{
	__ENTER_FUNCTION

	g_pServerManager = new ServerManager ;
	Assert( g_pServerManager ) ;
	Log::SaveLog( "./Log/Billing.log", "���� ServerManager ���" ) ;

	g_pPlayerPool = new PlayerPool ;
	Assert( g_pPlayerPool ) ;
	Log::SaveLog( "./Log/Billing.log", "���� PlayerPool ���" ) ;

	g_pWebPlayer = new WebPlayer ;
	Assert( g_pWebPlayer ) ;
	Log::SaveLog( "./Log/Billing.log", "���� WebPlayer ���" ) ;

	g_pPacketFactoryManager = new PacketFactoryManager ;
	Assert( g_pPacketFactoryManager ) ;
	Log::SaveLog( "./Log/Billing.log", "���� PacketFactoryManager ���" ) ;

	
	__LEAVE_FUNCTION

	return TRUE ;
}

BOOL Billing::InitStaticManager( )
{
	__ENTER_FUNCTION

	BOOL ret ;
	INT nTemp = 0 ;

	ret = g_pServerManager->Init( ) ;
	Assert( ret ) ;
	Log::SaveLog( "./Log/Billing.log", "ServerManager ��ʼ�����" ) ;

	ret = g_pWebPlayer->Init();
	Assert( ret ) ;
	Log::SaveLog( "./Log/Billing.log", "WebPlayer ��ʼ�����" ) ;

	if( g_Config.m_ConfigInfo.m_SystemModel == 0 )
	{
		nTemp = 5;
	}
	else
	{
		nTemp = MAX_POOL_SIZE;
	}
	ret = g_pPlayerPool->Init( nTemp ) ;
	Assert( ret ) ;
	Log::SaveLog( "./Log/Billing.log", "pPlayerPool ��ʼ�����" ) ;
	
	//
	ret = g_pPacketFactoryManager->Init( ) ;
	Assert( ret ) ;
	Log::SaveLog( "./Log/Billing.log", "PacketFactoryManager ��ʼ�����" ) ;

	
	__LEAVE_FUNCTION

	return TRUE ;
}

BOOL Billing::DelStaticManager( )
{
	__ENTER_FUNCTION


	SAFE_DELETE( g_pTimeManager ) ;
	Log::SaveLog( "./Log/Billing.log", "TimeManager ɾ�����" ) ;


	//����ģ��ź���ɾ��
	SAFE_DELETE( g_pPacketFactoryManager ) ;
	Log::SaveLog( "./Log/Billing.log", "PacketFactoryManager ɾ�����" ) ;
	
	SAFE_DELETE( g_pPlayerPool ) ;
	Log::SaveLog( "./Log/Billing.log", "PlayerPool ɾ�����" ) ;

	SAFE_DELETE(g_pWebPlayer);
	Log::SaveLog( "./Log/Billing.log", "WebPlayer ɾ�����" ) ;
	
	SAFE_DELETE( g_pServerManager ) ;
	Log::SaveLog( "./Log/Billing.log", "ServerManager ɾ�����" ) ;

	__LEAVE_FUNCTION

	return TRUE ;
}