#ifndef __LANDLORDROOM_H__
#define __LANDLORDROOM_H__
#include <vector>

#include "Poker.h"
#include "Player.h"
#include "PlayPoker.h"
#include "ILandlordRoom.h"

enum class PLAY_STATE :int {
	BEGIN,            // 刚开始
	CREATE,           // 创建玩家
	DISPATCH,         // 发牌
	GRAB,             // 抢地主
	PLAY,             // 打牌
	END               // 结束
};

enum class PLAY_RET :int {
	OK,               
	ERR,              
	WIN,              
};

class LandlordRoom : public ILandlordRoom
{
	friend class Player;
public:
	LandlordRoom();
	virtual ~LandlordRoom();

	// 创建玩家
	void CreatePlayer(std::vector<Player*>& players);
	// 获得玩家
	std::vector<Player*>& GetPlayers(){ return m_players; }
	// 发牌
	int DispatchPoker();
	// 抢地主
	int GrabLandlord(int id, bool b);
	// 获得底牌
	std::vector<int> TakeLastPokers(int id);
	// 出牌
	PLAY_RET PlayPokers(int id, std::vector<int> vals);
	// 谁赢了
	int Win();
	// 获得玩家的牌
	std::vector<int> GetPlayerPoker(int id);
	// 该谁出牌
	int GetTurn();
    int GetTurnId();
	// 获得当前状态
	PLAY_STATE GetState() { return m_state; }
	// 获得当前谁是地主
	int GetLandlord() { return m_players[m_landlord]->GetId(); }
protected:
	// 洗牌
	std::vector<int> ShufflePoker();
	int GetIdx(int id);
private:
	std::vector<Player*> m_players;
    /* 谁是地主(数组下标) */
	int m_landlord;
	/* 抢地主次数 */
	int m_grab_cnt;
	/* 最后3张牌 */
	int m_last_three_poker[3] = { -1,-1,-1 };
	/* 明牌下标 */
	int m_clear_poker;
	/* 洗牌数组 */
	std::vector<int> m_pokers;

	/* 当前状态 */
	PLAY_STATE m_state;
    /* 轮到谁出牌(数组下标) */
	int m_turn;
	/* 上次出的牌 */
	PlayPoker m_play_poker;
	/* pass的次数 */
	int m_pass_cnt;
	
};


#endif
