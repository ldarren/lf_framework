#include "./dlg_network.h"

using namespace soc;

enum CommandId
{
	ECMDID_NetStart=100,
	ECMDID_NetCancel,
};

DlgNetwork::DlgNetwork()
: nMode_(EDNS_Host), pFrame_(0), pEditName_(0), pEditIP_(0), pEditPort_(0)
{
}

DlgNetwork::~DlgNetwork()
{
}

void DlgNetwork::init(IScene *scene, lf::gui::CGUIWidget *root)
{
	Parent::init(scene, root);

	pRcsMgr_->loadResources("gui/dlg_network.lfm", 0, 0, pRoot_);
	pFrame_ = static_cast<lf::gui::CGUIWindow*>(pRoot_->getChildById(0, true));
	pFrame_->setHidden(true);

	pEditName_ = static_cast<lf::gui::CGUIEditBox*>(pFrame_->getChildById(3, true));
	pEditIP_ = static_cast<lf::gui::CGUIEditBox*>(pFrame_->getChildById(5, true));
	pEditPort_ = static_cast<lf::gui::CGUIEditBox*>(pFrame_->getChildById(7, true));
	static_cast<lf::gui::CGUIButton*>(pFrame_->getChildById(8, true))->addActionListener(this);
	static_cast<lf::gui::CGUIButton*>(pFrame_->getChildById(9, true))->addActionListener(this);
}

void DlgNetwork::deinit()
{
	pEditPort_=0;
	pEditIP_=0;
	pEditName_=0;
	pFrame_=0;
	Parent::deinit();
}

void DlgNetwork::show(int mode)
{
	nMode_ = mode;

	switch(nMode_)
	{
	case EDNS_Host:
		pFrame_->setTitleText(L"Host Setting");
		pEditName_->setText(L"");
		pEditIP_->setText(L"");
		pEditPort_->setText(L"");
		pFrame_->setHidden(false);
		break;
	case EDNS_Guest:
		pFrame_->setTitleText(L"Guest Setting");
		pEditName_->setText(L"");
		pEditIP_->setText(L"");
		pEditPort_->setText(L"");
		pFrame_->setHidden(false);
		break;
	}
}

void DlgNetwork::hide()
{
	pFrame_->setHidden(true);
}

void DlgNetwork::performAction(lf::gui::CGUIActionEvent &evt)
{
	switch (evt.getCommandID())
	{
	case ECMDID_NetStart:
		hide();
		switch(nMode_)
		{
		case EDNS_Host:
			pScene_->onEvent(EDNE_HostStart, 0);
			break;
		case EDNS_Guest:
			pScene_->onEvent(EDNE_GuestStart, 0);
			break;
		}
		break;
	case ECMDID_NetCancel:
		hide();
		break;
	}
}

const lf::core::stringw& DlgNetwork::getName() const
{
	return pEditName_->getText();
}

const lf::core::stringw& DlgNetwork::getIP() const
{
	return pEditIP_->getText();
}

const lf::u16 DlgNetwork::getPort() const
{
	return pEditPort_->getText().getAsU16();
}
