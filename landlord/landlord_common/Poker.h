#ifndef __POKER_H__
#define __POKER_H__

/**
 *	黑桃(spade)   黑(1 ~ 13)
 *		0 ~ 12
 *	红心(heart)   红(1 ~ 13)
 *		13 ~ 25
 *	方块(dimond)  红(1 ~ 13)
 *		26 ~ 38
 *	梅花(club)    黑(1 ~ 13)
 *		39 ~ 51
 *	小王 52
 *	大王 53
 */

class Poker
{
public:
	Poker();
	Poker(int id);
	virtual ~Poker();

	int GetValue();
	int GetWeight();
	int GetType();
	int GetId();

	bool operator<(Poker& t);
	bool operator==(Poker& t);
private:
	int m_id;
};

#endif
