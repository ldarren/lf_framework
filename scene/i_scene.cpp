#include "./i_scene.h"

using namespace soc;
using namespace lf;

void IScene::create(lf::render::IRenderWindow *win)
{
	pWin_ = win;
	pRcsMgr_ = CResourceManager::getInstancePtr();
}

void IScene::destroy()
{
	pRcsMgr_ = 0;
	pWin_ = 0;
}

void IScene::onWake()
{
	nNewId_ = nId_;
}

void IScene::onSleep()
{
}

bool IScene::onEvent(lf::s32 type, lf::s32 state)
{
	return true;
}

void IScene::setId(lf::u8 id)
{
	nId_ = id;
	nNewId_ = id;
}

lf::u8 IScene::getId() const
{
	return nId_;
}

void IScene::changeScene(lf::u8 id)
{
	nNewId_ = id;
}
