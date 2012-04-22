#include "./dialog.h"

using namespace soc;

Dialog::Dialog()
:	pRoot_(0), pRcsMgr_(0)
{
}

Dialog::~Dialog()
{
}

void Dialog::init(IScene *scene, lf::gui::CGUIWidget *root)
{
	pScene_ = scene;
	pRoot_ = root;
	pRcsMgr_ = lf::CResourceManager::getInstancePtr();
}

void Dialog::deinit()
{
	pRcsMgr_= 0;
	pRoot_ = 0;
}
