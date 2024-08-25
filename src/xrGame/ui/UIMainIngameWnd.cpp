/*
* ChatGPT Provided Comment's
* GhastlyDo
*/
#include "stdafx.h"

#include "UIMainIngameWnd.h"
#include "UIMessagesWindow.h"
#include "../UIZoneMap.h"


#include <dinput.h>
#include "../actor.h"
#include "../ActorCondition.h"
#include "../EntityCondition.h"
#include "../CustomOutfit.h"
#include "../ActorHelmet.h"
#include "../PDA.h"
#include "../xrServerEntities/character_info.h"
#include "../inventory.h"
#include "../UIGameSP.h"
#include "../weaponmagazined.h"
#include "../missile.h"
#include "../Grenade.h"
#include "../xrServerEntities/xrServer_objects_ALife.h"
#include "../alife_simulator.h"
#include "../alife_object_registry.h"
#include "../game_cl_base.h"
#include "../level.h"
#include "../seniority_hierarchy_holder.h"

#include "../date_time.h"
#include "../xrServerEntities/xrServer_Objects_ALife_Monsters.h"
#include "../../xrEngine/LightAnimLibrary.h"

#include "UIInventoryUtilities.h"
#include "UIHelper.h"
#include "UIMotionIcon.h"

#include "UIXmlInit.h"
#include "UIPdaMsgListItem.h"
#include "UIPdaWnd.h"
#include "../alife_registry_wrappers.h"

#include "../string_table.h"

#ifdef DEBUG
#	include "../attachable_item.h"
#	include "../../xrEngine/xr_input.h"
#endif

#include "UIScrollView.h"
#include "map_hint.h"
#include "../game_news.h"

#include "static_cast_checked.hpp"
#include "game_cl_capture_the_artefact.h"
#include "UIHudStatesWnd.h"
#include "UIActorMenu.h"

// Forward declarations of functions that are likely used later in this file.
void test_draw();  // Function for drawing test elements on the screen (likely for debugging or UI testing).
void test_key(int dik);  // Function to handle key input for testing purposes, 'dik' is likely the key code.

#include "../Include/xrRender/Kinematics.h" 
// Include directive for the Kinematics header, which is part of the rendering engine.
// This might include classes and functions related to skeletal animations or other kinematic processes.

using namespace InventoryUtilities;
// Using the InventoryUtilities namespace, which means that all symbols (functions, classes, etc.)
// from this namespace can be used without qualification.

const u32 g_clWhite = 0xffffffff;
// Define a constant color value representing white (fully opaque), using a 32-bit unsigned integer.

#define DEFAULT_MAP_SCALE 1.f
// Define a constant for the default map scale, set to 1.0f (no scaling).

#define C_SIZE 0.025f
// Define a constant for the size of some UI elements, possibly for scaling purposes.

#define NEAR_LIM 0.5f
// Define a constant for a "near limit" value, possibly used for near clipping or proximity checks.

#define SHOW_INFO_SPEED 0.5f
// Define a constant representing the speed at which information is shown on the screen (probably in UI animations).

#define HIDE_INFO_SPEED 10.f
// Define a constant for the speed at which information is hidden from the screen.

#define C_ON_ENEMY D3DCOLOR_XRGB(0xff,0,0)
// Define a color constant for enemies, set to bright red (using Direct3D color format).

#define C_DEFAULT D3DCOLOR_XRGB(0xff,0xff,0xff)
// Define a default color constant for UI elements, set to white (using Direct3D color format).

#define MAININGAME_XML "maingame.xml"
// Define a constant for the main in-game UI XML file name, which likely contains UI layout and settings.


// Constructor for the CUIMainIngameWnd class.
// This constructor initializes various member variables, setting some to NULL (indicating no object or not initialized).
CUIMainIngameWnd::CUIMainIngameWnd()
	:/*m_pGrenade(NULL),m_pItem(NULL),*/m_pPickUpItem(NULL), m_pMPChatWnd(NULL), UIArtefactIcon(NULL), m_pMPLogWnd(NULL)
{
	// Allocate and initialize a new CUIZoneMap object, which likely handles the UI map display in the game.
	UIZoneMap = xr_new<CUIZoneMap>();
}

#include "UIProgressShape.h" 
// Include directive for the UIProgressShape header, which might define UI elements like progress bars or shapes.

extern CUIProgressShape* g_MissileForceShape;
// Declare an external pointer to a CUIProgressShape object, likely used to represent missile forces or impacts in the UI.

// Destructor for the CUIMainIngameWnd class.
// This is responsible for cleaning up resources when an instance of this class is destroyed.
CUIMainIngameWnd::~CUIMainIngameWnd()
{
	// Destroy flashing icons that may be part of the HUD (Head-Up Display).
	// This likely involves removing or stopping any flashing effects.
	DestroyFlashingIcons();

	// Delete the UIZoneMap object to free up memory.
	// xr_delete is a custom deletion function (likely a safer delete that checks for null pointers).
	xr_delete(UIZoneMap);

	// Destroy the sound associated with contact (possibly a sound that plays when the player contacts something).
	// HUD_SOUND_ITEM::DestroySound ensures that the sound is properly stopped and memory is freed.
	HUD_SOUND_ITEM::DestroySound(m_contactSnd);

	// Delete the g_MissileForceShape object, which is likely used to represent missile forces or effects in the UI.
	xr_delete(g_MissileForceShape);

	// Delete the UIWeaponJammedIcon object, which represents an icon in the UI that indicates the player's weapon is jammed.
	xr_delete(UIWeaponJammedIcon);

	// Delete the UIInvincibleIcon object, which represents an icon indicating the player is invincible (God Mode).
	xr_delete(UIInvincibleIcon);

	// Delete the UIArtefactIcon object, which represents an icon for artefacts in the UI.
	xr_delete(UIArtefactIcon);
}

