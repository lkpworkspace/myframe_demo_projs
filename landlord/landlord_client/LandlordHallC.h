#ifndef __LandlordHall_H__
#define __LandlordHall_H__
#include <string>
#include <google/protobuf/message.h>
#include "ILandlordHall.h"
#include "PlayerC.h"
#include "LandlordRoomC.h"
#include "imgui.h"
#include "MyContext.h"
#include "LandlordUtils.h"

class LandlordHallC : public ILandlordHall, public LandlordUtils
{
public:
	LandlordHallC(MyModule* c) :
		ILandlordHall(),
		LandlordUtils(c),
		m_room(c),
		m_assign_player_id(0)
	{}
	virtual ~LandlordHallC(){}

	void Parse(int player_id, std::string& dat);

	void UIUpdate();

private:
	void CratePlayer();

	void player_login(int player_id, ::google::protobuf::Message* msg);
	void player_match(int player_id, ::google::protobuf::Message* msg);
	void player_deal(int player_id, ::google::protobuf::Message* msg);
	void player_grab(int player_id, ::google::protobuf::Message* msg);
	void player_landlord(int player_id, ::google::protobuf::Message* msg);
	void player_play(int player_id, ::google::protobuf::Message* msg);
	void player_win(int player_id, ::google::protobuf::Message* msg);

	int m_assign_player_id;
	LandlordRoomC m_room;
	std::unordered_map<int, PlayerC*> m_players;
	struct ImGuiTextBuffer m_log;
	MyContext* m_c;
};

#endif