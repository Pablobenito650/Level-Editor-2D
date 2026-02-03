import os
import shutil
from pathlib import Path

build_dir = Path("build")

# Nettoyage
def clean():
    if build_dir.exists():
        shutil.rmtree(build_dir)
        print("Dossier build nettoyer !")
    else:
        print("Dossier build introuvable !")
    
if __name__ == "__main__":
    clean()