void CUIMainIngameWnd::Init()
{
	// Create an instance of CUIXml to handle the loading and parsing of the XML configuration for the UI.
	CUIXml uiXml;

	// Load the XML configuration file for the main in-game UI.
	// CONFIG_PATH and UI_PATH are likely constants that define the paths to the configuration directories.
	// MAININGAME_XML is the filename of the XML file to load.
	uiXml.Load(CONFIG_PATH, UI_PATH, MAININGAME_XML);

	// Create an instance of CUIXmlInit to initialize various UI elements based on the XML configuration.
	CUIXmlInit xml_init;

	// Initialize the main UI window using the XML data.
    // "main" specifies the root node in the XML file for the main window setup.
    // The `0` typically indicates no specific parameters or default values.
    // `this` refers to the current instance of `CUIMainIngameWnd`.
	xml_init.InitWindow(uiXml, "main", 0, this);

	// Initially disable the main in-game UI window.
    // This is often done to prevent the UI from being visible until it is fully initialized or ready.
	Enable(false);

	//	AttachChild					(&UIStaticHealth);	xml_init.InitStatic			(uiXml, "static_health", 0, &UIStaticHealth);
	//	AttachChild					(&UIStaticArmor);	xml_init.InitStatic			(uiXml, "static_armor", 0, &UIStaticArmor);
	//	AttachChild					(&UIWeaponBack);
	//	xml_init.InitStatic			(uiXml, "static_weapon", 0, &UIWeaponBack);

	/*	UIWeaponBack.AttachChild	(&UIWeaponSignAmmo);
		xml_init.InitStatic			(uiXml, "static_ammo", 0, &UIWeaponSignAmmo);
		UIWeaponSignAmmo.SetEllipsis	(CUIStatic::eepEnd, 2);
	
		UIWeaponBack.AttachChild	(&UIWeaponIcon);
		xml_init.InitStatic			(uiXml, "static_wpn_icon", 0, &UIWeaponIcon);
		UIWeaponIcon.SetShader		(GetEquipmentIconsShader());
		UIWeaponIcon_rect			= UIWeaponIcon.GetWndRect();
	*/ //---------------------------------------------------------

	// Create a static UI element for the pickup item icon using the XML configuration.
	// "pick_up_item" specifies the node in the XML for this element.
	UIPickUpItemIcon = UIHelper::CreateStatic(uiXml, "pick_up_item", this);

	// Set the shader for the pickup item icon to the one used for equipment icons.
	UIPickUpItemIcon->SetShader(GetEquipmentIconsShader());

	// Store the width of the pickup item icon.
	m_iPickUpItemIconWidth = UIPickUpItemIcon->GetWidth();
	// Store the height of the pickup item icon.
	m_iPickUpItemIconHeight = UIPickUpItemIcon->GetHeight();
	// Store the x-coordinate of the pickup item icon's position.
	m_iPickUpItemIconX = UIPickUpItemIcon->GetWndRect().left;
	// Store the y-coordinate of the pickup item icon's position.
	m_iPickUpItemIconY = UIPickUpItemIcon->GetWndRect().top;
	//---------------------------------------------------------

	//Indicators
	UIZoneMap->Init();

	//Attachments, which are visible when the player is in the menu
	UIStaticQuickHelp = UIHelper::CreateTextWnd(uiXml, "quick_info", this);

	// Set the local root of the XML document to the root element.
// This ensures that subsequent XML operations are relative to the root.
	uiXml.SetLocalRoot(uiXml.GetRoot());

	// Create a new scroll view for displaying icons and enable automatic deletion of this view.
	m_UIIcons = xr_new<CUIScrollView>();
	m_UIIcons->SetAutoDelete(true);

	// Initialize the scroll view based on the XML configuration under the "icons_scroll_view" node.
	// The `0` typically indicates default settings or no specific parameters.
	xml_init.InitScrollView(uiXml, "icons_scroll_view", 0, m_UIIcons);

	// Attach the initialized scroll view to the current UI window.
	AttachChild(m_UIIcons);

	// Create static UI elements for various status indicators using the XML configuration.
	m_ind_bleeding = UIHelper::CreateStatic(uiXml, "indicator_bleeding", this);
	m_ind_radiation = UIHelper::CreateStatic(uiXml, "indicator_radiation", this);
	m_ind_starvation = UIHelper::CreateStatic(uiXml, "indicator_starvation", this);
	m_ind_weapon_broken = UIHelper::CreateStatic(uiXml, "indicator_weapon_broken", this);
	m_ind_helmet_broken = UIHelper::CreateStatic(uiXml, "indicator_helmet_broken", this);
	m_ind_outfit_broken = UIHelper::CreateStatic(uiXml, "indicator_outfit_broken", this);
	m_ind_overweight = UIHelper::CreateStatic(uiXml, "indicator_overweight", this);

	// Create static UI elements for various booster indicators using the XML configuration.
	m_ind_boost_psy = UIHelper::CreateStatic(uiXml, "indicator_booster_psy", this);
	m_ind_boost_radia = UIHelper::CreateStatic(uiXml, "indicator_booster_radia", this);
	m_ind_boost_chem = UIHelper::CreateStatic(uiXml, "indicator_booster_chem", this);
	m_ind_boost_wound = UIHelper::CreateStatic(uiXml, "indicator_booster_wound", this);
	m_ind_boost_weight = UIHelper::CreateStatic(uiXml, "indicator_booster_weight", this);
	m_ind_boost_health = UIHelper::CreateStatic(uiXml, "indicator_booster_health", this);
	m_ind_boost_power = UIHelper::CreateStatic(uiXml, "indicator_booster_power", this);
	m_ind_boost_rad = UIHelper::CreateStatic(uiXml, "indicator_booster_rad", this);

	// Initially hide all booster indicators.
	m_ind_boost_psy->Show(false);
	m_ind_boost_radia->Show(false);
	m_ind_boost_chem->Show(false);
	m_ind_boost_wound->Show(false);
	m_ind_boost_weight->Show(false);
	m_ind_boost_health->Show(false);
	m_ind_boost_power->Show(false);
	m_ind_boost_rad->Show(false);

	//Load icons
	/*	if ( IsGameTypeSingle() )
		{
			xml_init.InitStatic		(uiXml, "starvation_static", 0, &UIStarvationIcon);
			UIStarvationIcon.Show	(false);
	
	//		xml_init.InitStatic		(uiXml, "psy_health_static", 0, &UIPsyHealthIcon);
	//		UIPsyHealthIcon.Show	(false);
		}
	*/

	// Create a static UI element for the weapon jammed icon using the XML configuration.
	// "weapon_jammed_static" specifies the node in the XML for this element.
	UIWeaponJammedIcon = UIHelper::CreateStatic(uiXml, "weapon_jammed_static", NULL);

	// Initially hide the weapon jammed icon.
	// This is often done to keep the icon hidden until it needs to be displayed based on game conditions.
	UIWeaponJammedIcon->Show(false);

	//	xml_init.InitStatic			(uiXml, "radiation_static", 0, &UIRadiaitionIcon);
	//	UIRadiaitionIcon.Show		(false);

	//	xml_init.InitStatic			(uiXml, "wound_static", 0, &UIWoundIcon);
	//	UIWoundIcon.Show			(false);

	// Create a static UI element for the invincibility icon using the XML configuration.
	// "invincible_static" specifies the node in the XML for this element.
	UIInvincibleIcon = UIHelper::CreateStatic(uiXml, "invincible_static", NULL);
	// Initially hide the invincibility icon.
	// This ensures the icon is not visible until it needs to be shown based on game conditions.
	UIInvincibleIcon->Show(false);
	
	// Check if the current game ID is either Artefact Hunt or Capture the Artefact.
	// These are specific game modes that require a special UI element for artefacts.
	if ((GameID() == eGameIDArtefactHunt) || (GameID() == eGameIDCaptureTheArtefact))
	{
		// Create a static UI element for the artefact icon using the XML configuration.
		// "artefact_static" specifies the node in the XML for this element.
		UIArtefactIcon = UIHelper::CreateStatic(uiXml, "artefact_static", NULL);
		// Initially hide the artefact icon.
		// This ensures the icon is not visible until it needs to be shown based on game conditions.
		UIArtefactIcon->Show(false);
	}

	// Define an array of strings to represent different warning indicators.
	// These strings correspond to various game status conditions that will be used for managing and displaying warnings.
	shared_str warningStrings[7] =
	{
		"jammed",      // Indicates a weapon is jammed
		"radiation",   // Indicates radiation exposure
		"wounds",      // Indicates wounds or injuries
		"starvation",  // Indicates starvation or lack of food
		"fatigue",     // Indicates fatigue or tiredness
		"invincible",  // Indicates invincibility or immunity
		"artefact"     // Indicates an artefact is present
	};

	//Load threshold values for indicators
	EWarningIcons j = ewiWeaponJammed;
	while (j < ewiInvincible)
	{
		// Retrieve the threshold values for the current indicator.
		// `pSettings->r_string` fetches the configuration record for the current indicator type.
		// `warningStrings[static_cast<int>(j) - 1]` provides the name of the indicator from the array of warning strings.
		shared_str cfgRecord = pSettings->r_string("main_ingame_indicators_thresholds",
		                                           *warningStrings[static_cast<int>(j) - 1]);
		// Get the number of threshold values in the configuration record.
		u32 count = _GetItemCount(*cfgRecord);
		char singleThreshold[8];

		// Iterate over each threshold value in the configuration record.
		float f = 0;
		for (u32 k = 0; k < count; ++k)
		{
			// Extract the threshold value from the configuration record.
			_GetItem(*cfgRecord, k, singleThreshold);
			// Convert the extracted string value to a float.
			sscanf(singleThreshold, "%f", &f);
			// Add the threshold value to the list of thresholds for the current indicator.
			m_Thresholds[j].push_back(f);
		}
		// Move to the next indicator type.
		j = static_cast<EWarningIcons>(j + 1);
	}


	// Flashing icons initialize
	uiXml.SetLocalRoot(uiXml.NavigateToNode("flashing_icons"));
	InitFlashingIcons(&uiXml);

	//ChatGPT Got Lazy here :)
	uiXml.SetLocalRoot(uiXml.GetRoot());

	UIMotionIcon = xr_new<CUIMotionIcon>();
	UIMotionIcon->SetAutoDelete(true);
	UIZoneMap->MapFrame().AttachChild(UIMotionIcon);
	UIMotionIcon->Init(UIZoneMap->MapFrame().GetWndRect());

	UIStaticDiskIO = UIHelper::CreateStatic(uiXml, "disk_io", this);

	m_ui_hud_states = xr_new<CUIHudStatesWnd>();
	m_ui_hud_states->SetAutoDelete(true);
	AttachChild(m_ui_hud_states);
	m_ui_hud_states->InitFromXml(uiXml, "hud_states");

	for (int i = 0; i < 4; i++)
	{ 
		// Create a new static UI element for each quick slot icon.
		m_quick_slots_icons.push_back(xr_new<CUIStatic>());
		m_quick_slots_icons.back()->SetAutoDelete(true); // Enable automatic deletion when no longer needed.
		AttachChild(m_quick_slots_icons.back()); // Attach the quick slot icon to the main UI window.
		// Format the path for the quick slot icon.
		string32 path;
		xr_sprintf(path, "quick_slot%d", i);
		// Initialize the quick slot icon with the XML configuration.
		CUIXmlInit::InitStatic(uiXml, path, 0, m_quick_slots_icons.back());
		// Format the path for the quick slot counter and create it.
		xr_sprintf(path, "%s:counter", path);
		UIHelper::CreateStatic(uiXml, path, m_quick_slots_icons.back());
	}

	// Create text windows for each quick slot to display associated text.
	m_QuickSlotText1 = UIHelper::CreateTextWnd(uiXml, "quick_slot0_text", this);
	m_QuickSlotText2 = UIHelper::CreateTextWnd(uiXml, "quick_slot1_text", this);
	m_QuickSlotText3 = UIHelper::CreateTextWnd(uiXml, "quick_slot2_text", this);
	m_QuickSlotText4 = UIHelper::CreateTextWnd(uiXml, "quick_slot3_text", this);

	// Load a new contact sound for the HUD from the specified configuration.
	// "maingame_ui" is the sound set and "snd_new_contact" is the specific sound to be loaded.
	// SOUND_TYPE_IDLE indicates the sound type is idle.
	HUD_SOUND_ITEM::LoadSound("maingame_ui", "snd_new_contact", m_contactSnd, SOUND_TYPE_IDLE);
}

