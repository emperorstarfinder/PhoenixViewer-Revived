/* Copyright (C) 2011 LordGregGreg Back (Greg Hendrickson)

   This is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; version 2.1 of
   the License.
 
   This is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.
 
   You should have received a copy of the GNU Lesser General Public
   License along with the viewer; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "llviewerprecompiledheaders.h"

#include "lggfloaterirc.h"
#include "lggfloaterircedit.h"
#include "message.h"

#include "llagent.h"
#include "llbutton.h"
#include "llfloaterdirectory.h"
#include "llfocusmgr.h"
#include "llalertdialog.h"
#include "llselectmgr.h"
#include "llscrolllistctrl.h"
#include "lltextbox.h"
#include "lluictrlfactory.h"
#include "llviewercontrol.h"
#include "llviewerwindow.h"
#include "llimview.h"

// helper functions
void init_irc_list(LLScrollListCtrl* ctrl);

lggPanelIRC::lggPanelIRC() :
	LLPanel()
{
	//gAgent.addListener(this, "new group");
}

lggPanelIRC::~lggPanelIRC()
{
	//gAgent.removeListener(this);
}
bool lggPanelIRC::handleEvent(LLPointer<LLEvent> event, const LLSD& userdata)
{
	
	return false;
}
// clear the group list, and get a fresh set of info.
void lggPanelIRC::newList()
{
// 	LLCtrlListInterface *group_list = childGetListInterface("PhoenixIRC_list");
// 	if (group_list)
// 	{
// 		group_list->operateOnAll(LLCtrlListInterface::OP_DELETE);
// 	}
	llinfos << "refreshing..." << llendl;

	init_irc_list(getChild<LLScrollListCtrl>("PhoenixIRC_list"));
	enableButtons();
}
void lggPanelIRC::initHelpBtn(const std::string& name, const std::string& xml_alert)
{
	childSetAction(name, onClickHelp, new std::string(xml_alert));
}
void lggPanelIRC::onClickHelp(void* data)
{
	std::string* xml_alert = (std::string*)data;
	LLNotifications::instance().add(*xml_alert);
}
BOOL lggPanelIRC::postBuild()
{
	childSetCommitCallback("PhoenixIRC_list", onIrcList, this);

	//init_irc_list(getChild<LLScrollListCtrl>("PhoenixIRC_list"));

	childSetAction("PhoenixIRC_IM", onBtnIM, this);

	childSetAction("PhoenixIRC_new", onBtnNewIrc, this);

	childSetAction("PhoenixIRC_edit", onBtnEdit, this);

	childSetAction("PhoenixIRC_remove", onBtnRemove, this);
	childSetAction("PhoenixIRC_refresh", onBtnRefresh, this);

	setDefaultBtn("PhoenixIRC_IM");

	childSetDoubleClickCallback("PhoenixIRC_list", onBtnIM);
	childSetUserData("PhoenixIRC_list", this);

	initHelpBtn("PhoenixIRC_Help",	"PhoenixHelp_IRCSettings");

	glggIrcGroupHandler.setListPanel(this);
	//newList();

	return TRUE;
}

void lggPanelIRC::enableButtons()
{
	LLCtrlListInterface *irc_list = childGetListInterface("PhoenixIRC_list");
	LLUUID irc_id;
	if (irc_list)
	{
		irc_id = irc_list->getCurrentID();
	}
	if (irc_id.notNull())
	{
		childEnable("PhoenixIRC_remove");
		childEnable("PhoenixIRC_IM");
		childEnable("PhoenixIRC_edit");
	}
	else
	{
		childDisable("PhoenixIRC_IM");
		childDisable("PhoenixIRC_edit");
		childDisable("PhoenixIRC_remove");
	}
	childEnable("PhoenixIRC_new");
	refresh();
}


void lggPanelIRC::onBtnNewIrc(void* userdata)
{
	llinfos << "lggPanelIRC::onbuttonnewirc" << llendl;
	
	lggPanelIRC* self = (lggPanelIRC*)userdata;
	if(self) self->newirc();
}


void lggPanelIRC::onBtnEdit(void* userdata)
{
	lggPanelIRC* self = (lggPanelIRC*)userdata;
	if(self) self->editirc();
}

void lggPanelIRC::onBtnIM(void* userdata)
{
	lggPanelIRC* self = (lggPanelIRC*)userdata;
	if(self) self->startirc();
}
void lggPanelIRC::onBtnRefresh(void* userdata)
{
	lggPanelIRC* self = (lggPanelIRC*)userdata;
	if(self) self->newList();
}
void lggPanelIRC::onBtnRemove(void* userdata)
{
	lggPanelIRC* self = (lggPanelIRC*)userdata;
	if(self) self->removeirc();
}
////////////////////////////////////
void lggPanelIRC::newirc()
{
	llinfos << "lggPanelIRC::newirc" << llendl;
	//lggPanelIRC::showCreateGroup(NULL);
	LggIrcFloaterStarter::show(lggIrcData(),this);
}
void lggPanelIRC::editirc()
{
	llinfos << "lggPanelIRC::editirc" << llendl;
	LLCtrlListInterface *irc_list = childGetListInterface("PhoenixIRC_list");
	LLUUID irc_id;

	if (irc_list && (irc_id = irc_list->getCurrentID()).notNull())
	{
		
		LggIrcFloaterStarter::show(glggIrcGroupHandler.getIrcGroupInfoByID(irc_list->getCurrentID()),this);
		//TODO CLeanup shit
		
	}
	
}
void lggPanelIRC::startirc()
{
	LLCtrlListInterface *irc_list = childGetListInterface("PhoenixIRC_list");
	LLUUID irc_id;

	if (irc_list && (irc_id = irc_list->getCurrentID()).notNull())
	{
		llinfos << " starting irc buttons.." << llendl;
		lggIrcData idat = glggIrcGroupHandler.getIrcGroupInfoByID(irc_list->getCurrentID());
		//delete indat; TODO CLEANUP 
		llinfos << " got idat... it is.." << llendl;
		llinfos << idat.toString() << llendl;//FFFFFFFFFFFFFFFFFFFFFFFFFFFffff
		glggIrcGroupHandler.startUpIRCListener(idat);
		
		
	}
}

void lggPanelIRC::removeirc()
{
	llinfos << "lggPanelIRC::removeirc" << llendl;
	LLCtrlListInterface *irc_list = childGetListInterface("PhoenixIRC_list");
	LLUUID irc_id;

	if (irc_list && (irc_id = irc_list->getCurrentID()).notNull())
	{
		llinfos << "lggPanelIRC::removeirc" << irc_list->getSelectedValue() << " and " << irc_list->getCurrentID() << llendl;
	
		glggIrcGroupHandler.deleteIrcGroupByID(irc_list->getCurrentID());
		
	}
	newList();
}



void lggPanelIRC::onIrcList(LLUICtrl* ctrl, void* userdata)
{
	lggPanelIRC* self = (lggPanelIRC*)userdata;
	if(self) self->enableButtons();
}

void init_irc_list(LLScrollListCtrl* ctrl)
{
	if(!ctrl)return;
	llinfos << "more refreshing.." << llendl;

	std::vector<lggIrcData> allGroups = glggIrcGroupHandler.getFileNames();
	S32 count = allGroups.size();
	LLUUID id;
	LLCtrlListInterface *irc_list = ctrl->getListInterface();
	if (!irc_list) return;

	irc_list->operateOnAll(LLCtrlListInterface::OP_DELETE);

	for(S32 i = 0; i < count; ++i)
	{
		llinfos << "init_irc_list and " << allGroups[i].name << llendl;
	
		id = allGroups[i].id;//gAgent.mGroups.get(i).mID;
		
		std::string style = "NORMAL";
		LLSD element;
		
		element["columns"][0]["color"] = gColors.getColor("DefaultListText").getValue();
		
		element["id"] = id;
		element["columns"][0]["column"] = "PhoenixIRC_name";
		element["columns"][0]["value"] = allGroups[i].name;
		element["columns"][0]["font"] = "SANSSERIF";
		element["columns"][0]["font-style"] = style;

		irc_list->addElement(element, ADD_SORTED);
		
	}
	//irc_list->selectFirstItem();
}

