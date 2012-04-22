#pragma once
#ifndef _SOC_DLG_NETWORK_H_
#define _SOC_DLG_NETWORK_H_

#include "./dialog.h"

namespace soc {

class DlgNetwork : public Dialog
{
	typedef Dialog Parent;

public:
	enum DlgNetworkEvent
	{
		EDNE_HostStart,
		EDNE_GuestStart,
	};

	enum DlgNetworkState
	{
		EDNS_Host,
		EDNS_Guest,
	};

public:
	DlgNetwork();
	virtual ~DlgNetwork();

	virtual void init(IScene *scene, lf::gui::CGUIWidget *root);
	virtual void deinit();

	void show(int mode);
	void hide();

	int getMode() const { return nMode_; }
	const lf::core::stringw& getName() const;
	const lf::core::stringw& getIP() const;
	const lf::u16 getPort() const;

	virtual void performAction(lf::gui::CGUIActionEvent &evt);

protected:
	int nMode_;

	lf::gui::CGUIWindow	*pFrame_;
	lf::gui::CGUIEditBox *pEditName_;
	lf::gui::CGUIEditBox *pEditIP_;
	lf::gui::CGUIEditBox *pEditPort_;
};

} // namespace soc

#endif // _SOC_DLG_NETWORK_H_