// Time when the disk I/O indicator started showing
float UIStaticDiskIO_start_time = 0.0f;

void CUIMainIngameWnd::Draw()
{
	// Get the current view entity from the level and cast it to CActor.
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());

	// Determine if the disk I/O indicator should be active.
	bool IOActive = (FS.dwOpenCounter > 0);
	if (IOActive)
		UIStaticDiskIO_start_time = Device.fTimeGlobal; // Update the start time if I/O is active

	// Check if the I/O indicator should be hidden based on the elapsed time.
	if ((UIStaticDiskIO_start_time + 1.0f) < Device.fTimeGlobal)
		UIStaticDiskIO->Show(false); // Hide the indicator if more than 1 second has passed
	else
	{
		// Calculate the alpha transparency based on the elapsed time
		u32 alpha = clampr(iFloor(255.f * (1.f - (Device.fTimeGlobal - UIStaticDiskIO_start_time) / 1.f)), 0, 255);
		UIStaticDiskIO->Show(true); // Ensure the indicator is shown
		UIStaticDiskIO->SetTextureColor(color_rgba(255, 255, 255, alpha)); // Set the texture color with calculated alpha
	}
	FS.dwOpenCounter = 0; // Reset the open counter

	// If the game is not in single-player mode
	if (!IsGameTypeSingle())
	{
		// Get the current luminosity of the entity
		float luminocity = smart_cast<CGameObject*>(Level().CurrentEntity())->ROS()->get_luminocity();
		float power = log(luminocity > .001f ? luminocity : .001f) * (1.f/*luminocity_factor*/); // Calculate power
		luminocity = exp(power); // Recalculate luminosity

		// Smoothly update the current luminosity
		static float cur_lum = luminocity;
		cur_lum = luminocity * 0.01f + cur_lum * 0.99f;
		UIMotionIcon->SetLuminosity((s16)iFloor(cur_lum * 100.0f)); // Update motion icon luminosity
	}

	// Return if the actor is null or not alive
	if (!pActor || !pActor->g_Alive())
		return;

	// Update the noise level of the motion icon based on the actor's sound noise
	UIMotionIcon->SetNoise((s16)(0xffff & iFloor(pActor->m_snd_noise * 100)));

	// Draw the motion icon
	UIMotionIcon->Draw();

	// Make the zone map visible and render it
	UIZoneMap->visible = true;
	UIZoneMap->Render();

	// Temporarily hide the motion icon while drawing the main window
	bool tmp = UIMotionIcon->IsShown();
	UIMotionIcon->Show(false);
	CUIWindow::Draw(); // Call the base class draw method

	// Restore the motion icon visibility
	UIMotionIcon->Show(tmp);

	// Render quick info elements
	RenderQuickInfos();
}


void CUIMainIngameWnd::SetMPChatLog(CUIWindow* pChat, CUIWindow* pLog)
{
	// Set the multiplayer chat window pointer
	m_pMPChatWnd = pChat;
	// Set the multiplayer log window pointer
	m_pMPLogWnd = pLog;
}

