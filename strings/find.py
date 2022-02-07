from typing import List

# 1) Написать алгоритм поиска подстроки полным перебором, 2 байта.


# Поиск в строке text подстроки patern
# Возвращает индекс начала подстрки или -1, если подстрока отсутсвует
def find_string_brute(patern: str, text: str) -> int:
    "Поиск полным перебором"
    t = 0  # index of text
    p = 0  # index of patern
    len_t, len_p = len(text), len(patern)
    while t < len_t - len_p:
        p = 0
        while p < len_p and text[t + p] == patern[p]:
            p += 1
        if p == len_p:
            return t
        t += 1
    return -1


# 2) Оптимизировать алгоритм, используя сдвиги по префиксу шаблона, 2 байта.
# Поиск в строке text подстроки patern
# Возвращает индекс начала подстрки или -1, если подстрока отсутсвует
def find_string_shift(patern: str, text: str) -> int:
    """Смещение на 1 или длину шаблона"""

    def make_shift_table(pat: str) -> List[int]:
        # AABCDEAx......
        #    ABCDE
        #        ->ABCDE
        psz = len(pat)
        table = [psz] * 128

        for i in range(psz - 1):
            table[ord(pat[i])] = 1
        return table

    len_t, len_p = len(text), len(patern)
    if len_p > len_t or len_p == 0:
        return -1
    shift = make_shift_table(patern)
    t = len_p  # index of text
    while t <= len_t:
        # xxxxSTRINGxxxxx
        #          ^ ->
        #        (t-1)
        #    PATTERN
        #       <- ^
        #          p=-1
        p = 1  # index of patern
        while p <= len_p and text[t - p] == patern[-p]:
            p += 1
        if p > len_p:
            return t - len_p
        t += shift[ord(text[t - 1])]
    return -1


# 3) Оптимизировать алгоритм, используя сдвиги по суффиксу текста, 2 байта.

# Поиск в строке text подстроки patern
# Возвращает индекс начала подстрки или -1, если подстрока отсутсвует
def find_string_BMH(patern: str, text: str) -> int:
    """Алгоритм Бойера — Мура — Хорспула"""

    def make_shift_table(pat: str) -> List[int]:
        psz = len(pat)
        table = [psz] * 128
        # ABCDAE
        # 14321-
        for i in range(psz - 1):
            table[ord(pat[i])] = psz - 1 - i
        return table

    len_t, len_p = len(text), len(patern)
    if len_p > len_t or len_p == 0:
        return -1
    shift = make_shift_table(patern)
    t = len_p  # index of text
    while t <= len_t:
        # xxxxSTRINGxxxxx
        #          ^ ->
        #        (t-1)
        #    PATTERN
        #       <- ^
        #          p=-1
        p = 1  # index of patern
        while p <= len_p and text[t - p] == patern[-p]:
            p += 1
        if p > len_p:
            return t - len_p
        t += shift[ord(text[t - 1])]
    return -1


# 4) Реализовать алгоритм Бойера-Мура, 2 байта.

# Вспомогательная функция для алгоритма Боера-Мура


def make_suffix_table(pat: str) -> List[int]:
    """Строит таблицу, в которой индекс - размер суффикса,
    а значение - смещение шаблона для полного повтора этого суффикса
    или для его частичного повтора с начала строки
    """
    psz = len(pat)
    table = [0] * psz
    table[0] = psz

    #           Example
    # A B A B C A B           \\ patern (len=7)
    #   <-^
    #     i = 3  (AB)       \\  i = 1, 2, 3, 4, 5, 6
    #        <- ^
    #           j = len-1-1 = 4     \\  j = 5, 4, 3, 2, 1, 0
    for i in range(1, psz):
        # Строка делится на две части индексом  i; i - размер суффикса.
        # ищем совпадение суфикса в левой части шаблна;
        for j in range(psz - 1 - 1, -1, -1):
            k = 1  # index on suffix
            # сравниваем справа налево суфикс и строку
            while pat[j + 1 - k] == pat[-k] and k <= i and k <= j + 1:
                k += 1
            if k > i or k > j + 1:  # полное или частичное совпадение суффикса
                table[i] = psz - j - 1
                break
        if table[i] == 0:  # cуффикс не был найден
            table[i] = table[i - 1]
    return table


# Поиск в строке text подстроки patern
# Возвращает индекс начала подстрки или -1, если подстрока отсутсвует
def find_string_BM(patern: str, text: str) -> int:
    """Алгоритм Бойера — Мура"""

    def make_shift_table(pat: str) -> List[int]:
        psz = len(pat)
        table = [psz] * 128
        # ABCDAE
        # 14321-
        for i in range(psz - 1):
            table[ord(pat[i])] = psz - 1 - i
        return table

    len_t, len_p = len(text), len(patern)
    if len_p > len_t or len_p == 0:
        return -1
    shift_bad = make_shift_table(patern)  # таблица смещений, если суффикс не совпал
    shift_good = make_suffix_table(patern)  # таблица смещений, если есть суффикс
    t = len_p  # index of text
    while t <= len_t:
        # xxxxSTRINGxxxxx
        #          ^ ->
        #        (t-1)
        #    PATTERN
        #       <- ^
        #          p=-1
        p = 1  # index of patern
        while p <= len_p and text[t - p] == patern[-p]:
            p += 1
        if p > len_p:
            return t - len_p
        if p == 1:
            t += shift_bad[ord(text[t - 1])]
        else:
            t += shift_good[p - 1]
    return -1


if __name__ == "__main__":

    # Демонтсрация и проверка функции make_suffix_table(pat) для Боера-Мура
    pat = "BC.ABC.BC.C.ABC"  # из лекции
    print("pat =", pat)
    table = make_suffix_table(pat)
    assert table == [15, 4, 6, 9, 9, 9, 13, 13, 13, 13, 13, 13, 13, 13, 13]
    print(table)

    #             *
    # =....xKOL......
    #       KOLOKOL
    #
    # 1 L 4
    # 2 OL 4
    # 3 KOL 4
    # 3 OKOL 4
    # 4 LOKOL 4
    # 5 OLOKOL 4
    # -
    pat = "KOLOKOL"
    print("pat =", pat)
    table = make_suffix_table(pat)
    print(table)
    assert table == [7, 4, 4, 4, 4, 4, 4]

    #                        *       *
    # =.............xAB......
    #       ABABABBBABABAB
    #           ABABABBBABABAB

    # 1 B 2
    # 2 AB 2
    # 3 BAB 2
    # 4 ABAB 2
    # 5 BABAB 2
    # 6 ABABAB 8
    # ...
    # 9 BBBABАBAB 8
    pat = "ABABABBBABABAB"
    print("pat =", pat)
    table = make_suffix_table(pat)
    print(table)

    # "Человеческая" строка
    pat = "They're putting"
    print("pat =", pat)
    table = make_suffix_table(pat)
    print(table)

    # "Человеческая" строка
    pat = "We say that Napoleon wished to invade Russia and invaded it."
    print("pat =", pat)
    table = make_suffix_table(pat)
    print(table)
