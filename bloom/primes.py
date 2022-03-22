#!/usr/bin/env python

# To check memory usage
# pip install -U memory_profiler
# mprof run ./primes.py

# Depends on numpy and bitarray
# python -m pip install numpy bitarray

from math import sqrt
from bitarray import bitarray


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


def primes_d(n):
    """Решето Эратосфена" c оптимизацией по памтя и для быстрого поиска простых чисел O(N Log Log N)."""
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


def primes_e(n):
    """Решето Эратосфена" O(N)."""
    lp = [0]*(n+1)
    primes = []
    for i in range(2, n + 1 , 1):
        if lp[i] == 0:
            primes.append(i)
            yield i
            lp[i] = i
        for p in primes:
            p_x_i = p*i
            if p > lp[i] or p_x_i > n:
                break
            lp[p_x_i] = p
    return len(primes)
