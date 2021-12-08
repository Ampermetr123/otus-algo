################################################################################
#                        SELECTION AND HEAP SORTS                              #
################################################################################

from array import array


def swap(d, i, j):
    '''swaps d[j] and d[i]'''
    x = d[i]
    d[i] = d[j]
    d[j] = x

   

def select_sort(size_, d):
    '''Selection sort'''
    if (size := len(d)) == 0:
        return d

    for i in range(size-1, 0, -1):  # from size-1 to 1

        # look for index of max value
        max_idx = i
        for j in range(0, i):
            if d[j] > d[max_idx]:
                max_idx = j

        swap(d, i, max_idx)

    return d


def heapify(d, root, size):
    L = 2*root+1
    R = 2*root+2
    X = root

    if L < size and d[L] > d[X]:
        X = L
    if R < size and d[R] > d[X]:
        X = R
    
    if (X == root):
        return
    swap(d, X, root)
    heapify(d, X, size)


def heap_sort(size_, d):
    '''Пиромидальная сорт.'''
    if (size := len(d)) == 0:
        return d

    # превращаем бинарное дерево в кучу
    for i in range(size // 2 -1, -1, -1):  # i =  size/2 - 1, ... , 0
        heapify(d, i, size)
    # now d[0] is max

    for j in range(size-1, 0, -1):  # j =  size - 1, ... , 1
        swap(d, 0 , j )
        heapify(d, 0, j)

    return d
