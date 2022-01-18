from typing import Dict, FrozenSet, Iterator, List, Set

Node = str
AdjencyVector = List[Node]
Graph = Dict[Node, AdjencyVector]


def rec_dfs(g: Graph, node: Node, visited: AdjencyVector) -> None:
    """
    Рекурсивный алгоритм поиска в глубину DFS
    Для графа G все вершниы, достижимые из s, сохраняются в множетсве S
    """
    visited.append(node)  # фиксируем посещение узла
    for u in g[node]:  # обход соседей
        if u in visited:
            continue  # уже посещали: пропускаем
        rec_dfs(g, u, visited)  # не посещали: рекурсивно ищем


def iter_dfs(
    g: Graph, node: Node, ignore: Set[Node] = set()
) -> Iterator[Node]:
    """
    Генератор поиска в глубину DFS
    Узлы, указанные в ignore - не используются
    """
    visited = set()
    stack = []
    stack.append(node)  # планируем посещение узла
    while stack:
        u = stack.pop()  # выбираем последнее значение

        # не обрабатываем ранее посещенные узлы
        if u in visited:
            continue

        visited.add(u)  # фиксируем посещение узла

        stack.extend(
            (x for x in g[u] if x not in ignore)
        )  # планируем посещение всех смежных вершин

        yield u  # сообщаем клиенту посещенный узел


def dfs_topsort(g: Graph) -> AdjencyVector:
    """
    Топологическая сортировка на базе DFS
    ( ~ алгоритм Тарьяна/Trémaux, но без раскраски)
    Возвращает спискок узлов, отсортированных топлогически
    """
    visited = set()
    res = []  # History and result

    def recurse(u: Node) -> None:
        if u in visited:
            return  # не посещаем повторно
        visited.add(u)  # сохраняем
        for v in g[u]:
            recurse(v)  # рекурсия по смежным врешинам
        res.append(u)  # добавление вершины после возвращения из рекурсии

    for u in g:
        recurse(u)  # для покрытия всего графа

    res.reverse()  # топологический обход в обратном порядке
    return res


def transpose(g: Graph) -> Graph:
    """
    Возвращает граф, в котором дуги изменили направление
    """
    # создаем с этими же вершинами
    gt: Graph = {}
    for u in g:
        gt[u] = []

    for u in g:
        for v in g[u]:
            gt[v].append(u)
    return gt


def kosaraju_scc(g: Graph) -> List[FrozenSet[Node]]:
    """
    Поиск компонент сильной связности - КСС
    (SCC - Strongly connetcted components)
    Алгортимом Косарайю
    """
    gt = transpose(g)  # Get the transposed graph
    sccs = []
    seen: Set[Node] = set()
    for u in dfs_topsort(gt):  # идем в пордке топологической сортировки
        if u in seen:
            continue
        scc = frozenset(
            iter_dfs(g, u, seen)
        )  # игнориуруем узлы из найденных компонент связности
        seen.update(scc)  # сохраняем вершины из найденных компонент связности
        sccs.append(scc)  # сохраняем найденную КС
    return sccs
