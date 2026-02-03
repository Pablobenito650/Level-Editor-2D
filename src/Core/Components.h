#pragma once
#include <SDL3/SDL.h>

namespace minieditor
{
    /**
     * @brief Structure pour le component Animation (chaque frame est une image 32x32)
     * 
     * @param mFrameCount nombre de frame de l'animation
     * @param mCurrentFrame frame a afficher
     * @param mFrameTime duree de vie d'une frame
     * @param mTimer decompte avant de passer a une autre frame
     * @param mLoop definit si l'animation doit se jouer en boucle
     */
    struct Animation
    {
        int mFrameCount = 0;
        int mCurrentFrame = 0;
        float mFrameTime = 0.01f;
        float mTimer = 0.0f;
        bool mLoop = true;
    };

    class Components
    {
        public:
            static const float FRAME_WIDTH;
            
            /**
             * @brief Initialiser le composant Animation d'une tile (trouver le nombre de frames qui vont constitue l'animation)
             * 
             * @param texture l'atlas (texture constituee de plusieurs sprites > 2)
             * @param anim composant Animation 
             * @param w largeur deml'image
             * @param h hauteur de l'image
             */
            static void SetAnimationFromAtlas(SDL_Texture* texture, Animation& anim, float& w, float& h);
    };
}