################################################################################
#                                  SHELL SORTS                                  #
################################################################################

from array import array
import itertools as itools


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
