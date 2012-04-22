#include "./scene_lobby.h"

using namespace soc;

enum CommandId
{
	ECMDID_Ready=200,
	ECMDID_Change,
	ECMDID_Back,
};

SceneLobby::SceneLobby(lf::u8 id)
:	pGui__(0), p3D__(0), pComboBox__(0), pCam__(0), nSelectedPP__(0), nOldPP__(0)
{
	setId(id);
	memset(pPostEfx__, 0, sizeof(pPostEfx__));
}

SceneLobby::~SceneLobby()
{
}

void SceneLobby::create(lf::render::IRenderWindow *win)
{
	Parent::create(win);

	setupGUI();
	setupScene();
	setupPostEffect();
}

void SceneLobby::destroy()
{
	if (pCam__) pCam__->drop();
	if (pGui__) pGui__->drop();
	if (p3D__ ) p3D__->drop();
	pCam__ = 0;
	pGui__ = 0;
	p3D__ = 0;

	resetPostEffect();

	Parent::destroy();
}

void SceneLobby::onWake()
{
	Parent::onWake();

	pWin_->addKeyListener(this);
	if (p3D__) pWin_->add(p3D__, 100);
	if (pGui__) pWin_->add(pGui__, 200);	// post effect layers are on 150

	if (pComboBox__) pComboBox__->setSelected(nSelectedPP__);
}

void SceneLobby::onSleep()
{
	if (pPostEfx__[nSelectedPP__]) pPostEfx__[nSelectedPP__]->enable(pCam__, false);
	nSelectedPP__ = nOldPP__ = 0;

	if (pGui__) pWin_->remove(pGui__);
	if (p3D__) pWin_->remove(p3D__);
	pWin_->removeKeyListener(this);

	Parent::onSleep();
}

void SceneLobby::setupGUI()
{
	pWin_->getCursorControl()->setVisible(false);

	pGui__ = new lf::gui::CGUIManager(
		lf::core::CColorI(0, 0, 0, 255),
		false); // only true if purely, no 3d refresh

	pRcsMgr_->loadResources("gui/dlg_lobby.lfm", 0, 0, pGui__);

	pComboBox__ = static_cast<lf::gui::CGUIComboBox*>(pGui__->getChildById(3, true));
	pComboBox__->addSelectionListener(this);
	lf::gui::CGUIButton *btn = static_cast<lf::gui::CGUIButton*>(pGui__->getChildById(1, true));
	btn->addActionListener(this);
	btn = static_cast<lf::gui::CGUIButton*>(pGui__->getChildById(4, true));
	btn->addActionListener(this);
	btn = static_cast<lf::gui::CGUIButton*>(pGui__->getChildById(6, true));
	btn->addActionListener(this);
}