void CUIMainIngameWnd::Update()
{
	// Call the base class's Update method to ensure any base class updates are performed.
	CUIWindow::Update();

	// Get the current actor (player character) from the level.
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());

	// If a multiplayer chat window exists, update it.
	if (m_pMPChatWnd)
		m_pMPChatWnd->Update();

	// If a multiplayer log window exists, update it.
	if (m_pMPLogWnd)
		m_pMPLogWnd->Update();

	// If no actor is available, exit the function early.
	if (!pActor)
		return;

	// Update the zone map UI.
	UIZoneMap->Update();

	// Update the pickup item UI.
	UpdatePickUpItem();

	// Only proceed with the following logic every 10 frames.
	if (Device.dwFrame % 10)
		return;

	// Determine which player to look at (local player or another player in demo play).
	game_PlayerState* lookat_player = Game().local_player;

	if (Level().IsDemoPlayStarted())
	{
		lookat_player = Game().lookat_player();
	}

	// Determine whether the player is in "God Mode" or is invincible.
	// GodMode() checks if God Mode is globally enabled for the player.
	// If the local_player pointer is null, b_God is set to true to avoid null pointer issues.
	// If GodMode is false and lookat_player is valid, check if the player has the invincible flag set.
	bool b_God = (GodMode() || (!lookat_player)) ? true : lookat_player->testFlag(GAME_PLAYER_FLAG_INVINCIBLE);

	// Set the warning icon color based on whether the player is invincible.
	if (b_God)
	{
		SetWarningIconColor(ewiInvincible, 0xffffffff); // Set icon color to opaque white.
	}
	else
	{
		SetWarningIconColor(ewiInvincible, 0x00ffffff); // Set icon color to transparent white.
	}

	// Update the main indicators on the UI.
	UpdateMainIndicators();

	// If the game is single-player, exit the function early.
	if (IsGameTypeSingle())
		return;

	// Handle the artefact icon update based on the game type.
	if (GameID() == eGameIDArtefactHunt)
	{
		// Check if the player has an artefact equipped.
		bool b_Artefact = !!(pActor->inventory().ItemFromSlot(ARTEFACT_SLOT));

		// Set the artefact warning icon color based on whether the player has an artefact.
		if (b_Artefact)
		{
			SetWarningIconColor(ewiArtefact, 0xffffff00); // Set icon color to yellow.
		}
		else
		{
			SetWarningIconColor(ewiArtefact, 0x00ffffff); // Set icon color to transparent white.
		}
	}
	else if (GameID() == eGameIDCaptureTheArtefact)
	{
		// For Capture the Artefact game mode, update the artefact icon color based on the artefact owner.
		game_cl_CaptureTheArtefact* cta_game = static_cast_checked<game_cl_CaptureTheArtefact*>(&Game());
		R_ASSERT(cta_game); // Ensure the game instance is valid.
		R_ASSERT(lookat_player); // Ensure the lookat_player is valid.

		// Check if the actor is the current owner of the artefact.
		if ((pActor->ID() == cta_game->GetGreenArtefactOwnerID()) ||
			(pActor->ID() == cta_game->GetBlueArtefactOwnerID()))
		{
			SetWarningIconColor(ewiArtefact, 0xffff0000); // Set icon color to red (indicating the player is holding the artefact).
		}
		else if (pActor->inventory().ItemFromSlot(ARTEFACT_SLOT)) // Check if the player owns the artefact.
		{
			SetWarningIconColor(ewiArtefact, 0xff00ff00); // Set icon color to green (indicating the player owns the artefact).
		}
		else
		{
			SetWarningIconColor(ewiArtefact, 0x00ffffff); // Set icon color to transparent white.
		}
	}
}


void CUIMainIngameWnd::RenderQuickInfos()
{
	// Retrieve the current player character from the level.
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());

	// If no player character is available, exit the function.
	if (!pActor)
		return;

	// Static pointer to hold the object the player is currently looking at.
	static CGameObject* pObject = NULL;

	// Get the default action text for the object the player is looking at.
	LPCSTR actor_action = pActor->GetDefaultActionForObject();

	// Show or hide the quick help UI element based on whether an action is available.
	UIStaticQuickHelp->Show(NULL != actor_action);

	// Update the text of the quick help UI element if the action text has changed.
	if (NULL != actor_action)
	{
		if (stricmp(actor_action, UIStaticQuickHelp->GetText()))
			UIStaticQuickHelp->SetTextST(actor_action);
	}

	// Check if the object the player is looking at has changed.
	if (pObject != pActor->ObjectWeLookingAt())
	{
		// Update the quick help text and reset its color animation if the object has changed.
		UIStaticQuickHelp->SetTextST(actor_action ? actor_action : " ");
		UIStaticQuickHelp->ResetColorAnimation();

		// Update the static pointer to reference the new object.
		pObject = pActor->ObjectWeLookingAt();
	}
}

void CUIMainIngameWnd::ReceiveNews(GAME_NEWS_DATA* news)
{
	// Verify that the texture name provided in the news data is not empty.
	VERIFY(news->texture_name.size());

	// Add the news message to the PDA messages window with an associated icon.
	CurrentGameUI()->m_pMessagesWnd->AddIconedPdaMessage(news);

	// Update the PDA to reflect the new news message.
	CurrentGameUI()->UpdatePda();
}

void CUIMainIngameWnd::SetWarningIconColorUI(CUIStatic* s, const u32 cl)
{
	// Extract the alpha component from the color value (assuming the format is ARGB).
	int bOn = (cl >> 24);

	// Check if the static UI element is currently visible.
	bool bIsShown = s->IsShown();

	// If the alpha component is non-zero (bOn), set the texture color of the static UI element.
	if (bOn)
	{
		s->SetTextureColor(cl);
	}

	// If the alpha component is non-zero and the element is not currently shown,
	// add the element to the UIIcons container and make it visible.
	if (bOn && !bIsShown)
	{
		m_UIIcons->AddWindow(s, false);
		s->Show(true);
	}

	// If the alpha component is zero and the element is currently shown,
	// remove the element from the UIIcons container and hide it.
	if (!bOn && bIsShown)
	{
		m_UIIcons->RemoveWindow(s);
		s->Show(false);
	}
}

