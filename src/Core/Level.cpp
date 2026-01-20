#include "Level.h"

namespace minieditor
{
    std::vector<Tile> Level::mtileSet;
    std::vector<Light> Level::mLights;
    int Level::mcurrentTileID = -1;
    int Level::mcurrentLayerID = 0;
    int Level::mambient = 10;
    std::vector<Layer> Level::mLayers;
    std::string Level::mfileName = "Level";
    const std::string Level::msavePath = "build/Levels/";
    const std::string Level::mtileSetPath = "build/Assets/";
    const std::string Level::mextension = ".txt";

    bool Level::mmodeSelection = false;
    bool Level::mIsLoading = false;
    bool Level::mSingleLayerRendering = false;

    int Level::mGridHeight = 31;
    int Level::mGridWidth = 32;
    int Level::mtileSize = 32;
    int Level::mLayerCount = 3;
    int Level::mindex = 0;
    int Level::mglobalLightIntensity = 100;

    void Level::TilePlacement()
    {
        if(mPlace)
        {
            // Conversion coordonnees ecran en coordonnees grille
            float worldX = mMouseX + mcamera.x;
            float worldY = mMouseY + mcamera.y;
            int xPos = worldX / mtileSize;
            int yPos = worldY / mtileSize;

            // Ne rien faire lorsque le clique est effectuer hors grille
            if(xPos < 0 || xPos >= mGridWidth || yPos < 0 || yPos >= mGridHeight)
            {
                std::cout << "Grid error : Out of bounds\n";
                mPlace = false;
                return;
            }

            // Trouver l'index de la grille (xPos, yPos) dans la hierarchie de la couche courante
            mindex = FindIDInScene(mcurrentLayerID, xPos, yPos);

            // Ajouter ou retirer une tile de la grille si on est pas en mode selection de tuile
            if(!mmodeSelection)
            {
                // Ajouter ou retirer en fonction de l'id (-1 pour retirer)
                mLayers[mcurrentLayerID].Grid[yPos][xPos] = mcurrentTileID;

                ResetSelectionStatus();
                
                // Ajouter la tuile a la scene (hierarchie de la couche courante)
                TileInScene _tileInScene;
                _tileInScene.mID = mcurrentTileID;
                _tileInScene.mName = "Tile";
                _tileInScene.size = 1.0f;
                _tileInScene.mIsSelected = true;
                _tileInScene.mgridX = xPos;
                _tileInScene.mgridY = yPos;

                mLayers[mcurrentLayerID].hierarchie.push_back(_tileInScene);
            }
  
            mPlace = false;
        }   
    }

    void Level::RemoveTile(int gridX, int gridY)
    {
        int index = FindIDInScene(mcurrentLayerID, gridX, gridY);
        mLayers[mcurrentLayerID].Grid[gridY][gridX] = -1;
        mLayers[mcurrentLayerID].hierarchie.erase(mLayers[mcurrentLayerID].hierarchie.begin() + index);
    }

    void Level::Update()
    {
        TilePlacement();
    }

