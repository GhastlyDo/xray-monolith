////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_script.cpp
//	Created 	: 17.01.2008
//  Modified 	: 17.01.2008
//	Author		: Dmitriy Iassenev
//	Description : actor script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"          // Precompiled header file for script support
#include "actor.h"               // Header for the CActor class
#include "level_changer.h"       // Header for the CLevelChanger class
#include "ActorCondition.h"      // Header for the CActorCondition class

#include <luabind/luabind.hpp>

//using namespace luabind;         // Use the luabind namespace for Lua binding functionality, luabind is ambigus

#pragma optimize("s",on)          // Optimize for speed

// Function to register script bindings for CActor and CLevelChanger classes with Lua
void CActor::script_register(lua_State* L)
{
    // Register CActorCondition class with Lua
    CActorCondition::script_register(L);

    using namespace luabind;
    // Define Lua bindings for the CActor and CLevelChanger classes
    module(L)
        [
            // Register the CActor class with Lua
            class_<CActor, CGameObject>("CActor")
                .def(constructor<>())                 // Default constructor
                .def("conditions", &CActor::conditions) // Bind the conditions() method
                .def("inventory_disabled", &CActor::inventory_disabled) // Bind the inventory_disabled() method
                .def("set_inventory_disabled", &CActor::set_inventory_disabled) // Bind the set_inventory_disabled() method

#ifndef BENCHMARK_BUILD
                // If not in benchmark build, register the CLevelChanger class with Lua
                ,
                class_<CLevelChanger, CGameObject>("CLevelChanger")
                .def(constructor<>()) // Default constructor
#endif  // BENCHMARK_BUILD
        ];
}