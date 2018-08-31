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
    FLAGS_minloglevel   = 0;            //输出日志的最小级别，即高于等于该级别的日志都将输出
    FLAGS_max_log_size  = 20;           //每个日志文件最大大小（MB级别）
        
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


