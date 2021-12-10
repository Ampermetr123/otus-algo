
from array import array
from math import log2
import itertools as itools
import sys
import threading


def swap(d, i, j):
    '''swaps d[j] and d[i]'''
    x = d[i]
    d[i] = d[j]
    d[j] = x


def reqursion_func_safe_runner(func, recursion_limit, args):
    ''' Запуск рекурсивной функции , при необходимости, в отдельном потоке c увеличенным стеком'''
    if (recursion_limit > 3000):
        threading.stack_size(32000 + 768 * recursion_limit)
        thread = threading.Thread(target=func, args=(args, ))
        thread.start()
        thread.join()
    else:
       func(args)



################################################################################
#                                  QUICK SORT                                  #
################################################################################

def qs_recurion_limit(size):
    '''Наибольшая глубина рекурсии для быстрой сортировки'''
    return size+100


def quick_sort_1_impl(d):
    '''Быстрая сорт. classic'''

    def split(d, ls, rs):
        '''return index такой, что d[index, ] > p
            ls <=  index <= rs
        '''
        i = ls
        j = rs
        p = d[rs]
        while i <= j:
            if d[i] <= p:
                i += 1
            elif d[j] > p:
                j -= 1
            else:
                swap(d, i, j)
                
        # Если index > rs, то p = d[rs] - максимальный элемент. 
        # Тогда чтобы разделить на две части берем p = d[rs]-1,  index = d[rs]
        return min(i, rs) 

    def sort(d, ls, rs):    # d - data, ls - left side, rs -right side
        if ls < rs:
            p = split(d, ls, rs)
            sort(d, ls, p-1)
            sort(d, p, rs)

    limit = sys.getrecursionlimit()
    sys.setrecursionlimit(max(limit, qs_recurion_limit(len(d))))

    sort(d, 0, len(d)-1)

    sys.setrecursionlimit(limit)

    return d


def quick_sort_2_impl(d):
    '''Быстрая сорт. modern '''

    def split(d, ls, rs):
        '''return index for d, такой, что d[index + 1, ...] > p && d[index] = p
            part1           part2           part3
        | xxxx <=p  xxxx | xxxx >p xxxx | xxxx raw xxxx |  
                       a                i ->            p 
        '''
        a = ls-1
        pval = d[rs]
        for i in range(ls, rs+1):
            if d[i] <= pval:
                a += 1
                swap(d, a, i)

        return a

    def sort(d, ls, rs):
        '''sort d[ls,rs]'''   
        if ls < rs:
            p = split(d, ls, rs)  # p is on right place
            if (p > ls):
                sort(d, ls, p-1)
            if (p < rs):
                sort(d, p+1, rs)

    limit = sys.getrecursionlimit()
    sys.setrecursionlimit(max(limit, qs_recurion_limit(len(d))))
    
    sort(d, 0, len(d)-1)
    
    sys.setrecursionlimit(limit)

    return d


def quick_sort_1(size_, d):
    reqursion_func_safe_runner(quick_sort_1_impl,  qs_recurion_limit(len(d)), d)
    return d


def quick_sort_2(size_, d):
    reqursion_func_safe_runner(quick_sort_2_impl,  qs_recurion_limit(len(d)), d)
    return d



################################################################################
#                                  MERGE SORT                                  #
################################################################################

def ms_recurion_limit(size):
    '''Наибольшая глубина рекурсии для сортировки слияением'''
    return log2(size)+100


def merge_sort_impl(d):
    '''Сортировка слиянием'''
    def merge( d, ls, m, rs): 
        '''Merge d[ls1, m) and d[m, rs ]'''
        k = ls
        i = ls 
        j = m
        s = []
        while ( k <= rs):
            if i>=m:
                s.append(d[j])
                j+=1
            elif j>rs:
                s.append(d[i])
                i+=1
            elif d[i] < d[j]:
                s.append(d[i])
                i += 1
            else:
                s.append(d[j])
                j+=1
            k +=1

        for k in range(ls, rs+1):
            d[k] = s.pop(0)


    def sort(d, ls, rs):
        ''' Recursive sort d[ls,rs]'''
        if rs-ls > 1:
            m = ( ls + rs ) // 2
            sort(d, ls, m)
            sort(d, m+1, rs)
            merge(d, ls, m+1, rs)
        elif rs-ls == 1:
            if d[rs]<d[ls]:
                swap(d,ls,rs) 

    limit = sys.getrecursionlimit()
    sys.setrecursionlimit(max(limit, ms_recurion_limit(len(d))))

    sort(d, 0, len(d)-1)

    sys.setrecursionlimit(limit)

    return d


def merge_sort(size_, d):
    reqursion_func_safe_runner(merge_sort_impl,  ms_recurion_limit(len(d)), d)
    return d
