#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import math
from folder_test import run_test, CompareAs, int_args

@int_args
def fibo_a(n):
    """Рекусрсивный метод - по опредлению. O(2^N) """
    if n == 0: 
      return 0
    elif n <= 2:
        return 1
    return fibo_a(n - 1) + fibo_a(n - 2)

@int_args
def fibo_b(n):
    """По формуле золотого сечения"""
    SQRT5 = math.sqrt(5)
    FI = ( 1 +  SQRT5) / 2
    return math.floor( math.pow(FI, n) / SQRT5 + 0.5 )


class Mx2:
    '''Матрица из двух эелементов c операцией умножения'''
    def __init__(self, a1, a2, b1, b2):
        self.a1 , self.a2 = a1, a2
        self.b1, self.b2 = b1, b2
    
    def __str__(self):
        print(self.a1, self.a2)
        print(self.b1, self.b2)
    
    def __mul__(self, other):
        a1 = self.a1 * other.a1 + self.a2 * other.b1
        a2 = self.a1 * other.a2 + self.a2 * other.b2
        b1 = self.b1 * other.a1 + self.b2 * other.b1
        b2 = self.b1 * other.a2 + self.b2 * other.b2
        return Mx2(a1, a2, b1, b2)
    

@int_args
def fibo_c(n):
    """Возведение матрицы в степень"""
    if n == 0:
        return 0
    elif n <= 2:
        return 1
    
    x = Mx2( 1, 1, 
             1, 0 )
    p = Mx2( 1, 0, 
             0, 1 )
    n = n -1   # f3 = (x^2).a1  -> Чтобы найти число фибоначи N, следует возвести в степень N-1
    while n > 0:
        n , odd = divmod(n, 2)
        if odd == 1:
            p = p * x
        x*=x
    return p.a1


if __name__ == "__main__":
    sys.setrecursionlimit(10000000 + 20)
    run_test("test/4.Fibo", fibo_a, compare_strategy = CompareAs.INT, max_tests = 7)
    run_test("test/4.Fibo", fibo_b, compare_strategy = CompareAs.INT, max_tests = 7)
    run_test("test/4.Fibo", fibo_c, compare_strategy = CompareAs.INT, max_tests = 11)