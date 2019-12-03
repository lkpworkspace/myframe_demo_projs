#include <iostream>
#include "LandlordHallC.h"
#include "landlord.pb.h"
#include "MyMsg.h"

void LandlordHallC::player_login(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::player_login * l = static_cast<landlord::player_login*>(msg);
    m_log.appendf("[player %d] login\n", l->player_id());
    m_log.append("\n");
}
void LandlordHallC::player_match(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::player_match * l = static_cast<landlord::player_match*>(msg);

    m_log.appendf("[player %d] match state: %s\n", l->player_id(), 
        (l->s() == landlord::player_match_state_MATCH) ? "matching" : "playing");
    if(l->player_ids_size() > 0){
        for(int i = 0; i < l->player_ids_size(); ++i){
            m_log.appendf("[player %d] match's player: %d\n", l->player_id(), l->player_ids(i));
        }
        m_log.append("\n");
    }else{
        m_log.append("\n");
    }
}
void LandlordHallC::player_deal(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::player_deal * l = static_cast<landlord::player_deal*>(msg);

    if(l->pokers_size() > 0){
        for(int i = 0; i < l->pokers_size(); ++i){
            m_log.appendf("[player %d] get poker: %d\n", l->player_id(), l->pokers(i));
            m_players[player_id]->TakePoker(l->pokers(i));
        }
    }
    m_log.appendf("[player %d] trun to: player %d\n", l->player_id(), l->turn());
    m_log.append("\n");

    
}
void LandlordHallC::player_grab(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::player_grab * l = static_cast<landlord::player_grab*>(msg);

    m_log.appendf("[player %d] grab: %s, turn to player %d\n", l->player_id(), 
        (l->s() == landlord::player_grab_state_OK) ? "OK" : "ERR", l->turn());
    m_log.append("\n");
}
void LandlordHallC::player_landlord(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::player_landlord * l = static_cast<landlord::player_landlord*>(msg);

    m_log.appendf("[player %d] player %d is landlord\n", l->player_id(), l->landlord_id());
    if(l->pokers_size() > 0){
        for(int i = 0; i < l->pokers_size(); ++i){
            m_log.appendf("[player %d] landlord %d get poker: %d\n", l->player_id(), l->landlord_id(), l->pokers(i));
            if(player_id == l->landlord_id()){
                m_players[player_id]->TakePoker(l->pokers(i));
            }
        }
    }
    m_log.appendf("[player %d] trun player %d play\n", l->player_id(), l->turn());
    m_log.append("\n");
}
void LandlordHallC::player_play(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::player_play * l = static_cast<landlord::player_play*>(msg);

    if(l->pokers_size() > 0){
        for(int i = 0; i < l->pokers_size(); ++i){
            m_log.appendf("[player %d] play poker: %d\n", l->player_id(), l->pokers(i));
            if(player_id == l->player_id()){
                m_players[player_id]->PlayPoker(std::vector<int>{l->pokers(i)});
            }
        }
    }
    m_log.appendf("[player %d] trun player %d play\n", l->player_id(), l->turn());
    m_log.append("\n");
}
void LandlordHallC::player_win(int player_id, ::google::protobuf::Message* msg)
{
    using namespace ::google::protobuf;
    landlord::player_win * l = static_cast<landlord::player_win*>(msg);
    if(player_id == l->winner_id()){
        m_players[player_id]->SetWin();
    }
    m_log.appendf("[player %d] player %d win\n", player_id, l->winner_id());
    m_log.append("\n");
}

void LandlordHallC::Parse(int player_id, std::string& dat)
{
    using namespace ::google::protobuf;
    Message* m = DeserialMsg(dat);
    m_log.appendf("[player %d] get msg: %s\n", player_id, m->GetTypeName().c_str());
    if(m->GetTypeName() == "landlord.player_login"){
        player_login(player_id, m);
    }
    if(m->GetTypeName() == "landlord.player_match"){
        player_match(player_id, m);
    }
    if(m->GetTypeName() == "landlord.player_deal"){
        player_deal(player_id, m);
    }
    if(m->GetTypeName() == "landlord.player_grab"){
        player_grab(player_id, m);
    }
    if(m->GetTypeName() == "landlord.player_landlord"){
        player_landlord(player_id, m);
    }
    if(m->GetTypeName() == "landlord.player_play"){
        player_play(player_id, m);
    }
    if(m->GetTypeName() == "landlord.player_win"){
        player_win(player_id, m);
    }
    delete m;
}

void LandlordHallC::CratePlayer()
{
    int id = m_assign_player_id++;
    PlayerC* p = new PlayerC(GetModule(), id, id);
    m_players[id] = p;

    // tell landlord server
    using namespace ::google::protobuf;
    landlord::hall_login login;
    login.set_player_name(std::to_string(id));
    login.set_player_id(id);
    Send(id, &login);
}

void LandlordHallC::UIUpdate()
{
    int counter = 0;
    ImGui::Begin("Landlord Hall");

    if (ImGui::Button("Create player")){
        CratePlayer();
    }
        
    ImGui::SameLine();
    if (ImGui::Button("Clear Log"))
        m_log.clear();

    ImGui::SameLine();
    if (ImGui::Button("Test Serial")){
        using namespace ::google::protobuf;
        using namespace landlord;
        hall_login login;
        login.set_player_id(0);
        login.set_player_name("lkp");
        std::string d = SerialMsg(&login);
        Message* m = DeserialMsg(d);
        hall_login* l = static_cast<hall_login*>(m);
        m_log.append(m->GetTypeName().c_str(), NULL);
        m_log.append(l->player_name().c_str(), NULL);
        m_log.appendf("id : %d\n", l->player_id());
    }

    ImGui::BeginChild("Log");
    ImGui::TextUnformatted(m_log.begin(), m_log.end());
    ImGui::EndChild();

    ImGui::End();

    m_room.UIUpdate();

    for(auto begin = m_players.begin(); begin != m_players.end(); ++begin){
        (*begin).second->UIUpdate();
    }
}