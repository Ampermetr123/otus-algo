##
# Добавляем в фильтр Блума все простые числа в диапазоне от 1 до N (M чисел)
# Далее для диапазона [1,N] вычисляем количество ложно-положительных результатов
# (когда не простое число проходит фильтр) - К чисел.
# Рассчитыаем полученную вероятность ошибки P = K/(N-M)

from bloom import BloomFilter
import math
from hashlib import blake2b
from bisect import bisect_left
from typing import List, Any


def get_primes(n):
    """Решето Эратосфена" O(N).
    Возвращает сортированный список простых чисел в диапазоне [2,n]
    """
    lp = [0] * (n + 1)
    primes = []
    for i in range(2, n + 1, 1):
        if lp[i] == 0:
            primes.append(i)
            lp[i] = i
        for p in primes:
            p_x_i = p * i
            if p > lp[i] or p_x_i > n:
                break
            lp[p_x_i] = p
    return primes


def make_hash_func(seed: int, m: int):
    """Создает хэшфункцию
    seed - соль
    m - максимальное плнируемое значение хэш функции
    """
    digits = int(math.log2(m) / 8 + 1)
    h = blake2b(digest_size=digits, salt=str(seed).encode())

    def func(x):
        h.update(str(x + seed).encode())
        return int(h.hexdigest(), base=16) % m

    return func


def bin_search(a: List[Any], x: int):
    """Бинарный поиск в сортированном списке"""
    i = bisect_left(a, x)
    if i != len(a) and a[i] == x:
        return i
    else:
        return -1


if __name__ == "__main__":

    # Входные параметры - диапазон и вероятность FP
    total = 1000000
    p = 0.03

    # Находим простые числа и их количество N
    print("Calculating primes...")
    primes = get_primes(total)
    n = len(primes)
    # Расчет параметров M и k для фильтра Блума
    M = int(-n / math.log(2) * math.log2(p))
    k = round(math.log(1 / p))

    print(
        f"""
    range = {total}
    n (primes) = {n}
    FalsePositive p = {p}
    k = {k}
    M = {M} bit ({M//8} bytes)"""
    )

    # Создаем k хещ-функций и фильтр Блума
    hash_funcs = [make_hash_func(x * 33, M) for x in range(k)]
    bf = BloomFilter(M, hash_funcs)

    # Добавляем в фильтр Блума все простые числа в диапазоне от 1 до N (M чисел)
    for x in primes:
        bf.insert(x)

    # Проверяем
    print("\nChecking...")
    fp = 0
    fn = 0
    for x in range(total):
        res = bf.contains(x)
        real = bin_search(primes, x) >= 0
        if real != res and real is False:
            fp += 1
        if real != res and real is True:
            fn += 1  # не должны попадать сюда!

    print(f"False Positive count {fp}  - {fp/total*100}% ")
