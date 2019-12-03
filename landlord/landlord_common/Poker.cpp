#include "Poker.h"

Poker::Poker()
{}

Poker::Poker(int id) :
	m_id(id)
{}

Poker::~Poker()
{}

bool Poker::operator<(Poker& t)
{
	if (GetWeight() < t.GetWeight())
		return true;
	return false;
}

bool Poker::operator==(Poker& t)
{
	if (GetValue() == t.GetValue())
		return true;
	return false;
}

int Poker::GetValue()
{
	int id = m_id;
	if (id == 52 || id == 53) return id;
	return (id % 13 + 1);
}

int Poker::GetType()
{
	int id = m_id;
	if (id == 52 || id == 53) return id;
	return (id / 13);
}

int Poker::GetWeight()
{
	int weight = 0;
	if (m_id == 52 || m_id == 53) return m_id;
	weight = (m_id % 13 + 1);
	weight = (weight == 1) ? 14 : weight;
	weight = (weight == 2) ? 15 : weight;
	return weight;
}

int Poker::GetId()
{
	return m_id;
}