void CUIMainIngameWnd::SetWarningIconColor(EWarningIcons icon, const u32 cl)
{
	// Variable to control whether subsequent cases in the switch statement should be executed.
	bool bMagicFlag = true;

	// Handle different warning icon types.
	switch (icon)
	{
	case ewiAll:
		// If the icon type is 'All', set bMagicFlag to false to prevent other icons from being updated.
		bMagicFlag = false;

	case ewiWeaponJammed:
		// Set the color for the 'Weapon Jammed' icon.
		SetWarningIconColorUI(UIWeaponJammedIcon, cl);
		// If bMagicFlag is true, exit the switch statement to prevent further updates.
		if (bMagicFlag) break;

		/* The following cases are commented out in this implementation, but they would handle
		   setting colors for other warning icons if they were included in the enum and implemented.

	case ewiRadiation:
		// Set the color for the 'Radiation' icon.
		SetWarningIconColorUI(&UIRadiationIcon, cl);
		if (bMagicFlag) break;

	case ewiWound:
		// Set the color for the 'Wound' icon.
		SetWarningIconColorUI(&UIWoundIcon, cl);
		if (bMagicFlag) break;

	case ewiStarvation:
		// Set the color for the 'Starvation' icon.
		SetWarningIconColorUI(&UIStarvationIcon, cl);
		if (bMagicFlag) break;

	case ewiPsyHealth:
		// Set the color for the 'Psy Health' icon.
		SetWarningIconColorUI(&UIPsyHealthIcon, cl);
		if (bMagicFlag) break;
		*/

	case ewiInvincible:
		// Set the color for the 'Invincible' icon.
		SetWarningIconColorUI(UIInvincibleIcon, cl);
		if (bMagicFlag) break;

		// The break here ensures that execution does not fall through to the next case.
		break;

	case ewiArtefact:
		// Set the color for the 'Artefact' icon.
		SetWarningIconColorUI(UIArtefactIcon, cl);
		break;

	default:
		// Assert that an unknown warning icon type should not occur.
		R_ASSERT(!"Unknown warning icon type");
	}
}

void CUIMainIngameWnd::TurnOffWarningIcon(EWarningIcons icon)
{
	// Set the specified warning icon to be fully transparent (invisible).
	// The color 0x00ffffff represents white with 0% opacity.
	SetWarningIconColor(icon, 0x00ffffff);
}

void CUIMainIngameWnd::SetFlashIconState_(EFlashingIcons type, bool enable)
{
	// Find the flashing icon by its type in the map of flashing icons
	FlashingIcons_it icon = m_FlashingIcons.find(type);

	// Ensure that the flashing icon with the specified type exists
	R_ASSERT2(icon != m_FlashingIcons.end(), "Flashing icon with this type does not exist");

	// Set the visibility state of the flashing icon
	icon->second->Show(enable);
}

void CUIMainIngameWnd::InitFlashingIcons(CUIXml* node)
{
	// Name of the XML node representing a flashing icon
	const char* const flashingIconNodeName = "flashing_icon";

	// Get the number of nodes with the name 'flashing_icon'
	int staticsCount = node->GetNodesNum("", 0, flashingIconNodeName);

	// Initialize an XML utility object
	CUIXmlInit xml_init;
	CUIStatic* pIcon = NULL;

	// Iterate through all nodes of type 'flashing_icon' and initialize each one
	for (int i = 0; i < staticsCount; ++i)
	{
		// Create a new static UI element for the flashing icon
		pIcon = xr_new<CUIStatic>();

		// Initialize the static UI element using XML data
		xml_init.InitStatic(*node, flashingIconNodeName, i, pIcon);

		// Read the 'type' attribute of the flashing icon from XML; default to "none" if not present
		shared_str iconType = node->ReadAttrib(flashingIconNodeName, i, "type", "none");

		// Determine the type of flashing icon based on the 'type' attribute
		EFlashingIcons type = efiPdaTask; // Default type

		if (iconType == "pda")
			type = efiPdaTask;
		else if (iconType == "mail")
			type = efiMail;
		else
			R_ASSERT(!"Unknown type of mainingame flashing icon"); // Error if the type is unknown

		// Assert that the icon type does not already exist in the map
		R_ASSERT2(m_FlashingIcons.find(type) == m_FlashingIcons.end(), "Flashing icon with this type already exists");

		// Store the initialized icon in the map using its type as the key
		CUIStatic*& val = m_FlashingIcons[type];
		val = pIcon;

		// Attach the new icon to the parent window
		AttachChild(pIcon);

		// Hide the icon initially
		pIcon->Show(false);
	}
}

void CUIMainIngameWnd::DestroyFlashingIcons()
{
	// Iterate through each entry in the map of flashing icons
	for (FlashingIcons_it it = m_FlashingIcons.begin(); it != m_FlashingIcons.end(); ++it)
	{
		// Detach the icon from its parent window
		DetachChild(it->second);

		// Delete the icon object to free memory
		xr_delete(it->second);
	}

	// Clear the map of flashing icons
	m_FlashingIcons.clear();
}

void CUIMainIngameWnd::UpdateFlashingIcons()
{
	// Iterate through each entry in the map of flashing icons
	for (FlashingIcons_it it = m_FlashingIcons.begin(); it != m_FlashingIcons.end(); ++it)
	{
		// Update each flashing icon
		it->second->Update();
	}
}

void CUIMainIngameWnd::AnimateContacts(bool b_snd)
{
	// Reset the color animation for the zone map counter
	UIZoneMap->Counter_ResetClrAnimation();

	// If the sound flag is true and the zone map is not disabled
	if (b_snd && !UIZoneMap->disabled)
	{
		// Play the contact sound at position (0, 0, 0) with no attenuation, looping enabled
		HUD_SOUND_ITEM::PlaySound(m_contactSnd, Fvector().set(0, 0, 0), 0, true);
	}
}


void CUIMainIngameWnd::SetPickUpItem(CInventoryItem* PickUpItem)
{
	// Set the internal pointer to the item being picked up
	m_pPickUpItem = PickUpItem;
}

void CUIMainIngameWnd::UpdatePickUpItem()
{
	// Check if there is a valid item to pick up, and if the current view entity is an actor
	if (!m_pPickUpItem || !Level().CurrentViewEntity() || !smart_cast<CActor*>(Level().CurrentViewEntity()))
	{
		// Hide the pick-up item icon if the conditions are not met
		UIPickUpItemIcon->Show(false);
		return;
	};

	// Get the section name of the item being picked up
	shared_str sect_name = m_pPickUpItem->object().cNameSect();

	// Retrieve the grid properties for the item from settings
	int m_iGridWidth = pSettings->r_u32(sect_name, "inv_grid_width");
	int m_iGridHeight = pSettings->r_u32(sect_name, "inv_grid_height");
	int m_iXPos = pSettings->r_u32(sect_name, "inv_grid_x");
	int m_iYPos = pSettings->r_u32(sect_name, "inv_grid_y");

	// Set the shader for the pick-up item icon based on the item's settings
	if (pSettings->line_exist(sect_name.c_str(), "icons_texture"))
	{
		LPCSTR icons_texture = pSettings->r_string(sect_name.c_str(), "icons_texture");
		UIPickUpItemIcon->SetShader(InventoryUtilities::GetCustomIconTextureShader(icons_texture));
	}
	else
	{
		UIPickUpItemIcon->SetShader(GetEquipmentIconsShader());
	}

	// Calculate the scale factors based on the icon and grid dimensions
	float scale_x = m_iPickUpItemIconWidth / float(m_iGridWidth * INV_GRID_WIDTH);
	float scale_y = m_iPickUpItemIconHeight / float(m_iGridHeight * INV_GRID_HEIGHT);

	// Use the smaller scale factor to maintain the aspect ratio
	float scale = (scale_x < scale_y) ? scale_x : scale_y;

	// Define the rectangle for the icon texture based on the item's grid settings
	Frect texture_rect;
	texture_rect.lt.set(m_iXPos * INV_GRID_WIDTH, m_iYPos * INV_GRID_HEIGHT);
	texture_rect.rb.set(m_iGridWidth * INV_GRID_WIDTH, m_iGridHeight * INV_GRID_HEIGHT);
	texture_rect.rb.add(texture_rect.lt);

	// Set the texture rectangle and stretch the texture
	UIPickUpItemIcon->GetStaticItem()->SetTextureRect(texture_rect);
	UIPickUpItemIcon->SetStretchTexture(true);

	// Update the width and height of the icon with scaling applied
	UIPickUpItemIcon->SetWidth(m_iGridWidth * INV_GRID_WIDTH * scale * UI().get_current_kx());
	UIPickUpItemIcon->SetHeight(m_iGridHeight * INV_GRID_HEIGHT * scale);

	// Position the icon centrally within the allocated space
	UIPickUpItemIcon->SetWndPos(Fvector2().set(
		m_iPickUpItemIconX + (m_iPickUpItemIconWidth - UIPickUpItemIcon->GetWidth()) / 2.0f,
		m_iPickUpItemIconY + (m_iPickUpItemIconHeight - UIPickUpItemIcon->GetHeight()) / 2.0f));

	// Set the icon's texture color and make it visible
	UIPickUpItemIcon->SetTextureColor(color_rgba(255, 255, 255, 192));
	UIPickUpItemIcon->Show(true);
}

