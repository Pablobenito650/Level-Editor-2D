import subprocess
import sys
from pathlib import Path

build_dir = Path("build")
platform = sys.platform
output_name = "LevelEditor.exe" if platform == "win32" else "LevelEditor"

# Lancement de l'application
def run():
    output_path = build_dir / output_name
    if output_path.exists():
        print(f"Lancement de {output_path}......")

        subprocess.run([f"./{output_path}" if platform != "win32" else str(output_path)])
    else:
        print("Executable introuvable. Veuillez rebuild")

if __name__ == "__main__":
    sys.exit(run())