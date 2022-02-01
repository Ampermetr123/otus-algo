import folder_test as ft
import multi_func_tester as mft

from strings import find_string_brute

if __name__ == "__main__":

    test_folders = ("tests/data",)

    testing_func_list = [
        (find_string_brute, range(5)),
    ]

    # def make_description_for_test(args):
    #     arr_line_1 = args[0]
    #     return str(f'N={arr_line_1[0]}')

    tester = mft.MultiFuncFolderTester(testing_func_list, "Поиск в строке  ")

    with open("report_all.md", "w", encoding="utf-8") as f:
        # with open(sys.stdout, "w", encoding="utf-8") as f:
        # f = sys.stdout
        f.write("# Поиск в строке \n\n")
        for folder_path in test_folders:
            folder_name = folder_path.split("/")[-1]
            tester.do_test(folder_path, ft.load_as_2_string, ft.load_as_ints)
            tester.write_report_tr(open(1, "w"))
            tester.write_report_tr(f, f"Тесты '{folder_name}'")
            f.flush()
