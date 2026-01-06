import shutil
import os

#This script is required to adopt the TFT_eSPI library for using in esp32 c3.

#Sources
src_dir="TFT_eSPI_Patches"

#Target inside .pio folder. Tested with: bodmer/TFT_eSPI@2.5.43

dst_dir= os.path.join(".pio", "libdeps","c3", "TFT_eSPI")

print(">>> Patching TFT_eSPI library files ...")

for root, dirs, files in os.walk(src_dir):
    rel_path = os.path.relpath(root, src_dir)
    dst_path = os.path.join(dst_dir, rel_path)

    os.makedirs(dst_path, exist_ok=True)

    for f in files:
        src_file = os.path.join(root, f)
        dst_file = os.path.join(dst_path, f)
        shutil.copy2(src_file, dst_file)
        print(f"    {src_file} -> {dst_file}")