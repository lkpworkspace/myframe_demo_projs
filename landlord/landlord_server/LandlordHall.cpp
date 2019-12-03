#include "LandlordHall.h"
#include "landlord.pb.h"
#include <iostream>

LandlordHall::LandlordHall(MyModule* c) :
    LandlordUtils(c),
    m_assign_room_id(0)
{
    RegFunc();
}

void LandlordHall::Parse(int player_id, std::string& dat)
{
    using namespace ::google::protobuf;
    Message* msg = DeserialMsg(dat);
    std::cout << "[landlord server]: get msg " << msg->GetTypeName() << std::endl;
    if(msg != nullptr){
        if(m_funcs.find(msg->GetTypeName()) != m_funcs.end()){
            std::cout << "landlord server call: " << msg->GetTypeName() << std::endl;
            (this->*m_funcs[msg->GetTypeName()])(player_id, msg);
        }
        delete msg;
    }
}

void LandlordHall::hall_login(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::hall_login* login = static_cast<landlord::hall_login*>(msg);
    // create palyer
    Player* p = new Player(0, login->player_id());
    if(m_players.find(login->player_id()) != m_players.end()) {
        std::cout << "login player exist" << std::endl;
        delete p;
        return;
    }
    m_players[player_id] = p;
    // resp login
    landlord::player_login resp;
    resp.set_player_id(player_id);
    m_resps.push_back(std::make_pair(player_id,SerialMsg(&resp)));
}

void LandlordHall::hall_match(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::hall_match* match = static_cast<landlord::hall_match*>(msg);
    // 将玩家id加入匹配队列
    m_match_queue.push(player_id);
    // 检查队列是否大于等于3个玩家
    if(m_match_queue.size() >= 3){
        // 弹出三个玩家
        std::vector<Player*> players;
        for(int i = 0; i < 3; ++i){
            players.emplace_back(m_players[m_match_queue.front()]);
            m_match_queue.pop();
        }
        // 创建房间
        int room_id = CreateRoomId();
        LandlordRoom* room = new LandlordRoom();
        room->CreatePlayer(players);
        m_rooms[room_id] = room;

        int first_id = room->DispatchPoker();
        // 广播这3个玩家的玩家ID
        for(int i = 0; i < 3; ++i){
            players[i]->SetRoom(room_id);
            landlord::player_match* resp_match = new landlord::player_match();
            resp_match->set_s(landlord::player_match_state_PLAY);
            resp_match->set_player_id(players[i]->GetId());
            resp_match->clear_player_ids();
            for(int i = 0; i < 3; ++i){
                resp_match->add_player_ids(players[i]->GetId());
            }
            m_resps.push_back(std::make_pair(players[i]->GetId(), SerialMsg(resp_match)));
            delete resp_match;
        }
        // 广播 发牌信息
        for(int i = 0; i < 3; ++i){
            landlord::player_deal*  resp_deal  = new landlord::player_deal();
            resp_deal->set_player_id(players[i]->GetId());
            resp_deal->set_turn(first_id);
            resp_deal->clear_pokers();
            auto pokers = room->GetPlayerPoker(players[i]->GetId());
            for(auto begin = pokers.begin(); begin != pokers.end(); ++begin){
                resp_deal->add_pokers(*begin);
            }
            m_resps.push_back(std::make_pair(players[i]->GetId(), SerialMsg(resp_deal)));
            delete resp_deal;
        }
    }else{
        // 小于3个玩家，回复匹配中消息
        landlord::player_match* resp_match = new landlord::player_match();
        resp_match->set_s(landlord::player_match_state_MATCH);
        resp_match->set_player_id(player_id);
        resp_match->clear_player_ids();
        m_resps.push_back(std::make_pair(player_id, SerialMsg(resp_match)));
        delete resp_match;
    }
}

void LandlordHall::hall_quit(int player_id, ::google::protobuf::Message* msg)
{
    // do nothing
}

