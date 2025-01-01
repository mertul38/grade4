


import tarfile
import os


files = os.listdir("./")

with tarfile.open("tetrisGL.tar.gz", "w:gz") as tar:
    for f in files:
        if f != "create_submission.py":
            tar.add(f)

