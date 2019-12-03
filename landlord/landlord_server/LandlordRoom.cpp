#include "LandlordRoom.h"
#include "landlord.pb.h"

#include <random>
#include <iostream>
#include <algorithm>

LandlordRoom::LandlordRoom() :
	m_landlord(-1),
	m_clear_poker(-1),
	m_state(PLAY_STATE::BEGIN),
	m_turn(-1),
	m_pass_cnt(0),
	m_grab_cnt(0)
{
	m_pokers.clear();
}

LandlordRoom::~LandlordRoom()
{}

void LandlordRoom::CreatePlayer(std::vector<Player*>& players)
{
	int i = 0;
	for (auto p = players.begin(); p != players.end(); ++p) {
		(*p)->SetIdx(i);
		m_players.push_back(*p);
		i++;
	}
	m_state = PLAY_STATE::CREATE;
}

std::vector<int> LandlordRoom::ShufflePoker()
{
	std::default_random_engine dre;
	std::random_device rd{};
	std::seed_seq seeds{ rd(), rd() };
	dre.seed(seeds);
	m_pokers.clear();
	for (int i = 0; i < 54; ++i) {
		m_pokers.push_back(i);
	}
	// 洗牌
	std::shuffle(m_pokers.begin(), m_pokers.end(), dre);
	// 设置一张明牌
	std::uniform_int_distribution<int> di(1, 50);
	m_clear_poker = di(dre);
	// 算出地主是谁(或者由谁先抢地主)
	m_landlord = m_clear_poker % 3;
	m_turn = m_landlord;
	// 最后3张牌
	for (int i = 0; i < 3; ++i) {
		m_last_three_poker[i] = m_pokers[m_pokers.size() -  (2 - i) - 1];
	}
	return m_pokers;
}

int LandlordRoom::DispatchPoker()
{
	std::vector<int> pokers = ShufflePoker();
	int size = pokers.size();
	int i = 0, j = 0;
	for (i = 0; i < (size - 3); i += 3) {
		m_players[0]->TakePoker(pokers[i]);
		m_players[1]->TakePoker(pokers[i + 1]);
		m_players[2]->TakePoker(pokers[i + 2]);
	}
	m_state = PLAY_STATE::DISPATCH;
	return m_players[m_landlord]->GetId();
}

// -1: err, 0: grab, 1: landlord, 2: 重新发牌
int LandlordRoom::GrabLandlord(int id, bool b)
{
	// 从随机某个人开始叫地主，按顺序叫，如果叫了，则地主就归该人； 如果都没叫， 则重新发牌。
	#if 1
	m_state = PLAY_STATE::GRAB;
	if(b && (m_players[m_turn]->GetId() == id)){
		m_landlord = m_turn;
		m_players[m_turn]->SetLandlord();
		return 1;
	}
	if(!b && (m_players[m_turn]->GetId() == id)){
		if(m_grab_cnt && m_turn == m_landlord){
			// 重新发牌
			// TODO...
			return 2;
		}else{
			m_grab_cnt++;
			m_turn = (m_turn + 1) % 3;
			return 0;
		}
	}
	return -1;
	#else
	for (int i = 0; i < m_players.size(); ++i) {
		if (m_players[i]->GetId() == id) {
			if(false == b){
				m_turn = (m_turn + 1) % 3;
				if(m_grab_cnt == 2) return -2;
				m_grab_cnt++;
				break;
			}
			if(m_turn != i || m_grab_cnt > 2) return -1;
			m_landlord = i;
            m_turn = (m_turn + 1) % 3;
            if(m_grab_cnt == 2){
                m_turn = m_players[i]->GetId();
                m_players[i]->SetLandlord();
                return -2;
            }
            m_grab_cnt++;
			break;
		}
	}
	m_state = PLAY_STATE::GRAB;
	return id;
	#endif
}

