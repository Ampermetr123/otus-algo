from subprocess import run
import sys
import os
from pathlib import Path

if __name__ == '__main__':

    cwd = os.path.dirname(sys.argv[0])
    unpackdir = os.path.join(cwd, "unpacked")
    if not os.path.exists(unpackdir):
        os.makedirs(unpackdir)

    # MSVC artifact path  -  change it if you have other location
    rle = os.path.join(cwd, "..", "Debug", "Debug", "rle.exe")
    temp_file = os.path.join(cwd, 'temp1.rle')

    pathlist = Path(cwd).glob('data_test_*')
    for file in pathlist:
        file_basename = os.path.basename(file)
        # pack
        sz1 = os.path.getsize(file)
        ret = run([rle, "-c", file, temp_file])
        assert(ret.returncode == 0)
        sz2 = os.path.getsize(temp_file)
        p = sz2 / sz1 * 100
        print(file_basename, f" - compressed size is {p:.0f}%")

        # unpack
        temp_file_unpacked = os.path.join(unpackdir, file_basename)
        if os.path.exists(temp_file_unpacked):
            os.remove(temp_file_unpacked)
        ret = run([rle, "-x", temp_file, temp_file_unpacked])
        assert(ret.returncode == 0)
        os.remove(temp_file)