void CUIMainIngameWnd::OnConnected()
{
	// Initialize or update the map display to reflect the current state
	UIZoneMap->SetupCurrentMap();

	// Check if the HUD states window is valid
	if (m_ui_hud_states)
	{
		// Notify the HUD states window that the connection has been established
		m_ui_hud_states->on_connected();
	}
}

void CUIMainIngameWnd::OnSectorChanged(int sector)
{
	// Update the map display to reflect the new sector
	UIZoneMap->OnSectorChanged(sector);
}

void CUIMainIngameWnd::reset_ui()
{
	// Clear the item currently being picked up
	m_pPickUpItem = NULL;

	// Reset the visibility of the motion icon
	UIMotionIcon->ResetVisibility();

	// Reset the HUD states UI, if it exists
	if (m_ui_hud_states)
	{
		m_ui_hud_states->reset_ui();
	}
}

void CUIMainIngameWnd::ShowZoneMap(bool status)
{
	// Set the visibility of the zone map UI element
	UIZoneMap->visible = status;
}

void CUIMainIngameWnd::DrawZoneMap()
{
	// Render the zone map UI element
	UIZoneMap->Render();
}

void CUIMainIngameWnd::UpdateZoneMap()
{
	// Update the zone map UI element's state
	UIZoneMap->Update();
}

void CUIMainIngameWnd::UpdateMainIndicators()
{
	// Retrieve the current actor from the level's view entity
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!pActor)
		return;

	// Update quick slots (not shown in this snippet but presumably updates UI elements related to item quick slots)
	UpdateQuickSlots();

	// If the game is single-player, update the PDA menu ranking window
	if (IsGameTypeSingle())
		CurrentGameUI()->GetPdaMenu().UpdateRankingWnd();

	// Define flags for color animation
	u8 flags = 0;
	flags |= LA_CYCLIC;       // Animation should cycle
	flags |= LA_ONLYALPHA;    // Only alpha (transparency) should change
	flags |= LA_TEXTURECOLOR; // Texture color should change

	// ** Bleeding Icon **
	// Check the bleeding speed of the actor
	float bleeding = pActor->conditions().BleedingSpeed();
	if (fis_zero(bleeding, EPS)) // If bleeding speed is effectively zero
	{
		m_ind_bleeding->Show(false); // Hide the bleeding indicator
		m_ind_bleeding->ResetColorAnimation(); // Reset the color animation
	}
	else
	{
		m_ind_bleeding->Show(true); // Show the bleeding indicator
		// Determine the texture and animation based on the bleeding intensity
		if (bleeding < 0.35f)
		{
			m_ind_bleeding->InitTexture("ui_inGame2_circle_bloodloose_green");
			m_ind_bleeding->SetColorAnimation("ui_slow_blinking_alpha", flags);
		}
		else if (bleeding < 0.7f)
		{
			m_ind_bleeding->InitTexture("ui_inGame2_circle_bloodloose_yellow");
			m_ind_bleeding->SetColorAnimation("ui_medium_blinking_alpha", flags);
		}
		else
		{
			m_ind_bleeding->InitTexture("ui_inGame2_circle_bloodloose_red");
			m_ind_bleeding->SetColorAnimation("ui_fast_blinking_alpha", flags);
		}
	}

	// ** Radiation Icon **
	// Check the radiation level of the actor
	float radiation = pActor->conditions().GetRadiation();
	if (fis_zero(radiation, EPS)) // If radiation level is effectively zero
	{
		m_ind_radiation->Show(false); // Hide the radiation indicator
		m_ind_radiation->ResetColorAnimation(); // Reset the color animation
	}
	else
	{
		m_ind_radiation->Show(true); // Show the radiation indicator
		// Determine the texture and animation based on the radiation intensity
		if (radiation < 0.35f)
		{
			m_ind_radiation->InitTexture("ui_inGame2_circle_radiation_green");
			m_ind_radiation->SetColorAnimation("ui_slow_blinking_alpha", flags);
		}
		else if (radiation < 0.7f)
		{
			m_ind_radiation->InitTexture("ui_inGame2_circle_radiation_yellow");
			m_ind_radiation->SetColorAnimation("ui_medium_blinking_alpha", flags);
		}
		else
		{
			m_ind_radiation->InitTexture("ui_inGame2_circle_radiation_red");
			m_ind_radiation->SetColorAnimation("ui_fast_blinking_alpha", flags);
		}
	}

	// ** Satiety Icon **
	// Check the actor's satiety level and its critical threshold
	float satiety = pActor->conditions().GetSatiety();
	float satiety_critical = pActor->conditions().SatietyCritical();
	float satiety_koef = (satiety - satiety_critical) / (satiety >= satiety_critical
		? 1 - satiety_critical
		: satiety_critical);
	if (satiety_koef > 0.5)
		m_ind_starvation->Show(false); // Hide the starvation indicator if satiety is high
	else
	{
		m_ind_starvation->Show(true); // Show the starvation indicator if satiety is low
		// Determine the texture based on the satiety coefficient
		if (satiety_koef > 0.0f)
			m_ind_starvation->InitTexture("ui_inGame2_circle_hunger_green");
		else if (satiety_koef > -0.5f)
			m_ind_starvation->InitTexture("ui_inGame2_circle_hunger_yellow");
		else
			m_ind_starvation->InitTexture("ui_inGame2_circle_hunger_red");
	}

	// ** Armor Broken Icon **
	// Check the condition of the actor's outfit (armor)
	CCustomOutfit* outfit = smart_cast<CCustomOutfit*>(pActor->inventory().ItemFromSlot(OUTFIT_SLOT));
	m_ind_outfit_broken->Show(false); // Hide the icon by default
	if (outfit)
	{
		float condition = outfit->GetCondition();
		if (condition < 0.75f) // Show the armor broken indicator if the condition is low
		{
			m_ind_outfit_broken->Show(true);
			// Determine the texture based on the armor condition
			if (condition > 0.5f)
				m_ind_outfit_broken->InitTexture("ui_inGame2_circle_Armorbroken_green");
			else if (condition > 0.25f)
				m_ind_outfit_broken->InitTexture("ui_inGame2_circle_Armorbroken_yellow");
			else
				m_ind_outfit_broken->InitTexture("ui_inGame2_circle_Armorbroken_red");
		}
	}

	// ** Helmet Broken Icon **
	// Check the condition of the actor's helmet
	CHelmet* helmet = smart_cast<CHelmet*>(pActor->inventory().ItemFromSlot(HELMET_SLOT));
	m_ind_helmet_broken->Show(false); // Hide the icon by default
	if (helmet)
	{
		float condition = helmet->GetCondition();
		if (condition < 0.75f) // Show the helmet broken indicator if the condition is low
		{
			m_ind_helmet_broken->Show(true);
			// Determine the texture based on the helmet condition
			if (condition > 0.5f)
				m_ind_helmet_broken->InitTexture("ui_inGame2_circle_Helmetbroken_green");
			else if (condition > 0.25f)
				m_ind_helmet_broken->InitTexture("ui_inGame2_circle_Helmetbroken_yellow");
			else
				m_ind_helmet_broken->InitTexture("ui_inGame2_circle_Helmetbroken_red");
		}
	}

	// ** Weapon Broken Icon **
	// Check the condition of the currently active weapon
	u16 slot = pActor->inventory().GetActiveSlot();
	m_ind_weapon_broken->Show(false); // Hide the icon by default
	if (slot == INV_SLOT_2 || slot == INV_SLOT_3)
	{
		CWeapon* weapon = smart_cast<CWeapon*>(pActor->inventory().ItemFromSlot(slot));
		if (weapon)
		{
			float condition = weapon->GetCondition();
			float start_misf_cond = weapon->GetMisfireStartCondition();
			float end_misf_cond = weapon->GetMisfireEndCondition();
			if (condition < start_misf_cond) // Show the weapon broken indicator if the condition is below the misfire start condition
			{
				m_ind_weapon_broken->Show(true);
				// Determine the texture based on the weapon condition
				if (condition > (start_misf_cond + end_misf_cond) / 2)
					m_ind_weapon_broken->InitTexture("ui_inGame2_circle_Gunbroken_green");
				else if (condition > end_misf_cond)
					m_ind_weapon_broken->InitTexture("ui_inGame2_circle_Gunbroken_yellow");
				else
					m_ind_weapon_broken->InitTexture("ui_inGame2_circle_Gunbroken_red");
			}
		}
	}

	// ** Overweight Icon **
	// Check if the actor's current weight is close to or exceeds the maximum allowable weight
	float cur_weight = pActor->inventory().TotalWeight();
	float max_weight = pActor->MaxWalkWeight();
	m_ind_overweight->Show(false); // Hide the icon by default
	if (cur_weight >= max_weight - 10.0f && IsGameTypeSingle())
	{
		m_ind_overweight->Show(true);
		// Determine the texture based on whether the weight exceeds the maximum allowable weight
		if (cur_weight > max_weight)
			m_ind_overweight->InitTexture("ui_inGame2_circle_Overweight_red");
		else
			m_ind_overweight->InitTexture("ui_inGame2_circle_Overweight_yellow");
	}
}


