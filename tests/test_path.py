import pytest

from mygraph.heap_queue import HeapQueue
from mygraph.path import *  # noqa


@pytest.fixture
def graph():
    """Граф из лекции от 13.01.2002"""
    # fmt: off
    o = None
    g = [
        # a b  c  d  e  f  g
        [o, 2, 3, 6, o, o, o, ],  # a 0
        [2, o, 4, o, 9, o, o, ],  # b 1
        [3, 4, o, 1, 7, 6, o, ],  # c 2
        [6, o, 1, o, o, 4, o, ],  # d 3
        [o, 9, 7, o, o, 1, 5, ],  # e 4
        [o, o, 6, 4, 1, o, 8, ],  # f 5
        [o, o, o, o, 5, 8, o, ],  # g 6

    ]
    # fmt: on
    return g


@pytest.fixture
def distance():
    """для графа из лекции от 13.01.2002
    дистанции из вершниы a"""
    d = [0, 2, 3, 4, 9, 8, 14]
    return d


@pytest.fixture
def path():
    """для графа из лекции от 13.01.2002
    кратчайшие пути из вершниы a"""
    p = [0, 0, 0, 2, 5, 3, 4]
    return p


@pytest.fixture
def path_to_e():
    a, b, c, d, e, f = range(6)
    edges = [
        (a, c),  # 1
        (c, d),  # 2
        (d, f),  # 3
        (f, e),  # 4
    ]
    return edges


def test_heap_queue():
    hq = HeapQueue()

    hq.push(4)
    assert hq.pop() == 4

    hq.push(4)
    hq.push(3)
    hq.push(4)
    hq.push(14)
    hq.push(7)
    hq.push(12)
    hq.push(1)
    hq.push(5)
    assert hq.pop() == 1
    assert hq.pop() == 3
    assert hq.pop() == 4
    assert hq.pop() == 4
    assert hq.pop() == 5
    assert hq.pop() == 7
    assert hq.pop() == 12
    assert hq.pop() == 14


def test_deijkstra(graph, distance, path):
    d, p = deijkstra(graph, 0)
    assert distance == d
    assert path == p


def test_deijkstra_path(graph, path_to_e):
    res = deijkstra_path(graph, 0, 4)
    assert res == path_to_e
