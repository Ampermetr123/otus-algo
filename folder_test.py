import unittest
import os
import time
import sys
import array


###############################################################################
#                              LOADERS FUNCTIONS                              #
# ВХОДНОЙ ПАРАМЕТР - ФАЙЛ (ПРЕДВАРИТЕЛЬНО ОТКРЫТ)                             #
# ВЫХОДНОЙ ПАРАМЕТР ЗНАЧЕНИЕ НУЖНОГО ТИПА: СТРОКА, ЧИСЛО, СПИСОК, МАССИВ...   #
# ИЛИ TUPLE ЗНАЧЕНИЙ - КАЖДЫЙ ЭЛЕМЕНТ TUPLE ПЕРЕДАЕТСЯ КАК АРГУМЕНТ В ФУНКЦИЮ #
################################################################################

def load_as_strings(f):
    ''' строки, разделенные \n -> список строк '''
    str = f.readall()
    return (str.splitlines())


def load_as_ints(f):
    '''целые числа разделенные \n и проблеами -> cписок целых чисел, или единственное число
       *если в файле всего одно число - то возварщается это число без списка'''
    args = []
    for ln in f.readlines():
        ln = ln.rstrip('\r\n').split()
        for x in ln:
            args.append(int(x))
    if (size := len(args)) > 1:
        return tuple(args)
    elif size == 1:
        return args[0]
    else:
        return None


def load_as_floats(f):
    ''' вещественные числа разделенные \n и проблеами -> cписок целых чисел
        *если в файле всего одно число - то возварщается это число без списка'''
    args = []
    for ln in f.readlines():
        ln = ln.rstrip('\r\n').split()
        for x in ln:
            args.append(float(x))
    if (size := len(args)) > 1:
        return tuple(args)
    elif size == 1:
        return args[0]
    else:
        return None


def load_as_int_arrays(f):
    args = []
    for ln in f.readlines():
        arr = array.array('i')
        if len(ln) == 0:
            continue
        gen = (int(x) for x in ln.rstrip('\r\n').split())
        arr.extend(gen)
        args.append(arr)
    if (size := len(args)) > 1:
        return tuple(args)
    elif size == 1:
        return args[0]
    else:
        return None


################################################################################
#              ОСНОВНОЙ КЛАСС ДЛЯ ТЕСТИРОВАНИЯ В СИСТЕМЕ UNITTEST              #
################################################################################

class FolderTestCase(unittest.TestCase):

    def setUp(self):
        test_name = self.id().split('.')[-1]
        print("\n %s:" % test_name, end="", flush=True)
        file_in = self.input_files[test_name]
        with open(file_in, encoding='locale') as f:
            self.input_args = self.loaders['in'](f)

        file_out = self.output_files[test_name]
        with open(file_out, encoding='locale') as f:
            self.correct_answer = self.loaders['out'](f)

        self.start_time = self.stop_time = self.run_time = 0
        self.test_result = False

    def tearDown(self):
        test_name = self.id().split('.')[-1]

        # Признак того, что тест не пройден
        if self.stop_time < self.start_time:
            self.stop_time = time.perf_counter()

        self.run_time = self.stop_time - self.start_time
        print(" %.3f s  " % self.run_time, end='', flush=True)
        self.times[test_name] = self.run_time

        if self.test_callback:
            self.test_callback[0](test_name, self)

    @classmethod
    def setUpClass(self):
        print(f'\nTesting "{self.testing_func_name}"() {self.testing_func_doc}')
        print('Tests folder:', self.test_source, end='', flush=True)
        self.times = {}  # test_name -> duration

    @classmethod
    def tearDownClass(self):
        test_method_list = [func for func in dir(self) if callable(
            getattr(self, func)) and func.startswith('test_')]
        for func in test_method_list:
            delattr(self, func)


###############################################################################
#                    ФУНКЦИИ НАСТРОЙКИ КЛАССА ТЕСТИРВАНИЯ                     #
################################################################################

def create_test(func):
    """Add test to FolderTestCase"""

    def test(self):
        self.start_time = time.perf_counter()

        if isinstance(self.input_args, tuple):
            func_result = func(*self.input_args)
        else:
            func_result = func(self.input_args)

        self.stop_time = time.perf_counter()

        if (isinstance(self.correct_answer, float)):
            self.assertAlmostEqual(func_result, self.correct_answer,
                                   msg="with input %s" % repr(self.input_args)[:50])
        else:
            self.assertEqual(func_result, self.correct_answer,
                             msg="with input %s" % repr(self.input_args)[:50])
        self.test_result = True

    return test


def create_test_cases(folder_path, func, inp_loader, out_loader, rng, test_callback) -> int:
    """Create FolderTestCase for files in folder"""
    if not os.path.isdir(folder_path):
        raise RuntimeError("%s is not a directory" % folder_path)

    setattr(FolderTestCase, 'test_source', folder_path)
    setattr(FolderTestCase, 'input_files', dict())
    setattr(FolderTestCase, 'output_files', dict())
    setattr(FolderTestCase, 'loaders', {'in': inp_loader, 'out': out_loader})
    setattr(FolderTestCase, 'test_callback', [test_callback])
    setattr(FolderTestCase, 'testing_func_name', func.__name__)
    setattr(FolderTestCase, 'testing_func_doc', func.__doc__)

    test_count = 0
    if rng is None:
        rng = range(sys.maxsize)
    for i in rng:
        inp_file = os.path.join(folder_path, f"test.{i}.in")
        out_file = os.path.join(folder_path, f"test.{i}.out")
        if os.path.isfile(inp_file) and os.path.isfile(out_file):
            test_count += 1
            setattr(FolderTestCase, test_name :=
                    f"test_{i}", create_test(func))
            FolderTestCase.input_files[test_name] = inp_file
            FolderTestCase.output_files[test_name] = out_file
        else:
            break
    return test_count


################################################################################
#                            ФУНКЦИЯ ЗАПУСКА ТЕСТОВ                            #
################################################################################

def run_test(folder, func, inp_loader=load_as_strings, out_loader=load_as_strings,
             rng=None, test_callback=None):
    """Setup and run test for _func_ using test data files in _folder_"""

    test_count = create_test_cases(os.path.abspath(
        folder), func, inp_loader, out_loader, rng, test_callback)

    if test_count == 0:
        raise UserWarning(
            """%s directory doesn't consist any tests.      
            Each test must heave 2 files: test_{x}.in and test_{x}.out""" % folder
        )
    else:
        unittest.main(module=__name__, exit=False, verbosity=1)
