###############################################################################
#          TEST AND REPORT FOR SELECTION AND PYRAMID SORTS                    #
###############################################################################

import folder_test as ft
import multi_func_tester as mft
from array import array

from sort_selection import select_sort, heap_sort

def python_sort(size_, d):
    '''sorted() от Python'''
    return array('i', sorted(d))

if __name__ == "__main__":

    test_folders = (
        'test/0.random',
        'test/1.digits',
        'test/2.sorted',
        'test/3.revers'
    )

    testing_func_list = [
        (select_sort, range(5)),
        (heap_sort, None),
        (python_sort, None),
    ]

    def make_description_for_test(args):
        arr_line_1 = args[0]
        return str(f'N={arr_line_1[0]}')

    tester = mft.MultiFuncFolderTester(
        testing_func_list,
        f"Сортировки вставкой ",
        make_description_for_test
    )

    with open("report_insertion_and_heap.md", 'w', encoding='utf-8') as f:
        f.write("# Сортировки вставкой \n\n")
        for folder_path in test_folders:
            folder_name = folder_path.split('/')[-1]
            tester.do_test(folder_path, 
                           ft.load_as_int_arrays,
                           ft.load_as_int_arrays)
            tester.write_report(open(1, 'w'))
            tester.write_report(f, f"Тесты '{folder_name}'")
            f.flush()
