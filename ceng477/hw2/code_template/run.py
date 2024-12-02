

import os
import sys
import subprocess
# run terminal commands



def run_empty_box():
    cmd = "./rasterizer ../input_outputs/culling_disabled_inputs/empty_box.xml"
    subprocess.run(cmd, shell=True)



if __name__ == "__main__":
    run_empty_box()
