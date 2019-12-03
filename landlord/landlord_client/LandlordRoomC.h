#ifndef __LANDLORDROOM_H__
#define __LANDLORDROOM_H__
#include <vector>
#include "ILandlordRoom.h"
#include "LandlordUtils.h"

class LandlordRoomC : public ILandlordRoom, public LandlordUtils
{
public:
	LandlordRoomC(MyContext* c);
	virtual ~LandlordRoomC();

//	void player_match();

	void UIUpdate();

private:
	//std::unordered_map<int, PlayerC*> m_players;
};


#endif
