import unittest
import os
import glob
import enum
import time
import functools



def int_args(func):
    """Decorator to make conversion to int for each input argument"""
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        new_args = [int(a) for a in args]
        new_kwargs = {}
        for k, v in kwargs.items():
            new_kwargs[k] = int(v)
        return func(*new_args, **new_kwargs)
    return wrapper


class FolderTestCase(unittest.TestCase):

    def setUp(self):
        test_name = self.id().split(".")[-1]
        print("\n %s:" % test_name, end="", flush=True)
        self.start_time = time.perf_counter()

    def tearDown(self):
        run_time = time.perf_counter() - self.start_time
        test_name = self.id().split(".")[-1]
        print(" %.3f s  " % run_time, end='', flush=True)

    @classmethod
    def setUpClass(self):
        print('\nTest source:', self.test_source, end='', flush=True)

    @classmethod
    def tearDownClass(self):
        pass


class CompareAs(enum.Enum):
    STR = 1
    INT = 2
    FLOAT = 3
    LIST = 4


def create_test(fileIn, fileOut, func, compare_strategy):
    """Add test to FolderTestCase"""

    def test(self):
        with open(fileIn) as f:
            file_lines = f.readlines()
        input_params = [s.rstrip("\n ") for s in file_lines]
        with open(fileOut) as f:
            result = f.read().rstrip(" \r\n")
      
        func_result = func(*input_params)
        
        if compare_strategy is CompareAs.INT:
            self.assertEqual(
                func_result, int(result), msg="with input %s" % str(input_params)
            )
        elif compare_strategy is CompareAs.FLOAT:
            self.assertAlmostEqual(
                func_result, float(result), msg="with input %s" % str(input_params)
            )
        elif compare_strategy is CompareAs.LIST:
            stringed_func_res = "\n".join( str(x) for x in func_result )
            self.assertEqual(
                stringed_func_res, result, msg="with input %s" % str(input_params)
            )
        else:
            self.assertEqual(
                func_result, result, msg="with input %s" % str(input_params)
            )

    return test


def create_test_case(folder_path, func, compare_strategy, max_tests) -> int:
    """Create FolderTestCase for files in folder"""

    setattr(FolderTestCase, 'test_source', folder_path)

    files = [
        f for f in glob.glob(os.path.join(folder_path, "test*.in")) if os.path.isfile(f)
    ]
    files.sort()
    test_count = 0
    for in_file in files:
        out_file_name = os.path.basename(in_file)[0:-3] + ".out"
        out_file = os.path.join(folder_path, out_file_name)
        if os.path.isfile(out_file):
            test_count += 1
            test_func = create_test(in_file, out_file, func, compare_strategy)
            test_name = out_file_name[0:-4].replace(".", "_")
            setattr(FolderTestCase, test_name, test_func)
            if (max_tests is not None) and (max_tests > 0) and test_count == max_tests:
                break
    return test_count


def run_test(folder, func, compare_strategy=CompareAs.STR, max_tests=0):
    """Setup and run test for _func_ using test data files in _folder_"""
    test_folder = os.path.abspath(folder)
    if not os.path.isdir(test_folder):
        raise RuntimeError("%s is not a directory" % test_folder)

    test_count = create_test_case(test_folder, func, compare_strategy, max_tests)

    if test_count == 0:
        raise UserWarning(
            """%s directory doesn't consist any tests.\n 
         Each test must heave 2 files: test*.in and test*.out"""
            % test_folder
        )
    else:
        unittest.main(module=__name__, exit=False, verbosity=1)
