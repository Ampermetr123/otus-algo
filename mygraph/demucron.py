from typing import FrozenSet, List

# Для алгоритма Демукрона граф задается матрицей смежности


def demucron_topsort(g: List[List[int]]) -> List[FrozenSet[int]]:
    """
    Топологическая сортировка графа. Алгоритм Демукрона.
    """
    topology = []

    sz = len(g[0])

    # Складываем строки
    acc = [0] * sz
    for i in range(sz):
        for j in range(sz):
            acc[j] += g[i][j]

    nodes_found = 0

    while nodes_found < sz:
        zero_indexes = [i for i in range(sz) if acc[i] == 0]
        if len(zero_indexes) == 0:
            return (
                []
            )  # в графе имеется цикл, топологическая сортировка невозможна

        # сохраняем вершины из очередного топологического уровня
        topology.append(frozenset(zero_indexes))

        # запоминаем сколько вершин было обработано
        nodes_found += len(zero_indexes)

        for i in zero_indexes:
            acc[i] = -1  # помечаем как найденные

        # вычитаем строки индексов найденных вершин (эффект удаление вершин из графа)
        for i in zero_indexes:
            for j in range(sz):
                acc[j] -= g[i][j]

    return topology
