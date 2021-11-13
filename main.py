class Matrix:
    def __init__(self, n: int) -> None:
        self._table = [[0 for _ in range(n)] for _ in range(n)]
        self.dim = n

    def __iter__(self):
        return self._table.__iter__()

    def __getitem__(self, inx):
        return self._table.__getitem__(inx)

    def __str__(self) -> str:
        res = ""
        for i in range(self.dim):
            for j in range(self.dim):
                res += str(self._table[i][j]) + " "
            res += "\n"
        return res

    def copy(self) -> "Matrix":
        res = Matrix(self.dim)
        for i in range(self.dim):
            res._table[i] = self._table[i].copy()
        return res

class Semiring:
    def __init__(self, mult: Matrix, add: Matrix) -> None:
        assert mult.dim == add.dim
        self.mult = mult
        self.add = add
        self.dim = mult.dim

    def __str__(self) -> str:
        arr1 = str(self.mult).split("\n")
        arr2 = str(self.add).split("\n")
        top = "mult" + (2*self.dim - 2)*" " + "add\n"
        #  top += "    "
        #  for i in range(self.dim):
            #  top += f"{i} "
        #  top += "\n    " + self.dim * 2 * "-"
        res = top + "\n"
        for i in range(self.dim):
            #  res += f"{i} | " + arr1[i] + f"  {i}|" + arr2[i] + "\n"
            res += arr1[i] + "  " + arr2[i] + "\n"
        return res
        #  return tabulate([arr1, arr2], headers=["mult", "add"])

def isassociative(table: Matrix) -> bool:
    n = table.dim
    for i in range(n):
        for j in range(n):
            for k in range(n):
                if table[table[i][j]][k] != table[i][table[j][k]]: # (i + j) + k = i + (j + k)
                    return False
    return True

def iscommutative(table: Matrix) -> bool:
    n = table.dim
    for i in range(n):
        for j in range(i, n):
            if table[i][j] != table[j][i]: # i + j = j + i
                return False
    return True

def isidempotent(table: Matrix) -> bool:
    n = table.dim
    for i in range(n):
        if table[i][i] != i: # i * i = i
            return False
    return True

#  def isdistributive(mult: Matrix, add: Matrix) -> bool:
    #  assert add.dim == mult.dim, "Dims are not equal"
    #  n = add.dim
    #  for i in range(n):
        #  for j in range(n):
            #  for k in range(n):
                #  if (
                    #  mult[i][add[j][k]] != add[mult[i][j]][mult[i][k]]
                    #  or mult[add[j][k]][i] != add[mult[j][i]][mult[k][i]]
                #  ):
                    #  return False
    #  return True
  
def isdistributive(mult: Matrix, add: Matrix) -> bool:
    assert add.dim == mult.dim
    n = add.dim
    for i in range(n):
        for j in range(n):
            for k in range(n):
                if (mult[i][add[j][k]] != add[mult[i][j]][mult[i][k]]): # i*(j + k) = i*j + i*k
                    return False
    for i in range(n):
        for j in range(n):
            for k in range(n):
                if (mult[add[i][j]][k] != add[mult[i][k]][mult[j][k]]): # (i + j)*k = i*k + j*k
                    return False
    return True

def arrays_rec(arr: list[int], pos: int = 0) -> list[list[int]]:
    if pos == len(arr):
        yield arr.copy()
    else:
        for i in range(len(arr)):
            if i in arr[:pos]:
                continue
            arr[pos] = i
            for item in arrays_rec(arr, pos + 1):
                yield item

def arrays(dim: int) -> list[list[int]]:
    return arrays_rec([0 for _ in range(dim)])

def isisomorphism(f: list[int], r1: Semiring, r2: Semiring) -> bool:
    n = r1.dim
    for a in range(n):
        for b in range(n):
            if (
                f[r1.mult[a][b]] != r2.mult[f[a]][f[b]]
                or f[r1.add[a][b]] != r2.add[f[a]][f[b]]
            ):
                return False
    return True

def areisomorphic(r1: Semiring, r2: Semiring) -> bool:
    assert r1.dim == r2.dim
    n = r1.dim
    for f in arrays(n):
        if isisomorphism(f, r1, r2):
            return True
    return False

# 0 0  0 0  0 0  0 0  0 1  0 1  0 1  0 1  1 0  1 0  1 0  1 0  1 1  1 1  1 1  1 1 
# 0 0  0 1  1 0  1 1  0 0  0 1  1 0  1 1  0 0  0 1  1 0  1 1  0 0  0 1  1 0  1 1

def tables_rec(matrix: Matrix, pos: int = 0) -> list[Matrix]:
    if pos == matrix.dim * matrix.dim:
        yield matrix.copy()
    else:
        for i in range(matrix.dim):
            row = pos // matrix.dim
            col = pos % matrix.dim
            matrix[row][col] = i
            for item in tables_rec(matrix, pos + 1):
                yield item

def tables(dim: int) -> list[Matrix]:
    return tables_rec(Matrix(dim))

def semirings(dim: int) -> list[Semiring]:
    mult_tables: list[Matrix] = []
    add_tables: list[Matrix] = []
    for table in tables(dim):
        if isassociative(table):
            if isidempotent(table):
                mult_tables.append(table)
            if iscommutative(table):
                add_tables.append(table)

    for mult in mult_tables:
        for add in add_tables:
            if isdistributive(mult, add):
                yield Semiring(mult, add)

def main():
    #  n = int(input("n = "))
    n = 3

    assert n < 4, "Too big n. Not supported yet"

    arr = list(semirings(n))
    n = len(arr)
    count = 0
    for i in range(n):
        found = False
        for j in range(i + 1, n):
            if areisomorphic(arr[i], arr[j]):
                found = True
        if not found:
            print("******")
            print(arr[i])
            count += 1
    print("\ncount: " + str(count))

if __name__ == "__main__":
    main()
