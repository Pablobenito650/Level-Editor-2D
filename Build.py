import subprocess
import sys
import shutil 
import os
from pathlib import Path

class Builder: 
    def __init__(self):
        self.platform = sys.platform
        self.compiler = "clang++"
        self.flags = ["-std=c++17", "-O2", "-g"]
        self.imgui_dir = Path("thirdparty/imgui")
        self.build_dir = Path("build")
        self.assets_dir = Path("Assets")
        self.output_name = "LevelEditor.exe" if self.platform == "win32" else "LevelEditor"

    # compilation
    def build(self):
        self.build_dir.mkdir(exist_ok=True)

        sources = self.find_sources()
        includes = self.get_includes()
        libraries = self.get_libraries()
        output_path = self.build_dir / self.output_name

        # commande
        cmd = [
            self.compiler,
            *self.flags,
            *includes,
            *sources,
            *libraries,
            "-o", 
            str(output_path)
        ]

        # gestion plateforme
        if self.platform == "win32":
            cmd.extend(["-lSDL3", "-lSDL3_image", "-limm32", "-loleaut32"])
        else:
            cmd.extend(["-lSDL3", "-lSDL3_image", "-ldl", "-lpthread", "-lGL"])
        
        print(f"Compilation en cours.......")

        # Lancement de la commande de compilation
        result = subprocess.run(cmd)
        if result.returncode == 0:
            print(f"Build reussi dans : {output_path}")

            # copie des resources necessaires
            print("Copie des ressources....")
            shutil.copytree(self.assets_dir, os.path.join(self.build_dir, "Assets"), dirs_exist_ok=True)
            print("Assets copies avec succes")
        else:
            print("Erreur de compilation")

    # fichiers sources
    def find_sources(self):
        sources = list(Path("src").rglob("*.cpp"))

        if self.imgui_dir.exists():
            sources.extend(list(self.imgui_dir.glob("*.cpp")))
            sources.extend(list(self.imgui_dir.glob("backends/imgui_impl_sdl3.cpp")))
            sources.extend(list(self.imgui_dir.glob("backends/imgui_impl_sdlrenderer3.cpp")))
            sources.extend(list(self.imgui_dir.glob("misc/cpp/imgui_stdlib.cpp")))

            return [str(s) for s in sources]
    
    # paths des includes
    def get_includes(self):
        includes = [
            "-Isrc",
            f"-I{self.imgui_dir}",
            f"-I{self.imgui_dir}/backends",
            f"-I{self.imgui_dir}/misc/cpp"
        ]

        return includes
        
    def get_libraries(self):
        return []

    # lancer l'application generer
    def run(self):
        output_path = self.build_dir / self.output_name
        if output_path.exists():
            print(f"Lancement de {output_path}......")

            subprocess.run([f"./{output_path}" if self.platform != "win32" else str(output_path)])
        else:
            print("Executable introuvable. Veuillez rebuild")

if __name__ == "__main__":
    builder = Builder()
    builder.build()
    builder.run()