#include "./scene_game.h"

using namespace soc;

SceneGame::SceneGame(lf::u8 id)
:	pGui__(0), p3D__(0)
{
	setId(id);
}

SceneGame::~SceneGame()
{
}

void SceneGame::create(lf::render::IRenderWindow *win)
{
	Parent::create(win);

	setupGUI();
	setupScene();
}

void SceneGame::destroy()
{
	if (pGui__) pGui__->drop();
	if (p3D__ ) p3D__->drop();
	pGui__ = 0;
	p3D__ = 0;

	Parent::destroy();
}

void SceneGame::onWake()
{
	Parent::onWake();

	pWin_->addKeyListener(this);
	pWin_->add(p3D__);
	pWin_->add(pGui__);
}

void SceneGame::onSleep()
{
	pWin_->remove(pGui__);
	pWin_->remove(p3D__);
	pWin_->removeKeyListener(this);

	Parent::onSleep();
}

void SceneGame::setupGUI()
{
	pWin_->getCursorControl()->setVisible(false);

	pGui__ = new lf::gui::CGUIManager(
		lf::core::CColorI(0, 0, 0, 255),
		false); // only true if purely, no 3d refresh

	lf::res::CTexture *tex = pRcsMgr_->loadTexture("textures/logo2008.png", "lflogo");
	pGui__->drawImage(tex,
		lf::core::vector2di(pWin_->getInnerSize().X-280, pWin_->getInnerSize().Y-128),
		lf::core::vector2di(280, 128),
		lf::gui::ETT_TEST);
}

void SceneGame::setupScene()
{
	p3D__ = new lf::render::CRenderLayer3DOPPL();
	lf::scene::CSceneManager *scene = p3D__->getSceneManager();

	// Add 3D models

	pRcsMgr_->loadResources("models/indoor.lfm", 0, 0);
	lf::res::CModel *room = pRcsMgr_->getModel("Room");
	lf::scene::CSceneNode *rmNode = new lf::scene::CModelSceneNode(room);
	scene->addSceneNode(rmNode);
	rmNode->drop();

	pRcsMgr_->loadResources("models/player.lfm", 0, rmNode);
	lf::res::CModel *model = pRcsMgr_->getModel("atog");
	model->setSkeleton(();
	lf::scene::CSceneNode *node = new lf::scene::CModelSceneNode(model);
	rmNode->addChild(node);
	node->setPosition(lf::core::vector3df(2, -1.0, -2));
	node->drop();

	lf::scene::C3DCamera *cam = new lf::scene::C3DCamera(pWin_,
		lf::core::PI/3.0f,
		1.0f,
		1000.0f,
		true);
	cam->setBackgroundColor(lf::core::CColorI(255, 128, 191, 0));
	cam->setPosition(lf::core::vector3df(0, 0, 0.0f));

	p3D__->add(cam);
	rmNode->addChild(cam);
	cam->drop();

	lf::scene::CFPSController *camController = new lf::scene::CFPSController(pWin_, cam, true );
	scene->add(camController);
	camController->setForwardSpeed(0.1f);
	camController->setBackwardSpeed(0.05f);
	camController->setStrafeLeftSpeed(0.08f);
	camController->setStrafeRightSpeed(0.08f);
	camController->drop();

	// Add light RenderStates to camera
	lf::render::CRenderStateLighting *rsLight = new lf::render::CRenderStateLighting(true);
	cam->add(rsLight);
	rsLight->drop();

	lf::render::CRenderStateSpecularLighting *rsSpec = new lf::render::CRenderStateSpecularLighting(true,true);
	cam->replace(rsSpec);
	rsSpec->drop();

	lf::render::CRenderStateAmbientLight *rsAmb = new lf::render::CRenderStateAmbientLight(lf::core::CColorI(50,50,50,255));
	cam->replace(rsAmb);
	rsAmb->drop();

	// Add a light source
	lf::scene::CLight *light = new lf::scene::CLight( lf::scene::ELT_POINT,
		lf::core::CColorF(0.3f,0.3f,0.3f,1.0f),
		lf::core::CColorF(0.0f,0.0f,0.0f,1.0f),
		lf::core::CColorF(0.0f,0.0f,0.0f,1.0f),
		50.0f, 0.01f,lf::core::PI/4,lf::core::PI/3,0.0f,0.005f,0.0f,0.0f );
	node->addChild( light );
	light->setPosition(lf::core::vector3df(0, 0, 0));
	light->drop();

	lf::scene::CSceneStateLight *ssLight = new lf::scene::CSceneStateLight(light,true);
	node->addSceneState(ssLight);
	ssLight->drop();
}

lf::u8 SceneGame::update(lf::u32 elapse)
{
	return nNewId_;
}

void SceneGame::keyPressed(lf::input::CKeyEvent &evt)
{
	switch(evt.getKey())
	{
	case lf::input::KEY_HOME:
		changeScene(ESID_Welcome);
		break;
	}
}

void SceneGame::performAction(lf::gui::CGUIActionEvent &evt)
{
	switch (evt.getCommandID())
	{
	case 101:
		break;
	}
}

void SceneGame::selectionChanged(lf::gui::CGUISelectionEvent &evt)
{
	switch (evt.getSource()->getId())
	{
	case 4:
		{
		}
		break;
	}
}
