# Каждый элемент квадратной матрицы размеренности N x N равен нулю, либо единице.
# Найдите количество «островов», образованных единицами.
# Под «островом» понимается группа единиц (либо одна единица),
# со всех сторон окруженная нулями (или краями матрицы).
# Единицы относятся к одному «острову», если из одной из них можно перейти к другой «наступая»
# на единицы, расположенные в соседних клетках. Соседними являются клетки, граничащие по горизонтали
# или вертикали.

# Входные данные
# В первой строке вводится натуральное число N не больше 100 - размер квадратной матрицы.
# В следующих N строках задаются элементы матрицы, по N чисел через пробел на каждой.

# Выходные данные
# Вывести единственное число - количество островов.

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

    def union(self, u, v) -> None:
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

    def find(self, u) -> int:
        """Возвращает главного в множетсве, в котором элемент u"""
        if u == self.parents[u]:
            return u
        self.parents[u] = self.find(self.parents[u])
        return self.parents[u]

    def components(self):
        """Возврщает список главных элементов множеств"""
        return self.roots


def load_world():
    n = int(input())
    world = []
    for _ in range(n):
        str = input()
        world.append(list(map(int, str.split())))
    return world


if __name__ == '__main__':
    w = load_world()
    n = len(w)
    uf = UnionFind(n*n+1)

    for i in range(n):
        for j in range(n):
            if w[i][j] == 0:
                uf.union(i*n+j, n*n)
                continue
            # left
            if j>0 and w[i][j-1] == 1:
                uf.union(i*n+j, i*n+j-1)
            # right
            if j<n-1 and w[i][j+1] == 1:
                uf.union(i*n+j, i*n+j+1)
            # top
            if i>0 and w[i-1][j] == 1:
                uf.union(i*n+j, (i-1)*n+j)
            # down
            if i<n-1 and w[i+1][j] == 1:
                uf.union(i*n+j, (i+1)*n+j)


    count = len(uf.components()) - 1

    print(count)
