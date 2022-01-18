import pytest

from mygraph import demucron_topsort


def test_demucron():
    o = 0
    g = [
        # 0  1  2  3  4  5  6  7  8  9  10 11 12 13
        [o, o, 1, o, o, o, o, o, o, o, o, o, 1, o],  # 0
        [o, o, o, o, o, o, o, o, o, o, o, o, 1, o],  # 1
        [o, o, o, o, o, o, o, o, o, o, o, o, o, o],  # 2
        [o, o, 1, o, o, o, o, o, o, o, o, o, o, o],  # 3
        [o, o, 1, o, o, o, o, o, 1, 1, o, o, o, o],  # 4
        [o, o, o, 1, o, o, o, o, o, o, 1, 1, 1, o],  # 5
        [o, o, o, o, o, o, o, o, o, o, 1, o, o, o],  # 6
        [o, 1, o, 1, o, 1, 1, o, o, o, o, o, o, o],  # 7
        [1, o, o, o, o, o, o, o, o, o, o, o, o, 1],  # 8
        [1, o, o, o, o, o, 1, o, o, o, o, 1, o, o],  # 9
        [o, o, 1, o, o, o, o, o, o, o, o, o, o, o],  # 10
        [o, o, o, o, o, o, o, o, o, o, o, o, o, o],  # 11
        [o, o, 1, o, o, o, o, o, o, o, o, o, o, o],  # 12
        [o, o, o, o, o, 1, o, o, o, o, o, o, o, o],  # 13
    ]

    topology = [
        frozenset([4, 7]),
        frozenset([1, 8, 9]),
        frozenset([0, 6, 13]),
        frozenset([5]),
        frozenset([3, 10, 11, 12]),
        frozenset([2]),
    ]

    assert topology == demucron_topsort(g)


if __name__ == "__main__":
    pytest.main()
