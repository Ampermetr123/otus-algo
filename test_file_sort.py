import os
from struct import pack, unpack
from random import randint

from file_sort import FileSortMerge, FileSortMergeHeap, FileSortCounting, FileSortCountingNoCats
from timer import Log_profile


def get_unsorted_file(size):

    def generate_u16_file(filename, size):
        with open(filename, 'wb') as f:
            for i in range(size):
                f.write(pack('H', randint(0, 65535)))

    fname = f'test_u16_{size//1000}k.bin'

    if os.path.exists(f'{fname}.unsorted'):
        #print("Restore unsorted file from backup")
        os.system(f'copy /Y {fname}.unsorted {fname} >NUL')
    else:
        #print(f"Generating new unsorted file size = {size}")
        generate_u16_file(fname, size)
        os.system(f'copy {fname} {fname}.unsorted >NUL')

    return fname


def check_file_sorted(filename):
    ret = True
    v1 = (0, )
    v2 = (0, )
    pos = 0
    with open(filename, 'rb') as f:
        while (v1 <= v2):
            buf = f.read(2)
            if len(buf) == 0:
                return True  # end of File
            v1 = v2
            v2 = unpack("H", buf)
            pos += 1

    print(f'Unsorted on pos {pos} with {hex(v1[0])} > {hex(v2[0])} ')
    return False


def test_file_sort(FileSort, size):
    fname = get_unsorted_file(size)
    fsort = FileSort(fname)
    print(f"\nSorting file with {size} items using {FileSort.__name__} ...", end='')
    
    with Log_profile("time:"):
        fsort.sort()
    
    print(f"Checking file is sorted...", end='')
    print(check_file_sorted(fname))


if __name__ == "__main__":

    tests = [
        ( FileSortMerge,  (10**4, 10**5, 10**6) ),
        ( FileSortMergeHeap,  (10**4, 10**5, 10**6, 10**7) ),
        ( FileSortCounting,  (10**4, 10**5, 10**6, 10**7) ),
        ( FileSortCountingNoCats,  (10**4, 10**5, 10**6, 10**7, 10**8) )
    ]

    for sorter, rng in tests:
        for size in rng:
            test_file_sort(sorter, size)