void CUIMainIngameWnd::UpdateQuickSlots()
{
	// Temporary buffer for storing translated strings
	string32 tmp;

	// Update the text for each quick slot (1 to 4) based on translated strings
	LPCSTR str = CStringTable().translate("quick_use_str_1").c_str();
	strncpy_s(tmp, sizeof(tmp), str, 3);  // Copy up to 3 characters
	if (tmp[2] == ',')
		tmp[1] = '\0';  // Truncate if the third character is a comma
	m_QuickSlotText1->SetTextST(tmp);

	str = CStringTable().translate("quick_use_str_2").c_str();
	strncpy_s(tmp, sizeof(tmp), str, 3);
	if (tmp[2] == ',')
		tmp[1] = '\0';
	m_QuickSlotText2->SetTextST(tmp);

	str = CStringTable().translate("quick_use_str_3").c_str();
	strncpy_s(tmp, sizeof(tmp), str, 3);
	if (tmp[2] == ',')
		tmp[1] = '\0';
	m_QuickSlotText3->SetTextST(tmp);

	str = CStringTable().translate("quick_use_str_4").c_str();
	strncpy_s(tmp, sizeof(tmp), str, 3);
	if (tmp[2] == ',')
		tmp[1] = '\0';
	m_QuickSlotText4->SetTextST(tmp);

	// Retrieve the current actor from the level's view entity
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!pActor)
		return;  // Exit if no valid actor is found

	// Iterate over the 4 quick slots to update their icons and counts
	for (u8 i = 0; i < 4; i++)
	{
		// Find the counter text element within the quick slot icon
		CUIStatic* wnd = smart_cast<CUIStatic*>(m_quick_slots_icons[i]->FindChild("counter"));
		if (wnd)
		{
			// Get the item name for the current quick slot
			shared_str item_name = g_quick_use_slots[i];
			if (item_name.size())
			{
				// Retrieve the count of the specified item from the actor's inventory
				u32 count = pActor->inventory().dwfGetSameItemCount(item_name.c_str(), true);
				string32 str;
				xr_sprintf(str, "x%d", count);  // Format the count as a string
				wnd->TextItemControl()->SetText(str);
				wnd->Show(true);  // Show the counter text element

				// Retrieve and set the texture and shader for the quick slot icon
				CUIStatic* main_slot = m_quick_slots_icons[i];

				if (pSettings->line_exist(item_name.c_str(), "icons_texture"))
				{
					LPCSTR icons_texture = pSettings->r_string(item_name.c_str(), "icons_texture");
					main_slot->SetShader(InventoryUtilities::GetCustomIconTextureShader(icons_texture));
				}
				else
					main_slot->SetShader(InventoryUtilities::GetEquipmentIconsShader());

				// Set the texture rectangle based on item settings
				Frect texture_rect;
				texture_rect.x1 = pSettings->r_float(item_name, "inv_grid_x") * INV_GRID_WIDTH;
				texture_rect.y1 = pSettings->r_float(item_name, "inv_grid_y") * INV_GRID_HEIGHT;
				texture_rect.x2 = pSettings->r_float(item_name, "inv_grid_width") * INV_GRID_WIDTH;
				texture_rect.y2 = pSettings->r_float(item_name, "inv_grid_height") * INV_GRID_HEIGHT;
				texture_rect.rb.add(texture_rect.lt);
				main_slot->SetTextureRect(texture_rect);
				main_slot->TextureOn();
				main_slot->SetStretchTexture(true);

				// Update the visibility and color of the counter and icon based on item count
				if (!count)
				{
					wnd->SetTextureColor(color_rgba(255, 255, 255, 0));  // Transparent if no items
					wnd->TextItemControl()->SetTextColor(color_rgba(255, 255, 255, 0));
					m_quick_slots_icons[i]->SetTextureColor(color_rgba(255, 255, 255, 100));  // Slightly visible
				}
				else
				{
					wnd->SetTextureColor(color_rgba(255, 255, 255, 255));  // Fully visible if items are present
					wnd->TextItemControl()->SetTextColor(color_rgba(255, 255, 255, 255));
					m_quick_slots_icons[i]->SetTextureColor(color_rgba(255, 255, 255, 255));  // Fully visible
				}
			}
			else
			{
				// Hide the counter and set icon to transparent if no item is assigned to the quick slot
				wnd->Show(false);
				m_quick_slots_icons[i]->SetTextureColor(color_rgba(255, 255, 255, 0));
				// Optionally hide the quick slot icon itself (commented out)
				// m_quick_slots_icons[i]->Show(false);
			}
		}
	}
}


