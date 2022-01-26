"""
Алгоритмы поска минимального остовного дерева (minimum spanning tree, MST)

Алгоритм Краскала -
"Аdd a shortest edge that joins two different fragments"

Алгоритм Борувки -
"For every fragment, add the shortest edge that joins it to another fragment"

Алгортим Прима -
"Add a shortest edge that joins the fragment containing
the root to another fragment"
"""

from collections import namedtuple
from typing import List, Set, Tuple, Union

from .heap_queue import HeapQueue

Edge = Tuple[int, int]  # ребро
AdjencyMatrix = List[
    List[Union[int, None]]
]  # граф, представленный матрицей смежности


##############################################################################
#                                  UNION FIND                                #
##############################################################################


class UnionFind:
    """
    Структура для объединения элементов в множества и проверки
    принадлежности элементов одному множеству
    """

    def __init__(self, size):
        # индекс - идент. элемента; значение - родитель элемента
        self.parents = [x for x in range(size)]
        self.rank = [1] * size
        self.roots = set(self.parents)

    def union(self, u: int, v: int) -> None:
        """Объединенение множеств, в которых находятся элементы u и v"""
        root_u = self.find(u)
        root_v = self.find(v)
        if root_u != root_v:
            if self.rank[root_v] > self.rank[root_u]:
                self.parents[root_u] = root_v
                self.rank[root_v] += self.rank[root_u]
                self.roots.discard(root_u)
            else:
                self.parents[root_v] = root_u
                self.rank[root_u] += self.rank[root_v]
                self.roots.discard(root_v)

    def find(self, u: int) -> int:
        """Возвращает главного в множетсве, в котором элемент u"""
        if u == self.parents[u]:
            return u
        self.parents[u] = self.find(self.parents[u])
        return self.parents[u]

    def components(self) -> Set[int]:
        """Возврщает список главных элементов множеств"""
        return self.roots


##############################################################################
#                              АЛГОРИТМ КРУСКАЛА                             #
##############################################################################


def get_sorted_edges(gm: AdjencyMatrix) -> List[Edge]:
    """
    Из неориентированного графа, представленного матрицей смежности,
    получаем список ребер, отсортированных в порядке возрастания весов
    """
    edges: List[Edge] = []
    sz = len(gm)

    # Проход по верхней диагонали матрицы графа
    for a in range(sz):
        for b in range(a, sz):
            w = gm[a][b]
            if w is None:
                continue
            # Сортировка вставкой используется для учебных целей.
            # можно использовать встроенную функцию сортировки списка или heapq
            i = 0
            while i < len(edges) and gm[edges[i][0]][edges[i][1]] < w:  # type: ignore
                i += 1
            edges.insert(i, (a, b))
    return edges


def dsf_path(gm: AdjencyMatrix, a: int, b: int) -> bool:
    """Проверка наличия пути из a в b в графе g"""
    stack = [a]
    visited = []
    sz = len(gm)
    while len(stack):
        s = stack.pop()  # текущая вершина
        if s in visited:
            continue
        visited.append(s)
        if gm[s][b] is not None:
            return True  # есть путь из i в j
        adj_nodes = (j for j in range(sz) if gm[s][j] is not None)
        stack.extend(adj_nodes)
    return False


def kruskal_dsf(gm: AdjencyMatrix) -> List[Edge]:
    """Поиск минимального оставного дерева алгоритмом Крускала
    c использщованием DSF"""
    sz = len(gm[0])
    ostov: AdjencyMatrix = [
        [None for i_ in range(sz)] for j_ in range(sz)
    ]  # результирующий граф - оставное дерево
    ostov_edges: List[Edge] = []  # список ребер реузльтирующего графа
    edges = get_sorted_edges(gm)
    for i, j in edges:
        # проверка, что в дереве нет пути из a в b
        if dsf_path(ostov, i, j):
            continue
        else:
            # добавляем ребро в дерево
            ostov[i][j] = ostov[j][i] = gm[i][j]
            ostov_edges.append((i, j))
    return ostov_edges


def kruskal_uf(gm: AdjencyMatrix) -> List[Edge]:
    """Поиск минимального оставного дерева алгоритмом Крускала
    с использщованием UnionFind"""
    sz = len(gm[0])
    uf = UnionFind(sz)
    ostov_edges: List[Edge] = []  # список ребер реузльтирующего дерева
    edges = get_sorted_edges(gm)
    for i, j in edges:
        # проверка, что i и j не в одном множестве нешего дерева
        if uf.find(i) != uf.find(j):
            ostov_edges.append((i, j))
            uf.union(i, j)
    return ostov_edges


##############################################################################
#                               АЛГОРИТМ БОРУВКИ                             #
##############################################################################


def find_min_edge_out(
    gm: AdjencyMatrix, uf: UnionFind, r: int
) -> Tuple[int, int]:
    stack = [r]
    visited = []
    sz = len(gm)
    # min_edge: Tuple[int, int] = None
    min_weight = None
    while len(stack):
        s = stack.pop()  # текущая вершина
        if s in visited:
            continue
        visited.append(s)
        for j in range(sz):  # проверяем все рёбра вершины s
            if gm[s][j] is None:
                continue  # нет ребра
            if uf.find(j) == r:
                stack.append(j)  # своя вершина, будем обходить
            else:
                # чужая вершина
                if min_weight is None or min_weight > gm[s][j]:
                    min_weight = gm[s][j]
                    min_edge: Tuple[int, int] = (min(j, s), max(j, s))

    return min_edge


def boruvka(gm: AdjencyMatrix) -> List[Edge]:
    sz = len(gm[0])
    uf = UnionFind(sz)
    ostov_edges: List[Edge] = []  # список ребер реузльтирующего графа

    while len(ostov_edges) < sz - 1:
        # найти мин ребро наружу для каждой компоненты
        edges = []
        for r in uf.components():
            edge = find_min_edge_out(gm, uf, r)
            edges.append(edge)

        # все компоненты связываем найденными ребрами
        for e in edges:
            if uf.find(e[0]) != uf.find(e[1]):
                uf.union(e[0], e[1])
                ostov_edges.append(e)

    return ostov_edges


##############################################################################
#                                АЛГОРИТМ ПРИМА                              #
##############################################################################


def prim(gm: AdjencyMatrix) -> List[Edge]:
    sz = len(gm)
    stack = [0]  # начинаем с перовй вершины
    EdgeTuple = namedtuple("EdgeTuple", ["w", "v1", "v2"])
    visited: List[int] = []
    edges = HeapQueue()
    ostov_edges: List[Edge] = []  # список ребер реузльтирующего графа
    while len(stack):
        s = stack.pop()  # текущая вершина
        if s in visited:
            continue
        visited.append(s)
        # добавляем в список ребер для выбора новые ребера
        for j in range(sz):
            if gm[s][j] is None:
                continue
            if j in visited:
                continue  # не добавляем ребра к тем вершинам, которые уже есть в дереве
            else:
                edges.push(EdgeTuple(gm[s][j], s, j))

        # выбираем минимальное ребро
        while len(edges) > 0:
            e = edges.pop()
            if e.v2 in visited:
                continue  # если ребро образует цикл, то пропускаем его
            ostov_edges.append((min(e.v1, e.v2), max(e.v1, e.v2)))
            stack.append(e.v2)
            break

    return ostov_edges
