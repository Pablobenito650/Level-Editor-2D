#include "Level.h"

namespace minieditor
{
    std::vector<Tile> Level::mtileSet;
    int Level::mcurrentTileID = -1;
    int Level::mcurrentLayerID = 0;
    std::vector<Layer> Level::mLayers;
    std::string Level::mfileName = "Level";
    const std::string Level::msavePath = "build/Levels/";
    const std::string Level::mtileSetPath = "build/Assets/";
    const std::string Level::mextension = ".txt";

    bool Level::mRemove = false;
    bool Level::mSingleLayerRendering = false;

    int Level::mGridHeight = 31;
    int Level::mGridWidth = 32;
    int Level::mtileSize = 32;
    int Level::mLayerCount = 3;

    void Level::TilePlacement()
    {
        if(mPlace)
        {
            // Conversion coordonnees ecran en coordonnees grille
            float worldX = mMouseX + mcamera.x;
            float worldY = mMouseY + mcamera.y;
            int xPos = worldX / mtileSize;
            int yPos = worldY / mtileSize;

            if(xPos < 0 || xPos >= mGridWidth || yPos < 0 || yPos >= mGridHeight)
            {
                std::cout << "Grid error : Out of bounds\n";
                mPlace = false;
                return;
            }

            // Ajout de la tile a la grille
            if(mRemove)
            {
                mcurrentTileID = -1;
            }
            
            mLayers[mcurrentLayerID].Grid[yPos][xPos] = mcurrentTileID;
  
            mPlace = false;
        }   
    }

    void Level::Update()
    {
        TilePlacement();
    }

    void Level::Render(SDL_Renderer* mRenderer)
    {      
        if(mSingleLayerRendering)
        {
            for (int y = 0; y < mGridHeight; y++)
            {
                for (int x = 0; x < mGridWidth; x++)
                {
                    int _tileID = mLayers[mcurrentLayerID].Grid[y][x];
                    if(_tileID == -1) continue; 
        
                    SDL_FRect rect;
                    rect.x = (x * mtileSize) - mcamera.x;
                    rect.y = (y * mtileSize) - mcamera.y;
                    rect.w = mtileSize;
                    rect.h = mtileSize;

                    // Afficher la texture s'il y a une texture correspondante a l'ID
                    // Afficher un carre de debug sinon
                    if(IDIsUsed(_tileID))
                    {
                        SDL_Texture* _tex = mtileSet[_tileID].texture;
                        SDL_RenderTexture(mRenderer, _tex, nullptr, &rect);
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(mRenderer, 255, 0, 255, 255);
                        SDL_RenderRect(mRenderer, &rect);
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

                        SDL_Texture* _tex = mtileSet[_tileID].texture;
            
                        SDL_FRect rect;
                        rect.x = (x * mtileSize) - mcamera.x;
                        rect.y = (y * mtileSize) - mcamera.y;
                        rect.w = mtileSize;
                        rect.h = mtileSize;

                        if(IDIsUsed(_tileID))
                        {
                            SDL_Texture* _tex = mtileSet[_tileID].texture;
                            SDL_RenderTexture(mRenderer, _tex, nullptr, &rect);
                        }
                        else
                        {
                            SDL_SetRenderDrawColor(mRenderer, 255, 0, 255, 255);
                            SDL_RenderRect(mRenderer, &rect);
                        }
                    }
                }
            }
        }
    }

    void Level::MouseGetter(SDL_Event event)
    {
        // Verification d'evenement souris (clic gauche)
        if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            mMouseX = event.button.x;
            mMouseY = event.button.y;
            mPlace = true;
        }