void LandlordHall::room_grab(int player_id, ::google::protobuf::Message* msg)
{
    LandlordRoom* room = m_rooms[m_players[player_id]->GetRoom()];
    landlord::room_grab* room_grab = static_cast<landlord::room_grab*>(msg);
    // 抢地主
    int ret = room->GrabLandlord(player_id, room_grab->grab());
    if(ret == -1){
        // error
        std::cout << "error grab player id: " << player_id << std::endl;
        return;
    }
    if(ret == 0 || ret == 1){
        // 广播下一个抢地主的玩家
        landlord::player_grab resp_grab;
        for(auto begin = room->GetPlayers().begin(); begin != room->GetPlayers().end(); ++begin){
            resp_grab.set_player_id((*begin)->GetId());
            resp_grab.set_turn(room->GetTurnId());
            resp_grab.set_s(landlord::player_grab_state_OK);
            
            m_resps.push_back(std::make_pair((*begin)->GetId(), SerialMsg(&resp_grab)));
        }
    }
    
    if(ret == 1){
        // 广播地主人选
        landlord::player_landlord resp_landlord;
        std::vector<int> last_pokers = room->TakeLastPokers(room->GetLandlord());
        for(auto begin = room->GetPlayers().begin(); begin != room->GetPlayers().end(); ++begin){
            resp_landlord.set_player_id((*begin)->GetId());
            resp_landlord.set_turn(room->GetTurnId());
            resp_landlord.set_landlord_id(room->GetLandlord());

            resp_landlord.clear_pokers();
            for(int j = 0; j < 3; ++j){
                resp_landlord.add_pokers(last_pokers[j]);
            }
            
            m_resps.push_back(std::make_pair((*begin)->GetId(), SerialMsg(&resp_landlord)));
        }
    }
    if(ret == 2){
        std::cout << "error grab: " << "no one grab landlord, *_*^" << std::endl;
    }
}

void LandlordHall::room_play(int player_id, ::google::protobuf::Message* msg)
{
    LandlordRoom* room = m_rooms[m_players[player_id]->GetRoom()];
    landlord::room_play* room_play = static_cast<landlord::room_play*>(msg);

    std::vector<int> play_pokers;
    play_pokers.clear();
    for(int i = 0; i < room_play->pokers_size(); ++i){
        play_pokers.push_back(room_play->pokers(i));
    }
    PLAY_RET ret = room->PlayPokers(player_id, play_pokers);
    landlord::player_play resp_play;
    // 轮到该玩家出牌，并且出牌不符合规则，则返回错误
    if(PLAY_RET::ERR == ret){
        std::cout << "player play err" << std::endl;
        return;
    }
    // 出牌成功, 则广播该玩家出的牌
    if(PLAY_RET::OK == ret){
        for(int i = 0; i < 3; ++i){
            resp_play.set_player_id(player_id);
            resp_play.set_turn(room->GetTurnId());

            resp_play.clear_pokers();
            for(int j = 0; j < play_pokers.size(); ++j){
                resp_play.add_pokers(play_pokers[j]);
            }
            m_resps.push_back(std::make_pair(room->GetPlayers()[i]->GetId(), SerialMsg(&resp_play)));
        }
    }
    int win_id = room->Win();
    // 如果该玩家没牌了, 则广播该玩家获得胜利
    if(win_id != -1){
        landlord::player_win resp_win;
        resp_win.set_player_id(player_id);
        resp_win.set_winner_id(player_id);
        for(int i = 0; i < 3; ++i){
            m_resps.push_back(std::make_pair(room->GetPlayers()[i]->GetId(), SerialMsg(&resp_win)));
        }
    }
}

void LandlordHall::RegFunc()
{
    m_funcs["landlord.hall_login"] = &LandlordHall::hall_login;
    m_funcs["landlord.hall_quit"] = &LandlordHall::hall_quit;
    m_funcs["landlord.hall_match"] = &LandlordHall::hall_match;
    m_funcs["landlord.room_grab"] = &LandlordHall::room_grab;
    m_funcs["landlord.room_play"] = &LandlordHall::room_play;
}