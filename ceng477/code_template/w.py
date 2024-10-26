
import numpy as np

direction = np.array([-0.5+1/1024, 0.5-1/768, -1])

print("direction: ", direction)

# normalize
direction = direction / np.linalg.norm(direction)
print("direction: ", direction)

# v0 = np.array([0, 0, 0])
# v1 = np.array([3, 4, 0])
# v2 = np.array([6, 0, 0])

# p1 = np.array([4, 2, 0])
# p2 = np.array([6, 3, 0])

# v0v1 = v1 - v0
# v0v2 = v2 - v0
# normal = np.cross(v0v1, v0v2)
# print("normal: ", normal)

# v1v2 = v2 - v1
# v1P1 = p1 - v1

# c1 = np.cross(v1v2, v1P1)
# print("c: ", c1)

# if np.dot(normal, c1) > 0:
#     print("p1 is on the right side of v1v2")
# else:
#     print("p1 is on the left side of v1v2")

# v1p2 = p2 - v1
# c2 = np.cross(v1v2, v1p2)

# if np.dot(normal, c2) > 0:
#     print("p2 is on the right side of v1v2")
# else:
#     print("p2 is on the left side of v1v2")




