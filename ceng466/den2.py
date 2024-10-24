

from the1 import Interpolation
import numpy as np


interpolation = Interpolation()

arr = np.array([
    [1, 2],
    [3, 4]
])

interpolation.calculate_cubic_weights((0.6, 0), arr, Interpolation.NEIGHBOUR_TYPE.Y_DIM_POINT)
