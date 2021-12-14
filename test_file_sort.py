import os
from struct import pack, unpack
from random import randint

from file_sort import FileSortMerge, FileSortMergeHeap, FileSortCounting
from timer import Log_profile


def generate_u16_file(filename, size):
    with open(filename, 'wb') as f:
        for i in range(size):
            f.write(pack('H', randint(0, 65535)))


def check_file_sorted(filename):
    ret = True
    v1 = (0, )
    v2 = (0, )
    with open(filename, 'rb') as f:
        while (v1 <= v2):
            buf = f.read(2)
            if len(buf) == 0:
                return True  # end of File
            v1 = v2
            v2 = unpack("H", buf)
    return False


if __name__ == "__main__":

    size = 10**3
    fname = f'test_u16_{size//1000}k.bin'

    if os.path.exists(f'{fname}.unsorted'):
        print("Restore unsorted file from backup")
        os.system(f'copy {fname}.unsorted {fname}')
    else:
        print(f"Generating new unsorted file size = {size}")
        generate_u16_file(fname, size)
        print("Make file backup")
        os.system(f'copy {fname} {fname}.unsorted')

    print(f"Sort file...")

    # fsort = FileSortMerge(fname)
    # with Log_profile("Sort time"):
    #     fsort.sort_without_cats()

    # fsort = FileSortMergeHeap(fname)
    # with Log_profile("Sort time"):
    #     fsort.sort_without_cats()

    # fsort = FileSortCounting(fname)
    # with Log_profile("Sort time"):
    #     fsort.sort_with_cats()

    fsort = FileSortCounting(fname)
    with Log_profile("Sort time"):
        fsort.sort_without_cats()


    print(f"Check result...")
    result = check_file_sorted(fname)
    print(f'Sorted file check result: {result}')
