#pragma once

#include <cassert>
#include <string>

#include "EntityState.h"

template <class EntityType>
class EntityStateMachine
{
private:
    EntityType& m_owner;

public:
    EntityState<EntityType>* currentState;
    EntityState<EntityType>* previousState;
    EntityState<EntityType>* globalState;

public:
    EntityStateMachine(EntityType& owner)
        : m_owner{owner}, currentState{nullptr}, previousState{nullptr}, globalState{nullptr}
    {
    }

    virtual ~EntityStateMachine() {}

    void update()
    {
        if (globalState != nullptr)
        {
            globalState->execute(m_owner);
        }

        if (currentState != nullptr)
        {
            currentState->execute(m_owner);
        }
    }

    void changeState(EntityState<EntityType>* newState)
    {
        assert(newState && "changing to null state");

        previousState = currentState;

        // Check if this is the first changeState call
        if (currentState != nullptr)
        {
            currentState->onExit(m_owner);
        }
        currentState = newState;
        currentState->onEnter(m_owner);
    }

    void revertToPreviousState() { changeState(previousState); }
};
