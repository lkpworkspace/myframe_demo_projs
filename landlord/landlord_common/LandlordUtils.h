#ifndef __LANDLORDUTILS_H__
#define __LANDLORDUTILS_H__
#include "google/protobuf/message.h"
#include "MyContext.h"
#include "MyMsg.h"
#include <iostream>

class LandlordUtils
{
public:
	LandlordUtils(MyContext* c) :
        m_c(c)
    {}
	virtual ~LandlordUtils();

    void SetContext(MyContext* c) { m_c = c; }
    MyContext* GetContext() { return m_c; }

    std::string SerialMsg(::google::protobuf::Message* msg);
	::google::protobuf::Message* DeserialMsg(std::string& dat);

    void Send(int player_id, ::google::protobuf::Message* msg)
    {
        MyTextMsg* tmsg = new MyTextMsg(my_handle_name("landlord_server"), SerialMsg(msg));
        tmsg->session = player_id;
        if(m_c == nullptr){
            std::cout << "send context is nullptr" << std::endl;
            return;
        }
        my_send(m_c, tmsg);
    }
private:
    MyContext* m_c;
};


#endif
