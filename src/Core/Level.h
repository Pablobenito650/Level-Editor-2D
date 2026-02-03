#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>

#include "Components.h"

namespace minieditor
{
    /**
     * @brief structure pour les tiles aidant a l'ecriture du fichier de mapping Tileset.txt
     */
    struct TileForTS
    {
        int mID = 0;
        std::string mName;
    };

    /**
     * @brief lumieres simples
     */
    struct Light
    {
        std::string mName  = "Light";
        SDL_Texture* mTexture;
        SDL_FRect mProps;
        int mFixedPosX;
        int mFixedPosY;
        SDL_Color mColorMod;
        bool mIsRayonnant = true;
        bool mActivate = true;
    };

    /**
     * @brief structure pour les tiles placees dans la scene
     */
    struct TileInScene : TileForTS
    {
        int mGridX;
        int mGridY;
        float mPosX;
        float mPosY;
        Animation mAnim;
        bool mIsSelected = false;
        bool mIsAnimating = false;
    };

    /**
     * @brief structure pour les tiles dans le tile piker 
     */
    struct Tile : TileForTS
    {
        SDL_Texture* mTexture;          
    };

    /**
     * @brief les differentes couches du niveau
     */
    struct Layer
    {
        std::string mName;
        std::vector<std::vector<int>> mGrid;
        std::vector<TileInScene> mHierarchie;
    };

    /**
     * @brief gestion camera et zoom
     */
    struct Camera
    {
        float x = 0.0f;
        float y = 0.0f;
        float mZoom = 1.0f;
        const float mZoomSpeed = 0.1f;
        const float mMinZoom = 0.5f;
        const float mMaxZoom = 2.0f;

    };

    /**
     * @brief Gestion des niveaux (Rendu et Actions)
     */
    class Level
    {
        public:
            static std::vector<Tile> sTileSet;
            static int sCurrentTileID;
            static int sCurrentLayerID;
            
            static std::vector<Layer> sLayers;
            static std::vector<Light> sLights;
            std::vector<int> mUsedIDs;
            static std::string sFileName;
            static const std::string SAVE_PATH;
            static const std::string TILE_SET_PATH;
            static const std::string EXTENSION; 

            static bool sModeSelection;
            static bool sIsLoading;
            static bool sSingleLayerRendering;
            static bool sCameraIsMoving;
            static bool sIsStartingNew;
            

            static int sGridHeight;
            static int sGridWidth;
            static int sTileSize;
            static float sLightSize;
            static int sLayerCount;
            static int sGlobalLightIntensity;
            
            SDL_Event event;
            Camera mCamera;
            SDL_Texture* mLightMap;

            static SDL_Texture* mLightTexture;

            std::string mTilesPath = "build/Assets/Tiles/";

            int mMouseX;
            int mMouseY;
            static int sIndex;  
            static int sAmbient;
            static int sNextID;         

            bool mPlace = false;

        public:
            /**
             * @brief Constructeur, ici pour les initialisations
             */
            Level();

            /**
             * @brief gere le zoom
             */
            void HandleZoom(Camera& camera, bool zoom);

            /**
             * @brief rendre le niveau a l'ecran
             * @param renderer rendu SDL
             */
            void Render(SDL_Renderer* renderer);

            /**
             * @brief rendre la grille d'editeur et les limites du niveau
             * @param renderer rendu SDL
             */
            void RenderBackGroundGrid(SDL_Renderer* renderer);

            /**
             * @brief mettre a jour l'editeur
             */
            void Update(float deltatime);

            /**
             * @brief placement une tile 
             */ 
            void TilePlacement();

            /**
             * @brief suppression d'une tile
             * @param gridX position x dans la grille
             * @param gridY position y dans la grille 
             */ 
            static void RemoveTile(int gridX, int gridY);

            /** 
             * @brief recuperatioin des coordonnees de la souris au clic 
             * @param event evenements SDL
             */             
            void MouseGetter(SDL_Event event);

            /**
             * @brief initialisation des couches
             */
            void InitLayers();

            /**
             * @brief initialisation de la liste de tiles
             * @param renderer Rendu SDL
             */
            void InitTiles(SDL_Renderer* renderer);

            /**
             * @brief sauvegarder un niveau dans un format texte (.txt)
             */
            static void SaveLevel();

            /**
             * @brief charger un niveau depuis le fichier txt
             */
            static void LoadLevel();

            /**
             * @brief ramener la camera a (0, 0)
             */
            void ResetCamera();

            /**
             * @brief verifier si un identifiant est deja utilise
             */
            bool IDIsUsed(int tileID);

            /**
             * @brief ecrire le fichier Tileset.txt (fichier qui fait le lien entre ID et iamge)
             */
            void WriteTileSet();

            /**
             * @brief recupere le numero (id) au debut du nom d'un fichier
             * @param name nom du fichier
             */
            int ExtractLeadingNumber(std::string name);

            /**
             * @brief bonne pratique : detruire les texture
             */
            void DestroyTextures();

            /**
             * @brief rechercher l'index correspondant a un id dans le tableau mtileSet
             */
            static int FindID(int tileID);

            /**
             * @brief rechercher l'index correspondant a la tuile qui a pour coordonne (x,y) dans la grille hierarchique (hierarchie) de la couche d'id layerID 
             */
            static int FindIDInScene(int layerID, int x, int y);

            /**
             * @brief ramener toutes les tiles dans la scene a "non seectionner"
             */
            static void ResetSelectionStatus();

            /**
             * @brief dessiner un carre au tour de la tile selectionnee
             */
            void DrawSelectionSquare(SDL_Renderer* renderer, int x, int y, float size);

            /**
             * @brief Initialisation de lumieres
             */
            bool InitLight(SDL_Renderer* renderer);

            /**
             * @brief rendu de lumieres 
             */
            void RenderLight(SDL_Renderer* renderer);

            /**
             * @brief Ajouter une lumiere
             */
            static void AddLight();

            /**
             * @brief supprimer une lumiere
             */
            static void RemoveLight(int index);

            /**
             * @brief demarer un nouveau niveau
             */
            static void StartNewLevel();

            /**
             * @brief Gestion de l'animation de chaque tile animee
             */
            void UpdateAnimations(float deltatime);

            /**
             * @brief Animer une tile
             */
            void Animate(float deltatime, TileInScene& tile);

    };
} // namespace minieditor