void SceneLobby::setupScene()
{
	p3D__ = new lf::render::CRenderLayer3DOPPL();
	lf::scene::CSceneManager *scene = p3D__->getSceneManager();

	// Add dummy scenenode here as a rotation pivot point
	lf::scene::CSceneNode *node = new lf::scene::CGroupSceneNode();
	node->setPosition(lf::core::vector3df(0, 0, 0));
	scene->addSceneNode(node);
	node->drop();

	// Add Camera
	pCam__ = new lf::scene::C3DCamera(pWin_,
		lf::core::PI/3.0f,
		1.0f,
		1000.0f,
		true);
	pCam__->setBackgroundColor(lf::core::CColorI(0, 0, 255, 0));
	pCam__->setPosition(lf::core::vector3df(0, 5, -45.0f));
	pCam__->setTarget(lf::core::vector3df(0, 0, 0));

	p3D__->add(pCam__);
	node->addChild(pCam__);

	// Add motion to camera
	lf::scene::CRotationController *camController = new lf::scene::CRotationController( node,
		lf::core::vector3df(0.0f,5.0f,0.0f),
		0,
		false);

	scene->add(camController);
	camController->drop();

	// Add object space renderpass
	lf::render::CRenderContextOpenGLStub *context = reinterpret_cast<lf::render::CRenderContextOpenGLStub*>(pWin_->getRenderContext());
	lf::render::CRenderFeatureRegistry *reg = lf::CLFRender::getInstance().getRenderFeatureRegistry();

	// create renderpass for predefined render feature
	lf::render::CRPOGLSLCustom *rpGLSL = new lf::render::CRPOGLSLCustom(
		context,
		"shaders/light.vsh", "shaders/light.psh", 0,
		lf::render::ERPL_OPPL, lf::render::ERPF_PER_PIXEL_LIGHTING, lf::render::ERP_MAIN, 101);
	context->add(rpGLSL);
	rpGLSL->drop();

	// create renderpass for user defined render feature
	reg->registerRenderFeature("ERPF_SP_NORMALMAP");
	reg->addPasses(lf::render::ERPL_OPPL, 
		reg->getFeatureByName("ERPF_SP_NORMALMAP"), lf::render::ERP_DEPTH_ONLY, lf::render::ERP_MAIN);
	lf::render::CRPOGLSLCustom *rpNormalMap = new lf::render::CRPOGLSLCustom(
		reinterpret_cast<lf::render::CRenderContextOpenGLStub*>(context),
		"shaders/normalmap.vsh", "shaders/normalmap.psh", 0,
		lf::render::ERPL_OPPL, reg->getFeatureByName("ERPF_SP_NORMALMAP"), lf::render::ERP_MAIN, 100, 2);
	context->add(rpNormalMap);
	rpNormalMap->drop();

	// create renderpass for user defined render feature with user supplied parameters
	reg->registerRenderFeature("ERPF_SP_PARALLAX");
	reg->addPasses(lf::render::ERPL_OPPL, 
		reg->getFeatureByName("ERPF_SP_PARALLAX"), lf::render::ERP_DEPTH_ONLY, lf::render::ERP_MAIN);
	lf::render::CRPOGLSLCustom *rpParallaxMap = new lf::render::CRPOGLSLCustom(
		reinterpret_cast<lf::render::CRenderContextOpenGLStub*>(context),
		"shaders/parallax.vsh", "shaders/parallax.psh", 0,
		lf::render::ERPL_OPPL, reg->getFeatureByName("ERPF_SP_PARALLAX"), lf::render::ERP_MAIN, 100, 2);
	context->add(rpParallaxMap);
	rpParallaxMap->useRenderPassParameter("EyePos", lf::render::ERPPB_EYEPOS_VEC3);
	rpParallaxMap->useRenderPassParameter("scale", lf::render::ERPPS_NODE, lf::render::ERPPT_F32, 1);
	rpParallaxMap->useRenderPassParameter("bias", lf::render::ERPPS_NODE, lf::render::ERPPT_F32, 1);
	// default parameters
	rpParallaxMap->setRenderPassParameter("scale", 0.04f);
	rpParallaxMap->setRenderPassParameter("bias", 0.02f);
	rpParallaxMap->drop();

	// create renderpass for user defined render feature with user supplied parameters
	reg->registerRenderFeature("ERPF_SP_LIT_PARALLAX");
	reg->addPasses(lf::render::ERPL_OPPL, 
		reg->getFeatureByName("ERPF_SP_LIT_PARALLAX"), lf::render::ERP_DEPTH_ONLY, lf::render::ERP_MAIN);
	lf::render::CRPOGLSLCustom *rpLitParallaxMap = new lf::render::CRPOGLSLCustom(
		reinterpret_cast<lf::render::CRenderContextOpenGLStub*>(context),
		"shaders/litparallax.vsh", "shaders/litparallax.psh", 0,
		lf::render::ERPL_OPPL, reg->getFeatureByName("ERPF_SP_LIT_PARALLAX"), lf::render::ERP_MAIN, 100, 3);
	context->add(rpLitParallaxMap);
	rpLitParallaxMap->useRenderPassParameter("EyePos", lf::render::ERPPB_EYEPOS_VEC3);
	rpLitParallaxMap->useRenderPassParameter("scale", lf::render::ERPPS_NODE, lf::render::ERPPT_F32, 1);
	rpLitParallaxMap->useRenderPassParameter("bias", lf::render::ERPPS_NODE, lf::render::ERPPT_F32, 1);
	// default parameters
	rpLitParallaxMap->setRenderPassParameter("scale", 0.04f);
	rpLitParallaxMap->setRenderPassParameter("bias", 0.02f);
	rpLitParallaxMap->drop();

	// create renderpass for user defined render feature with user supplied parameters
	reg->registerRenderFeature("ERPF_SP_ITER_PARALLAX");
	reg->addPasses(lf::render::ERPL_OPPL, 
		reg->getFeatureByName("ERPF_SP_ITER_PARALLAX"), lf::render::ERP_DEPTH_ONLY, lf::render::ERP_MAIN);
	lf::render::CRPOGLSLCustom *rpIterParallaxMap = new lf::render::CRPOGLSLCustom(
		reinterpret_cast<lf::render::CRenderContextOpenGLStub*>(context),
		"shaders/iterativeparallax.vsh", "shaders/iterativeparallax.psh", 0,
		lf::render::ERPL_OPPL, reg->getFeatureByName("ERPF_SP_ITER_PARALLAX"), lf::render::ERP_MAIN, 100, 3);
	context->add(rpIterParallaxMap);
	rpIterParallaxMap->useRenderPassParameter("EyePos", lf::render::ERPPB_EYEPOS_VEC3);
	rpIterParallaxMap->useRenderPassParameter("scale", lf::render::ERPPS_NODE, lf::render::ERPPT_F32, 1);
	rpIterParallaxMap->useRenderPassParameter("bias", lf::render::ERPPS_NODE, lf::render::ERPPT_F32, 1);
	// default parameters
	rpIterParallaxMap->setRenderPassParameter("scale", 0.04f);
	rpIterParallaxMap->setRenderPassParameter("bias", 0.02f);
	rpIterParallaxMap->drop();

	// Add light RenderStates to camera
	lf::render::CRenderStateLighting *rsLight = new lf::render::CRenderStateLighting(true);
	pCam__->add(rsLight);
	rsLight->drop();

	lf::render::CRenderStateSpecularLighting *rsSpec = new lf::render::CRenderStateSpecularLighting(true,true);
	pCam__->replace(rsSpec);
	rsSpec->drop();

	lf::render::CRenderStateAmbientLight *rsAmb = new lf::render::CRenderStateAmbientLight(lf::core::CColorI(50,50,50,255));
	pCam__->replace(rsAmb);
	rsAmb->drop();

	// Add a light source
	lf::scene::CLight *light1 = new lf::scene::CLight(lf::scene::ELT_SPOT,
		lf::core::CColorF(0.1f,0.1f,0.1f,1.0f), // diffuse color of the light
		lf::core::CColorF(0.0f,0.0f,0.0f,1.0f), // the color of specular hightlights
		lf::core::CColorF(0.0f,0.0f,0.0f,1.0f), // ambient color
		20.0f, // the range of the light, ignore in opengl
		0.5f, // the constant attenuation for the light
		lf::core::PI/10, // linear attenuation for the light
		lf::core::PI/6, // quadratic attenuation factor
		0.01f, // exponential attenuation factor
		0.005f
	);
	// set the initial position of the light
	//light1->setPosition(lf::core::vector3df(0.0f,15.0f,40.0f));
	// since we want to add a child to the light (so its position is visible) we will also add it to the scene
	// as a scenenode
	pCam__->addChild(light1); // or scene->add(light) if no need to attach model to it
	light1->drop();

	// and last but not least we want everything in the scene to be affected by this light, so we add
	// a scenestate to the rootscenenode
	lf::scene::CSceneStateLight *ssLight = new lf::scene::CSceneStateLight(light1,true);
	scene->getRootSceneNode()->addSceneState(ssLight);
	ssLight->drop();

	lf::scene::CLight *light2 = new lf::scene::CLight( lf::scene::ELT_POINT,
		lf::core::CColorF(0.05f,0.05f,0.35f,1.0f),
		lf::core::CColorF(0.0f,0.0f,0.0f,1.0f),
		lf::core::CColorF(0.0f,0.0f,0.0f,1.0f),
		50.0f, 0.01f,lf::core::PI/4,lf::core::PI/3,0.0f,0.005f,0.0f,0.0f );
	scene->add( light2 );
	light2->drop();

	lf::scene::CSceneNodeController *lgt2Controller = new lf::scene::CFlyCircleController(light2,
		lf::core::vector3df(0.0f,10.0f,0.0f),lf::core::vector3df(0.0f,1.0f,0.0f),40.0f,0.1f);
	scene->add(lgt2Controller);
	lgt2Controller->drop();

	ssLight = new lf::scene::CSceneStateLight(light2,true);
	scene->getRootSceneNode()->addSceneState(ssLight);
	ssLight->drop();

	// Add textures
	lf::res::CTexture *rtexRockBase = pRcsMgr_->loadTexture("shaders/rockbump.jpg","rockBase");
	lf::res::CTexture *rtexRockHeight = pRcsMgr_->loadTexture("shaders/rockbump_height.png","rockHeight");
	lf::res::CTexture *rtexRockNormal = pRcsMgr_->loadTexture("shaders/rockbump.tga","rockNormal");

	lf::res::CTexture *rtexBrickBase = pRcsMgr_->loadTexture("shaders/brickbump.jpg","brickBase");
	lf::res::CTexture *rtexBrickNormal = pRcsMgr_->loadTexture("shaders/brickbump.tga","brickNormal");

	// Add 3D models
	lf::res::CPlaneModel *plane = new lf::res::CPlaneModel("ground", 
		lf::core::vector2df(100.0f, 100.0f),	// size
		lf::core::vector2dui(2, 2),				// segments
		lf::core::vector2dui(10, 10),				// texture-repeat
		true,									// alighment, horinzontal=true
		lf::core::CColorI(255, 255, 255, 255),	// color
		true);									// center at 0, 0
	//plane->getMaterial(0)->setRenderFeature(lf::render::ERPF_DIFFUSEMAP);
	plane->getMesh(0)->convertToMeshWithTangents(true, false);
	plane->getMaterial(0)->replaceRenderFeature(reg->getFeatureByName("ERPF_SP_LIT_PARALLAX") );//lf::render::ERPF_PER_PIXEL_LIGHTING);
	plane->getMaterial(0)->setTexture(0, rtexRockBase);
	plane->getMaterial(0)->setTexture(1, rtexRockHeight);
	plane->getMaterial(0)->setTexture(2, rtexRockNormal);

	node = new lf::scene::CModelSceneNode(plane);
	scene->addSceneNode(node);
	node->setPosition(lf::core::vector3df(0, -5.0f, 0));
	node->drop();

	lf::res::CModel *cyl = new lf::res::CCylinderModel("cylinder1", 10.0f, 50.0f, 12);
	//cyl->getMesh(0)->convertToMeshWithTangents(true, false);
	//cyl->getMaterial(0)->replaceRenderFeature( reg->getFeatureByName("ERPF_SP_NORMALMAP") );
	//cyl->getMaterial(0)->setTexture(0, rtexBrickBase);
	//cyl->getMaterial(0)->setTexture(1, rtexBrickNormal);
	
	node = new lf::scene::CModelSceneNode(cyl);
	scene->addSceneNode(node);
	node->setPosition(lf::core::vector3df(0, 0, 0));
	node->drop();
}

