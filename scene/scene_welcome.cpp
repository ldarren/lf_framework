#include "./scene_welcome.h"

enum CommandId
{
	ECMDID_Host=101,
	ECMDID_Join,
	ECMDID_Quit,
};

using namespace soc;

SceneWelcome::SceneWelcome(lf::u8 id)
:	pGui__(0), p3D__(0)
{
	setId(id);
}

SceneWelcome::~SceneWelcome()
{
}

void SceneWelcome::create(lf::render::IRenderWindow *win)
{
	Parent::create(win);

	setupGUI();
	setupScene();

	mDlgNet__.init(this, pGui__);
}

void SceneWelcome::destroy()
{
	mDlgNet__.deinit();

	if (pGui__) pGui__->drop();
	if (p3D__ ) p3D__->drop();
	pGui__ = 0;
	p3D__ = 0;

	Parent::destroy();
}

void SceneWelcome::onWake()
{
	Parent::onWake();

	pWin_->addKeyListener(this);
	pWin_->add(p3D__);
	pWin_->add(pGui__);
}

void SceneWelcome::onSleep()
{
	pWin_->remove(pGui__);
	pWin_->remove(p3D__);
	pWin_->removeKeyListener(this);

	Parent::onSleep();
}

bool SceneWelcome::onEvent(lf::s32 type, lf::s32 state)
{
	switch(type)
	{
	case DlgNetwork::EDNE_HostStart:
		changeScene(ESID_Lobby);
		break;
	case DlgNetwork::EDNE_GuestStart:
		changeScene(ESID_Game);
		break;
	}

	return false;
}

void SceneWelcome::setupGUI()
{
	pWin_->getCursorControl()->setVisible(false);

	pGui__ = new lf::gui::CGUIManager(
		lf::core::CColorI(0, 0, 0, 255),
		false); // only true if purely, no 3d refresh

	lf::gui::CGUIButton *btn = new lf::gui::CGUIButton(L"Host", ECMDID_Host, 10);
	btn->setPosition(50, 100);
	btn->setSize(100, 25);
	btn->addActionListener(this);
	pGui__->add(btn);
	btn->drop();

	btn = new lf::gui::CGUIButton(L"Join", ECMDID_Join, 11);
	btn->setPosition(50, 150);
	btn->setSize(100, 25);
	btn->addActionListener(this);
	pGui__->add(btn);
	btn->drop();

	btn = new lf::gui::CGUIButton(L"Quit", ECMDID_Quit, 12);
	btn->setPosition(50, 200);
	btn->setSize(100, 25);
	btn->addActionListener(this);
	pGui__->add(btn);
	btn->drop();
}

void SceneWelcome::setupScene()
{
	p3D__ = new lf::render::CRenderLayer3DOPPL();
	lf::scene::CSceneManager *scene = p3D__->getSceneManager();

	//// Add 3D models
	pRcsMgr_->loadResources("models/indoor.lfm", 0, 0);
	lf::res::CModel *room = pRcsMgr_->getModel("Room");
	lf::scene::CSceneNode *node = new lf::scene::CModelSceneNode(room);

	scene->addSceneNode(node);
	node->drop();

	// Add Camera
	lf::scene::C3DCamera *cam = new lf::scene::C3DCamera(pWin_,
		lf::core::PI/3.0f,
		1.0f,
		1000.0f,
		true);
	cam->setBackgroundColor(lf::core::CColorI(0, 0, 255, 0));
	cam->setPosition(lf::core::vector3df(0, 0, 0));

	p3D__->add(cam);
	node->addChild(cam);
	cam->drop();

	// Add motion to camera
	lf::scene::CRotationController *camController = new lf::scene::CRotationController( cam,
		lf::core::vector3df(0.0f,5.0f,0.0f),
		0,
		false);

	scene->add(camController);
	camController->drop();
}

lf::u8 SceneWelcome::update(lf::u32 elapse)
{
	return nNewId_;
}

void SceneWelcome::performAction(lf::gui::CGUIActionEvent &evt)
{
	switch (evt.getCommandID())
	{
	case ECMDID_Host:
		mDlgNet__.show(DlgNetwork::EDNS_Host);
		break;
	case ECMDID_Join:
		mDlgNet__.show(DlgNetwork::EDNS_Guest);
		break;
	case ECMDID_Quit:
		pWin_->closeWindow();
		break;
	}
}
