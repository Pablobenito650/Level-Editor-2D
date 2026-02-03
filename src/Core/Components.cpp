#include "Components.h"

namespace minieditor
{
    const float Components::FRAME_WIDTH = 32.0f;

    void Components::SetAnimationFromAtlas(SDL_Texture *texture, Animation& anim, float& w, float& h)
    {
        SDL_GetTextureSize(texture, &w, &h);

        anim.mFrameCount = static_cast<int>(w / FRAME_WIDTH);
    }
} // namespace minieditor
