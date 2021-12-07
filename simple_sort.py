from array import array
import folder_test as ft
import multi_func_tester as mft
import itertools as itools


def swap(d: array, i, j):
    # for list or array we can do: d[i], d[j] = d[j], d[i]

    # only for array  (a little bit more efficient)
    x = d[i]
    d[i] = d[j]
    d[j] = x


################################################################################
#                                 BUBBLE SORTS                                 #
################################################################################


def bubble_sort(size, d):
    '''Сорт. пузырьком'''
    size = len(d)
    for i in range(0, size):
        for j in range(0, size-i-1):
            if d[j] > d[j+1]:
                swap(d, j, j+1)
    return d


def bubble_sort_opt(size, d):
    '''Сорт. пузырьком с опт. '''
    size = len(d)
    swapped = True if size > 0 else False
    i = 0
    while swapped == True:
        swapped = False
        i += 1
        for j in range(0, size-i):
            if d[j] > d[j+1]:
                swap(d, j, j+1)
                swapped = True
    return d


################################################################################
#                                 INSERT SORTS                                 #
################################################################################

def insert_sort_swap(size, d):
    '''Сорт. вставкой с перемещением'''
    size = len(d)
    sz = 0  # граница отсортированной части
    while sz < size:
        x = d[sz]

        # перемещение перестановкой
        j = sz
        while j > 0 and d[j] < d[j-1]:
            swap(d, j, j-1)
            j -= 1

        sz += 1

    return d


def insert_sort_shift(size, d):
    '''Сорт. вставкой со сдвигом'''
    size = len(d)
    sz = 0  # граница отсортированной части
    while sz < size:
        x = d[sz]

        # сдвиг вправо до точки вставки
        j = sz-1
        while j >= 0 and x < d[j]:
            d[j+1] = d[j]
            j -= 1

        d[j+1] = x  # вставка
        sz += 1

    return d


def insert_sort_bs(size, d):
    '''Сорт. вставкой с бин. поиском'''

    def bin_search(arr, val, p1, p2):
        '''find insert pos in array in range [p1,p2]'''
        if val >= arr[p2]:
            return p2+1
        if val <= arr[p1]:
            return p1
        if p1+1 == p2:
            return p2
        mid = (p1 + p2) // 2
        if val >= arr[mid]:
            return bin_search(arr, val, mid, p2)
        else:
            return bin_search(arr, val, p1, mid)

    sd = array(d.typecode, (d[0],))  # отсортированный массив помещаем сюда
    size = len(d)
    for i in range(1, size):
        j = bin_search(sd, d[i], 0, len(sd)-1)
        sd.insert(j, d[i])
    return sd


################################################################################
#                                  SHELL SORTS                                  #
################################################################################

def shell_sort_impl(d, gaps):
    '''Сорт. Шелла'''
    size = len(d)
    for gap in gaps:
        # insertion sort with gap
        for i in range(gap, size, gap):   # for (i=0; i<size; i+=gap)
            x = d[i]  # sorted < i <= unsorted
            # сдвиг вправо до точки вставки
            j = i - gap    # sorted <=j < unsorted
            while j >= 0 and x < d[j]:
                d[j+gap] = d[j]
                j -= gap
            d[j+gap] = x  # вставка
    return d


def shell_sort_1(size, d):
    '''Сорт. Шелла  1 / 2^k '''
    n = len(d)
    gaps_gen = itools.starmap(lambda k: n // 2**k, ((k,)
                              for k in itools.count(1, 1)))
    gaps = list(itools.takewhile(lambda k: k > 1, gaps_gen))
    gaps.append(1)
    return shell_sort_impl(d, gaps)


def shell_sort_2(size, d):
    '''Сорт. Шелла 2^k+1'''
    n = len(d)
    # последовательность по формуле
    seq = itools.starmap(lambda k: 2**k + 1,
                         ((k,) for k in itools.count(1, 1)))
    # ограничили размером N / 2, префикс списка 1
    gaps = list(itools.chain(
        (1,), itools.takewhile(lambda x: x <= (n // 2), seq)))
    return shell_sort_impl(d, list(reversed(gaps)))


def shell_sort_3(size_, d):
    '''Сорт. Шелла 3-smooth'''
    def three_smooth():
        '''2^p  3^q '''
        s = [1]
        ip = 0  # index of p
        iq = 0  # index of q
        while True:
            yield s[-1]
            np = 2 * s[ip]  # 2^p
            nq = 3 * s[iq]  # 3^q
            s.append(min(np, nq))
            ip += np <= nq
            iq += nq <= np

    n = len(d)
    # ограничили размером N / 3
    gaps = list(itools.takewhile(lambda x: x <= (n // 3),  three_smooth()))
    return shell_sort_impl(d, list(reversed(gaps)))


################################################################################
#                          MAIN (TEST AND REPORT GENERATION)                   #
################################################################################


if __name__ == "__main__":

    test_folders = (
        'test/0.random',
        'test/1.digits',
        'test/2.sorted',
        'test/3.revers'
    )

    testing_func_list = [
       # (bubble_sort, range(5)),
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
