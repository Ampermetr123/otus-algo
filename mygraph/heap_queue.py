"""
    Очердь на основе кучи
    Учебный класс. Содержит только минимальный функционал,
    необходимый для реализации алгоритмов
    В продакшн разумно испльзовать модуль heapq
"""
from typing import Sized


class HeapQueue(Sized):
    def __init__(self):
        self.data = []

    def __len__(self) -> int:
        return len(self.data)

    def heapify(self, root):
        size = len(self.data)
        L = 2 * root + 1
        R = 2 * root + 2
        X = root
        if L < size and self.data[L] < self.data[X]:
            X = L
        if R < size and self.data[R] < self.data[X]:
            X = R
        if X == root:  # Корень не поменялся
            return

        # swap root and X
        t = self.data[X]
        self.data[X] = self.data[root]
        self.data[root] = t

        self.heapify(X)  # т.к в позиции X теперь новое число

    def push(self, x):
        self.data.insert(0, x)
        self.heapify(0)

    def pop(self):
        t = self.data.pop(0)
        self.heapify(0)
        return t
