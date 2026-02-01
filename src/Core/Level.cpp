#include "Level.h"

namespace minieditor
{
    // Definiions
    std::vector<Tile> Level::sTileSet;
    std::vector<Light> Level::sLights;
    int Level::sCurrentTileID = -1;
    int Level::sCurrentLayerID = 0;
    int Level::sAmbient = 255;
    std::vector<Layer> Level::sLayers;
    std::string Level::sFileName = "Level";
    const std::string Level::SAVE_PATH = "build/Levels/";
    const std::string Level::TILE_SET_PATH = "build/Assets/";
    const std::string Level::EXTENSION = ".txt";

    bool Level::sModeSelection= false;
    bool Level::sIsLoading = false;
    bool Level::sSingleLayerRendering = false;
    bool Level::sCameraIsMoving = false;

    int Level::sGridHeight = 31;
    int Level::sGridWidth = 32;
    int Level::sTileSize = 32;
    float Level::sLightSize = 256.0f;
    int Level::sLayerCount = 3;
    int Level::sIndex = 0;
    int Level::sNextID = 0;
    int Level::sGlobalLightIntensity = 100;

    SDL_Texture* Level::mLightTexture = nullptr;

    void Level::TilePlacement()
    {
        if(mPlace)
        {
            // Conversion coordonnees ecran en coordonnees grille prennant zoom et camera en compte
            float worldX = mMouseX / mCamera.mZoom + mCamera.x;
            float worldY = mMouseY / mCamera.mZoom + mCamera.y;
            int xPos = worldX / sTileSize;
            int yPos = worldY / sTileSize;

            // Ne rien faire lorsque le clique est effectuer hors grille
            if(xPos < 0 || xPos >= sGridWidth || yPos < 0 || yPos >= sGridHeight)
            {
                std::cout << "Grid error : Out of bounds\n";
                mPlace = false;
                return;
            }

            // Trouver l'index de la grille (xPos, yPos) dans la hierarchie de la couche courante
            sIndex = FindIDInScene(sCurrentLayerID, xPos, yPos);

            // Ajouter ou retirer une tile de la grille si on est pas en mode selection de tuile
            if(!sModeSelection)
            {
                // Ajouter ou retirer en fonction de l'id (-1 pour retirer)
                sLayers[sCurrentLayerID].mGrid[yPos][xPos] = sCurrentTileID;

                ResetSelectionStatus();
                
                // Ajouter la tuile a la scene (hierarchie de la couche courante)
                TileInScene tileInScene;
                tileInScene.mID = sCurrentTileID;
                tileInScene.mName = "Tile";
                tileInScene.mIsSelected = true;
                tileInScene.mGridX = xPos;
                tileInScene.mGridY = yPos;

                sLayers[sCurrentLayerID].mHierarchie.push_back(tileInScene);
            }
  
            mPlace = false;
        }   
    }

    void Level::RemoveTile(int gridX, int gridY)
    {
        int index = FindIDInScene(sCurrentLayerID, gridX, gridY);
        sLayers[sCurrentLayerID].mGrid[gridY][gridX] = -1;
        sLayers[sCurrentLayerID].mHierarchie.erase(sLayers[sCurrentLayerID].mHierarchie.begin() + index);
    }

    void Level::Update()
    {
        TilePlacement();
    }

