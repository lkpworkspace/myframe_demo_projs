#ifndef __PLAYER_H__
#define __PLAYER_H__
#include <unordered_map>
#include <vector>

#include "Poker.h"
#include "IPlayer.h"

class Player : public IPlayer
{
public:
	Player(int idx, int id);
	virtual ~Player();
};

#endif
