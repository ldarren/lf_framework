#pragma once
#ifndef _I_SCENE_H_
#define _I_SCENE_H_

#include "./scene_id.h"

#pragma warning (push)
#pragma warning (disable: 4251)

#include <lf/Lightfeather.h>

#pragma warning (pop)

namespace soc {

struct IScene : public lf::input::IKeyListener, public lf::gui::IGUIActionListener, public lf::gui::IGUISelectionListener
{
	virtual void create(lf::render::IRenderWindow *win);
	virtual void destroy();

	virtual void onWake();
	virtual void onSleep();
	virtual lf::u8 update(lf::u32 elapse) = 0;
	virtual bool onEvent(lf::s32 type, lf::s32 state);

	void setId(lf::u8 id);
	lf::u8 getId() const;
	void changeScene(lf::u8 id);

protected:
	lf::u8	nId_;
	lf::u8	nNewId_;

	lf::render::IRenderWindow	*pWin_;
	lf::CResourceManager		*pRcsMgr_;
};

} // namespace soc

#endif // _I_SCENE_H_