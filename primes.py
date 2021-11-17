#!/usr/bin/env python

# To check memory usage
# pip install -U memory_profiler
# mprof run ./primes.py

# Depends on numpy and bitarray
# python -m pip install numpy bitarray

from folder_test import run_test, CompareAs, int_args
from math import sqrt
import numpy
from bitarray import bitarray

@int_args
def primes_a(n):
    """Метод перебор делителей O(n^2)"""
    def is_prime(n):
        for d in range(2, n):
            if n % d == 0:
                return False
        return True

    s = 0
    for i in range(2, n + 1):
        if is_prime(i):
            s += 1
    return s


@int_args
def primes_b(n):
    """Поиск простых чисел с делением только на простые числа, O(N * Sqrt(N) / Ln (N)).O(n^2)"""
    def is_prime(n, primes_storage):
        top = round(sqrt(n))
        # range = [x  for x in primes_storage if x<=top]
        for d in primes_storage:
            if d > top:
                return True
            if n % d == 0:
                return False
        return True

    if n == 2:
        return 1
    elif n < 2:
        return 0

    primes = [2]
    counter = 1
    for i in range(3, n + 1):
        if is_prime(i, primes):
            counter += 1
            primes.append(i)

    return counter


@int_args
def primes_c(n):
    """Решето Эратосфена" для быстрого поиска простых чисел O(N Log Log N)."""
    r = numpy.zeros(n + 1, dtype=numpy.uint8)
    count = 0
    sqrt_n = round(sqrt(n))
    for i in range(2, n + 1):
        if r[i] == 0:
            count += 1
            if i <= sqrt_n:
                for j in range(i * i, n + 1, i):
                    r[j] = 1
    return count


@int_args
def primes_d(n):
    """Решето Эратосфена" c оптимизацией по памтяи для быстрого поиска простых чисел O(N Log Log N)."""
    r = bitarray(n // 2 + 1)
    #bit 0 is 3, bit 1 is 5 , etc
    r.setall(0)
    if n < 2:
        return 0
    count = 1 # '2' is counted
    sqrt_n = round(sqrt(n))
    for i in range(3, n + 1 , 2):
        if r[i//2] == 0:
            count += 1
            if i <= sqrt_n:
                for j in range(i * i, n + 1, 2*i):
                    r[j // 2] = 1
    return count


@int_args
def primes_e(n):
    """Решето Эратосфена" O(N)."""
    # lp = numpy.zeros(n + 1 , dtype = numpy.uint64)  - очень медленно, из-за преобразований. 
    lp = [0]*(n+1)
    primes = []
    for i in range(2, n + 1 , 1):
        if lp[i] == 0:
            primes.append(i)
            lp[i] = i
        for p in primes:
            p_x_i = p*i
            if p > lp[i] or p_x_i > n:
                break
            lp[p_x_i] = p
    return len(primes)


if __name__ == "__main__":
    run_test("test/5.Primes", primes_a, CompareAs.INT, max_tests = 8)
    run_test("test/5.Primes", primes_b, CompareAs.INT, max_tests = 8)
    run_test("test/5.Primes", primes_c, CompareAs.INT, max_tests = 8)
    run_test("test/5.Primes", primes_d, CompareAs.INT, max_tests = 10)
    run_test("test/5.Primes", primes_e, CompareAs.INT, max_tests = 0)
    