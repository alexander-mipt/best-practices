#!/usr/bin/python3
import logging
import re
import subprocess
import sys


class ProcessException(Exception):
    pass


def execute(cmd):
    process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    out, err = process.communicate()
    if process.returncode:
        logging.error("OUT: %s", out.decode('utf-8'))
        logging.error("ERR: %s", err.decode('utf-8'))
        raise ProcessException(err.decode('utf-8'))
    return out, err


def find_dimensions():
    out, _ = execute("xdpyinfo | grep dimensions")
    dimensions = re.findall(r'dimensions:\s*([0-9]+)x([0-9]+) pixels', out.decode('utf-8'))
    if len(dimensions) != 1:
        raise ProcessException(f"Got {dimensions} from regexp")
    return dimensions[0]


def main():
    try:
        width, height = find_dimensions()
    except ProcessException as pe:
        logging.error("Failed to get size of screen: %s", pe)
        sys.exit(1)
    print(f"{width} {height}", end='')


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    main()
