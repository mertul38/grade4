import cv2
import numpy as np

# Load the image from the path
img = np.array([
    [30, 40],
    [10, 60]
], dtype=np.uint8)


scale = 2
# Resize the rotated image using bilinear interpolation (2x scaling)
linear_scaled = cv2.resize(img, None, fx=scale, fy=scale, interpolation=cv2.INTER_LINEAR)

# Resize the rotated image using bicubic interpolation (2x scaling)
cubic_scaled = cv2.resize(img, None, fx=scale, fy=scale, interpolation=cv2.INTER_CUBIC)

print("Rotated")
print(img)
print("----------------")
print("Scaled Image with Bilinear Interpolation")
print(linear_scaled)
print("----------------")
print("Scaled Image with Bicubic Interpolation")
print(cubic_scaled)

