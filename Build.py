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
    def compile_sources(self, sourceFile: Path) -> Path:
        includes = self.get_includes()
        
        objectFile = self.object_dir  / f"{sourceFile.stem}.o"

        if objectFile.exists():
            sourceTime = os.path.getmtime(sourceFile)
            objectTime = os.path.getmtime(objectFile)
            if sourceTime < objectTime:
                print(f"{sourceFile.name} deja a jour")
                self.object_files[str(sourceFile)] = str(objectFile)
                return objectFile
        
        compilerCmd = [
            self.compiler, 
            *includes,
            "-c", str(sourceFile),
            "-o", str(objectFile),
            *self.compiler_flags
        ] 

        print(f"Compilation de {sourceFile.name}....")
        result = subprocess.run(compilerCmd, capture_output=True, text=True)

        if result.returncode != 0:
            print(f"Erreur de compilation pour {sourceFile.name}:")
            print(result.stderr)
            sys.exit(1)
        
        self.object_files[str(sourceFile)] = str(objectFile)
        return objectFile
    
    def LinkObjects(self, objectFiles: list, outputName: Path) -> bool:
        includes = self.get_includes()

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

        print(f"Linking vers {outputName.name}....")
        result = subprocess.run(linkCmd, capture_output=True, text=True)

        
        if result.returncode != 0:
            print("Erreur de linking: ")
            print(result.stderr)
            return False
        
        print(f"Executable cree: {outputName}")
        return True
    
    def CopyAssets(self):
        # copie des resources necessaires
        print("\nCopie des ressources....")
        shutil.copytree(self.assets_dir, os.path.join(self.build_dir, "Assets"), dirs_exist_ok=True)
        shutil.copytree(self.ressources_dir, self.build_dir, dirs_exist_ok=True)
        print("Assets copies avec succes")
        
    def Main(self) -> int:
        self.build_dir.mkdir(exist_ok=True)
        self.object_dir.mkdir(exist_ok=True)

        sourceFiles = self.find_sources()

        objectFiles = []

        for sourceFile in sourceFiles:
            objectFile = self.compile_sources(sourceFile)
            objectFiles.append(objectFile)
        
        outputPath = self.build_dir / self.output_name

        if not self.LinkObjects(self.object_files, outputPath):
            return 1
        
        self.CopyAssets()

        print("\nBuild terminer avec succes!")
        print(f"Executable: {outputPath}")

        return 0


    # fichiers sources
    def find_sources(self) -> bool:
        sources = list(Path("src").rglob("*.cpp"))

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