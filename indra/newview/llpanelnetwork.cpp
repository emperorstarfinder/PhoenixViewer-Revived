/** 
 * @file llpanelnetwork.cpp
 * @brief Network preferences panel
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

//file include
#include "llpanelnetwork.h"
#include "llstartup.h"

// project includes
#include "llcheckboxctrl.h"
#include "llradiogroup.h"
#include "lldirpicker.h"
#include "llfilepicker.h"
#include "lluictrlfactory.h"
#include "llviewercontrol.h"
#include "llviewerwindow.h"
#include "llagent.h"

#include "llviewerthrottle.h"

bool LLPanelNetwork::sSocksSettingsChanged;

LLPanelNetwork* LLPanelNetwork::sInstance = NULL;

LLPanelNetwork::LLPanelNetwork()
{
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_preferences_network.xml");
	sInstance = this;
}

//virtual
LLPanelNetwork::~LLPanelNetwork()
{
	sInstance = NULL;
}

//virtual
BOOL LLPanelNetwork::postBuild()
{
	std::string cache_location = gDirUtilp->getExpandedFilename(LL_PATH_CACHE, "");
	childSetText("cache_location", cache_location);
	std::string sound_cache_location = gDirUtilp->getExpandedFilename(MM_SNDLOC, "");
	if(sound_cache_location=="")sound_cache_location="Not Set Yet.";
	childSetText("sound_cache_location", sound_cache_location);
	std::string chat_logs_location = gDirUtilp->getExpandedFilename(LL_PATH_CHAT_LOGS, "");
	childSetText("chat_logs_location",chat_logs_location);
	std::string crash_logs_location = gDirUtilp->getExpandedFilename(LL_PATH_LOGS, "");
	childSetText("crash_logs_location",crash_logs_location);
	std::string user_setting_location = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "");
	childSetText("user_setting_location",user_setting_location);
		
	childSetAction("clear_cache", onClickClearCache, this);
	childSetAction("clear_inv_cache", onClickClearInvCache, this);
	if(LLStartUp::getStartupState() >= STATE_INVENTORY_SEND)childSetEnabled("clear_inv_cache",true);
	childSetAction("set_cache", onClickSetCache, this);
	childSetAction("reset_cache", onClickResetCache, this);
	childSetAction("set_sound_cache", onClickSetSoundCache, this);
	childSetAction("reset_sound_cache", onClickResetSoundCache, this);
	childSetAction("set_chat_logs", onClickSetChatLogs, this);
	childSetAction("reset_chat_logs", onClickResetChatLogs, this);

	childSetAction("open_user_loc",onClickBrowseUserSetting,this);
	childSetAction("open_crash_logs_loc",onClickBrowseCrashLogs,this);
	childSetAction("open_chat_logs_loc",onClickBrowseChatLogs,this);
	childSetAction("open_soundcache_loc",onClickBrowseSoundCache,this);
	childSetAction("open_cache_loc",onClickBrowseCache,this);
	
	childSetEnabled("connection_port", gSavedSettings.getBOOL("ConnectionPortEnabled"));
	childSetCommitCallback("connection_port_enabled", onCommitPort, this);

	childSetValue("cache_size", (F32)gSavedSettings.getU32("CacheSize"));
	childSetValue("max_bandwidth", LLViewerThrottle::sThrottleBandwidthKBPS);
	childSetValue("connection_port_enabled", gSavedSettings.getBOOL("ConnectionPortEnabled"));
	childSetValue("connection_port", (F32)gSavedSettings.getU32("ConnectionPort"));

	// Socks 5 proxy settings, commit callbacks
	childSetCommitCallback("socks5_proxy_enabled", onCommitSocks5ProxyEnabled, this);
	childSetCommitCallback("socks5_auth", onSocksAuthChanged, this);

	//Socks 5 proxy settings, saved data
	childSetValue("socks5_proxy_enabled",   gSavedSettings.getBOOL("Socks5ProxyEnabled"));
	childSetValue("socks5_http_proxy_type", gSavedSettings.getString("Socks5HttpProxyType"));

	childSetValue("socks5_proxy_host",     gSavedSettings.getString("Socks5ProxyHost"));
	childSetValue("socks5_proxy_port",     (F32)gSavedSettings.getU32("Socks5ProxyPort"));
	childSetValue("socks5_proxy_username", gSavedSettings.getString("Socks5Username"));
	childSetValue("socks5_proxy_password", gSavedSettings.getString("Socks5Password"));
	childSetValue("socks5_auth", gSavedSettings.getString("Socks5AuthType"));

	// Socks 5 proxy settings, check if settings modified callbacks
	childSetCommitCallback("socks5_proxy_host", onSocksSettingsModified,this);
	childSetCommitCallback("socks5_proxy_port", onSocksSettingsModified,this);
	childSetCommitCallback("socks5_proxy_username", onSocksSettingsModified,this);
	childSetCommitCallback("socks5_proxy_password", onSocksSettingsModified,this);
	
	// Socks 5 settings, Set all controls and labels enabled state
	updateProxyEnabled(this, gSavedSettings.getBOOL("Socks5ProxyEnabled"), gSavedSettings.getString("Socks5AuthType"));

	sSocksSettingsChanged = false;

	return TRUE;
}

//virtual
void LLPanelNetwork::draw()
{
	bool filePickerControlsEnabled = (!LLFilePickerThread::isInUse() && !LLDirPickerThread::isInUse());
	childSetEnabled("set_cache", filePickerControlsEnabled);
	childSetEnabled("set_sound_cache", filePickerControlsEnabled);
	childSetEnabled("set_chat_logs", filePickerControlsEnabled);
	LLPanel::draw();
}

void LLPanelNetwork::apply()
{
	gSavedSettings.setU32("CacheSize", childGetValue("cache_size").asInteger());
	gSavedSettings.setF32("ThrottleBandwidthKBPS", childGetValue("max_bandwidth").asReal());
	gSavedSettings.setBOOL("ConnectionPortEnabled", childGetValue("connection_port_enabled"));
	gSavedSettings.setU32("ConnectionPort", childGetValue("connection_port").asInteger());

	gSavedSettings.setBOOL("Socks5ProxyEnabled", childGetValue("socks5_proxy_enabled"));
	gSavedSettings.setString("Socks5HttpProxyType", childGetValue("socks5_http_proxy_type"));
	gSavedSettings.setString("Socks5ProxyHost", childGetValue("socks5_proxy_host"));
	gSavedSettings.setU32("Socks5ProxyPort", childGetValue("socks5_proxy_port").asInteger());

	gSavedSettings.setString("Socks5AuthType", childGetValue("socks5_auth"));
	gSavedSettings.setString("Socks5Username", childGetValue("socks5_proxy_username"));
	gSavedSettings.setString("Socks5Password", childGetValue("socks5_proxy_password"));

	if (sSocksSettingsChanged)
	{
		if (LLStartUp::getStartupState() != STATE_LOGIN_WAIT)
		{
			LLNotifications::instance().add("ProxyNeedRestart");
		}
		else
		{
			// Mark the socks class that it needs to update its connection
			LLSocks::getInstance()->updated();
		}
	}
}

void LLPanelNetwork::cancel()
{
}

// static
void LLPanelNetwork::onClickClearCache(void*)
{
	// flag client cache for clearing next time the client runs
	gSavedSettings.setBOOL("PurgeCacheOnNextStartup", TRUE);
	LLNotifications::instance().add("CacheWillClear");
}

void LLPanelNetwork::onClickClearInvCache(void*)
{
	gSavedSettings.setString("PhoenixPurgeInvCache",gAgent.getID().asString());
	LLNotifications::instance().add("CacheWillClear");
}

// static
void LLPanelNetwork::setCacheCallback(std::string& dir_name, void* data)
{
	LLPanelNetwork* self = (LLPanelNetwork*)data;
	if (!self || self != sInstance)
	{
		LLNotifications::instance().add("PreferencesClosed");
		return;
	}

	std::string cur_name = gSavedSettings.getString("CacheLocation");
	if (!dir_name.empty() && dir_name != cur_name)
	{
		self->childSetText("cache_location", dir_name);
		LLNotifications::instance().add("CacheWillBeMoved");
		gSavedSettings.setString("NewCacheLocation", dir_name);
	}
}

// static
void LLPanelNetwork::onClickSetCache(void* data)
{
	std::string suggestion = gSavedSettings.getString("CacheLocation");

	(new LLCallDirPicker(LLPanelNetwork::setCacheCallback,
						 data))->getDir(&suggestion);
}

// static
void LLPanelNetwork::onClickResetCache(void* data)
{
 	LLPanelNetwork* self = (LLPanelNetwork*)data;
	if (!gSavedSettings.getString("CacheLocation").empty())
	{
		gSavedSettings.setString("NewCacheLocation", "");
		LLNotifications::instance().add("CacheWillBeMoved");
	}
	std::string cache_location = gDirUtilp->getCacheDir(true);
	self->childSetText("cache_location", cache_location);
}

void LLPanelNetwork::setSoundCacheCallback(std::string& dir_name, void* data)
{
	LLPanelNetwork* self = (LLPanelNetwork*)data;
	if (!self || self != sInstance)
	{
		LLNotifications::instance().add("PreferencesClosed");
		return;
	}

	std::string cur_name = gSavedSettings.getString("Phoenixmm_sndcacheloc");
	if (!dir_name.empty() && dir_name != cur_name)
	{
		self->childSetText("sound_cache_location", dir_name);
		gSavedSettings.setString("Phoenixmm_sndcacheloc", dir_name);
	}
}

// static
void LLPanelNetwork::onClickSetSoundCache(void* user_data)
{
	std::string suggestion = gSavedSettings.getString("Phoenixmm_sndcacheloc");

	(new LLCallDirPicker(LLPanelNetwork::setSoundCacheCallback,
						 user_data))->getDir(&suggestion);
}

// static
void LLPanelNetwork::onClickResetSoundCache(void* user_data)
{
	LLPanelNetwork* self = (LLPanelNetwork*)user_data;
 	gSavedSettings.setString("Phoenixmm_sndcacheloc","");
	self->childSetText("sound_cache_location",std::string("None"));
}

void LLPanelNetwork::setChatLogsCallback(std::string& dir_name, void* data)
{
	LLPanelNetwork* self = (LLPanelNetwork*)data;
	if (!self || self != sInstance)
	{
		LLNotifications::instance().add("PreferencesClosed");
		return;
	}

	std::string cur_name = gSavedSettings.getString("InstantMessageLogPath");
	if (!dir_name.empty() && dir_name != cur_name)
	{
		gSavedPerAccountSettings.setString("InstantMessageLogPath",dir_name);
		LLNotifications::instance().add("Restart4SettingsChange");
		self->childSetText("chat_logs_location", dir_name);
	}
}

// static
void LLPanelNetwork::onClickSetChatLogs(void* user_data)
{
	std::string suggestion = gSavedSettings.getString("InstantMessageLogPath");

	(new LLCallDirPicker(LLPanelNetwork::setChatLogsCallback,
						 user_data))->getDir(&suggestion);
}

// static
void LLPanelNetwork::onClickResetChatLogs(void* user_data)
{
	{
		LLPanelNetwork* self = (LLPanelNetwork*)user_data;
		//gDirUtilp->setChatLogsDir(gDirUtilp->getOSUserAppDir());
		gSavedPerAccountSettings.setString("InstantMessageLogPath",gDirUtilp->getChatLogsDir());
		self->childSetText("chat_logs_location", gDirUtilp->getChatLogsDir());
		LLNotifications::instance().add("Restart4SettingsChange");
	}
}

//static
void LLPanelNetwork::onClickBrowseUserSetting(void* user_data)
{
	gViewerWindow->getWindow()->openFile(gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS,""));
}
//static
void LLPanelNetwork::onClickBrowseChatLogs(void* user_data)
{
	gViewerWindow->getWindow()->openFile(gDirUtilp->getExpandedFilename(LL_PATH_CHAT_LOGS,""));
}
//static
void LLPanelNetwork::onClickBrowseCrashLogs(void* user_data)
{
	gViewerWindow->getWindow()->openFile(gDirUtilp->getExpandedFilename(LL_PATH_LOGS,""));
}
//static
void LLPanelNetwork::onClickBrowseCache(void* user_data)
{
	gViewerWindow->getWindow()->openFile(gDirUtilp->getExpandedFilename(LL_PATH_CACHE,""));
}
//static
void LLPanelNetwork::onClickBrowseSoundCache(void* user_data)
{
	std::string path = gDirUtilp->getExpandedFilename(MM_SNDLOC, "");
	if(path!="")gViewerWindow->getWindow()->openFile(path);
}

// static
void LLPanelNetwork::onCommitPort(LLUICtrl* ctrl, void* data)
{
  LLPanelNetwork* self = (LLPanelNetwork*)data;
  LLCheckBoxCtrl* check = (LLCheckBoxCtrl*)ctrl;

  if (!self || !check) return;
  self->childSetEnabled("connection_port", check->get());
  LLNotifications::instance().add("ChangeConnectionPort");
}

// static
void LLPanelNetwork::onCommitSocks5ProxyEnabled(LLUICtrl* ctrl, void* data)
{
	LLPanelNetwork* self  = (LLPanelNetwork*)data;
	LLCheckBoxCtrl* check = (LLCheckBoxCtrl*)ctrl;

	if (!self || !check) return;

	sSocksSettingsChanged = true;
	
	updateProxyEnabled(self, check->get(), self->childGetValue("socks5_auth"));
}

// static
void LLPanelNetwork::onSocksSettingsModified(LLUICtrl* ctrl, void* data)
{
	sSocksSettingsChanged = true;
}

// static
void LLPanelNetwork::onSocksAuthChanged(LLUICtrl* ctrl, void* data)
{
	LLRadioGroup* radio  = static_cast<LLRadioGroup*>(ctrl);
	LLPanelNetwork* self = static_cast<LLPanelNetwork*>(data);

	sSocksSettingsChanged = true;

	std::string selection = radio->getValue().asString();
	updateProxyEnabled(self, true, selection);
}

// static
void LLPanelNetwork::updateProxyEnabled(LLPanelNetwork * self, bool enabled, std::string authtype)
{
	// Manage all the enable/disable of the socks5 options from this single function
	// to avoid code duplication


	// Update all socks labels and controls except auth specific ones
	self->childSetEnabled("socks5_proxy_port",  enabled);
	self->childSetEnabled("socks5_proxy_host",  enabled);
	self->childSetEnabled("socks5_host_label",  enabled);
	self->childSetEnabled("socks5_proxy_label", enabled);
	self->childSetEnabled("socks5_proxy_port",  enabled);
	self->childSetEnabled("socks5_auth_label",  enabled);
	self->childSetEnabled("socks5_auth",        enabled);

	// disable the web option if the web proxy has not been configured
	// this is still not ideal as apply or ok is needed for this to be saved to the preferences
	self->childSetEnabled("Web", gSavedSettings.getBOOL("BrowserProxyEnabled"));

	self->childSetEnabled("Socks", enabled);

	// Hide the auth specific lables if authtype is none or
	// we are not enabled.
	if ((authtype.compare("None") == 0) || (enabled == false))
	{
		self->childSetEnabled("socks5_username_label", false);
		self->childSetEnabled("socks5_password_label", false);
		self->childSetEnabled("socks5_proxy_username", false);
		self->childSetEnabled("socks5_proxy_password", false);
	}

	// Only show the username and password boxes if we are enabled
	// and authtype is username pasword.
	if ((authtype.compare("UserPass") == 0) && (enabled == true))
	{
		self->childSetEnabled("socks5_username_label", true);
		self->childSetEnabled("socks5_password_label", true);
		self->childSetEnabled("socks5_proxy_username", true);
		self->childSetEnabled("socks5_proxy_password", true);
	}
}
