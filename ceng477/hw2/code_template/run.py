

import os
import sys
import subprocess
from enum import Enum
# run terminal commands

class PATH(Enum):
    EMPTY_BOX = "../input_outputs/culling_disabled_inputs/empty_box.xml"
    DEBUG_BOX = "../input_outputs/culling_disabled_inputs/empty_box_debug.xml"
    FILLED_BOX = "../input_outputs/culling_disabled_inputs/filled_box.xml"
    FLAG_C = "../input_outputs/culling_disabled_inputs/flag_czechia.xml"
    FLAG_C_ALTERNATIVE = "../input_outputs/culling_disabled_inputs/flag_czechia_alternative.xml"
    FLAG_G = "../input_outputs/culling_disabled_inputs/flag_germany.xml"

def run_debug_box():
    cmd = f"./rasterizer {PATH.DEBUG_BOX.value}"
    subprocess.run(cmd, shell=True)

def run_empty_box():
    cmd = f"./rasterizer {PATH.EMPTY_BOX.value}"
    subprocess.run(cmd, shell=True)

def run_filled_box():
    cmd = f"./rasterizer {PATH.FILLED_BOX.value}"
    subprocess.run(cmd, shell=True)

def run_flag_c():
    cmd = f"./rasterizer {PATH.FLAG_C.value}"
    subprocess.run(cmd, shell=True)

def run_flag_c_alternative():
    cmd = f"./rasterizer {PATH.FLAG_C_ALTERNATIVE.value}"
    subprocess.run(cmd, shell=True)

def run_flag_g():
    cmd = f"./rasterizer {PATH.FLAG_G.value}"
    subprocess.run(cmd, shell=True)

if __name__ == "__main__":

    try:
        arg = sys.argv[1]

        if arg == "debug":
            run_debug_box()
        if arg == "empty":
            run_empty_box()
        if arg == "filled":
            run_filled_box()
        if arg == "flag_c":
            run_flag_c()
        if arg == "flag_c_a":
            run_flag_c_alternative()
        if arg == "flag_g":
            run_flag_g()

    except:
        raise "Invalid argument"