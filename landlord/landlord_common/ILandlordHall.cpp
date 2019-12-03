#include "ILandlordHall.h"
// #include "landlord.pb.h"

// // 消息格式：
// //  - 消息头长度(2字节) + 消息头(不定长) + 消息内容(不定长)

// std::string ILandlordHall::SerialMsg(::google::protobuf::Message* msg)
// {
//     using namespace ::google::protobuf;
//     using namespace landlord;
//     bool b;
//     unsigned short head_len;
//     meta_msg meta;
//     std::string meta_data;
//     std::string data;

//     meta.set_msg_name(msg->GetTypeName());
//     b = msg->SerializeToString(&data);
//     if(!b){
//         return std::string();
//     }
//     meta.set_data_size(data.size());

//     b = meta.SerializeToString(&meta_data);
//     if(!b){
//         return std::string();
//     }

//     std::string d;
//     head_len = meta_data.size();
//     d.append((const char*)&head_len, 2);
//     d.append(meta_data);
//     d.append(data);

//     return d;
// }

// ::google::protobuf::Message* ILandlordHall::DeserialMsg(std::string& dat)
// {
//     using namespace ::google::protobuf;
//     using namespace landlord;
//     bool b;
//     Message* msg = nullptr;
//     const char* d = dat.data();
//     unsigned short head_len;
//     memcpy(&head_len, d, 2);

//     meta_msg meta;
//     b = meta.ParseFromArray(&d[2], head_len);
//     if(!b){
//         return msg;
//     }

//     std::string msg_name = meta.msg_name();
//     if(msg_name == "landlord.hall_login"){
//         msg = new hall_login();
//     }
//     if(msg_name == "landlord.hall_match"){
//         msg = new hall_match();
//     }
//     if(msg_name == "landlord.hall_quit"){
//         msg = new hall_quit();
//     }
//     if(msg_name == "landlord.room_grab"){
//         msg = new room_grab();
//     }
//     if(msg_name == "landlord.room_play"){
//         msg = new room_play();
//     }
//     if(msg_name == "landlord.player_deal"){
//         msg = new player_deal();
//     }
//     if(msg_name == "landlord.player_grab"){
//         msg = new player_grab();
//     }
//     if(msg_name == "landlord.player_landlord"){
//         msg = new player_landlord();
//     }
//     if(msg_name == "landlord.player_login"){
//         msg = new player_login();
//     }
//     if(msg_name == "landlord.player_match"){
//         msg = new player_match();
//     }
//     if(msg_name == "landlord.player_play"){
//         msg = new player_play();
//     }
//     if(msg_name == "landlord.player_win"){
//         msg = new player_win();
//     }
//     if(msg == nullptr) return nullptr;
//     b = msg->ParseFromArray(&d[2 + head_len], meta.data_size());
//     if(!b){
//         delete msg;
//         return nullptr;
//     }
//     return msg;
// }