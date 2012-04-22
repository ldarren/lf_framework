#pragma once
#ifndef _SOCCER_SCENE_LOBBY_H_
#define _SOCCER_SCENE_LOBBY_H_

#include "./i_scene.h"

namespace soc {

class SceneLobby : public IScene
{
	typedef IScene Parent;

public:
	explicit SceneLobby(lf::u8 id);
	virtual ~SceneLobby();

	virtual void create(lf::render::IRenderWindow *win);
	virtual void destroy();

	virtual void onWake();
	virtual void onSleep();
	virtual lf::u8 update(lf::u32 elapse);

	virtual void keyPressed(lf::input::CKeyEvent &evt);
	virtual void performAction(lf::gui::CGUIActionEvent &evt);
	virtual void selectionChanged(lf::gui::CGUISelectionEvent &evt);

protected:
	void setupGUI();
	void setupScene();
	void setupPostEffect();
	void resetPostEffect();

private:
	lf::gui::CGUIManager		*pGui__;
	lf::render::CRenderLayer3D	*p3D__;

	lf::render::CPostProcessor	*pPostEfx__[8];

	lf::s32						nSelectedPP__;
	lf::s32						nOldPP__;
	lf::gui::CGUIComboBox		*pComboBox__;
	lf::scene::C3DCamera		*pCam__;
};

} // namespace soc

#endif // _SOCCER_SCENE_LOBBY_H_