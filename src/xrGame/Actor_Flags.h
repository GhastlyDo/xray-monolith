//Comments provided by ChatGPT
//GhastlyDo

#pragma once

// Define flags for actor-related settings and features using bitwise enumeration
enum
{
    AF_GODMODE = (1 << 0),  // God mode: the actor cannot be harmed
    AF_NO_CLIP = (1 << 1),  // No clipping: the actor can pass through walls
    AF_UNLIMITEDAMMO = (1 << 3),  // Unlimited ammo: the actor has infinite ammunition
    AF_RUN_BACKWARD = (1 << 4),  // Allows the actor to run backward
    AF_AUTOPICKUP = (1 << 5),  // Auto-pickup: items are picked up automatically
    AF_PSP = (1 << 6),  // Possibly a special feature or mode (abbreviation unclear)
    AF_DYNAMIC_MUSIC = (1 << 7),  // Dynamic music: music changes based on the game state
    AF_GODMODE_RT = (1 << 8),  // Runtime god mode: god mode that can be toggled during gameplay
    AF_IMPORTANT_SAVE = (1 << 9),  // Important save: indicates a crucial save point
    AF_CROUCH_TOGGLE = (1 << 10), // Allows toggling crouch mode
    AF_USE_TRACERS = (1 << 11), // Use tracers: visual indicators for bullets or projectiles
    AF_FIREPOS = (1 << 12), // Fire position: specific firing positions or modes
    AF_MULTI_ITEM_PICKUP = (1 << 13), // Multi-item pickup: allows picking up multiple items at once
    AF_WALK_TOGGLE = (1 << 14), // Allows toggling walk mode
    AF_SPRINT_TOGGLE = (1 << 15), // Allows toggling sprint mode
    AF_LOOKOUT_TOGGLE = (1 << 16), // Allows toggling lookout mode
    AF_FREELOOK_TOGGLE = (1 << 17), // Allows toggling free look mode
    AF_SIMPLE_PDA = (1 << 18), // Simple PDA mode: a simplified version of the PDA interface
    AF_AIM_TOGGLE = (1 << 19), // Allows toggling aim mode
    AF_3D_PDA = (1 << 20)  // 3D PDA mode: a three-dimensional version of the PDA interface
};

// Declaration of external variables and functions

// Flags32 is a 32-bit integer type used to store the state of actor flags
extern Flags32 psActorFlags;

// Function to check if god mode is enabled
extern BOOL GodMode();

// Integer variable representing the sleep time for the actor (likely in milliseconds)
extern int psActorSleepTime;