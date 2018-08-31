#include "app_manager.h"
#include "evpp/logging.h"
#include "sys/common.h"

CAppManager::CAppManager():m_CreateLoopMyself(true)
{
    m_Loop = new evpp::EventLoop;
}


CAppManager::CAppManager(evpp::EventLoop *loop):m_CreateLoopMyself(false), m_Loop(loop)
{
}

CAppManager::~CAppManager()
{
    if (m_CreateLoopMyself && NULL != m_Loop) {
        delete m_Loop;
    }
}



bool CAppManager::Init()
{
    FLAGS_log_dir = "./log/";
    google::InitGoogleLogging("smartLightControl");
    FLAGS_minloglevel   = 0;            //�����־����С���𣬼����ڵ��ڸü������־�������
    FLAGS_max_log_size  = 20;           //ÿ����־�ļ�����С��MB����
        
    return this->OnInit();
}

bool CAppManager::Start()
{
    if (!this->OnStart()) {
        LOG_ERROR << "CAppManager::Start failed.";
    }

    m_Loop->Run();//loop

    LOG_INFO << "CAppManager exited.";
    Stop();

	return true;
}
    
bool CAppManager::Stop()
{
    m_Loop->Stop();
    return this->OnStop();
}

bool CAppManager::OnInit()
{
    return true;
}

bool CAppManager::OnStart()
{    
    return true;
}
    
bool CAppManager::OnStop()
{
    return true;
}


