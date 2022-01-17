import pytest

from kosaraju import iter_dfs, kosaraju_scc, rec_dfs, transpose


@pytest.fixture
def graph():
    #      b  -  d
    #    /   \
    #  a      \              h
    #    \     \            /
    #      c  -  e    f - g
    # Note all connectrions is left - to right
    G = {
        "a": ["b", "c"],
        "b": ["a", "e", "d"],
        "c": ["a", "e"],
        "d": ["b"],
        "e": ["b", "c"],
        "f": ["g"],
        "g": ["f", "h"],
        "h": ["g"],
    }
    return G


@pytest.fixture
def orgraph_normal():
    # Note all connectrions are left to right
    #      b  -  d
    #    /   \
    #  a      \              h
    #    \     \            /
    #      c  -  e    f - g
    G = {
        "a": ["b", "c"],
        "b": ["e", "d"],
        "c": ["e"],
        "d": [],
        "e": [],
        "f": ["g"],
        "g": ["h"],
        "h": [],
    }
    return G


@pytest.fixture
def orgraph_reversed():
    # Note all connectrions are right to left
    #      b  -  d
    #    /   \
    #  a      \              h
    #    \     \            /
    #      c  -  e    f - g
    G = {
        "a": [],
        "b": ["a"],
        "c": ["a"],
        "d": ["b"],
        "e": ["b", "c"],
        "f": [],
        "g": ["f"],
        "h": ["g"],
    }
    return G


@pytest.fixture
def graph_with_scc():
    #  Графи из примера, разобранного на лекции
    G = {
        "a": ["b"],
        "b": ["e", "f", "c"],
        "c": ["d"],
        "d": ["c", "h"],
        "e": ["a", "f"],
        "f": ["g"],
        "g": ["f", "h", "c"],
        "h": ["d"],
    }
    SCC = [frozenset("hdc"), frozenset("fg"), frozenset("aeb")]
    return G, SCC


def test_rec_dfs(graph):
    s = []
    rec_dfs(graph, "f", s)
    assert s == ["f", "g", "h"]
    s = []
    rec_dfs(graph, "a", s)
    assert s == ["a", "b", "e", "c", "d"]


def test_iter_dfs(graph):
    assert list(iter_dfs(graph, "f")) == ["f", "g", "h"]
    assert list(iter_dfs(graph, "a")) == ["a", "c", "e", "b", "d"]


def test_iter_dfs_with_ignore(graph):
    assert list(iter_dfs(graph, "a", "b")) == ["a", "c", "e"]


def test_transpose(orgraph_normal, orgraph_reversed):
    assert orgraph_reversed == transpose(orgraph_normal)


def test_kosaraju(graph_with_scc):
    graph, gsccs = graph_with_scc
    sccs = kosaraju_scc(graph)

    s1 = set(gsccs)
    s2 = set(sccs)
    s_dif = s1.difference(s2)

    assert len(s_dif) == 0


if __name__ == "__main__":
    pytest.main()
