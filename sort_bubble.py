################################################################################
#                                 BUBBLE SORTS                                 #
################################################################################

def swap(d, i, j):
    '''swaps d[j] and d[i]'''
    # for list or array we can do: d[i], d[j] = d[j], d[i]
    # but a little bit more efficient
    x = d[i]
    d[i] = d[j]
    d[j] = x


def bubble_sort(size_, d):
    '''Сорт. пузырьком'''
    size = len(d)
    for i in range(0, size):
        for j in range(0, size-i-1):
            if d[j] > d[j+1]:
                swap(d, j, j+1)
    return d


def bubble_sort_opt(size_, d):
    '''Сорт. пузырьком с опт. '''
    size = len(d)
    swapped = True if size > 0 else False
    i = 0
    while swapped == True:
        swapped = False
        i += 1  
        for j in range(0, size-i):
            if d[j] > d[j+1]:
                swap(d, j, j+1)
                swapped = True
    return d
