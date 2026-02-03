import subprocess
import sys
import shutil 
import os
from pathlib import Path

class Builder: 
    def __init__(self):
        self.platform = sys.platform
        self.compiler = "clang++"
        self.compiler_flags = ["-std=c++17", "-O2", "-Wall", "-Wextra", "-pedantic"]
        self.imgui_dir = Path("./thirdparty/imgui")
        self.build_dir = Path("build")
        self.object_dir = Path("build/Obj")
        self.assets_dir = Path("Assets")
        self.json_dir = Path("./thirdparty/json")
        self.ressources_dir = Path("Ressources")
        self.object_files = {}
        self.output_name = "LevelEditor.exe" if self.platform == "win32" else "LevelEditor"

    # compilation 
    # Ici on genere les fichiers objets de chaque fichier source utile au projet
    def compile_sources(self, sourceFile: Path) -> Path:

        # Recuperation des includes
        includes = self.get_includes()
        
        # Fichier objet corresponadant
        objectFile = self.object_dir  / f"{sourceFile.stem}.o"

        # Build incremental (compiler seulement un fichier non a jour)
        if objectFile.exists():
            sourceTime = os.path.getmtime(sourceFile)
            objectTime = os.path.getmtime(objectFile)
            if sourceTime < objectTime:
                print(f"{sourceFile.name} deja a jour")
                self.object_files[str(sourceFile)] = str(objectFile)
                return objectFile
        
        # commande de compilation
        compilerCmd = [
            self.compiler, 
            *includes,
            "-c", str(sourceFile),
            "-o", str(objectFile),
            *self.compiler_flags
        ] 

        # Execution de la commande 
        print(f"Compilation de {sourceFile.name}....")
        result = subprocess.run(compilerCmd, capture_output=True, text=True)

        if result.returncode != 0:
            print(f"Erreur de compilation pour {sourceFile.name}:")
            print(result.stderr)
            sys.exit(1)
        
        self.object_files[str(sourceFile)] = str(objectFile)
        return objectFile
    
    # Liaison des fichiers
    def LinkObjects(self, objectFiles: list, outputName: Path) -> bool:

        # Recuperation des includes
        includes = self.get_includes()

        # Commande de linking
        linkCmd = [
            self.compiler,
            *includes,
            *[str(obj) for obj in objectFiles],
            "-o", str(outputName),
            "-std=c++17"
        ]

        # gestion plateforme
        if self.platform == "win32":
            linkCmd.extend(["-lSDL3", "-lSDL3_image", "-limm32", "-loleaut32"])
        else:
            linkCmd.extend(["-lSDL3", "-lSDL3_image", "-ldl", "-lpthread", "-lGL"])

        # Lancement de la commande
        print(f"Linking vers {outputName.name}....")
        result = subprocess.run(linkCmd, capture_output=True, text=True)

        
        if result.returncode != 0:
            print("Erreur de linking: ")
            print(result.stderr)
            return False
        
        print(f"Executable cree: {outputName}")
        return True
    
    # copie des resources necessaires
    def CopyAssets(self):
        print("\nCopie des ressources....")

        # Copie des assets
        shutil.copytree(self.assets_dir, os.path.join(self.build_dir, "Assets"), dirs_exist_ok=True)

        # Copie des librairies (DLL)
        shutil.copytree(self.ressources_dir, self.build_dir, dirs_exist_ok=True)
        print("Assets copies avec succes")
        
    def Main(self) -> int:
        self.build_dir.mkdir(exist_ok=True)
        self.object_dir.mkdir(exist_ok=True)

        sourceFiles = self.find_sources()

        objectFiles = []

        # compilation (generation des fichiers objet)
        for sourceFile in sourceFiles:
            objectFile = self.compile_sources(sourceFile)
            objectFiles.append(objectFile)
        
        outputPath = self.build_dir / self.output_name

        # Linking et creation de l'application
        if not self.LinkObjects(self.object_files, outputPath):
            return 1
        
        # Copie des assets
        self.CopyAssets()

        print("\nBuild terminer avec succes!")
        print(f"Executable: {outputPath}")

        return 0


    # fichiers sources
    def find_sources(self) -> bool:
        sources = list(Path("src").rglob("*.cpp"))

        # Fichier ImGui necessaires
        if(self.imgui_dir).exists():
            sources.extend(list(self.imgui_dir.glob("*.cpp")))
            sources.extend(list(self.imgui_dir.glob("backends/imgui_impl_sdl3.cpp")))
            sources.extend(list(self.imgui_dir.glob("backends/imgui_impl_sdlrenderer3.cpp")))
            sources.extend(list(self.imgui_dir.glob("misc/cpp/imgui_stdlib.cpp")))

        return sources
    
    # paths des includes
    def get_includes(self):
        includes = [
            f"-I{self.imgui_dir}",
            f"-I{self.imgui_dir}/backends",
            f"-I{self.imgui_dir}/misc/cpp",
        ]

        return includes

    
if __name__ == "__main__":
    builder = Builder()
    sys.exit(builder.Main())