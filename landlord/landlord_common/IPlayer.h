#ifndef __IPLAYER_H__
#define __IPLAYER_H__
#include <unordered_map>
#include <vector>

#include "Poker.h"

class IPlayer 
{
public:
	IPlayer(int idx, int id);
	virtual ~IPlayer();

	// 起牌
	void TakePoker(int id);
	// 打牌
	bool PlayPoker(std::vector<int> vals);
	// 打完
	bool IsEmpty() { return m_pokers.empty(); }
	// 获得牌
	std::vector<int>& GetPoker();
	// 获得牌的数量
	int PokerSize() { return m_pokers.size(); }
	// 清空牌
	void ClearPokers() { m_pokers.clear(); }

	int GetIdx() { return m_idx; }
	void SetIdx(int idx) { m_idx = idx; }
	int GetId() { return m_id; }

	int GetRoom() { return m_room; }
	void SetRoom(int room) { m_room = room; }

	bool IsLandlord() { return m_landlord; }
	void SetLandlord(bool b = true) { m_landlord = b; }
private:
	std::unordered_map<int, Poker> m_pokers;
	std::vector<int> m_tmp_poksers;
	bool m_landlord;
	int m_id;
	int m_idx;
	int m_room;
};

#endif
