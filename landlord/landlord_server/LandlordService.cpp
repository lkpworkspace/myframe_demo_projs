#include <iostream>
#include <string.h>
#include <queue>
#include <map>
#include <sstream>

#include "MyModule.h"
#include "MyMsg.h"
#include "LandlordHall.h"

class MyLandlordService : public MyModule
{
public:
    MyLandlordService():
        m_hall(nullptr)
    {}
    virtual ~MyLandlordService(){}

    virtual int Init(const char* param) override
    {
        m_hall.SetModule(this);
        std::cout << "landlord server start " << GetServiceName() << std::endl;
        return 0;
    }

    virtual int CB(MyMsg* msg) override
    {
        if(msg->GetMsgType() == MyMsg::MyMsgType::TEXT){
            MyTextMsg* tmsg = static_cast<MyTextMsg*>(msg);
            m_hall.Parse(tmsg->session, tmsg->GetData());
            for(auto begin = m_hall.Resps().begin(); begin != m_hall.Resps().end(); ++begin){
                std::cout << "[landlord server]: resp msg to: " << msg->source << std::endl;
                MyTextMsg* send_tmsg = new MyTextMsg(msg->source, begin->second);
                send_tmsg->session = begin->first;
                Send(send_tmsg);
            }
            m_hall.Resps().clear();
        }
        return 1;        
    }

private:
    LandlordHall m_hall;
};

extern "C" MyModule* my_mod_create()
{
    return static_cast<MyModule*>(new MyLandlordService());
}

extern "C" void my_mod_destory(MyModule* m)
{
    delete m;
}


