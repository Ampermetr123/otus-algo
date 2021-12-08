################################################################################
#                                 INSERT SORTS                                 #
################################################################################

from array import array


def swap(d, i, j):
    '''swaps d[j] and d[i]'''
    x = d[i]
    d[i] = d[j]
    d[j] = x


def insert_sort_swap(size, d):
    '''Сорт. вставкой с перемещением'''
    size = len(d)
    sz = 0  # граница отсортированной части
    while sz < size:
        x = d[sz]

        # перемещение перестановкой
        j = sz
        while j > 0 and d[j] < d[j-1]:
            swap(d, j, j-1)
            j -= 1

        sz += 1

    return d


def insert_sort_shift(size, d):
    '''Сорт. вставкой со сдвигом'''
    size = len(d)
    sz = 0  # граница отсортированной части
    while sz < size:
        x = d[sz]

        # сдвиг вправо до точки вставки
        j = sz-1
        while j >= 0 and x < d[j]:
            d[j+1] = d[j]
            j -= 1

        d[j+1] = x  # вставка
        sz += 1

    return d


def insert_sort_bs(size, d):
    '''Сорт. вставкой с бин. поиском'''

    def bin_search(arr, val, p1, p2):
        '''find insert pos in array in range [p1,p2]'''
        if val >= arr[p2]:
            return p2+1
        if val <= arr[p1]:
            return p1
        if p1+1 == p2:
            return p2
        mid = (p1 + p2) // 2
        if val >= arr[mid]:
            return bin_search(arr, val, mid, p2)
        else:
            return bin_search(arr, val, p1, mid)

    sd = array(d.typecode, (d[0],))  # отсортированный массив помещаем сюда
    size = len(d)
    for i in range(1, size):
        j = bin_search(sd, d[i], 0, len(sd)-1)
        sd.insert(j, d[i])
    return sd
