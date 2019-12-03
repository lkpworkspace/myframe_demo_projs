#ifndef __PLAYPOKER_H__
#define __PLAYPOKER_H__
#include <vector>

#include "Poker.h"

enum class POKER_TYPE :int {
	UNKNOWN,          //0 未知
	ONE,              //1 单张
	PAIR,             //2 一对
	THREE,            //3 三张
	_31,              //4 三带一    4
	_32,              //5 三带二    5
	STRAIGHT,         //6 顺子      (3 ~ A) 5 ~ 12
	D_STRAIGHT,       //7 连对      (33 ~ AA) 6 ~ 20 偶数张
	T_STRAIGHT,       //8 三顺      (333 ~ AAA) 6 ~ 18 3的倍数
	T_STRAIGHT_WING,  //9 飞机带翅膀 8/10
	_42,              //10 四带二     6/8
	BOMB,             //11 炸弹      4
	KING_BOMB         //12 王炸      2
};

class PlayPoker
{
public:
	PlayPoker();
	PlayPoker(std::vector<int>& vals);
	virtual ~PlayPoker();

	/* 获得牌型 */
	POKER_TYPE GetType();
	/* 比较大小 */
	bool operator<(PlayPoker& other);
	/* 判断是否可以比较 */
	bool operator ==(PlayPoker& other);
	/* 赋值 */
	void operator=(const PlayPoker& other);
	/* 牌数 */
	int GetSize() { return m_pokers.size(); }
private:
	POKER_TYPE GetType4();
	POKER_TYPE GetType5();
	POKER_TYPE GetTypeN();

	bool IsStraight();
	bool IsDStraight();
	bool IsTStraight();
	bool IsTStraightWing();
	bool Is32();
	bool Is42();

	std::vector<Poker> m_pokers;
	POKER_TYPE m_type;
	int m_weight;
};

#endif
