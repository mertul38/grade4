

import os
import sys
import subprocess
# run terminal commands


def run_debug_box():
    cmd = "./rasterizer ../input_outputs/culling_disabled_inputs/empty_box_debug.xml"
    subprocess.run(cmd, shell=True)

def run_empty_box():
    cmd = "./rasterizer ../input_outputs/culling_disabled_inputs/empty_box.xml"
    subprocess.run(cmd, shell=True)



if __name__ == "__main__":

    try:
        arg = sys.argv[1]

        if arg == "debug":
            run_debug_box()
    except:
        run_empty_box()