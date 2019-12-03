#ifndef __LANDLORDHALL_H__
#define __LANDLORDHALL_H__
#include <queue>
#include <vector>
#include <memory>
#include <unordered_map>
#include "ILandlordHall.h"
#include "LandlordRoom.h"
#include "LandlordUtils.h"
#include "Player.h"

class LandlordHall : public ILandlordHall, public LandlordUtils
{
    typedef void (LandlordHall::*msg_handle)(int, ::google::protobuf::Message*);
public:
    LandlordHall(MyContext* c);

    void Parse(int player_id, std::string& dat);

    std::vector<std::pair<int,std::string>>& Resps() { return m_resps; }

private:
    void hall_login(int, ::google::protobuf::Message*);
    void hall_match(int, ::google::protobuf::Message*);
    void hall_quit(int, ::google::protobuf::Message*);
    void room_grab(int, ::google::protobuf::Message*);
    void room_play(int, ::google::protobuf::Message*);
    void RegFunc();
    int CreateRoomId(){ return m_assign_room_id++; }

    std::unordered_map<int, Player*> m_players;
    std::unordered_map<int, LandlordRoom*> m_rooms;
    std::queue<int> m_match_queue;

    int m_assign_room_id;
    std::vector<std::pair<int,std::string>> m_resps;
    std::unordered_map<std::string, msg_handle> m_funcs;
};

#endif
