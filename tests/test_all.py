import functools
import os
import sys

import folder_test as ft
import multi_func_tester as mft

currentdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(currentdir)
sys.path.append(parentdir)

from strings import find_string_BM, find_string_BMH, find_string_brute, find_string_shift


def repeat(func, n=1):
    """Decorator to reapeat call function for n times"""

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        for _ in range(n):
            rv = func(*args, **kwargs)
        return rv

    return wrapper


if __name__ == "__main__":

    test_folders = ("tests/data",)

    rt = 20
    # repeat times for each function call
    rng = range(3, 5)  # диапазон тестов

    testing_func_list = [
        (repeat(find_string_brute, rt), rng),
        (repeat(find_string_shift, rt), rng),
        (repeat(find_string_BMH, rt), rng),
        (repeat(find_string_BM, rt), rng),
    ]

    def make_description_for_test(args):
        plen = len(args[0])
        slen = len(args[1])
        return str(f"{plen} in {slen}")

    tester = mft.MultiFuncFolderTester(
        testing_func_list, "Поиск в строке  ", make_description_for_test
    )

    with open("report_all.md", "w", encoding="utf-8") as f:

        f.write("# Поиск в строке \n\n")
        for folder_path in test_folders:
            folder_name = folder_path.split("/")[-1]
            tester.do_test(folder_path, ft.load_as_2_string, ft.load_as_ints)
            tester.write_report_tr(open(1, "w"))
            tester.write_report_tr(f, f"Тесты '{folder_name}'")
            f.flush()
