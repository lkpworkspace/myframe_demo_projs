#include <iostream>
#include <string.h>
#include <queue>
#include <map>
#include <sstream>

#include "MyModule.h"
#include "MyFrame.h"
#include "MyContext.h"
#include "MyMsg.h"
#include "LandlordHall.h"

class MyLandlordService : public MyModule
{
public:
    MyLandlordService():
        m_hall(nullptr)
    {}
    virtual ~MyLandlordService(){}

    virtual int Init(MyContext* c, const char* param) override
    {
        m_handle = my_handle(c);
        my_callback(c, CB, this);
        m_hall.SetContext(c);
        std::cout << "landlord server start " << m_handle << std::endl;
        return 0;        
    }

    static int CB(MyContext* context, MyMsg* msg, void* ud)
    {
        MyLandlordService* self = static_cast<MyLandlordService*>(ud);

        if(msg->GetMsgType() == MyMsg::MyMsgType::TEXT){
            MyTextMsg* tmsg = static_cast<MyTextMsg*>(msg);
            self->m_hall.Parse(tmsg->session, tmsg->GetData());
            for(auto begin = self->m_hall.Resps().begin(); begin != self->m_hall.Resps().end(); ++begin){
                std::cout << "[landlord server]: resp msg to: " << msg->source << std::endl;
                MyTextMsg* send_tmsg = new MyTextMsg(msg->source, begin->second);
                send_tmsg->session = begin->first;
                my_send(context, send_tmsg);
            }
            self->m_hall.Resps().clear();
        }
        return 0;        
    }

private:
    uint32_t m_handle;
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


