#pragma once
#ifndef _SOCCER_SCENE_WELCOME_H_
#define _SOCCER_SCENE_WELCOME_H_

#include "./i_scene.h"
#include <gui/dlg_network.h>

namespace soc {

class SceneWelcome : public IScene
{
	typedef IScene Parent;

public:
	explicit SceneWelcome(lf::u8 id);
	virtual ~SceneWelcome();

	virtual void create(lf::render::IRenderWindow *win);
	virtual void destroy();

	virtual void onWake();
	virtual void onSleep();
	virtual lf::u8 update(lf::u32 elapse);
	virtual bool onEvent(lf::s32 type, lf::s32 state);

	virtual void performAction(lf::gui::CGUIActionEvent &evt);

protected:
	void setupGUI();
	void setupScene();

private:
	lf::gui::CGUIManager		*pGui__;
	lf::render::CRenderLayer3D	*p3D__;

	DlgNetwork					mDlgNet__;
};

} // namespace soc

#endif // _SOCCER_SCENE_WELCOME_H_