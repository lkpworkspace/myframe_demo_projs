#ifndef __PLAYER_H__
#define __PLAYER_H__
#include <unordered_map>
#include <vector>

#include "Poker.h"
#include "IPlayer.h"
#include "LandlordUtils.h"

class PlayerC : public IPlayer, public LandlordUtils
{
public:
	PlayerC(MyModule* c, int idx, int id);
	virtual ~PlayerC();

	void UIUpdate();

	void SetWin() { m_win = true; }
private:
	// 请求
	void Match();
	void Grab(bool b);
	void Play();
	void Quit();

	std::string m_window_name;
	// first: poker id, second: play state
	std::vector<int> m_play;
	bool m_checked[20];
	bool m_grab;
	bool m_win;
};

#endif