    void Level::Render(SDL_Renderer* renderer)
    {      
        if(sIsLoading)
        {
            return;
        }

        SDL_SetRenderTarget(renderer, nullptr);

        if(sSingleLayerRendering)
        {
            for (int y = 0; y < sGridHeight; y++)
            {
                for (int x = 0; x < sGridWidth; x++)
                {
                    int tileID = sLayers[sCurrentLayerID].mGrid[y][x];
                    if(tileID == -1) continue; 
                    
                    // Trouver l'index de la dans la hierarchie de la couche courante
                    int indexA = FindIDInScene(sCurrentLayerID, x, y);

                    SDL_FRect rect;
                    rect.x = (x * sTileSize - mCamera.x) * mCamera.mZoom;
                    rect.y = (y * sTileSize - mCamera.y) * mCamera.mZoom;
                    rect.w = sTileSize * mCamera.mZoom;
                    rect.h = sTileSize * mCamera.mZoom;

                    // Trouver l'index correspondant a l'id dans le tileset
                    int indexB = FindID(tileID);

                    SDL_Texture* tex = sTileSet[indexB].mTexture;
                    if(!tex)
                    { 
                        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                        SDL_RenderRect(renderer, &rect);
                    }
                    else
                    {
                        SDL_RenderTexture(renderer, tex, nullptr, &rect);

                        if(sLayers[sCurrentLayerID].mHierarchie[indexA].mIsSelected)
                        {
                            DrawSelectionSquare(renderer, rect.x, rect.y, rect.w);
                        }
                    }
                }
            }
        }
        else
        {
            for (int l = 0; l < sLayerCount; l++)
            {
                for (int y = 0; y < sGridHeight; y++)
                {
                    for (int x = 0; x < sGridWidth; x++)
                    {
                        int tileID = sLayers[l].mGrid[y][x];
                        if(tileID == -1) continue;
                        
                        // Trouver l'index de la dans la hierarchie de la couche courante
                        int indexA = FindIDInScene(l, x, y);

                        SDL_FRect rect;
                        rect.x = (x * sTileSize - mCamera.x) * mCamera.mZoom;
                        rect.y = (y * sTileSize - mCamera.y) * mCamera.mZoom;
                        rect.w = sTileSize * mCamera.mZoom;
                        rect.h = sTileSize * mCamera.mZoom;
                        
                        // Trouver l'index correspondant a l'id dans le tileset
                        int indexB = FindID(tileID);

                        SDL_Texture* tex = sTileSet[indexB].mTexture;
                        if(!tex)
                        { 
                            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                            SDL_RenderRect(renderer, &rect);
                        }
                        else
                        {
                            SDL_RenderTexture(renderer, tex, nullptr, &rect);

                            if(l == sCurrentLayerID && sLayers[l].mHierarchie[indexA].mIsSelected)
                            {
                                DrawSelectionSquare(renderer, rect.x, rect.y, rect.w);
                            }
                        }
                    }
                }
            }
        }

        RenderLight(renderer);
    }

    void Level::MouseGetter(SDL_Event event)
    {
        // Verification d'evenement souris (clic gauche)
        if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            // Recuperer la position de la souris au clic
            mMouseX = event.button.x;
            mMouseY = event.button.y;
            mPlace = true;
        }

        if(event.type == SDL_EVENT_MOUSE_MOTION && (event.motion.state & SDL_BUTTON_RMASK))
        {
            sCameraIsMoving = true;
            // mettre a jour la position de la camera quand la souris bouge et que le bouton droit est enfonce
            mCamera.x -= event.motion.xrel / mCamera.mZoom;
            mCamera.y -= event.motion.yrel / mCamera.mZoom;
        }
        else
        {
            sCameraIsMoving = false;
        }

