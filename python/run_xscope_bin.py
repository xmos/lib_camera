# Copyright 2023-2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import xscope_fileio
import argparse
import shutil
import subprocess
import glob
from pathlib import Path


def get_adapter_id():
    try:
        xrun_out = subprocess.check_output(['xrun', '-l'], text=True, stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        print('Error: %s' % e.output)
        assert False
    except FileNotFoundError:
        msg = ("please ensure you have XMOS tools activated in your environment")
        assert False, msg

    xrun_out = xrun_out.split('\n')
    # Check that the first 4 lines of xrun_out match the expected lines
    expected_header = ["", "Available XMOS Devices", "----------------------", ""]
    if len(xrun_out) < len(expected_header):
        raise RuntimeError(
            f"Error: xrun output:\n{xrun_out}\n"
            f"does not contain expected header:\n{expected_header}"
        )

    header_match = True
    for i, expected_line in enumerate(expected_header):
        if xrun_out[i] != expected_line:
            header_match = False
            break

    if not header_match:
        raise RuntimeError(
            f"Error: xrun output header:\n{xrun_out[:4]}\n"
            f"does not match expected header:\n{expected_header}"
        )

    try:
        if "No Available Devices Found" in xrun_out[4]:
            raise RuntimeError(f"Error: No available devices found\n")

    except IndexError:
        raise RuntimeError(f"Error: xrun output is too short:\n{xrun_out}\n")

    for line in xrun_out[6:]:
        if line.strip():
            adapterID = line[26:34].strip()
            status = line[34:].strip()
        else:
            continue
    print("adapter_id = ",adapterID)
    return adapterID


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("xe", nargs='?',
                        help=".xe file to run")
    args = parser.parse_args()
    return args

def run(xe, return_stdout=False):

    adapter_id = get_adapter_id()
    print("Running on adapter_id ",adapter_id)
    if return_stdout == False:
        xscope_fileio.run_on_target(adapter_id, xe)
    else:
        with open("prof.txt", "w+") as ff:
            xscope_fileio.run_on_target(adapter_id, xe, stdout=ff)
            ff.seek(0)
            stdout = ff.readlines()
        return stdout

def choose_file_with_extension(folder_path, extension):
    # Get a list of files with the specified extension in the folder
    files = list(Path(folder_path).rglob(f"*{extension}"))
    files = list(map(str, files)) # i dont like to much this map here but it works
    assert len(files) > 0 , (f"No {extension} files found in the folder.")
    
    [print(i,file) for i,file in enumerate(files)]
    
    choose = input("Choose the file to run: \n")
    if int(choose) in range(len(files)):
        run(files[int(choose)])

if __name__ == "__main__":
    args = parse_arguments()
    if (args.xe is None):
        build_folder = Path(__file__).parent.parent.resolve() / "examples"
        choose_file_with_extension(build_folder, ".xe")
    else:   
        run(args.xe)
