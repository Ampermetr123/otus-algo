
import unittest
import os
import glob


class FolderTestCase(unittest.TestCase):
    pass


def create_test(fileIn, fileOut, func):
    '''Add test to FolderTestCase'''
    def test(self):
        with open(fileIn) as f:
            file_lines = f.readlines()
        input_params = [s.rstrip('\n ') for s in file_lines]
        with open(fileOut) as f:
            result = f.read().rstrip(' \r\n')
        func_result = func(*input_params)
        self.assertEqual( func_result, result, msg = "with input %s" % str(input_params))  
    return test


def create_test_case(folder_path, func) -> int:
    '''Create FolderTestCase for files in folder  '''
    files = [f for f in glob.glob(os.path.join(folder_path, "test*.in")) if os.path.isfile(f)]
    files.sort()
    test_count = 0
    for in_file in files:
        out_file_name = os.path.basename(in_file)[0:-3]+".out"
        out_file = os.path.join(folder_path, out_file_name)
        if os.path.isfile(out_file):
            test_count += 1
            test_func = create_test(in_file, out_file, func)
            test_name = out_file_name[0:-4]
            setattr(FolderTestCase, test_name, test_func)
    return test_count


def run_test(folder, func):
    ''' Setup and run test for _func_ using test data files in _folder_  '''
    test_folder = os.path.abspath(folder)
    if not os.path.isdir(test_folder):
        raise RuntimeError("%s is not a directory" % test_folder)
    test_count = create_test_case(test_folder, func)
    if test_count == 0:
         raise UserWarning('''%s directory doesn't consist any tests.\n 
         Each test must heave 2 files: test*.in and test*.out''' % test_folder)
    else:
        unittest.main(module = __name__)
    