void SceneLobby::setupPostEffect()
{
	// loading render feature
	pRcsMgr_->loadResources("shaders/ppblackwhite.lfm");
	pRcsMgr_->loadResources("shaders/ppbright.lfm");
	pRcsMgr_->loadResources("shaders/ppcontrast.lfm");
	pRcsMgr_->loadResources("shaders/ppsaturate.lfm");
	pRcsMgr_->loadResources("shaders/bloom_n_dof.lfm");
	pRcsMgr_->loadResources("shaders/motion_blur.lfm");

	// coding method to add steps into post processor
	lf::render::CRenderFeatureRegistry *reg = lf::CLFRender::getInstance().getRenderFeatureRegistry();
	lf::render::CPostProcessorStep *ppStep;
	lf::core::vector2di size(512, 512);

	pPostEfx__[1] = new lf::render::CPostProcessor("BlackAndWhitePP", pWin_, 150, size);
	ppStep = pPostEfx__[1]->addStep("BlackAndWhite", reg->getFeatureByName("ERPF_PP_BW"), size);
	ppStep->addInput(lf::render::EPPSI_SCENE);
	pPostEfx__[2] = new lf::render::CPostProcessor("BrightnessPP", pWin_, 150, size);
	ppStep = pPostEfx__[2]->addStep("Brightness", reg->getFeatureByName("ERPF_PP_BRIGHTNESS"), size);
	ppStep->addInput(lf::render::EPPSI_SCENE);
	pPostEfx__[3] = new lf::render::CPostProcessor("ContrastPP", pWin_, 150, size);
	ppStep = pPostEfx__[3]->addStep("Contrast", reg->getFeatureByName("ERPF_PP_CONTRAST"), size);
	ppStep->addInput(lf::render::EPPSI_SCENE);
	pPostEfx__[4] = new lf::render::CPostProcessor("SaturationPP", pWin_, 150, size);
	ppStep = pPostEfx__[4]->addStep("Saturation", reg->getFeatureByName("ERPF_PP_SATURATE"), size);
	ppStep->addInput(lf::render::EPPSI_SCENE);
	
	// scripting method to load steps into post processor
	pPostEfx__[5] = pRcsMgr_->loadPostProcessor("shaders/bloom_pp.lfm");
	pPostEfx__[6] = pRcsMgr_->loadPostProcessor("shaders/dof_pp.lfm");
	pPostEfx__[7] = pRcsMgr_->loadPostProcessor("shaders/motion_blur_pp.lfm");
}

