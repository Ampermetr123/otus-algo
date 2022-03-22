# Фильтр Блума

from typing import Callable
from typing import List, Any
from bitarray import bitarray # pip install bitarray

class BloomFilter:
    def __init__(self, m: int, hash_fun: List[Callable[[Any], int]]):
        """
        m, size of the vector
        hash_fun, list of hash function to use; returm value of hash function must be in [0,m)
        """
        self.m = m
        self.vector = bitarray(m)
        self.vector.setall(0)
        self.k = len(hash_fun)
        self.hash_funcs = hash_fun

    def insert(self, key: Any):
        """ Вставка значения в фильтр """
        for i in range(self.k):
            self.vector[self.hash_funcs[i](key)] = 1

    def contains(self, key: Any):
        """ Проверка наличия ключа в фильтре"""
        for i in range(self.k):
            if self.vector[self.hash_funcs[i](key)] == 0:
                return False # the key doesn't exist
        return True # the key can be in the data set
