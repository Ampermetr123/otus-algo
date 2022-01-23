import pytest

from mygraph.mst import *  # noqa


@pytest.fixture
def graph():
    """Граф из лекции"""
    # fmt: off
    o = None
    g = [
        # a b   c   d   e   f   g   h   k
        [o, 11, o,  o,  o,  o,  o,  3, 12, ],  # a 0
        [11, o, 17, o,  o,  o,  o,  o, 10, ],  # b 1
        [o, 17,  o, 14, o,  o,  o,  o, 5, ],  # c 2
        [o,  o, 14,  o, 13, o,  o,  o, 2, ],  # d 3
        [o,  o,  o, 13, o,  7,  8,  o, 6, ],  # e 4
        [o,  o,  o,  o, 7,  o, 16,  4, o, ],  # f 5
        [o,  o,  o,  o, 8, 16,  o, 15, 1, ],  # g 6
        [3,  o,  o,  o, o,  4, 15,  o, 9, ],  # h 7
        [12, 10, 5,  2, 6,  o,  1,  9, o, ],  # k 8
    ]
    # fmt: on
    return g


@pytest.fixture
def ostov_edges():
    a, b, c, d, e, f, g, h, k = range(9)
    ostov_edges = [
        (g, k),  # 1
        (d, k),  # 2
        (a, h),  # 3
        (f, h),  # 4
        (c, k),  # 5
        (e, k),  # 6
        (e, f),  # 7
        (b, k),  # 10
    ]
    # fmt: on
    return ostov_edges


@pytest.fixture
def sorted_edges():
    a, b, c, d, e, f, g, h, k = range(9)
    sorted_edges = [
        (g, k),  # 1
        (d, k),  # 2
        (a, h),  # 3
        (f, h),  # 4
        (c, k),  # 5
        (e, k),  # 6
        (e, f),  # 7
        (e, g),  # 8
        (h, k),  # 9
        (b, k),  # 10
        (a, b),  # 11
        (a, k),  # 12
        (d, e),  # 13
        (c, d),  # 14
        (g, h),  # 15
        (f, g),  # 16
        (b, c),  # 17
    ]
    return sorted_edges


def test_get_sorted_edges(graph, sorted_edges):
    res = get_sorted_edges(graph)
    assert res == sorted_edges


def test_kruskal_dsf(graph, ostov_edges):
    res = kruskal_dsf(graph)
    assert res == ostov_edges


def test_UnionFind():
    uf = UnionFind(10)
    uf.union(0, 1)
    uf.union(1, 9)
    uf.union(9, 4)
    assert uf.find(0) == uf.find(4)
    assert uf.find(0) != uf.find(2)


def test_kruskal_uf(graph, ostov_edges):
    res = kruskal_uf(graph)
    assert res == ostov_edges


def test_boruvka(graph, ostov_edges):
    res = sorted(boruvka(graph))
    assert res == sorted(ostov_edges)


def test_prim(graph, ostov_edges):
    res = sorted(prim(graph))
    assert res == sorted(ostov_edges)