void SceneLobby::resetPostEffect()
{
	pPostEfx__[1]->drop();
	pPostEfx__[2]->drop();
	pPostEfx__[3]->drop();
	pPostEfx__[4]->drop();
	pPostEfx__[5]->drop();
	pPostEfx__[6]->drop();
	pPostEfx__[7]->drop();
}

lf::u8 SceneLobby::update(lf::u32 elapse)
{
	return nNewId_;
}

void SceneLobby::keyPressed(lf::input::CKeyEvent &evt)
{
	switch(evt.getKey())
	{
	case lf::input::KEY_HOME:
		changeScene(ESID_Welcome);
		break;
	}
}

void SceneLobby::performAction(lf::gui::CGUIActionEvent &evt)
{
	switch (evt.getCommandID())
	{
	case ECMDID_Ready:
		break;
	case ECMDID_Change:
		if (nOldPP__ == nSelectedPP__) break;
		if (pPostEfx__[nOldPP__]) pPostEfx__[nOldPP__]->enable(pCam__, false);
		if (pPostEfx__[nSelectedPP__]) pPostEfx__[nSelectedPP__]->enable(pCam__, true);
		//lf::CLFRender::getInstance().setAutoSleep(nSelectedPP__ == 7 ? 25 : 0); // make motion blur more obvious
		nOldPP__ = nSelectedPP__;
		break;
	case ECMDID_Back:
		changeScene(ESID_Welcome);
		break;
	}
}

void SceneLobby::selectionChanged(lf::gui::CGUISelectionEvent &evt)
{
	switch (evt.getSource()->getId())
	{
	case 3:
		lf::gui::CGUIComboBox *cboc = (lf::gui::CGUIComboBox*)evt.getSource();
		nSelectedPP__ = cboc->getSelected();
		break;
	}
}
