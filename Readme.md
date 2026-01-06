# Level Editor 2D * editeur de nieau pour jeu 2D

![C++]
![Python]
![SDL3]
![SDL3_image]
![ImGUI]

Level Editor 2D est une application qui permet de creer des zone de jeux en 2D en se basant sur le systeme de grille et de tilemap.

## 📦 Structure du Projet

```
Level Editor 2D/
├── Assets/               # Tiles
├── Build/                # Fichiers de compilation
├── src/
│   ├── Core/             # Base de l'Editor
│   │   └── Level.h       # Classe principale
│   ├── App.h             # Creation de l'Application
│   └── GUI.h             # Gestion de l'Interface Utilisateur
├── thirdparty            # Bibliotheques
│   └── imGui/            # Library GUI
└── build.py              # Scripts de compilation 
```
## Fonctionnalites

- **TileMap** : Placer ou supprimer des tiles sur une grille
- **Sauvegarde** : Sauvegarder dans un format txt structurer avec choix du nom du fichier 
- **Charger** : charger un niveau depuis un fichier txt conforme
- **Layer** : gestion des couches
- **RenduSeul** : Possibilite de rendre chaque couche individuellement
- **Camera** : camera (vue sur la scene)

## 🚀 Installation et Utilisation

### Prérequis

- **Compiler C++** : Clang++
- **SDL3** : Bibliothèque graphique
- **SDL3_image** : Bibliotheque graphique pour la gestion du rendu des images SDL (ici tiles en .png)
- **ImGUI** : Bibliotheque GUI
- **Système** : Windows (MSYS2) ou Linux

### Installation sur Windows (MSYS2)

# Clang et SDL3
1. Installer MSYS2 depuis https://www.msys2.org/
2. Ouvrir MSYS2 UCRT64 et installer les dépendances :
```bash
pacman -S mingw-w64-ucrt-x86_64-clang mingw-w64-ucrt-x86_64-SDL3 mingw-w64-ucrt-x86_64-SDL3_image 
```

### Installation sur Linux (Ubuntu/Debian)

```bash
sudo apt install clang libsdl3-dev
```
# ImGui

```bash
git clone https://github.com/ocornut/imgui.git
```

# SDL_iamge

```bash
sudo apt install libsdl3-image-dev
```

### Compilation et Exécution

```bash
# Compiler le projet

#Linux
python3 Build.py

#Windows
python Build.py
```

### Actions clavier/souris
1. S : sauvegarder le niveau
2. R : Recentrer la camera scene
3. Boutton droit de la souris : maintenir et deplacer la souris pour deplacer la camera scene

### A savoir

1. Tout les nom d'images contenues dans Assets/Tiles doivent respecter le format id_nom_de_iamge.png pour pouvoir etre enregistrer dans le Tileset
du projet et etre utiliser en tant que tuile. 
2. Il faut eviter de Modifier le Fichier Tileset.txt soit meme et aussi avoir deux images dont le nom commence par le meme id.
3. Garder le Tileset telquel pour eviter les surprise lors du chargement d'un niveau