void CUIMainIngameWnd::DrawMainIndicatorsForInventory()
{
	// Retrieve the current actor from the level's view entity
	CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (!pActor)
		return;  // Exit if no valid actor is found

	// Update the quick slot information and booster indicators
	UpdateQuickSlots();  // Refresh quick slot text and icons
	UpdateBoosterIndicators(pActor->conditions().GetCurBoosterInfluences());  // Refresh booster indicators based on current influences

	// Draw the quick slot icons
	for (int i = 0; i < 4; i++)
		m_quick_slots_icons[i]->Draw();  // Render each quick slot icon

	// Draw the text elements for quick slots
	m_QuickSlotText1->Draw();
	m_QuickSlotText2->Draw();
	m_QuickSlotText3->Draw();
	m_QuickSlotText4->Draw();

	// Conditionally update and draw each booster indicator if it is shown
	if (m_ind_boost_psy->IsShown())
	{
		m_ind_boost_psy->Update();
		m_ind_boost_psy->Draw();
	}

	if (m_ind_boost_radia->IsShown())
	{
		m_ind_boost_radia->Update();
		m_ind_boost_radia->Draw();
	}

	if (m_ind_boost_chem->IsShown())
	{
		m_ind_boost_chem->Update();
		m_ind_boost_chem->Draw();
	}

	if (m_ind_boost_wound->IsShown())
	{
		m_ind_boost_wound->Update();
		m_ind_boost_wound->Draw();
	}

	if (m_ind_boost_weight->IsShown())
	{
		m_ind_boost_weight->Update();
		m_ind_boost_weight->Draw();
	}

	if (m_ind_boost_health->IsShown())
	{
		m_ind_boost_health->Update();
		m_ind_boost_health->Draw();
	}

	if (m_ind_boost_power->IsShown())
	{
		m_ind_boost_power->Update();
		m_ind_boost_power->Draw();
	}

	if (m_ind_boost_rad->IsShown())
	{
		m_ind_boost_rad->Update();
		m_ind_boost_rad->Draw();
	}

	// Draw zone indicators from the HUD states
	m_ui_hud_states->DrawZoneIndicators();
}


void CUIMainIngameWnd::UpdateBoosterIndicators(const xr_map<EBoostParams, SBooster> influences)
{
	// Hide all booster indicators initially
	m_ind_boost_psy->Show(false);
	m_ind_boost_radia->Show(false);
	m_ind_boost_chem->Show(false);
	m_ind_boost_wound->Show(false);
	m_ind_boost_weight->Show(false);
	m_ind_boost_health->Show(false);
	m_ind_boost_power->Show(false);
	m_ind_boost_rad->Show(false);

	// Define the animation parameters for color change
	LPCSTR str_flag = "ui_slow_blinking_alpha";
	u8 flags = 0;
	flags |= LA_CYCLIC;          // Animation should loop
	flags |= LA_ONLYALPHA;       // Animation only affects alpha (transparency)
	flags |= LA_TEXTURECOLOR;    // Animation affects texture color

	// Iterate through each active booster influence
	xr_map<EBoostParams, SBooster>::const_iterator b = influences.begin(), e = influences.end();
	for (; b != e; b++)
	{
		// Based on the type of booster, update the corresponding indicator
		switch (b->second.m_type)
		{
		case eBoostHpRestore:
		{
			// Health restore booster
			m_ind_boost_health->Show(true);
			if (b->second.fBoostTime <= 3.0f)
				m_ind_boost_health->SetColorAnimation(str_flag, flags);  // Apply animation
			else
				m_ind_boost_health->ResetColorAnimation();  // Reset animation if boost time exceeds 3 seconds
		}
		break;
		case eBoostPowerRestore:
		{
			// Power restore booster
			m_ind_boost_power->Show(true);
			if (b->second.fBoostTime <= 3.0f)
				m_ind_boost_power->SetColorAnimation(str_flag, flags);
			else
				m_ind_boost_power->ResetColorAnimation();
		}
		break;
		case eBoostRadiationRestore:
		{
			// Radiation restore booster
			m_ind_boost_rad->Show(true);
			if (b->second.fBoostTime <= 3.0f)
				m_ind_boost_rad->SetColorAnimation(str_flag, flags);
			else
				m_ind_boost_rad->ResetColorAnimation();
		}
		break;
		case eBoostBleedingRestore:
		{
			// Bleeding restore booster
			m_ind_boost_wound->Show(true);
			if (b->second.fBoostTime <= 3.0f)
				m_ind_boost_wound->SetColorAnimation(str_flag, flags);
			else
				m_ind_boost_wound->ResetColorAnimation();
		}
		break;
		case eBoostMaxWeight:
		{
			// Max weight booster
			m_ind_boost_weight->Show(true);
			if (b->second.fBoostTime <= 3.0f)
				m_ind_boost_weight->SetColorAnimation(str_flag, flags);
			else
				m_ind_boost_weight->ResetColorAnimation();
		}
		break;
		case eBoostRadiationImmunity:
		case eBoostRadiationProtection:
		{
			// Radiation immunity or protection booster
			m_ind_boost_radia->Show(true);
			if (b->second.fBoostTime <= 3.0f)
				m_ind_boost_radia->SetColorAnimation(str_flag, flags);
			else
				m_ind_boost_radia->ResetColorAnimation();
		}
		break;
		case eBoostTelepaticImmunity:
		case eBoostTelepaticProtection:
		{
			// Telepathic immunity or protection booster
			m_ind_boost_psy->Show(true);
			if (b->second.fBoostTime <= 3.0f)
				m_ind_boost_psy->SetColorAnimation(str_flag, flags);
			else
				m_ind_boost_psy->ResetColorAnimation();
		}
		break;
		case eBoostChemicalBurnImmunity:
		case eBoostChemicalBurnProtection:
		{
			// Chemical burn immunity or protection booster
			m_ind_boost_chem->Show(true);
			if (b->second.fBoostTime <= 3.0f)
				m_ind_boost_chem->SetColorAnimation(str_flag, flags);
			else
				m_ind_boost_chem->ResetColorAnimation();
		}
		break;
		}
	}
}

/*
* ChatGPT Provided Comments :)
* GhastlyDo
*/
