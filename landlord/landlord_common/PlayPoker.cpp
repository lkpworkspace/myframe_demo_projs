#include "PlayPoker.h"

#include <algorithm>
#include <map>

PlayPoker::PlayPoker() :
	m_type(POKER_TYPE::UNKNOWN),
	m_weight(0)
{
	m_pokers.clear();
}

PlayPoker::PlayPoker(std::vector<int>& vals)
{
	m_pokers.clear();
	for (auto p = vals.begin(); p != vals.end(); ++p) {
		m_pokers.push_back(Poker(*p));
	}
	std::sort(m_pokers.begin(), m_pokers.end());
	GetType();
}

PlayPoker::~PlayPoker()
{}

POKER_TYPE PlayPoker::GetType()
{
	m_type = POKER_TYPE::UNKNOWN;
	int sz = m_pokers.size();
	// 玩家牌数 小于等于 20
	if (sz > 20) return m_type;
	switch (sz) {
	case 1:
		m_type = POKER_TYPE::ONE;
		m_weight = m_pokers[0].GetWeight();
		break;
	case 2: {
		// 一对 或者 王炸
		int vals[2] = { m_pokers[0].GetValue(), m_pokers[1].GetValue() };
		if ((vals[0] == 52 && vals[1] == 53) || (vals[1] == 52 && vals[0] == 53)) {
			m_type = POKER_TYPE::KING_BOMB;
			m_weight = 54;
		}
		if (vals[0] == vals[1]) {
			m_type = POKER_TYPE::PAIR;
			m_weight = m_pokers[0].GetWeight();
		}
		break;
	}
	case 3: {
		auto iter = std::unique(m_pokers.begin(), m_pokers.end());
		if (--iter == m_pokers.begin()) {
			m_type = POKER_TYPE::THREE;
			m_weight = m_pokers[0].GetWeight();
		}
		break;
	}
	case 4:
		m_type = GetType4();
		break;
	case 5:
		m_type = GetType5();
		break;
	default:
		m_type = GetTypeN();
		break;
	}
	return m_type;
}

POKER_TYPE PlayPoker::GetType4()
{
	POKER_TYPE type = POKER_TYPE::UNKNOWN;
	// 炸弹
	auto iter = std::unique(m_pokers.begin(), m_pokers.end());
	if (--iter == m_pokers.begin()) {
		type = POKER_TYPE::BOMB;
		m_weight = m_pokers[0].GetWeight();
		return type;
	}
	// 3带1
	if ((m_pokers[0] == m_pokers[1] &&
		m_pokers[0] == m_pokers[2])) {
		type = POKER_TYPE::_31;
		m_weight = m_pokers[0].GetWeight();
		return type;
	}
	if((m_pokers[1] == m_pokers[2] &&
		m_pokers[1] == m_pokers[3]))
	{
		type = POKER_TYPE::_31;
		m_weight = m_pokers[1].GetWeight();
		return type;
	}
	return type;
}

POKER_TYPE PlayPoker::GetType5()
{
	POKER_TYPE type = POKER_TYPE::UNKNOWN;
	// 3带2
	if (Is32()) {
		return POKER_TYPE::_32;
	}
	return type;
}

POKER_TYPE PlayPoker::GetTypeN()
{
	// size >= 5
	POKER_TYPE type = POKER_TYPE::UNKNOWN;
	int sz = (int)m_pokers.size();
	if (IsStraight()) {
		return POKER_TYPE::STRAIGHT;
	}
	if (sz == 6) {
		if (Is42()) {
			return POKER_TYPE::_42;
		}
	}
	if (sz == 8) {
		if (IsTStraightWing()) {
			return POKER_TYPE::T_STRAIGHT_WING;
		}
		if (Is42()) {
			return POKER_TYPE::_42;
		}
	}
	if (sz == 10) {
		if (IsTStraightWing()) {
			return POKER_TYPE::T_STRAIGHT_WING;
		}
	}
	if(sz % 2 == 0 && sz >= 6)
		if (IsDStraight()) {
			return POKER_TYPE::D_STRAIGHT;
		}
	if(sz % 3 == 0 && sz >= 6)
		if (IsTStraight()) {
			return POKER_TYPE::T_STRAIGHT;
		}
	return type;
}

bool PlayPoker::IsStraight()
{
	int sz = m_pokers.size();
	if (sz < 5) return false;
	for (int i = 0; i < sz - 1; ++i) {
		if ((m_pokers[i + 1].GetWeight() - m_pokers[i].GetWeight()) != 1) {
			return false;
		}
	}
	m_weight = m_pokers[0].GetWeight();
	return true;
}

bool PlayPoker::IsDStraight()
{
	bool ret = true;
	int sz = m_pokers.size();
	for (int i = 0; i < sz - 2; i += 2) {
		if (m_pokers[i + 1].GetWeight() == m_pokers[i].GetWeight() &&
			((m_pokers[i + 2].GetWeight() - m_pokers[i + 1].GetWeight()) == 1)) {
			ret = true;
		}
		else {
			return false;
		}
	}
	if (m_pokers[sz - 1].GetWeight() != m_pokers[sz - 2].GetWeight())
		ret = false;
	m_weight = m_pokers[0].GetWeight();
	return ret;
}

