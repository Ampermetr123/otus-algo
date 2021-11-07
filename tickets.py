#!/usr/bin/env python
# -*- coding: utf-8 -*-

from folder_test import run_test

def tickets(n) -> str:
    n = int(n)
    if n <= 0:
        return 0
    
    # Количество комбинаций цифр, сумма которых равно индеку списка 
    sum_current = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    
    for i in range(2, n + 1):  # 2, 3, ... n
        vlen = len(sum_current)
        sum_prev = sum_current.copy()
        beg = 0
        end = 0
        for d in range(0, 9 * i + 1):
            if d >=10:
                beg += 1
            end += 1
            if d >= vlen:
                sum_current.append(sum(sum_prev[beg:end]))
            else:
                sum_current[d] = sum(sum_prev[beg:end])
    total = sum([x*x for x in sum_current])
    return str(total)


if __name__ == "__main__":
    run_test("1.Tickets", tickets)