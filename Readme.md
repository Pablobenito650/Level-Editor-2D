# Level Editor 2D * editeur de nieau pour jeu 2D

![C++]
![Python]
![SDL3]
![SDL3_image]
![ImGUI]

Level Editor 2D est une application qui permet de creer des zone de jeux en 2D en se basant sur le systeme de grille et de tilemap.

### Comment configurer le projet

1. Au prealable telecharger ImGui depuis le github officiel (voir plus loin) et le mettre dans thirdparty/imgui
2. Tout les nom d'images contenues dans Assets/Tiles doivent respecter le format id_nom_de_iamge.png pour pouvoir etre enregistrer dans le Tileset
du projet et etre utiliser en tant que tuile. 
3. Si dans le Dossier Assets/Tiles il y a deux images dont le nom commence par le meme id, l'un sera pris et l'autre non
4. Garder le Tileset telquel pour eviter les surprise lors du chargement d'un niveau

## 📦 Structure du Projet

```
Level Editor 2D/
├── Assets/               # Tiles
├── Build/                # Application
├── src/
│   ├── Core/             # Base de l'Editor
│   │   └── Level.h       # Classe principale
│   ├── App.h             # Creation de l'Application
│   └── GUI.h             # Gestion de l'Interface Utilisateur
├── thirdparty            # Bibliotheques
│   └── imGui/            # Library GUI
└── build.py              # Script de compilation
└── Run.py                # Script de lancement de l'editeur
└── Clean.py              # Script de nettoyage de build 
```
## Fonctionnalites

- **TileMap** : Placer ou supprimer des tiles sur une grille
- **Sauvegarder** : Sauvegarder dans un format txt structurer avec choix du nom du fichier 
- **Charger** : charger un niveau depuis un fichier txt conforme
- **Layer** : gestion des couches (dispo: sol, joueur, ui)
- **RenduSeul** : Possibilite de rendre chaque couche individuellement
- **Camera** : camera (vue sur la scene)
- **Lumiere** : Eclairage simple a partir de textures de lumiere (.png dans Assets/Lights et lightmap) et eclairage global avec couche noire transparente

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
## Compiler 
#Linux
python3 Build.py

#Windows
python Build.py

## Executer
#Linux
python3 Run.py

#Windows
python Run.py

```

### Actions clavier/souris
1. Ctrl+S : sauvegarder le niveau 
2. Ctrl+O : Charger un niveau 
3. R : Recentrer la camera scene
4. T : Mettre a jour le tile set si l'on a ajouter une image pendant que le programme tournait
5. Boutton droit de la souris : maintenir et deplacer la souris pour deplacer la camera scene

### Limites

1. Difficultes a changer les positions des textures du aux differences positionGrille (reelle) et positionEcran (prenant non seulement en compte l'ecran mais aussi la camera).

2. Le systeme de lumiere est tres basique et ne gere pas les obstacles.
3. Pour le moment l'enregistrement des proprietes de scene (eclairage, etc) n'est pas encore possible
4. Pas de systeme d'undo/redo
5. Pas d'augmentation du nombre de couche
6. La vitesse de deplacement des couche est uniforme: tout les elements de la couche "UI" doivent etre a la meme position a l'ecran 
meme si l'on deplace les autres object donc les autres couches ("Sol", "Joueur")

