###############################################################################
#                       TEST AND REPORT FOR QUICK SORTS                       #
###############################################################################

import folder_test as ft
import multi_func_tester as mft
from sort_quick import quick_sort_1, quick_sort_2, merge_sort
from array import array


def python_sort(size_, d):
    '''sorted() от Python'''
    return array('i', sorted(d))


if __name__ == "__main__":

    test_folders = (
        'test/0.random',
        'test/1.digits',
        'test/2.sorted',
        'test/3.revers',
    )

    testing_func_list = [
        (quick_sort_1, range(0, 5)),
        (quick_sort_2, range(0, 5)),
        (merge_sort, range(0, 6)),
        (python_sort, range(0, 7)),
    ]

    def make_description_for_test(args):
        arr_line_1 = args[0]
        return str(f'N={arr_line_1[0]}')

    tester = mft.MultiFuncFolderTester(
        testing_func_list,
        f"Быстрая сортировка ",
        make_description_for_test
    )

    with open("report_quick_and_merge_sort.md", 'w', encoding='utf-8') as f:
        f.write("# Быстрая сортировка и сортировка слиянием \n\n")
        for folder_path in test_folders:
            folder_name = folder_path.split('/')[-1]
            tester.do_test(folder_path,
                           ft.load_as_int_arrays,
                           ft.load_as_int_arrays)
            tester.write_report(open(1, 'w'))
            tester.write_report(f, f"Тесты '{folder_name}'")
