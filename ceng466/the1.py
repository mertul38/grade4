import numpy as np
import cv2 
import matplotlib.pyplot as  plt
import enum
import os

input_folder = 'THE1_Images/'
output_folder = 'THE1_Outputs/'

class Rotation:
    def __init__(self):
        pass
    def rotate_image(self, img, degree):
        '''img: img to be rotated
        degree: degree of rotation in counter clockwise (30 means 30 degree rotation in counter clockwise, -30 means 30 degree rotation in clockwise)
        return rotated image'''
        # Rotate the image
        rad = np.deg2rad(-degree)
        rotation_matrix = np.array([
            [np.cos(rad), -np.sin(rad)],
            [np.sin(rad), np.cos(rad)]
        ])
        h, w = img.shape[:2]
        channels = 1 if len(img.shape) == 2 else img.shape[2]
        corners = np.array([
            [-h//2, -w//2],
            [h//2, -w//2],
            [h//2, w//2],
            [-h//2, w//2]
        ])
        new_corners = np.dot(rotation_matrix, corners.T)
        new_corners = new_corners.T

        min_corner = np.min(new_corners, axis=0)
        max_corner = np.max(new_corners, axis=0)
        # Calculate the size of the new image
        new_h = int(np.ceil(max_corner[0] - min_corner[0]))
        new_w = int(np.ceil(max_corner[1] - min_corner[1]))
        
        rotated_img = np.zeros((new_h, new_w, channels), dtype=img.dtype)
        center_y, center_x = new_h//2, new_w//2

        inverse_rotation_matrix = np.linalg.inv(rotation_matrix)

        for i in range(new_h):
            for j in range(new_w):
                # Get the coordinate in the original image space
                x, y = np.dot(inverse_rotation_matrix, [j-center_x, i-center_y])
                x = int(x + w//2)
                y = int(y + h//2)
                
                # Check if the calculated coordinates are within the bounds of the original image
                if 0 <= x < w and 0 <= y < h:
                    rotated_img[i, j] = img[y, x]
        return rotated_img

class Interpolation:
    class INTERPOLATION_TYPE(enum.Enum):
        LINEAR = "linear"
        CUBIC = "cubic"

    class BILINEAR_NEIGHBOUR_POINT_TYPE(enum.Enum):
        EXACT_POINT = 1
        ONE_DIM_POINT = 2
        TWO_DIM_POINT = 3
        
    class CUBIC_NEIGHBOUR_POINT_TYPE(enum.Enum):
        pass

    def __init__(self):
        pass

    # General Functions
    def get_reverse_scaled_point(self, point, scale, h, w):
        center_point = tuple(map(lambda x: x + 0.5, point))
        reverse_scaled_center_point = tuple(map(lambda x: x / scale, center_point))
        reverse_scaled_point = tuple(map(lambda x: x - 0.5, reverse_scaled_center_point))

        # Correcting out of bounds points
        reverse_scaled_point = (
            min(max(reverse_scaled_point[0], 0), h - 1),
            min(max(reverse_scaled_point[1], 0), w - 1)
        )
        return reverse_scaled_point

    def calculate_weighted_value(self, img, points, weights):
        final_value = 0
        values = [img[point[0], point[1]] for point in points]
        for value, weight in zip(values, weights):
            final_value += value * weight
        return final_value

    # Bilinear Interpolation Specific Functions
    def get_neighbour_points(self, point, h, w):
        point_y_int = int(point[0])
        point_x_int = int(point[1])
        if point[0] == point_y_int and point[1] == point_x_int:
            return ([point], self.BILINEAR_NEIGHBOUR_POINT_TYPE.EXACT_POINT)
        elif point[0] == point_y_int or point[1] == point_x_int:
            if point[0] == point_y_int:
                Q1 = (point_y_int, point_x_int)
                Q2 = (point_y_int, point_x_int + 1)
                return ([Q1, Q2], self.BILINEAR_NEIGHBOUR_POINT_TYPE.ONE_DIM_POINT)
            else:
                Q1 = (point_y_int, point_x_int)
                Q2 = (point_y_int + 1, point_x_int)
                return ([Q1, Q2], self.BILINEAR_NEIGHBOUR_POINT_TYPE.ONE_DIM_POINT)
        else:
            Q1 = (point_y_int, point_x_int)
            Q2 = (point_y_int + 1, point_x_int + 1)
            Q3 = (point_y_int, point_x_int + 1)
            Q4 = (point_y_int + 1, point_x_int)
            return ([Q1, Q2, Q3, Q4], self.BILINEAR_NEIGHBOUR_POINT_TYPE.TWO_DIM_POINT)

    def find_bilinear_weights(self, point, neighbour_points, neighbour_type):
        weights = []
        if neighbour_type == self.BILINEAR_NEIGHBOUR_POINT_TYPE.EXACT_POINT:
            weights = [1]
        elif neighbour_type == self.BILINEAR_NEIGHBOUR_POINT_TYPE.ONE_DIM_POINT:
            q1 = neighbour_points[0]
            q2 = neighbour_points[1]
            if q1[0] == q2[0]:
                w1 = abs(q2[1] - point[1])
                w2 = abs(q1[1] - point[1])
                weights = [w1, w2]
            else:
                w1 = abs(q2[0] - point[0])
                w2 = abs(q1[0] - point[0])
                weights = [w1, w2]
        elif neighbour_type == self.BILINEAR_NEIGHBOUR_POINT_TYPE.TWO_DIM_POINT:
            q1, q2, q3, q4 = neighbour_points
            w1 = abs(q2[0] - point[0]) * abs(q2[1] - point[1])
            w2 = abs(q1[0] - point[0]) * abs(q1[1] - point[1])
            w3 = abs(q4[0] - point[0]) * abs(q4[1] - point[1])
            w4 = abs(q3[0] - point[0]) * abs(q3[1] - point[1])
            weights = [w1, w2, w3, w4]

        return weights

    def bilinear_interpolation(self, img, scale):
        h, w = img.shape[:2]
        channels = 1 if len(img.shape) == 2 else img.shape[2]
        new_h = int(h * scale)
        new_w = int(w * scale)

        scaled_img = np.zeros((new_h, new_w, channels), dtype=img.dtype)

        for i in range(new_h):
            for j in range(new_w):
                point = (i, j)
                q = self.get_reverse_scaled_point(point, scale, h, w)
                neighbour_points, neighbour_type = self.get_neighbour_points(q, h, w)
                weights = self.find_bilinear_weights(q, neighbour_points, neighbour_type)
                placed_value = self.calculate_weighted_value(img, neighbour_points, weights)
                scaled_img[i, j] = np.round(placed_value)

        return scaled_img

    # Cubic Interpolation Specific Functions

    # General Interpolation Function
    def interpolate(self, img, scale, interpolation_type):
        if interpolation_type == self.INTERPOLATION_TYPE.LINEAR:
            return self.bilinear_interpolation(img, scale)
        elif interpolation_type == self.INTERPOLATION_TYPE.CUBIC:
            return self.cubic_interpolation(img, scale)

def read_image(filename, gray_scale = False):
    # CV2 is just a suggestion you can use other libraries as well
    if gray_scale:
        img = cv2.imread(input_folder + filename, cv2.CV_LOAD_IMAGE_GRAYSCALE)    
        return img
    img = cv2.imread(input_folder + filename)
    return img

def write_image(img, filename):
    # CV2 is just a suggestion you can use other libraries as well
    if os.path.exists(output_folder) == False:
        os.makedirs(output_folder)
    cv2.imwrite(output_folder+filename, img)

    
def rotate_upsample(img, scale, degree, interpolation_type):
    '''img: img to be rotated and upsampled
    scale: scale of upsampling (e.g. if current width and height is 64x64, and scale is 4, wodth and height of the output should be 256x256)
    degree: shows the degree of rotation
    interp: either linear or cubic'''
    rotation = Rotation()
    rotated_img = rotation.rotate_image(img, degree)
    interpolation = Interpolation()
    img = interpolation.interpolate(rotated_img, scale, interpolation_type)
    
    return img

def compute_distance(img1, img2):

    return distance

def desert_or_forest(img):
    '''img: image to be classified as desert or forest
    return a string: either 'desert'  or 'forest' 
    
    You should compare the KL Divergence between histograms of hue channel. Please provide images and discuss these histograms in your report'''
    desert1 = read_image('desert1.jpg')
    desert2 = read_image('desert2.jpg')
    forest1 = read_image('forest1.jpg')
    forest2 = read_image('forest2.jpg')

    return result

def difference_images(img1, img2):
    '''img1 and img2 are the images to take dhe difference
    returns the masked image'''

    return masked_image

def main():
    ###################### Q1
    # Read original image
    # Read corrupted image
    img_path = 'ratio_4_degree_30.png'
    img = read_image(img_path)
    # Correct the image with linear interpolation
    corrected_img_linear = rotate_upsample(img, 4, -30, Interpolation.INTERPOLATION_TYPE.LINEAR)
    write_image(corrected_img_linear, 'q1_1_corrected_linear.png')
    # Correct the image with cubic interpolation
    # corrected_img_cubic = rotate_upsample(img, 4, 30, 'cubic')
    # write_image(corrected_img_cubic, 'q1_1_corrected_cubic.png')

    # # Report the distances
    # print('The distance between original image and image corrected with linear interpolation is ', compute_distance(img_original, corrected_img_linear))
    # print('The distance between original image and image corrected with cubic interpolation is ', compute_distance(img_original, corrected_img_cubic))

    # # Repeat the same steps for the second image
    # img_original = read_image('q1_2.png')
    # img = read_image('ratio_8_degree_45.png')
    # corrected_img_linear = rotate_upsample(img, 8, 45, 'linear')
    # write_image(corrected_img_linear, 'q1_2_corrected_linear.png')
    # corrected_img_cubic = rotate_upsample(img, 8, 45, 'cubic')
    # write_image(corrected_img_cubic, 'q1_2_corrected_cubic.png')

    # # Report the distances
    # print('The distance between original image and image corrected with linear interpolation is ', compute_distance(img_original, corrected_img_linear))
    # print('The distance between original image and image corrected with cubic interpolation is ', compute_distance(img_original, corrected_img_cubic))

    # ###################### Q2
    # img = read_image('q2_1.jpg')
    # result = desert_or_forest(img)
    # print("Given image q2_1 is an image of a ", result)

    # img = read_image('q2_2.jpg')
    # result = desert_or_forest(img)
    # print("Given image q2_2 is an image of a ", result)

    # ###################### Q3
    # img1 = read_image('q3_a1.png',gray_scale=True)
    # img2 = read_image('q3_a2.png',gray_scale=True)
    # result = difference_images(img1,img2)
    # write_image(result, 'masked_image_a.png')

    # img1 = read_image('q3_b1.png')
    # img2 = read_image('q3_b2.png')
    # result = difference_images(img1,img2)
    # write_image(result, 'masked_image_b.png')

def test():
    img = read_image('ratio_4_degree_30.png')
    # img = read_image('cat.jpg')
    cv2.imshow('img', img)
    cv2.waitKey(0)
    rotation = Rotation()
    rotated_img = rotation.rotate_image(img, -30)
    cv2.imshow('rotated_img', rotated_img)
    cv2.waitKey(0)




if __name__ == '__main__':
    main()
