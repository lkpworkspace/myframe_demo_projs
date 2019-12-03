#ifndef __LANDLORDUTILS_H__
#define __LANDLORDUTILS_H__
#include "google/protobuf/message.h"
#include "MyMsg.h"
#include "MyModule.h"
#include <iostream>

class LandlordUtils
{
public:
	LandlordUtils(MyModule* c) :
        m_module(c)
    {}
	virtual ~LandlordUtils();

    void SetModule(MyModule* c) { m_module = c; }
    MyModule* GetModule() { return m_module; }

    std::string SerialMsg(::google::protobuf::Message* msg);
	::google::protobuf::Message* DeserialMsg(std::string& dat);

    void Send(int player_id, ::google::protobuf::Message* msg)
    {
        MyTextMsg* tmsg = new MyTextMsg(m_module->GetHandle("landlord_server"), SerialMsg(msg));
        tmsg->session = player_id;
        m_module->Send(tmsg);
    }
private:
    MyModule* m_module;
};


#endif
