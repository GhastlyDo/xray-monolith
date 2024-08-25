////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_memory.h
//	Created 	: 15.09.2005
//  Modified 	: 15.09.2005
//	Author		: Dmitriy Iassenev
//	Description : actor memory
////////////////////////////////////////////////////////////////////////////

//Comments provided by ChatGPT
//GhastlyDo

#pragma once

#include "vision_client.h"

// Forward declaration of the CActor class
class CActor;

// CActorMemory class inherits from vision_client
// It is used to manage and process memory-related functions for an actor
class CActorMemory : public vision_client
{
private:
    typedef vision_client inherited; // Alias for the base class type

private:
    CActor* m_actor; // Pointer to the CActor instance associated with this memory

public:
    // Constructor that initializes CActorMemory with a CActor pointer
    CActorMemory(CActor* actor);

    // Overridden method to determine if an object is relevant for vision-based perception
    virtual bool feel_vision_isRelevant(CObject* object);

    // Overridden method to set up camera parameters for the vision system
    virtual void camera(
        Fvector& position,         // Camera position
        Fvector& direction,        // Camera direction
        Fvector& normal,           // Camera normal
        float& field_of_view,      // Camera field of view
        float& aspect_ratio,       // Camera aspect ratio
        float& near_plane,         // Near clipping plane
        float& far_plane           // Far clipping plane
    );
};