bool PlayPoker::IsTStraight()
{
	bool ret = true;
	int sz = m_pokers.size();
	for (int i = 0; i < sz - 3; i += 3) {
		if (m_pokers[i + 1].GetWeight() == m_pokers[i].GetWeight() &&
			m_pokers[i + 2].GetWeight() == m_pokers[i].GetWeight() &&
			((m_pokers[i + 3].GetWeight() - m_pokers[i + 1].GetWeight()) == 1)) {
			ret = true;
		}
		else {
			return false;
		}
	}
	if (m_pokers[sz - 1].GetWeight() != m_pokers[sz - 2].GetWeight() ||
		m_pokers[sz - 1].GetWeight() != m_pokers[sz - 3].GetWeight())
		ret = false;
	m_weight = m_pokers[0].GetWeight();
	return ret;
}

bool PlayPoker::IsTStraightWing()
{
	bool ret = false;
	int sz = m_pokers.size();
	int airplane[2] = { 0, 0 };
	int tmp = 0;
	std::map<int, int> weights;
	// 飞机带两个单张 8
	if (sz == 8 || sz == 10) {
		for (int i = 0; i < sz; ++i) {
			int weight = m_pokers[i].GetWeight();
			if (weights.find(weight) == weights.end()) {
				weights[weight] = 1;
			}
			else {
				weights[weight] += 1;
			}
		}
	}
	if (sz == 8) {
		for (auto iter = weights.begin(); iter != weights.end(); iter++) {
			if (iter->second == 3) {
				airplane[tmp] = iter->first;
				tmp++;
			}
		}
		if (tmp == 2 && abs(airplane[0] - airplane[1]) == 1) {
			m_weight = airplane[0] > airplane[1] ? airplane[1] : airplane[0];
			ret = true;
		}
	}
	// 飞机带两对 10
	if (sz == 10) {
		int wings[2];
		int tmp2 = 0;
		for (auto iter = weights.begin(); iter != weights.end(); iter++) {
			if (iter->second == 3) {
				airplane[tmp] = iter->first;
				tmp++;
			}
			if (iter->second == 2) {
				wings[tmp2] = iter->first;
				tmp2++;
			}
		}
		if (tmp == 2 && abs(airplane[0] - airplane[1]) == 1) {
			if(tmp2 == 2){
				m_weight = airplane[0] > airplane[1] ? airplane[1] : airplane[0];
				ret = true;
			}
		}
	}
	return ret;
}

bool PlayPoker::Is32()
{
	bool ret = false;
	int sz = m_pokers.size();
	int three = 0;
	int two = 0;

	std::map<int, int> weights;
	// 3带一对
	for (int i = 0; i < sz; ++i) {
		int weight = m_pokers[i].GetWeight();
		if (weights.find(weight) == weights.end()) {
			weights[weight] = 1;
		}
		else {
			weights[weight] += 1;
		}
	}
	for (auto iter = weights.begin(); iter != weights.end(); iter++) {
		if (iter->second == 3) {
			three = iter->first;
		}
		if (iter->second == 2) {
			two = iter->first;
		}
	}
	if (three != 0 && two != 0) {
		m_weight = three;
		ret = true;
	}
	
	return ret;
}

bool PlayPoker::Is42()
{
	bool ret = false;

	int sz = m_pokers.size();
	int d[2] = { 0, 0 };
	int tmp = 0;
	std::map<int, int> weights;

	// 飞机带两个单张 8
	if (sz == 6 || sz == 8) {
		for (int i = 0; i < sz; ++i) {
			int weight = m_pokers[i].GetWeight();
			if (weights.find(weight) == weights.end()) {
				weights[weight] = 1;
			}
			else {
				weights[weight] += 1;
			}
		}
	}
	// 4带2个单张，6
	if (sz == 6) {
		for (auto iter = weights.begin(); iter != weights.end(); iter++) {
			if (iter->second == 4) {
				m_weight = iter->first;
				ret = true;
			}
		}
	}
	// 4带2对，8
	if (sz == 8) {
		int tmp2 = 0;
		for (auto iter = weights.begin(); iter != weights.end(); iter++) {
			if (iter->second == 4) {
				m_weight = iter->first;
			}
			if (iter->second == 2) {
				tmp2++;
			}
		}
		if (tmp2 == 2) {
			ret = true;
		}
	}
	return ret;
}

bool PlayPoker::operator<(PlayPoker& other)
{
	if (other.m_type == POKER_TYPE::KING_BOMB)
		return true;
	if (m_type == POKER_TYPE::KING_BOMB)
		return false;
	if (other.m_type == POKER_TYPE::BOMB && m_type != POKER_TYPE::BOMB)
		return true;
	if (other.m_type != POKER_TYPE::BOMB && m_type == POKER_TYPE::BOMB)
		return false;
	return m_weight < other.m_weight;
}

bool PlayPoker::operator ==(PlayPoker& other) 
{
	if (m_type == POKER_TYPE::KING_BOMB || other.m_type == POKER_TYPE::KING_BOMB)
		return true;
	if (m_type == POKER_TYPE::BOMB || other.m_type == POKER_TYPE::BOMB)
		return true;
	if (m_type == other.m_type &&
		other.m_pokers.size() == m_pokers.size()) {
		return true;
	}
	return false;
}

void PlayPoker::operator=(const PlayPoker& other)
{
	m_type = other.m_type;
	m_weight = other.m_weight;
	int sz = other.m_pokers.size();
	m_pokers.clear();
	for (int i = 0; i < sz; ++i) {
		m_pokers.push_back(other.m_pokers[i]);
	}
}