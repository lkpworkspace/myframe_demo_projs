#include "IPlayer.h"

IPlayer::IPlayer(int idx, int id) :
	m_landlord(false),
	m_id(id),
	m_idx(idx)
{}

IPlayer::~IPlayer()
{}

void IPlayer::TakePoker(int id)
{
	m_pokers[id] = Poker(id);
}

bool IPlayer::PlayPoker(std::vector<int> vals)
{
	for (auto p = vals.begin(); p != vals.end(); ++p) {
		if (m_pokers.find(*p) == m_pokers.end()) return false;
	}
	for (auto p = vals.begin(); p != vals.end(); ++p) {
		m_pokers.erase(*p);
	}
	return true;
}

std::vector<int>& IPlayer::GetPoker()
{
	m_tmp_poksers.clear();
	for (auto iter = m_pokers.begin(); iter != m_pokers.end(); iter++) {
		m_tmp_poksers.push_back(iter->first);
	}
	return m_tmp_poksers;
}