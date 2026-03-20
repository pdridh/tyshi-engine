#pragma once

template <class EntityType>
class EntityState
{
public:
    virtual ~EntityState() {}

    virtual void onEnter(EntityType&) = 0;
    virtual void execute(EntityType&) = 0;
    virtual void onExit(EntityType&)  = 0;
};
