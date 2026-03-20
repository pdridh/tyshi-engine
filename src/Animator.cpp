#include "Engine/Animator.h"

void Animator::update(double dt)
{
    Animation* anim = current();
    if (!anim) return;

    anim->frameTime += dt;
    if (anim->frameTime >= anim->frameDuration)
    {
        anim->frameTime    = 0;
        anim->currentFrame = (anim->currentFrame + 1) % anim->frames;
    }
}

void Animator::setTextureSheetForAllAnims(size_t textureSheetID)
{
    for (auto& [name, anim] : animations) anim.textureSheetID = textureSheetID;
}

Animation* Animator::current()
{
    auto it = animations.find(currentAnimationID);
    if (it == animations.end()) return nullptr;
    return &it->second;
}

const Animation* Animator::current() const
{
    auto it = animations.find(currentAnimationID);
    if (it == animations.end()) return nullptr;
    return &it->second;
}

void Animator::play(const std::string& id)
{
    assert(animations.count(id) &&
           "Animator::play - trying to play animation ID that does not exist");

    if (!animations.count(id))
    {
        printf("Animator::play — playing animation with animation ID '%s' that does not exist\n",
               id.c_str());
        return;
    }

    if (currentAnimationID == id)
    {
        return;
    }

    currentAnimationID          = id;
    animations[id].currentFrame = 0;
    animations[id].frameTime    = 0.0;
}
