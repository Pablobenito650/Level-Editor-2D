# Level Editor 2D * editeur de niveau pour jeu 2D

![C++]
![Python]
![SDL3]
![SDL3_image]
![ImGUI]

Level Editor 2D est un outil qui permet de créer des zones de jeux. Les editeurs de niveau existe pour tout type de jeux vidéos mais ici nous nous intéressons aux jeux 2D. Notre tache ici est donc créer un outil qui va permettre a un développeur de jeux de concevoir ses niveaux de façon simple et rapide.

Les Editeurs de Niveaux sont des outils qui se révèlent indispensable pour les grandes entreprises et aussi les développeurs indépendants et cela pour plusieurs raisons:
    a. Ils nous évitent de gérer les niveaux depuis le code;
    b. Ils nous permettent de séparer les scripts et les autres contenus (le code seulement pour les mécaniques et le Level Editor pour les maps, etc);
    c. Ils permettent un gain de temps énorme;
    e. Permet une scalabilité pour les gros jeux (grandes quantité de niveaux a concevoir);
    f. Réutilisation des niveaux créer s’ils sont stockées(JSON, XML ou format personnalise);

### Comment configurer le projet

1. Au prealable telecharger ImGui depuis le github officiel (voir plus loin) et le mettre dans thirdparty/imgui
2. Tout les nom d'images contenues dans Assets/Tiles doivent respecter le format id_nom_de_iamge.png pour pouvoir etre enregistrer dans le Tileset
du projet et etre utiliser en tant que tuile. 

## 📦 Structure du Projet

```
Level Editor 2D/
├── Assets/               # Tiles
├── Build/                # Application
├── src/
│   ├── Core/             # Base de l'Editor
|   |   └── Components.h  # Classe pour la gestion des composants
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
- **Camera** : camera (vue sur la scene) et zoom
- **Lumiere** : Eclairage simple a partir de textures de lumiere (.png dans Assets/Lights et lightmap) et eclairage global avec cache noire transparente
- **Animation** : animation a base d'un atlas de facon lineaire

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

## Nettoyer
#Linux
python3 Clean.py

#Windows
python Clean.py

```

### Actions clavier/souris

1. **Ctrl+S** : sauvegarder le niveau 
2. **Ctrl+O** : Charger un niveau 
3. **R** : Recentrer la camera scene
4. **T** : Mettre a jour le tile set si l'on a ajouter une image pendant que le programme tourne
5. **Right Mouse Button** : maintenir et deplacer la souris pour deplacer la camera scene (panning)
6. **Mouse Wheel** : gestion du zoom

### Extension possibles

1. Amelioration du systeme d'eclairage.
2. Systeme de sauvegarde et de chargements plus robuste
3. Systeme d'undo/redo
4. Augmentation du nombre de couche
5. Donner une vitesse de deplacement unique a chaque couche
6. Ajouter plusieurs components

