#!/usr/bin/env python
# -*- coding: utf-8 -*-

from folder_test import run_test, CompareAs

def pow_a(a, n):
    """Возведение в степень. 
       Итеративный (n умножений)
       O(N)
    """
    a = float(a)  
    n = int(n)
    p = 1.0
    for i in range(n):
        p = p * a
    return p


def pow_b(a, n):
    """ Возведение в степень. 
        Через степень 2 с домножением
        O(N/2 + LogN) = O(N)
    """
    a = float(a)  
    n = int(n)
    if n == 0:
      return 1
    # По степени два
    p = a
    s = 1
    while s*2 < n:
      p*=p
      s*=2
    # домножение
    for i in range(s, n):
        p = p * a
    return p


def pow_c(a, n):
    ''' Aлгоритм возведения в степень 
        через двоичное разложение показателя степени O(2LogN) = O(LogN).'''
    a = float(a)
    n = int(n)
    x = a
    p = 1
    while n > 0:
        n , odd = divmod(n, 2)
        if odd == 1:
            p = p * x
        x*=x
    return p


if __name__ == "__main__":
    run_test("test/3.Power", pow_a, CompareAs.FLOAT, max_tests = 8)
    run_test("test/3.Power", pow_b, CompareAs.FLOAT, max_tests = 8)
    run_test("test/3.Power", pow_c, CompareAs.FLOAT, max_tests = 0)
