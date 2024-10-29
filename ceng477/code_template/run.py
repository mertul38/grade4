


import subprocess
import os
import time
import sys
import cv2

ignored_input_files = ["horse_and_mug.xml"]
ignored_output_files = ["horse_and_mug.ppm"]

def process():

    input_files = os.listdir("../inputs")


    start = time.time() 
    for input_file in input_files:
        if input_file.endswith(".xml") and input_file not in ignored_input_files:
            subprocess.run(["./raytracer", "../inputs/" + input_file])
    end = time.time()
    print("Total time: ", (end - start)/60)

def test():

    output_files = os.listdir("../outputs")
    i = 0
    while True:
        while True:
            output_file = output_files[i]
            if output_file in ignored_output_files:
                i += 1
                if i >= len(output_files)-1:
                    i -= 2
            else:
                break
                
        if output_file.endswith(".ppm"):
            print("Testing: ", output_file)
            expected_img = cv2.imread("../outputs/" + output_file)
            my_img = cv2.imread("./"+output_file)
            cv2.imshow("expected", expected_img)
            cv2.imshow("my_output", my_img)
            key = cv2.waitKey(0)
            if key == ord('q'):
                break
            if key == ord('d'):
                i += 1
            if key == ord('s'):
                i -= 1
            if i < 0:
                i = 0
            if i >= len(output_files):
                i = len(output_files) - 1


if __name__ == "__main__":
    argv = sys.argv

    if len(argv) > 1:
        if argv[1] == "process":
            process()
        elif argv[1] == "test":
            test()
    else:
        print("Expected arguments: process or test")