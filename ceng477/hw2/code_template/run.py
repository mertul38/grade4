

import os
import sys
import subprocess
from enum import Enum
# run terminal commands

class NON_CULL_PATH(Enum):
    EMPTY_BOX = "../input_outputs/culling_disabled_inputs/empty_box.xml"
    DEBUG_BOX = "../input_outputs/culling_disabled_inputs/empty_box_debug.xml"
    FILLED_BOX = "../input_outputs/culling_disabled_inputs/filled_box.xml"
    FLAG_C = "../input_outputs/culling_disabled_inputs/flag_czechia.xml"
    FLAG_C_ALTERNATIVE = "../input_outputs/culling_disabled_inputs/flag_czechia_alternative.xml"
    FLAG_G = "../input_outputs/culling_disabled_inputs/flag_germany.xml"
    FLAG_T = "../input_outputs/culling_disabled_inputs/flag_turkey.xml"
    FLAG_T_ALTERNATIVE = "../input_outputs/culling_disabled_inputs/flag_turkey_alternative.xml"
    HORSE_AND_MUG = "../input_outputs/culling_disabled_inputs/horse_and_mug.xml"

class CULL_PATH(Enum):
    EMPTY_BOX = "../input_outputs/culling_enabled_inputs/empty_box.xml"
    DEBUG_BOX = "../input_outputs/culling_enabled_inputs/empty_box_debug.xml"
    FILLED_BOX = "../input_outputs/culling_enabled_inputs/filled_box.xml"
    FLAG_C = "../input_outputs/culling_enabled_inputs/flag_czechia.xml"
    FLAG_C_ALTERNATIVE = "../input_outputs/culling_enabled_inputs/flag_czechia_alternative.xml"
    FLAG_G = "../input_outputs/culling_enabled_inputs/flag_germany.xml"
    FLAG_T = "../input_outputs/culling_enabled_inputs/flag_turkey.xml"
    FLAG_T_ALTERNATIVE = "../input_outputs/culling_enabled_inputs/flag_turkey_alternative.xml"
    HORSE_AND_MUG = "../input_outputs/culling_enabled_inputs/horse_and_mug.xml"    

def get_output_path(path, culled):
    base = "./out/"
    if culled:
        base += "culling/"
    else:
        base += "no_culling/"

    base += path.split("/")[-1].split(".")[0]
    return base 

def run(path, culled=False):
    out_path = get_output_path(path, culled)
    if not os.path.exists(out_path):
        os.makedirs(out_path)
    cmd = f"./rasterizer {path}"
    subprocess.run(cmd, shell=True)


if __name__ == "__main__":

    try:
        arg = sys.argv[1]

        if arg == "debug":
            run(NON_CULL_PATH.DEBUG_BOX.value)
        if arg == "empty":
            run(NON_CULL_PATH.EMPTY_BOX.value)
        if arg == "filled":
            run(NON_CULL_PATH.FILLED_BOX.value)
        if arg == "flag_c":
            run(NON_CULL_PATH.FLAG_C.value)
        if arg == "flag_c_a":
            run(NON_CULL_PATH.FLAG_C_ALTERNATIVE.value)
        if arg == "flag_g":
            run(NON_CULL_PATH.FLAG_G.value)
        if arg == "flag_t":
            run(NON_CULL_PATH.FLAG_T.value)
        if arg == "flag_t_a":
            run(NON_CULL_PATH.FLAG_T_ALTERNATIVE.value)
        if arg == "ham":
            run(NON_CULL_PATH.HORSE_AND_MUG.value)
        if arg == "non_cull":
            for path in NON_CULL_PATH:
                run(path.value)

        if arg == "c_empty":
            run(CULL_PATH.EMPTY_BOX.value, True)
        if arg == "c_filled":
            run(CULL_PATH.FILLED_BOX.value, True)
        if arg == "c_flag_c":
            run(CULL_PATH.FLAG_C.value, True)
        if arg == "c_flag_c_a":
            run(CULL_PATH.FLAG_C_ALTERNATIVE.value, True)
        if arg == "c_flag_g":
            run(CULL_PATH.FLAG_G.value, True)
        if arg == "c_flag_t":
            run(CULL_PATH.FLAG_T.value, True)
        if arg == "c_flag_t_a":
            run(CULL_PATH.FLAG_T_ALTERNATIVE.value, True)
        if arg == "c_ham":
            run(CULL_PATH.HORSE_AND_MUG.value, True)
        if arg == "cull":
            for path in CULL_PATH:
                run(path.value, True)

        if arg == "all":
            for path in NON_CULL_PATH:
                run(path.value)
            for path in CULL_PATH:
                run(path.value, True)


    except:
        raise "Invalid argument"