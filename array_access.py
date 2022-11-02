import numpy as np


a: np.ndarray = np.array([[1, 2, 3, 4], [5, 6, 7, 8]])

print(a)
xk = a.flatten(order="K")
xf = a.flatten(order="F")
xc = a.flatten(order="C")
print(f"xc {xc}")
print(f"xf {xf}")
print(f"xk {xk}")


rows, cols = a.shape
print(f"cols sum {a.sum(axis=0)}")

print("\nattempt 1")
outarr = np.zeros(cols)
for x in range(rows):
    for y in range(cols):
        print(f"{xc[x*cols + y]}", end=" ")
        outarr[y] += xc[x * cols + y]
print("")
print(outarr)


print("\nattempt 2")
outarr = np.zeros(cols)
for x in range(rows):
    for y in range(cols):
        print(f"{xc[x*rows + y]}", end=" ")
        outarr[y] += xc[x * rows + y]
print("")
print(outarr)

print("\nattempt 3")
outarr = np.zeros(cols)
for x in range(rows):
    for y in range(cols):
        print(f"{xf[x*rows + y]}", end=" ")
        outarr[y] += xc[x * rows + y]
print("")
print(outarr)
