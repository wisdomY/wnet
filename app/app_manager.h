#pragma once
#include "net/qynet.h"

class CAppManager
{
public:
    CAppManager();
    explicit CAppManager(evpp::EventLoop *loop);
    ~CAppManager();
public:
    bool Init(void);
    bool Start(void);
    bool Stop(void);

    evpp::EventLoop *GetLoop(void)
    {
        return m_Loop;
    }
    
protected:
    virtual bool  OnInit();
    virtual bool  OnStart();
    virtual bool  OnStop();

private:
    evpp::EventLoop *m_Loop;
    bool m_CreateLoopMyself;
};



