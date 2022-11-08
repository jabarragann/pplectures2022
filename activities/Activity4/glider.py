import matplotlib.pyplot as plt
import numpy as np
import dask.array as da


def rules_of_life(subgrid):
    """Evaluate the rules of life on a 2-d subarray.
    The array should have an overlap of 1 cell in all dimension
    and on the corner.

    Args:
        subgrid (ndarray): Input array

    Returns:
        outgrid (ndarray): Array updated by rules of life
    """
    # we will receive an array with 1 dimension of padding
    xdim, ydim = subgrid.shape

    # output array to keep updates
    outgrid = np.zeros(shape=subgrid.shape, dtype=subgrid.dtype)

    # update only in center (non-overlapping) regaion
    for x in range(1, xdim - 1):
        for y in range(1, ydim - 1):
            sum = (
                subgrid[x - 1, y - 1]
                + subgrid[x, y - 1]
                + subgrid[x + 1, y - 1]
                + subgrid[x - 1, y]
                + subgrid[x + 1, y]
                + subgrid[x - 1, y + 1]
                + subgrid[x, y + 1]
                + subgrid[x + 1, y + 1]
            )
            # three neighbors birth
            if sum == 3:
                outgrid[x, y] = 1
            # two neighbors no change
            elif sum == 2:
                outgrid[x, y] = subgrid[x, y]
            # <2 or >3 death
            else:
                outgrid[x, y] = 0

    return outgrid


def main():
    # glider pattern
    glider = np.array([[0, 1, 0], [0, 0, 1], [1, 1, 1]], dtype=np.uint8)
    plt.imshow(glider, cmap="Greys", interpolation="nearest")
    plt.show()

    # put the glider in the top corner of a 16x16 grid
    grid = np.zeros((18, 18), dtype=np.uint8)
    grid[1 : glider.shape[0] + 1, 1 : glider.shape[1] + 1] = glider

    for i in range(70):
        grid = rules_of_life(grid)
        plt.imshow(grid, cmap="Greys", interpolation="nearest")
        plt.show()


if __name__ == "__main__":
    main()
