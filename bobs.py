# Входные данные: строка в виде a/b+c/d.
# вместо букв записаны числа, каждое число от 1 до 10000.
# Вывод данные: строка в виде x/y , представляющих собой несократимую дробь x/y = a/b+c/d
# Максимальное значение дроби 1/1.

import sys
import re


def load_data(inp_str):
    abcd = re.split('\+|\/', inp_str)
    a = int(abcd[0])
    b = int(abcd[1])
    c = int(abcd[2])
    d = int(abcd[3])
    return a, b, c, d


def find_NOD(a, b):
    def even(x): return x & 1 == 0  # четное
    def odd(x): return x & 1 == 1  # нечетное

    if a == 0:
        return b
    if b == 0:
        return a
    if a == b:
        return a

    if even(a) and even(b):
        return 2*find_NOD(a//2, b//2)
    if odd(a) and odd(b):
        if a > b:
            return find_NOD((a-b)//2, b)
        else:
            return find_NOD(a, (b-a)//2)
    if even(a):  # b is odd
        return find_NOD(a//2, b)
    if even(b):  # a is odd
        return find_NOD(a, b//2)



if __name__ == '__main__':

    # sys.argv.append('2/100+3/100')
    # if len(sys.argv) < 2:
    #     print(f'Usage {sys.argv[0]} a/b+c/d')
    #     exit(1)
    a, b, c, d = load_data(input())
    x = a*d+c*b
    y = b*d
    nod = find_NOD(x, y)
    print(f'{x//nod}/{y//nod}')