std::vector<int> LandlordRoom::TakeLastPokers(int id)
{
	if (m_players[m_landlord]->GetId() != id) return std::vector<int>();

	int i = 0;
	std::vector<int> lp;
	for (i = 0; i < 3; ++i) {
		m_players[m_landlord]->TakePoker(m_last_three_poker[i]);
		lp.push_back(m_last_three_poker[i]);
	}
	m_state = PLAY_STATE::PLAY;
	return lp;
}

int LandlordRoom::Win()
{
	int sz = (int)m_players.size();
	for (int i = 0; i < sz; ++i) {
		if (m_players[i]->IsEmpty()) {
			m_state = PLAY_STATE::END;
			return m_players[i]->GetId();
		}
	}
	return -1;
}

int LandlordRoom::GetIdx(int id)
{
	for (int i = 0; i < m_players.size(); ++i) {
		if (m_players[i]->GetId() == id) return i;
	}
	return -1;
}

/**
	客户端发起出牌请求，服务端判断出牌是否符合规则，并出牌
	返回值：
		OK
		ERR
			- 错误原因。。。
		WIN
*/
PLAY_RET LandlordRoom::PlayPokers(int id, std::vector<int> vals)
{
	PLAY_RET ret = PLAY_RET::ERR;
	int idx = -1;

	if (m_state != PLAY_STATE::PLAY) return ret;
	if ((idx = GetIdx(id)) == -1) return ret;
	if (idx != m_turn) return ret;

	// 判断是否为空
	//	- 如果为空，并且状态是刚开始，返回ERR
	//	- 如果为空，并且状态是打牌，并且没有经过一轮，表示不出，轮到下一位，pass加1，返回OK
	//	- 如果为空，并且状态是打牌，并且经过一轮，则返回ERR
	//	- 如果不为空，继续往下走
	if (vals.size() == 0 && m_play_poker.GetSize() == 0) {
		return ret;
	}
	if (vals.size() == 0 && m_pass_cnt < 2) {
		m_turn = (m_turn + 1) % 3;
		m_pass_cnt++;
		return PLAY_RET::OK;
	}

	// 判断是否符合出牌规则
	// 符合则继续，否则返回ERR
	PlayPoker pp(vals);
	if (pp.GetType() == POKER_TYPE::UNKNOWN) return ret;

	// 判断上次出的牌是否为空
	//	- 如果为空，则出牌，记录牌，轮到下一位，状态设置为出牌状态
	//	- 如果不为空, 并且经过一轮，则出牌, 轮到下一位出牌
	//	- 如果不为空, 并且没有经过一轮，则判断是否可以与上次出的牌做比较
	//		- 如果不可以, 返回ERR
	//		- 如果可以， 比较大小，大则记录牌，轮到下一位， 返回OK; 如果小，则返回ERR
	if (m_play_poker.GetSize() == 0) {
		m_players[idx]->PlayPoker(vals);
		m_play_poker = pp;
		m_turn = (m_turn + 1) % 3;
		m_pass_cnt = 0;
		return PLAY_RET::OK;
	}
	if (m_play_poker.GetSize() > 0 && m_pass_cnt >= 2) {
		m_players[idx]->PlayPoker(vals);
		m_play_poker = pp;
		m_turn = (m_turn + 1) % 3;
		m_pass_cnt = 0;
		return PLAY_RET::OK;
	}
	if (m_play_poker.GetSize() > 0 && m_pass_cnt < 2) {
		if (m_play_poker == pp && m_play_poker < pp) {
			m_players[idx]->PlayPoker(vals);
			m_play_poker = pp;
			m_turn = (m_turn + 1) % 3;
			m_pass_cnt = 0;
			return PLAY_RET::OK;
		}
		else {
			return ret;
		}
	}
	return PLAY_RET::OK;
}

std::vector<int> LandlordRoom::GetPlayerPoker(int id)
{
	int idx = GetIdx(id);
	if (idx == -1) return std::vector<int>();
	return m_players[idx]->GetPoker();
}

int LandlordRoom::GetTurn()
{
	return m_turn;
}

int LandlordRoom::GetTurnId()
{
    return m_players[m_turn]->GetId();
}
