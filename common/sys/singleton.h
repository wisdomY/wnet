#pragma once

//单列模板类
template <typename T>
class Singleton
{
public:
    static T *GetInstance()
    {
        static T m_Instance;
        return &m_Instance;
    }

private:

    Singleton(){}

    ~Singleton(){}

};