        if(event.type == SDL_EVENT_MOUSE_MOTION && (event.motion.state & SDL_BUTTON_RMASK))
        {
            mcamera.x -= event.motion.xrel;
            mcamera.y -= event.motion.yrel;
        }
    }

    void Level::InitLayers()
    {
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
        std::cout << "Initialisation des tuiles\n";

        std::ifstream file("build/Assets/Tileset.txt");
        if(!file)
        {
            std::cout << "Impossible d'ouvrir le tileset\n";
            return;
        }

        std::string header;
        file >> header;
        if(header != "MINIEDITORTILESET")
        {
            std::cout << "Fichier tileset non valide\n";
            return;
        }

        int _id;
        std::string _fileName;

        while(file >> _id >> _fileName)
        {
            Tile _tile;

            _tile.tileID = _id;
            _tile.name = std::filesystem::path(_fileName).string();

            std::filesystem::path _tilePath = mtilesPath + _fileName;

            if(!std::filesystem::exists(_tilePath))
            {
                std::cout << "Tile manquante " << _tilePath << "\n";
                continue;
            }

            _tile.texture = IMG_LoadTexture(mRenderer, _tilePath.c_str());
            if(!_tile.texture)
            {
                std::cout << "Impossible de charger la texture " << _tilePath << "\n";
                continue;
            }

            musedIDs.push_back(_tile.tileID);
            mtileSet.push_back({_tile.tileID, _tile.name, _tile.texture});
        }

        std::cout << "Tuiles initialisees\n";
    }

    Level::Level()
    {
        InitLayers();
    }

    void Level::SaveLevel()
    {
        std::cout << "Sauvegarde du niveau " << mfileName << ".txt\n";
        std::filesystem::create_directories(msavePath);
        
        // fichier de sauvegarde du niveau
        std::string completName = msavePath + mfileName + mextension;
        std::ofstream file(completName);

        // Cle du format
        file << "MiniEditorFile" << "\n";

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
        
        file.close();
    }

    void Level::LoadLevel()
    {
        std::cout << "Chargement du niveau " << mfileName << ".txt\n";

        std::string completName = msavePath + mfileName + mextension;

        std::ifstream file(completName);
        if(!file.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier. Veuillez verifier s'il exixte\n" << std::endl;
            return;
        }

        std::string token;

        file >> token;

        if(token != "MiniEditorFile")
        {
            std::cerr << "Fichier inconnu\n";
            return;
        }

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
        
        std::cout << "Niveau " << mfileName << ".txt Charger avec succes\n";
    }

    void Level::ResetCamera()
    {
        std::cout << "Camera remit a l'origin\n";
        mcamera.x = 0.0f;
        mcamera.y = 0.0f;
    }

    bool Level::IDIsUsed(int tileID)
    {
        for(const auto& _id : musedIDs)
        {
            if(tileID == _id)
            {
                return true;
            }
        }

        return false;
    }

    void Level::WriteTileSet()
    {
        // Les noms de fichiers
        std::vector<std::string> _fileNames;

        std::string completName = mtileSetPath + "Tileset" + mextension;
        std::ofstream file(completName);

        // Ajout du header personaliser
        file << "MINIEDITORTILESET" << "\n\n";

        // Recuperation des noms de tout les fichier image (format .png) dans Assets/Tiles
        for(const auto& entry : std::filesystem::directory_iterator("Assets/Tiles"))
        {
            if(!entry.is_regular_file())
                continue;
            
            auto path = entry.path();
            if(path.extension() != ".png")
                continue;
            std::string _fileName = path.stem().string();

            _fileNames.push_back(_fileName);
        }
        
        // Classe les noms par ordre alphabetique pour eviter les erreur dans les chargement de texture
        std::sort(_fileNames.begin(), _fileNames.end(), [](const auto& a, const auto& b){return a < b;});

        // Ecriture du TileSet (Tileset.txt)
        for(const auto& name : _fileNames)
        {
            int i = 0;
            while(i < name.size() && std::isdigit(name[i]))
                i++;
            
            if(i == 0)
                continue;
            
            int _id = std::stoi(name.substr(0, i));

            file << _id << " ";
            file << name << ".png\n";
        }

        file.close();
    }
}

