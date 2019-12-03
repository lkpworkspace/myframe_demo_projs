#include "PlayerC.h"
#include "imgui.h"
#include <google/protobuf/message.h>
#include "landlord.pb.h"

PlayerC::PlayerC(MyContext* c, int idx, int id) :
	IPlayer(idx, id),
	LandlordUtils(c),
	m_grab(false)
{
	m_window_name = "Landlord ";
	m_window_name += std::to_string(id);
	m_window_name += " Player";
	memset(m_checked, 0, sizeof(m_checked));
}

PlayerC::~PlayerC()
{}

void PlayerC::Match()
{
	// tell landlord server
	using namespace ::google::protobuf;
	landlord::hall_match match;
	match.set_player_id(GetId());
	Send(GetId(), &match);
}

void PlayerC::Grab(bool b)
{
	// tell landlord server
	using namespace ::google::protobuf;
	landlord::room_grab grab;
	grab.set_player_id(GetId());
	grab.set_grab(b);
	Send(GetId(), &grab);
}

void PlayerC::Play()
{
	memset(m_checked, 0, sizeof(m_checked));
	// tell landlord server
	using namespace ::google::protobuf;
	landlord::room_play play;
	play.set_player_id(GetId());
	play.clear_pokers();
	for(int i = 0; i < m_play.size(); ++i){
		play.add_pokers(m_play[i]);
	}
	Send(GetId(), &play);
}

void PlayerC::Quit()
{

}

void PlayerC::UIUpdate()
{
	ImGui::Begin(m_window_name.c_str());
	if (ImGui::Button("Match")){
        Match();
    }ImGui::SameLine();
	ImGui::Checkbox("",&m_grab); ImGui::SameLine();
	if (ImGui::Button("Grab")){
        Grab(m_grab);
    }ImGui::SameLine();
	if (ImGui::Button("Play")){
        Play();
    }ImGui::SameLine();
	if (ImGui::Button("Quit")){
        Quit();
    }

	// show poker
	std::vector<int>& pokers = GetPoker();
	m_play.clear();
	for(int i = 0; i < pokers.size(); ++i){
		std::string info = "poker ";
		Poker p(pokers[i]);
		info += std::to_string(p.GetValue());
		info += " (id: ";
		info += std::to_string(p.GetId());
		info += ")";
		ImGui::Checkbox(info.c_str(), &m_checked[i]);
		if(m_checked[i]) m_play.push_back(pokers[i]);
	}

	if(m_win){
		ImGui::Text("You Win ^_^");
	}
    ImGui::End();
}