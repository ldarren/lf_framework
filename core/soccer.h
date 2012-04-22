#pragma once
#ifndef _SOCCER_H_
#define _SOCCER_H_

#include <scene/scene_welcome.h>
#include <scene/scene_lobby.h>
#include <scene/scene_game.h>

#pragma warning (push)
#pragma warning (disable: 4251)

#include <lf/Lightfeather.h>

#pragma warning (pop)

namespace soc {

class Soccer : public lf::input::IKeyListener, lf::render::IRenderWindowListener
{
public:
	Soccer();
	~Soccer();

	void run();

	virtual void keyPressed(lf::input::CKeyEvent &evt);
	virtual void windowClosed(lf::render::CRenderWindowEvent& evt);

private:
	SceneWelcome				mSceneWelcome__;
	SceneLobby					mSceneLobby__;
	SceneGame					mSceneGame__;
	IScene						*pCurrScene__;

	lf::render::IRenderWindow	*pWin__;
	lf::CResourceManager		*pRcsMgr__;
};

} // namespace soc

#endif // _SOCCER_H_