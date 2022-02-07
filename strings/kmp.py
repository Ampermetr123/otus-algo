from typing import List


def make_pi_function(pat: str) -> List[int]:
    psz = len(pat)
    pf = [0] * psz
    # index i+1  = размер подстроки
    # значение - максимальный размер длины в подстроке,
    # такой что префикс = суфиксу подстроки

    for i in range(1, psz):  # начинаем с подстроки длинной 2
        j = pf[i - 1]  # длина макс(суфикс=префикс) для пред. длины

        # проход по суффиксным ссылкам вверх до совпадения
        while j > 0 and pat[i] != pat[j]:
            j = pf[j - 1]

        if pat[i] == pat[j]:
            j += 1  # т.к был найден суфикс, равный префиксу

        pf[i] = j

    return pf


def find_string_KMP(patern: str, text: str) -> int:
    """Алгоритм Кнута-Морриса-Пратта"""
    psz = len(patern)
    tsz = len(text)
    if tsz < psz or psz == 0:
        return -1

    pf = make_pi_function(patern)
    s = 0
    for i in range(tsz):
        while text[i] != patern[s] and s > 0:
            s = pf[s - 1]  # откат на пред. состояние
        if patern[s] == text[i]:
            s += 1  # совпадение, переход на след. состоние.
        if s == psz:
            return i - psz + 1  # дошли до конца
    return -1


if __name__ == "__main__":

    # Проверка make_suffix_table(pat) для Боера-Мура

    pat = "BC.ABC.BC.C.ABC"
    print("pat =", pat)
    table = make_pi_function(pat)
    print(table)

    i = find_string_KMP(pat, "AAABC.ABC.BC.C.ABCBBBB")
    print(i)

    pat = "ABABABBBABABAB"
    print("pat =", pat)
    table = make_pi_function(pat)
    print(table)
