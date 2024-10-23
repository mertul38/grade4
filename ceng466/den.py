import cv2
import numpy as np

# Load the image from the path
img = cv2.imread('THE1_Images/ratio_4_degree_30.png')


if img is None:
    print("Image not found, please check the path!")
else:
    # Display the original image
    cv2.imshow('Original', img)
    cv2.waitKey(0)
    # Rotate the image by 30 degrees clockwise
    # Get image dimensions
    (h, w) = img.shape[:2]
    center = (w // 2, h // 2)
    
    # Define the rotation matrix for 30-degree clockwise rotation (-30 degrees)
    rotation_matrix = cv2.getRotationMatrix2D(center, -30, 1.0)
    
    # Perform the affine transformation (rotation)
    rotated_img = cv2.warpAffine(img, rotation_matrix, (w, h))
    
    # Display the rotated image
    cv2.imshow('Rotated', rotated_img)
    cv2.waitKey(0)

    scale = 2
    # Resize the rotated image using bilinear interpolation (2x scaling)
    linear_scaled = cv2.resize(rotated_img, None, fx=scale, fy=scale, interpolation=cv2.INTER_LINEAR)
    
    # Resize the rotated image using bicubic interpolation (2x scaling)
    cubic_scaled = cv2.resize(rotated_img, None, fx=scale, fy=scale, interpolation=cv2.INTER_CUBIC)
    
    
    print("\nScaled Image with Bilinear Interpolation (2x):")
    cv2.imshow('Bilinear Interpolation', linear_scaled)
    cv2.waitKey(0)

    print("\nScaled Image with Bicubic Interpolation (2x):")
    cv2.imshow('Bicubic Interpolation', cubic_scaled)
    cv2.waitKey(0)
    # Wait for any key press to close the images
    cv2.waitKey(0)
    cv2.destroyAllWindows()
