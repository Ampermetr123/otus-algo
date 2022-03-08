# Условие задачи
# Фермер хочет построить на своей земле как можно больший по площади сарай.
# Но на его участке есть деревья и хозяйственные постройки, которые он не хочет никуда переносить.
# Для удобства представим ферму сеткой размера N × M.
# Каждое из деревьев и построек размещается в одном или нескольких узлах сетки.
# Найти максимально возможную площадь сарая и где он может размещаться.
# Начальные данные: Вводится матрица размера N × M из 0 и 1.
# 1 соответствует постройке, 0 - пустой клетке.
# Начальные данные:
# На первой строке вводится размер матрицы N M (через пробел) от 1 до 1000.
# На второй строке вводится количество построек T (от 0 до 10000).
# Далее на T строчках вводится координаты построек по два числа X Y, где 0 <= X < N; 0 <= Y < M.
# Вывод результата:
# Число, соответствующее максимальной площади сарая (количество ячеек).
# Решение задачи разделено на 4 этапа:

from random import random
from mytimer import Timer


class World:

    def load_world(self):
        """Загрузка карты из stdin"""
        n, m = input().split()
        w = []
        for _ in range(m):
            str = input()
            w.append(list(map(int, str.split())))
        self.m = m
        self.n = n
        self.w = w
        self.hline = []
        self.r = [[] for _ in range(n)]
        self.l = [[] for _ in range(n)]

    # f - [0;1)
    def generate(self, m, n, f):
        """Генрация карты"""
        w = []
        for _ in range(m):
            row = [1 if random() < f else 0 for i in range(n)]
            w.append(row)
        self.m = m
        self.n = n
        self.w = w
        self.hline = []
        self.r = [[] for _ in range(n)]
        self.l = [[] for _ in range(n)]

    def max_square_at(self, i, j):
        """ Максимальная площадь для точки (i;j) - левый нижний угол сарая
            Расчет методом перебора
        """
        max_square = 0
        min_height = i + 1
        for jj in range(j, self.n):
            if self.w[i][jj] == 1:
                break
            height = 0
            width = jj-j+1
            ii = i
            while ii >= 0 and height < min_height and self.w[ii][jj] == 0:
                ii -= 1
                height += 1
            min_height = height
            s = height * width
            if s > max_square:
                max_square = s
        return max_square

    def max_burn_small(self):
        """ Точка и площадь максимально возможного сарая.
            Решение задачи перебором O(N^4)"""
        smax = 0
        pos = (0, 0)
        for i in range(self.m):
            for j in range(self.n):
                s = self.max_square_at(i, j)
                if s > smax:
                    smax = s
                    pos = (i, j)
        return smax, pos

    def max_burn_big(self):
        """ Точка и площадь максимально возможного сарая.
            Решение задачи O(N^2)"""
        smax = 0
        pos = (0, 0)
        for i in range(self.m):
            for j in range(self.n):
                s = self.width_at(i, j) * self.height_at(i, j)
                if s > smax:
                    smax = s
                    pos = (i, j)
        return smax, pos

    def get_height_line(self, i):
        """ Возвращает список длины N с максимльно возомжными высотами для строки i"""
        """ Результаты кешируются"""
        hlen = len(self.hline)
        if hlen > i:
            return self.hline[i]
        if hlen == 0:
            first_line = [0 if self.w[0][j] == 1 else 1 for j in range(self.n)]
            self.hline.append(first_line)
            hlen += 1

        for k in range(hlen, i+1):
            line = [0 if self.w[k][j] == 1 else (
                self.hline[k-1][j] + 1) for j in range(self.n)]
            self.hline.append(line)
        return self.hline[i]

    def height_at(self, i, j):
        """Длина сарая. Вычисление максимальной длины сарая для каждой клетки"""
        return self.get_height_line(i)[j]

    def right_width_bounds(self, i):
        """ Вычисление вектора правых границ ширины для строки i"""
        if len(self.r[i]) > 0:
            return self.r[i]
        stack = []
        right_bound = [0]*self.n
        hline = self.get_height_line(i)
        for j in range(self.n):
            h = hline[j]  # высота добавляемого элемента
            while len(stack) and hline[stack[-1]] > h:
                right_bound[stack.pop()] = j - 1
            stack.append(j)
        while len(stack):
            right_bound[stack.pop()] = self.n-1
        self.r[i] = right_bound
        return right_bound

    def left_width_bounds(self, i):
        """ Вычисление вектора правых границ ширины для строки i"""
        if len(self.l[i]) > 0:
            return self.l[i]
        stack = []
        left_bound = [0]*self.n
        hline = self.get_height_line(i)
        for j in range(self.n-1, -1, -1):
            h = hline[j]  # высота добавляемого элемента
            while len(stack) and hline[stack[-1]] > h:
                left_bound[stack.pop()] = j + 1
            stack.append(j)
        while len(stack):
            left_bound[stack.pop()] = 0
        self.l[i] = left_bound
        return left_bound

    def width_at(self, i, j):
        return 0 if self.get_height_line(i)[j] == 0 else \
            self.right_width_bounds(i)[j] - self.left_width_bounds(i)[j] + 1

    def print(self):
        for i in range(self.m):
            print(self.w[i])


def console_test():
    ''' Проверка функций на небольшом сгенерированном сарае, с выводом на экран'''
    w = World()
    w.generate(6, 6, 0.1)
    w.print()
    s, p = w.max_burn_small()
    print(f'Max square is {s} at ({p[0]},{p[1]})')
    print(f'H for i=5 is {w.get_height_line(5)}')
    print(f'R for i=5 is {w.right_width_bounds(5)}')
    print(f'L for i=5 is {w.left_width_bounds(5)}')
    W = [w.width_at(5, j) for j in range(6)]
    print(f'W for i=5 is {W}')
    s, p = w.max_burn_big()
    print(f'Max square is {s} at ({p[0]},{p[1]})')


def time_test():
    '''Проверка времени и сложности алгоритмов поиска'''
    w = World()
    n = 100
    for i in range(5):
        n = n*2
        w.generate(n, n, 0.15)
        print(f'-------N={n}----')
        print(f"Small burn ")
        with Timer():
            s, p = w.max_burn_small()
        print(f'Max square is {s} at ({p[0]},{p[1]})')
        print(f"Big burn")
        with Timer():
            s, p = w.max_burn_big()
        print(f'Max square is {s} at ({p[0]},{p[1]})')


if __name__ == '__main__':
    console_test()
    time_test()
