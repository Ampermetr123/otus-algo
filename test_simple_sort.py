###############################################################################
#          TEST AND REPORT FOR BUUBLE, INSERTION AND SHELL SORTS              #
###############################################################################

import folder_test as ft
import multi_func_tester as mft

from sort_bubble import bubble_sort, bubble_sort_opt
from sort_insertion import insert_sort_bs, insert_sort_shift, insert_sort_swap
from sort_shell import shell_sort_1, shell_sort_2, shell_sort_3


if __name__ == "__main__":

    test_folders = (
        'test/0.random',
        'test/1.digits',
        'test/2.sorted',
        'test/3.revers'
    )

    testing_func_list = [
        (bubble_sort, range(5)),
        (bubble_sort_opt, range(5)),
        (insert_sort_swap, range(5)),
        (insert_sort_shift, range(6)),
        (insert_sort_bs, range(7)),
        (shell_sort_1, range(6)),
        (shell_sort_2, range(6)),
        (shell_sort_3, range(6))
    ]

    def make_description_for_test(args):
        arr_line_1 = args[0]
        return str(f'N={arr_line_1[0]}')

    tester = mft.MultiFuncFolderTester(
        testing_func_list,
        f"Простые сортировки ",
        make_description_for_test
    )

    with open("report_simple_all.md", 'w', encoding='utf-8') as f:

        f.write("# Простые сортировки \n\n")

        for folder_path in test_folders:
            folder_name = folder_path.split('/')[-1]
            tester.do_test(folder_path, 
                           ft.load_as_int_arrays,
                           ft.load_as_int_arrays)
            tester.write_report(open(1, 'w'))
            tester.write_report(f, f"Тесты '{folder_name}'")