        // Gestion du soom 
        if(event.type == SDL_EVENT_MOUSE_WHEEL)
        {
            bool zoom = (event.wheel.y > 0);

            HandleZoom(mCamera, zoom);
        }
    }

    void Level::HandleZoom(Camera& camera, bool zoom)
    {
        if(zoom)
        {
            // Zoomer
            camera.mZoom *= 1.0f + camera.mZoomSpeed;
        }
        else
        {
            // Dezoomer
            camera.mZoom /= 1.0f + camera.mZoomSpeed;
        }

        // Limiter le zoom pour eviter le zoom infini et le valeurs negatives
        camera.mZoom = std::clamp(camera.mZoom, camera.mMinZoom, camera.mMaxZoom);
    }

    void Level::InitLayers()
    {
        // Initialiser les couches
        // Chaque grille de couche est initialiser avec les valeurs -1 pour vide
        std::cout << "Initialisation des couches\n";
        sLayers.resize(sLayerCount);

        sLayers[0].mName = "Sol";
        sLayers[0].mGrid.resize(sGridHeight, std::vector<int>(sGridWidth, -1));

        sLayers[1].mName = "Joueur";
        sLayers[1].mGrid.resize(sGridHeight, std::vector<int>(sGridWidth, -1));

        sLayers[2].mName = "UI";
        sLayers[2].mGrid.resize(sGridHeight, std::vector<int>(sGridWidth, -1));

        std::cout << "Couches initialisees\n";
    }

    void Level::InitTiles(SDL_Renderer* renderer)
    {
        // Creations des tiles depuis le fichier de mapping de tile-id
        std::cout << "Initialisation des tuiles\n";

        std::ifstream file("build/Assets/Tileset.txt");
        if(!file)
        {
            std::cout << "Impossible d'ouvrir le tileset\n";
            return;
        }

        // Verification de la validiter du fichier (checker le header)
        std::string header;
        file >> header;
        if(header != "MINIEDITORTILESET")
        {
            std::cout << "Fichier tileset non valide\n";
            return;
        }

        int id;
        std::string fileName;

        // Creations des tiles
        while(file >> id >> fileName)
        {
            Tile tile;

            tile.mID = id;

            if(IDIsUsed(tile.mID))
            {
                continue;
            }

            // Ajouter l'id dans les IDs utilises
            mUsedIDs.push_back(tile.mID);

            tile.mName = std::filesystem::path(fileName).string();

            std::filesystem::path tilePath = mTilesPath + fileName;

            if(!std::filesystem::exists(tilePath))
            {
                std::cout << "Tile manquante " << tilePath << "\n";
                continue;
            }

            // Creer la texture depuis l'images (.png)
            tile.mTexture = IMG_LoadTexture(renderer, tilePath.c_str());
            if(!tile.mTexture)
            {
                std::cout << "Impossible de charger la texture " << tilePath << "\n";
                continue;
            }

            // Ajout
            sTileSet.push_back({{tile.mID, tile.mName}, tile.mTexture});
        }

        std::cout << sTileSet.size() << " Tuiles initialisees\n";

        if(sTileSet.size() != 0)
        {
            sCurrentTileID = 0;
        }
    }

    Level::Level()
    {
        InitLayers();
    }

    void Level::SaveLevel()
    {
        std::cout << "\nSauvegarde du niveau " << sFileName << ".txt\n";
        std::filesystem::create_directories(SAVE_PATH);
        
        // fichier de sauvegarde du niveau
        std::string completName = SAVE_PATH + sFileName + EXTENSION;
        std::ofstream file(completName);

        // fichier de sauvegarde des proprietes de niveau
        completName = SAVE_PATH + sFileName + "Hierarchie" + EXTENSION;
        std::ofstream levelProps(completName);

        // Cle du format
        file << "MiniEditorFile" << "\n";
        levelProps << "MiniEditorFile" << "\n\n";

        // Ecriture du niveau 
        file << "WIDTH      " << sGridWidth << "\n";
        file << "HEIGHT     " << sGridHeight << "\n";

        file << "GLOBALINTENSITY      " << sGlobalLightIntensity << "\n";
        file << "AMBIENT     " << sAmbient << "\n";

        file << "LAYERCOUNT " << sLayerCount << "\n\n";

        for (int l = 0; l < sLayerCount; l++)
        {
            file << "LAYER " << sLayers[l].mName << "\n";

            for (int y = 0; y < sGridHeight; y++)
            {
                for (int x = 0; x < sGridWidth; x++)
                {
                    file << sLayers[l].mGrid[y][x] << " ";
                }

                file << "\n";
            }

            file << "\n";
        }

        // Ecriture des propriete du niveau
        for (int l = 0; l < sLayerCount; l++)
        {
            levelProps << "HIERARCHIESIZE " << sLayers[l].mHierarchie.size() << "\n";

            for (size_t i = 0; i < sLayers[l].mHierarchie.size(); i++)
            {
                levelProps << sLayers[l].mHierarchie[i].mID << " ";
                levelProps << sLayers[l].mHierarchie[i].mName << " ";
                levelProps << sLayers[l].mHierarchie[i].mGridX << " ";
                levelProps << sLayers[l].mHierarchie[i].mGridY << "\n";
            }
            
            levelProps << "\n";
        }

        levelProps << "\n";
        levelProps << "LIGHTSIZE " << sLights.size() << "\n";

        for(const auto& light : sLights)
        {
            levelProps << light.mName << " ";
            levelProps << light.mProps.x << " ";
            levelProps << light.mProps.y << " ";
            levelProps << light.mFixedPosX << " ";
            levelProps << light.mFixedPosY << " ";
            levelProps << (int)(light.mColorMod.r) << " ";
            levelProps << (int)(light.mColorMod.g) << " ";
            levelProps << (int)(light.mColorMod.b) << "\n";
        }

        std::cout << "Niveau " << sFileName << ".txt sauvegarder avec succes\n";
        file.close();
        levelProps.close();
    }

    void Level::LoadLevel()
    {
        sIsLoading = true;
        std::cout << "\nChargement du niveau " << sFileName << ".txt\n";

        // Fichier de niveau
        std::string completName = SAVE_PATH + sFileName + EXTENSION;
        std::ifstream file(completName);
        if(!file.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier. Veuillez verifier s'il exixte\n" << std::endl;
            sIsLoading = false;
            return;
        }

        std::string token;

        file >> token;
        // Validation du fichier
        if(token != "MiniEditorFile")
        {
            std::cerr << "Fichier inconnu\n";
            sIsLoading = false;
            return;
        }
        
        // Recuperation des infos
        file >> token >> sGridWidth;
        file >> token >> sGridHeight;

        file >> token >> sGlobalLightIntensity;
        file >> token >> sAmbient;
        
        file >> token >> sLayerCount;

        sLayers.resize(sLayerCount);

        for (int l = 0; l < sLayerCount; l++)
        {
            file >> token >> sLayers[l].mName;

            for (int y = 0; y < sGridHeight; y++)
            {
                for (int x = 0; x < sGridWidth; x++)
                {
                    file >> sLayers[l].mGrid[y][x];
                }
            }
        }

        file.close();

        // Fichier de proprites de niveau
        completName = SAVE_PATH + sFileName + "Hierarchie" + EXTENSION;
        std::ifstream levelProps(completName);
        if(!levelProps.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier de propriete. Veuillez verifier s'il exixte\n" << std::endl;
            sIsLoading = false;
            return;
        }

        std::string getter;

        // Validation du fichier
        levelProps >> getter;
        if(getter != "MiniEditorFile")
        {
            std::cerr << "Fichier de proprieter inconnu\n";
            sIsLoading = false;
            return;
        }

        int size;

        // Recuperation des infos
        for (int l = 0; l < sLayerCount; l++)
        {
            levelProps >> getter >> size;
            sLayers[l].mHierarchie.resize(size);

            for (size_t i = 0; i < sLayers[l].mHierarchie.size(); i++)
            {
                levelProps >> sLayers[l].mHierarchie[i].mID;
                levelProps >> sLayers[l].mHierarchie[i].mName;
                levelProps >> sLayers[l].mHierarchie[i].mGridX;
                levelProps >> sLayers[l].mHierarchie[i].mGridY;
            }
        }

        levelProps >> getter >> size;
        sLights.resize(size);

        for(size_t i = 0; i < sLights.size(); i++)
        {
            sLights[i].mTexture = mLightTexture;
            if(!sLights[i].mTexture)
            {
                std::cout << "Impossible de charger la texture de lumiere !" << std::endl;
                continue;
            }

            int color[3];
            levelProps >> sLights[i].mName;
            levelProps >> sLights[i].mProps.x;
            levelProps >> sLights[i].mProps.y;
            levelProps >> sLights[i].mFixedPosX;
            levelProps >> sLights[i].mFixedPosY;
            levelProps >> color[0];
            levelProps >> color[1];
            levelProps >> color[2];

            sLights[i].mColorMod.r = (Uint8)(color[0]);
            sLights[i].mColorMod.g = (Uint8)(color[1]);
            sLights[i].mColorMod.b = (Uint8)(color[2]);
            sLights[i].mColorMod.a = 255;

            sLights[i].mProps.w = 256.0f;
            sLights[i].mProps.h = 256.0f;
        }

        levelProps.close();
        std::cout << "Niveau " << sFileName << ".txt Charger avec succes\n";
        sIsLoading = false;
    }

    void Level::ResetCamera()
    {
        std::cout << "Camera remit a l'origin\n";
        mCamera.x = 0.0f;
        mCamera.y = 0.0f;
        mCamera.mZoom = 1.0f;
    }

    bool Level::IDIsUsed(int tileID)
    {
        // Verifier si un id est deja utiliser
        for(const auto& id : mUsedIDs)
        {
            if(tileID == id)
            {
                return true;
            }
        }

        return false;
    }

    // Ecriture du fichier de mapping (tile->id)
    void Level::WriteTileSet()
    {
        // Les noms de fichiers
        std::vector<TileForTS> tileInfo;

        std::string completName = TILE_SET_PATH + "Tileset" + EXTENSION;
        std::ofstream file(completName);

        // Ajout du header personaliser
        file << "MINIEDITORTILESET" << "\n\n";

        // Recuperation des noms de tout les fichier image (format .png) dans Assets/Tiles
        for(const auto& entry : std::filesystem::directory_iterator("build/Assets/Tiles"))
        {
            if(!entry.is_regular_file())
                continue;
            
            auto path = entry.path();
            if(path.extension() != ".png")
                continue;

            std::string fileName = path.stem().string();
            int id = ExtractLeadingNumber(fileName);

            if(id < 0)
                continue;
            
            TileForTS tileHelp;
            tileHelp.mName = fileName;
            tileHelp.mID   = id;

            tileInfo.push_back(tileHelp);
        }
        
        // Classe les noms par ordre alphabetique pour eviter les erreur dans les chargement de texture
        std::sort(tileInfo.begin(), tileInfo.end(), [](const auto& a, const auto& b){return a.mID < b.mID;});

        // Ecriture du TileSet (Tileset.txt)
        for(const auto& tile : tileInfo)
        {
            file << tile.mID << " ";
            file << tile.mName << ".png\n";
        }

        file.close();
    }

    int Level::ExtractLeadingNumber(std::string name)
    {
        // Extraire le nombre(id) au debut du nom de fichier
        size_t i = 0;
        while(i < name.size() && std::isdigit(name[i]))
            i++;
        
        return (i > 0) ? std::stoi(name.substr(0, i)) : -1;
    }

    void Level::DestroyTextures()
    {
        // Detruire les texture a la fin du progamme
        for(auto& tile : sTileSet)
        {
            SDL_DestroyTexture(tile.mTexture);
        }

        // Destruction des texture de lumiere
        for(auto& light : sLights)
        {
            SDL_DestroyTexture(light.mTexture);
        }
        
        SDL_DestroyTexture(mLightMap);
        SDL_DestroyTexture(mLightTexture);
    }

    int Level::FindID(int tileID)
    {
        // Cherchez un index de tableau correspondant a un id de tuile dans mtileSet
        size_t i;
        for(i = 0; i < sTileSet.size(); i++)
        {
            if(sTileSet[i].mID == tileID)
            {
                break;
            }
        }
        return i;
    }

    int Level::FindIDInScene(int layerID, int x, int y)
    {
        if(sLayers[layerID].mGrid[y][x] == -1)
        {
            return 0;
        }
        
        // Cherchez un index de tableau correspondant a un id de tuile dans la scene (hierarchie de la couche courante)
        size_t i;
        for (i = 0; i < sLayers[layerID].mHierarchie.size(); i++)
        {
            if(sLayers[layerID].mHierarchie[i].mGridX == x && sLayers[layerID].mHierarchie[i].mGridY == y)
            {
                break;
            }
        }

        return i;
    }

    void Level::RenderBackGroundGrid(SDL_Renderer* renderer)
    {
        SDL_FRect limits;
        limits.x = (0.0f - mCamera.x) * mCamera.mZoom;
        limits.y = (0.0f - mCamera.y) * mCamera.mZoom;
        limits.h = sGridHeight * sTileSize * mCamera.mZoom;
        limits.w = sGridWidth  * sTileSize * mCamera.mZoom;

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderRect(renderer, &limits);
    }

    void Level::ResetSelectionStatus()
    {
        for(auto& tile : sLayers[sCurrentLayerID].mHierarchie)
        {
            tile.mIsSelected = false;
        }
    }

    void Level::DrawSelectionSquare(SDL_Renderer* renderer, int x, int y, float size)
    {
        x = x - 1.0f;
        y = y - 1.0f;
        size = size + 1.0f;
        
        SDL_FRect selcetionSquare = {static_cast<float>(x), static_cast<float>(y), size, size};
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderRect(renderer, &selcetionSquare);
    }

    void Level::RenderLight(SDL_Renderer* renderer)
    {
        SDL_SetRenderTarget(renderer, mLightMap);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, sAmbient, sAmbient, sAmbient, sGlobalLightIntensity);
        SDL_RenderFillRect(renderer, NULL);

        for(auto& light : sLights)
        {
            light.mProps.x = (light.mFixedPosX - mCamera.x) * mCamera.mZoom;
            light.mProps.y = (light.mFixedPosY - mCamera.y) * mCamera.mZoom;
            light.mProps.w = sLightSize * mCamera.mZoom;
            light.mProps.h = sLightSize * mCamera.mZoom;

            if(light.mActivate)
            {
                SDL_RenderTexture(renderer, light.mTexture, nullptr, &light.mProps);
            }
        }

        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderTexture(renderer, mLightMap, nullptr, nullptr);

        for(auto& light : sLights)
        {
            if(light.mIsRayonnant && light.mActivate)
            {
                SDL_SetTextureColorMod(light.mTexture, light.mColorMod.r, light.mColorMod.g, light.mColorMod.b);
                SDL_RenderTexture(renderer, light.mTexture, nullptr, &light.mProps);
            }
        }
    }

    bool Level::InitLight(SDL_Renderer* renderer)
    {
        mLightMap = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            1024, 992
        );

        if(!mLightMap)
        {
            std::cout << "Error: lightmap non initielisee." << std::endl;
            return false;
        }

        SDL_SetTextureBlendMode(mLightMap, SDL_BLENDMODE_MOD);

        mLightTexture = IMG_LoadTexture(renderer, "build/Assets/Lights/light.png");
        if(!mLightTexture)
        {
            std::cout << "Impossible de charger la texture de lumiere !" << std::endl;
            return false;
        }

        return true;
    }

    void Level::AddLight()
    {
        Light light;

        light.mTexture = mLightTexture;
        if(!light.mTexture)
        {
            std::cout << "Impossible de charger la texture de lumiere !" << std::endl;
            return;
        }

        SDL_SetTextureBlendMode(light.mTexture, SDL_BLENDMODE_ADD);
        
        light.mColorMod.r = 155;
        light.mColorMod.b = 155;
        light.mColorMod.g = 155;
        light.mColorMod.a = 255;

        light.mProps = SDL_FRect{
            512.0f, 
            496.0f, 
            sLightSize, 
            sLightSize
        };

        light.mFixedPosX = light.mProps.x;
        light.mFixedPosY = light.mProps.y;

        std::string name = "Light";
        light.mName = name + std::to_string(sNextID);

        sNextID++;

        sLights.push_back(light);
    }

    void Level::RemoveLight(int index)
    {
        sLights.erase(sLights.begin() + index);
    }
}