    void Level::Render(SDL_Renderer* mRenderer)
    {      
        if(mIsLoading)
        {
            return;
        }

        SDL_SetRenderTarget(mRenderer, nullptr);

        if(mSingleLayerRendering)
        {
            for (int y = 0; y < mGridHeight; y++)
            {
                for (int x = 0; x < mGridWidth; x++)
                {
                    int _tileID = mLayers[mcurrentLayerID].Grid[y][x];
                    if(_tileID == -1) continue; 
                    
                    // Trouver l'index de la dans la hierarchie de la couche courante
                    int _indexA = FindIDInScene(mcurrentLayerID, x, y);

                    SDL_FRect rect;
                    rect.x = (x * mtileSize) - mcamera.x;
                    rect.y = (y * mtileSize) - mcamera.y;
                    rect.w = mtileSize * mLayers[mcurrentLayerID].hierarchie[_indexA].size;
                    rect.h = mtileSize * mLayers[mcurrentLayerID].hierarchie[_indexA].size;

                    // Trouver l'index correspondant a l'id dans le tileset
                    int _indexB = FindID(_tileID);

                    SDL_Texture* _tex = mtileSet[_indexB].texture;
                    if(!_tex)
                    { 
                        SDL_SetRenderDrawColor(mRenderer, 255, 0, 255, 255);
                        SDL_RenderRect(mRenderer, &rect);
                    }
                    else
                    {
                        SDL_RenderTexture(mRenderer, _tex, nullptr, &rect);

                        if(mLayers[mcurrentLayerID].hierarchie[_indexA].mIsSelected)
                        {
                            DrawSelectionSquare(mRenderer, rect.x, rect.y, rect.w);
                        }
                    }
                }
            }
        }
        else
        {
            for (int l = 0; l < mLayerCount; l++)
            {
                for (int y = 0; y < mGridHeight; y++)
                {
                    for (int x = 0; x < mGridWidth; x++)
                    {
                        int _tileID = mLayers[l].Grid[y][x];
                        if(_tileID == -1) continue;
                        
                        // Trouver l'index de la dans la hierarchie de la couche courante
                        int _indexA = FindIDInScene(l, x, y);

                        SDL_FRect rect;
                        rect.x = (x * mtileSize) - mcamera.x;
                        rect.y = (y * mtileSize) - mcamera.y;
                        rect.w = mtileSize * mLayers[l].hierarchie[_indexA].size;
                        rect.h = mtileSize * mLayers[l].hierarchie[_indexA].size;
                        
                        // Trouver l'index correspondant a l'id dans le tileset
                        int _indexB = FindID(_tileID);

                        SDL_Texture* _tex = mtileSet[_indexB].texture;
                        if(!_tex)
                        { 
                            SDL_SetRenderDrawColor(mRenderer, 255, 0, 255, 255);
                            SDL_RenderRect(mRenderer, &rect);
                        }
                        else
                        {
                            SDL_RenderTexture(mRenderer, _tex, nullptr, &rect);

                            if(l == mcurrentLayerID && mLayers[l].hierarchie[_indexA].mIsSelected)
                            {
                                DrawSelectionSquare(mRenderer, rect.x, rect.y, rect.w);
                            }
                        }
                    }
                }
            }
        }

        if(lightInitialized)
        {
            RenderLight(mRenderer);
        }
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
            // mettre a jour la position de la camera quand la souris bouge et que le bouton droit est enfonce
            mcamera.x -= event.motion.xrel;
            mcamera.y -= event.motion.yrel;
        }
    }

    void Level::InitLayers()
    {
        // Initialiser les couches
        // Chaque grille de couche est initialiser avec les valeurs -1 pour vide
        std::cout << "Initialisation des couches\n";
        mLayers.resize(mLayerCount);

        mLayers[0].name = "Sol";
        mLayers[0].Grid.resize(mGridHeight, std::vector<int>(mGridWidth, -1));

        mLayers[1].name = "Joueur";
        mLayers[1].Grid.resize(mGridHeight, std::vector<int>(mGridWidth, -1));

        mLayers[2].name = "UI";
        mLayers[2].Grid.resize(mGridHeight, std::vector<int>(mGridWidth, -1));

        std::cout << "Couches initialisees\n";
    }

    void Level::InitTiles(SDL_Renderer* mRenderer)
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

        int _id;
        std::string _fileName;

        // Creations des tiles
        while(file >> _id >> _fileName)
        {
            Tile _tile;

            _tile.tileID = _id;

            if(IDIsUsed(_tile.tileID))
            {
                continue;
            }

            // Ajouter l'id dans les IDs utilises
            musedIDs.push_back(_tile.tileID);

            _tile.name = std::filesystem::path(_fileName).string();

            std::filesystem::path _tilePath = mtilesPath + _fileName;

            if(!std::filesystem::exists(_tilePath))
            {
                std::cout << "Tile manquante " << _tilePath << "\n";
                continue;
            }

            // Creer la texture depuis l'images (.png)
            _tile.texture = IMG_LoadTexture(mRenderer, _tilePath.c_str());
            if(!_tile.texture)
            {
                std::cout << "Impossible de charger la texture " << _tilePath << "\n";
                continue;
            }

            // Ajout
            mtileSet.push_back({_tile.tileID, _tile.name, _tile.texture});
        }

        std::cout << mtileSet.size() << " Tuiles initialisees\n";

        if(mtileSet.size() != 0)
        {
            mcurrentTileID = 0;
        }
    }

    Level::Level()
    {
        InitLayers();
    }

    void Level::SaveLevel()
    {
        std::cout << "\nSauvegarde du niveau " << mfileName << ".txt\n";
        std::filesystem::create_directories(msavePath);
        
        // fichier de sauvegarde du niveau
        std::string completName = msavePath + mfileName + mextension;
        std::ofstream file(completName);

        // fichier de sauvegarde des proprietes de niveau
        completName = msavePath + mfileName + "Hierarchie" + mextension;
        std::ofstream levelprops(completName);

        // Cle du format
        file << "MiniEditorFile" << "\n";
        levelprops << "MiniEditorFile" << "\n\n";

        // Ecriture du niveau 
        file << "WIDTH      " << mGridWidth << "\n";
        file << "HEIGHT     " << mGridHeight << "\n";

        file << "TILESIZE   " << mtileSize << "\n";

        file << "LAYERCOUNT " << mLayerCount << "\n\n";

        for (int l = 0; l < mLayerCount; l++)
        {
            file << "LAYER " << mLayers[l].name << "\n";

            for (int y = 0; y < mGridHeight; y++)
            {
                for (int x = 0; x < mGridWidth; x++)
                {
                    file << mLayers[l].Grid[y][x] << " ";
                }

                file << "\n";
            }

            file << "\n";
        }

        // Ecriture des propriete du niveau
        for (int l = 0; l < mLayerCount; l++)
        {
            levelprops << "HIERARCHIESIZE " << mLayers[l].hierarchie.size() << "\n";

            for (size_t i = 0; i < mLayers[l].hierarchie.size(); i++)
            {
                levelprops << mLayers[l].hierarchie[i].mID << " ";
                levelprops << mLayers[l].hierarchie[i].mName << " ";
                levelprops << mLayers[l].hierarchie[i].mgridX << " ";
                levelprops << mLayers[l].hierarchie[i].mgridY << " ";
                levelprops << mLayers[l].hierarchie[i].size << "\n";
            }
            
            levelprops << "\n";
        }

        std::cout << "Niveau " << mfileName << ".txt sauvegarder avec succes\n";
        file.close();
        levelprops.close();
    }

    void Level::LoadLevel()
    {
        mIsLoading = true;
        std::cout << "\nChargement du niveau " << mfileName << ".txt\n";

        // Fichier de niveau
        std::string completName = msavePath + mfileName + mextension;
        std::ifstream file(completName);
        if(!file.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier. Veuillez verifier s'il exixte\n" << std::endl;
            mIsLoading = false;
            return;
        }

        std::string token;

        file >> token;
        // Validation du fichier
        if(token != "MiniEditorFile")
        {
            std::cerr << "Fichier inconnu\n";
            mIsLoading = false;
            return;
        }
        
        // Recuperation des infos
        file >> token >> mGridWidth;
        file >> token >> mGridHeight;

        file >> token >> mtileSize;
        file >> token >> mLayerCount;

        mLayers.resize(mLayerCount);

        for (int l = 0; l < mLayerCount; l++)
        {
            file >> token >> mLayers[l].name;

            for (int y = 0; y < mGridHeight; y++)
            {
                for (int x = 0; x < mGridWidth; x++)
                {
                    file >> mLayers[l].Grid[y][x];
                }
            }
        }

        file.close();

        // Fichier de proprites de niveau
        completName = msavePath + mfileName + "Hierarchie" + mextension;
        std::ifstream levelprops(completName);
        if(!levelprops.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier de propriete. Veuillez verifier s'il exixte\n" << std::endl;
            mIsLoading = false;
            return;
        }

        std::string getter;

        // Validation du fichier
        levelprops >> getter;
        if(getter != "MiniEditorFile")
        {
            std::cerr << "Fichier de proprieter inconnu\n";
            mIsLoading = false;
            return;
        }

        // Recuperation des infos
        for (int l = 0; l < mLayerCount; l++)
        {
            int _size;
            levelprops >> getter >> _size;
            mLayers[l].hierarchie.resize(_size);

            for (size_t i = 0; i < mLayers[l].hierarchie.size(); i++)
            {
                levelprops >> mLayers[l].hierarchie[i].mID;
                levelprops >> mLayers[l].hierarchie[i].mName;
                levelprops >> mLayers[l].hierarchie[i].mgridX;
                levelprops >> mLayers[l].hierarchie[i].mgridY;
                levelprops >> mLayers[l].hierarchie[i].size;
            }
            
        }

        levelprops.close();
        std::cout << "Niveau " << mfileName << ".txt Charger avec succes\n";
        mIsLoading = false;
    }

    void Level::ResetCamera()
    {
        std::cout << "Camera remit a l'origin\n";
        mcamera.x = 0.0f;
        mcamera.y = 0.0f;
    }

    bool Level::IDIsUsed(int tileID)
    {
        // Verifier si un id est deja utiliser
        for(const auto& _id : musedIDs)
        {
            if(tileID == _id)
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

        std::string completName = mtileSetPath + "Tileset" + mextension;
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

            std::string _fileName = path.stem().string();
            int _id = ExtractLeadingNumber(_fileName);

            if(_id < 0)
                continue;
            
            TileForTS _tileHelp;
            _tileHelp.mName = _fileName;
            _tileHelp.mID   = _id;

            tileInfo.push_back(_tileHelp);
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
        for(auto& tile : mtileSet)
        {
            SDL_DestroyTexture(tile.texture);
        }

        // Destruction des texture de lumiere
        for(auto& light : mLights)
        {
            SDL_DestroyTexture(light.mTexture);
        }
        
        SDL_DestroyTexture(mlightMap);
    }

    int Level::FindID(int tileID)
    {
        // Cherchez un index de tableau correspondant a un id de tuile dans mtileSet
        size_t i;
        for(i = 0; i < mtileSet.size(); i++)
        {
            if(mtileSet[i].tileID == tileID)
            {
                break;
            }
        }
        return i;
    }

    int Level::FindIDInScene(int layerID, int x, int y)
    {
        if(mLayers[layerID].Grid[y][x] == -1)
        {
            return 0;
        }
        
        // Cherchez un index de tableau correspondant a un id de tuile dans la scene (hierarchie de la couche courante)
        size_t i;
        for (i = 0; i < mLayers[layerID].hierarchie.size(); i++)
        {
            if(mLayers[layerID].hierarchie[i].mgridX == x && mLayers[layerID].hierarchie[i].mgridY == y)
            {
                break;
            }
        }

        return i;
    }

    void Level::RenderBackGroundGrid(SDL_Renderer* mRenderer)
    {
        SDL_FRect limits;
        limits.x = 0.0f - mcamera.x;
        limits.y = 0.0f - mcamera.y;
        limits.h = mGridHeight * mtileSize;
        limits.w = mGridWidth  * mtileSize;

        SDL_SetRenderDrawColor(mRenderer, 50, 50, 50, 255);
        SDL_RenderRect(mRenderer, &limits);
    }

    void Level::ResetSelectionStatus()
    {
        for(auto& tile : mLayers[mcurrentLayerID].hierarchie)
        {
            tile.mIsSelected = false;
        }
    }

    void Level::DrawSelectionSquare(SDL_Renderer* mRenderer, int x, int y, float size)
    {
        x = x - 1.0f;
        y = y - 1.0f;
        size = size + 1.0f;
        
        SDL_FRect selcetionSquare = {static_cast<float>(x), static_cast<float>(y), size, size};
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 0, 255);
        SDL_RenderRect(mRenderer, &selcetionSquare);
    }

    void Level::RenderLight(SDL_Renderer* mRenderer)
    {
        SDL_SetRenderTarget(mRenderer, mlightMap);

        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mRenderer, mambient, mambient, mambient, mglobalLightIntensity);
        SDL_RenderFillRect(mRenderer, NULL);

        SDL_RenderTexture(mRenderer, mLights[0].mTexture, nullptr, &mLights[0].props);

        SDL_SetRenderTarget(mRenderer, nullptr);
        SDL_RenderTexture(mRenderer, mlightMap, nullptr, nullptr);

        SDL_SetTextureColorMod(mLights[0].mTexture, mLights[0].colorMod.r, mLights[0].colorMod.g, mLights[0].colorMod.b);
        SDL_RenderTexture(mRenderer, mLights[0].mTexture, nullptr, &mLights[0].props);
    }

    bool Level::InitLight(SDL_Renderer* mRenderer)
    {
        Light light;

        light.mTexture = IMG_LoadTexture(mRenderer, "build/Assets/Lights/light.png");
        if(!light.mTexture)
        {
            std::cout << "Impossible de charger la texture de lumiere !" << std::endl;
            return false;
        }

        SDL_SetTextureBlendMode(light.mTexture, SDL_BLENDMODE_ADD);
        light.colorMod.r = 155;
        light.colorMod.b = 155;
        light.colorMod.g = 155;
        light.colorMod.a = 255;

        light.props = SDL_FRect{512.0f, 496.0f, 256.0f, 256.0f};

        mLights.push_back(light);

        mlightMap = SDL_CreateTexture(
            mRenderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            1024, 992
        );

        SDL_SetTextureBlendMode(mlightMap, SDL_BLENDMODE_MOD);

        return true;
    }
}

