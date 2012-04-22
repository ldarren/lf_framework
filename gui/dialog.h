#pragma once
#ifndef _SOC_GUI_DIALOG_H_
#define _SOC_GUI_DIALOG_H_

#pragma warning (push)
#pragma warning (disable: 4251)

#include <scene/i_scene.h>
#include <lf/Lightfeather.h>

#pragma warning (pop)

namespace soc {

struct Dialog : public lf::input::IKeyListener, public lf::gui::IGUIActionListener, public lf::gui::IGUISelectionListener
{
	Dialog();
	virtual ~Dialog();

	virtual void init(IScene *scene, lf::gui::CGUIWidget *root);
	virtual void deinit();

protected:
	lf::gui::CGUIWidget		*pRoot_;
	IScene					*pScene_;
	lf::CResourceManager	*pRcsMgr_;
};

} // namespace soc

#endif // _SOC_GUI_DIALOG_H_