


import subprocess
import time
import sys
import cv2
import tarfile
import os


def delete_ppm_files():
    for file in os.listdir("./"):
        if file.endswith(".ppm"):
            if os.path.exists(file):
                os.remove(file)
                print(f"Deleted {file}")

# List of files to include in the tarball
files_to_pack = [
    "Makefile",          # Makefile to run `make`
    "raytracer.cpp",     # Main raytracer code
    "parser.cpp",       # Main raytracer header
    "parser.h",          # Main raytracer header
    "ppm.cpp",
    "ppm.h",
    "tinyxml2.cpp",
    "tinyxml2.h",
    "readme"
    # Add more files as needed
]
# Function to pack files into a tar.gz archive
def pack_files(files, output_filename="raytracer.tar.gz"):
    with tarfile.open(output_filename, "w:gz") as tar:
        for file in files:
            if os.path.exists(file):
                tar.add(file, arcname=os.path.basename(file))
                print(f"Added {file} to {output_filename}")
            else:
                print(f"File {file} not found. Skipping.")

ignored_input_files = ["horse_and_mug.xml"]
ignored_output_files = ["horse_and_mug.ppm"]

def run_tracer(input_file):
    subprocess.run(["./raytracer", "../inputs/" + input_file])


def process():

    input_files = os.listdir("../inputs")

    start = time.time() 
    for input_file in input_files:
        if input_file.endswith(".xml") and input_file not in ignored_input_files:
            run_tracer(input_file)
    end = time.time()
    print("Total time: ", (end - start)/60)
    print("Processing horse_and_mug.xml...")
    start = time.time()
    run_tracer("horse_and_mug.xml")
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
        elif argv[1] == "pack":
            pack_files(files_to_pack)
        elif argv[1] == "delete":
            delete_ppm_files()
        else:
            print("Expected arguments: process or test")
    else:
        print("Expected arguments: process or test")