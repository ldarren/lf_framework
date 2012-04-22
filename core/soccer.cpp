
#include "./soccer.h"
#include "scene/i_scene.h"
using namespace soc;

Soccer::Soccer()
: mSceneWelcome__(ESID_Welcome), mSceneLobby__(ESID_Lobby), mSceneGame__(ESID_Game), pCurrScene__(0)
{
	lf::initLF();

	pWin__ = lf::CLFRender::getInstance().createRenderWindow(
		lf::core::vector2di(0, 0),
		lf::core::vector2d<lf::s32>(1024, 768),
		32,
		32,
		lf::render::EWCF_AUTOCLOSE | lf::render::EWCF_CENTER_WINDOW | lf::render::EWCF_NO_DEFAULT_RL3D,
		lf::render::EAAF_NONE);
	pWin__->setWindowCaption(L"Robot Soccer");
	pWin__->setVisible(true);

	pWin__->addKeyListener(this);
	pWin__->addRenderWindowListener(this);

	pRcsMgr__ = lf::CResourceManager::getInstancePtr();
	lf::core::stringc mediaDir = lf::CLFOS::getInstance().getFileSystem()->getApplicationDirectory();
	mediaDir += "/../rcs";
	pRcsMgr__->getFileSystem()->addSearchPath(mediaDir.c_str());
	pRcsMgr__->loadResources("theme/clearlooks.lfgt");

	lf::CLFRender::getInstance().setAutoSleep(0);

	mSceneWelcome__.create(pWin__);
	mSceneLobby__.create(pWin__);
	mSceneGame__.create(pWin__);
}

Soccer::~Soccer()
{
	lf::deinitLF();
}

void Soccer::run()
{
	lf::u8 currSceneId;
	pCurrScene__ = &mSceneWelcome__;
	pCurrScene__->onWake();

#ifdef _DEBUG
	wchar_t caption[128];
	size_t caplen = sizeof(caption)/sizeof(caption[0]);
	lf::u32 polycount=0, lastPolyCount=0;
	lf::s32 fps=0, lastFps=0;
#endif

	lf::CLFRender &lfr = lf::CLFRender::getInstance();

	lf::u32 time=0, lastTime=0;
	lf::os::CSimulationTimer timer;
	lastTime = time = timer.getTime();

	do {
#ifdef _DEBUG
		polycount = pWin__->getTotalPolygonCount();
		fps = pWin__->getOneSecondFPS();
		if ((fps != lastFps) || (polycount != lastPolyCount))
		{
			snwprintf(caption, caplen, L"Robot Soccer - Poly[%u] FPS[%d]", polycount, fps);
			pWin__->setWindowCaption(caption);
			lastPolyCount = polycount;
			lastFps = fps;
		}
#endif
		currSceneId = pCurrScene__->update(time-lastTime);
		if (currSceneId != pCurrScene__->getId())
		{
			pCurrScene__->onSleep();
			switch(currSceneId)
			{
			case ESID_Welcome:
				pCurrScene__ = &mSceneWelcome__;
				break;
			case ESID_Lobby:
				pCurrScene__ = &mSceneLobby__;
				break;
			case ESID_Game:
				pCurrScene__ = &mSceneGame__;
				break;
			}
			pCurrScene__->onWake();
		}

		lastTime = time;
		time = timer.getTime();
	} while (lfr.update());
}

void Soccer::keyPressed(lf::input::CKeyEvent &evt)
{
	switch(evt.getKey())
	{
	case lf::input::KEY_ESCAPE:
		pWin__->closeWindow();
		break;
	}
}

void Soccer::windowClosed(lf::render::CRenderWindowEvent& evt)
{
	pCurrScene__->onSleep();
	pCurrScene__=0;

	mSceneGame__.destroy();
	mSceneLobby__.destroy();
	mSceneWelcome__.destroy();
}
