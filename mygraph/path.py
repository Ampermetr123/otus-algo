"""
Алгоритмы поиска кратчайшего пути
"""

# from heapq import heappop, heappush
from typing import List, Optional, Tuple

from .heap_queue import HeapQueue

Edge = Tuple[int, int]  # ребро
AdjencyMatrix = List[List[int]]  # граф, представленный матрицей смежности
Vector = List[int]  # вектор вершин


def relax(gm: AdjencyMatrix, u: int, v: int, dv: Vector, pv: Vector) -> bool:
    """Релаксация пути из вершины u в вершину v
        Если пусть из u в v будет короче, чем известный из вектора dv,
        то вектора дистанции dv и пути pv будут изменены,
        и функция вернет False
        Если кратчайший путь в v не изменился, то возвращает False.
    Аргументы:
        gm (AdjencyMatrix): граф, заданный матрицей смежности
        u (int): вершина "из"
        v (int): вершина "в"
        dv (Vector): содержит длины кратчайшего пути (distance vector)
                     в вершины (индекс - вершина, значение путь)
        pv (Vector): содержит кратчайший путь (path)
                (индекс вершина, значение - предыдущая вершина для кратчайшего пути)
    """
    d = dv[u] + gm[u][v]
    if dv[v] is None or d < dv[v]:
        dv[v] = d
        pv[v] = u
        return True
    return False


def deijkstra(
    gm: AdjencyMatrix, start: int, stop: Optional[int] = None
) -> Tuple[Vector, Vector]:
    """Поиск кратчайшего пути в графе gm из вершины start
    Args:
        gm (AdjencyMatrix): граф, заданный матрицей смежности
        start (int): исходящая вершина
        stop (int, optional): прекратить поиск по достижению вершины stop
    Returns:
        Tuple[Vector, Vector]: Вектор дистанции / вектор пути.
        Индекс вектор - вершина, значение - дистанции до вершины / пред. вершина
    """
    sz = len(gm)
    assert start < sz
    dist = [None] * sz
    path = [None] * sz
    path[start] = start  # type: ignore
    dist[start] = 0  # type: ignore
    visited = []

    queue = HeapQueue()  # alternative: queue = [(0, start)]
    queue.push((0, start))
    while len(queue):

        _, u = queue.pop()  # alternative: _, u = heappop(queue)
        if stop and u == stop:
            return dist, path  # type: ignore
        if u in visited:
            continue
        visited.append(u)

        # цикл по смежным вершинам
        for v in [v for v in range(sz) if gm[u][v]]:
            relax(gm, u, v, dist, path)  # type: ignore
            # alternative: heappush(queue, (dist[v], v))  # type: ignore
            queue.push((dist[v], v))  # type: ignore
    return dist, path  # type: ignore


def deijkstra_path(gm: AdjencyMatrix, start: int, stop: int) -> List[Edge]:
    """Поиск кратчайшего пути из start в stop
    Возвращает списко ребер, найденного пути.
    """
    edges: List[Edge] = []
    d, p = deijkstra(gm, start, stop)
    u = stop
    while p[u] != u:
        edges.insert(0, (p[u], u))
        u = p[u]
    return edges
