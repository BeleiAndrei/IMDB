import argparse
import os
import sys

EPS = 0.02

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

'''method for parsing command line arguments'''
def parse_args():
    """
    Parse input arguments
    """
    parser = argparse.ArgumentParser(description='Diff tool to compare text files')
    parser.add_argument(dest="ref_path", type=str)
    parser.add_argument(dest="out_path", type=str)

    args = parser.parse_args()
    return args

def equal(str1, str2):
    nr1 = float(str1)
    nr2 = float(str2)

    if abs(nr1 - nr2) < EPS:
        return True
    return False


def file_size(fileobj):
    fileobj.seek(0, os.SEEK_END)
    size = fileobj.tell()
    fileobj.seek(0, os.SEEK_SET)
    return size


def check():
    args = parse_args()
    ref_file = open(args.ref_path)
    out_file = open(args.out_path)

    if file_size(ref_file) != file_size(out_file):
        return False

    for out_line, ref_line in zip(out_file, ref_file):
        if is_number(ref_line) and is_number(out_line):
            if not equal(ref_line, out_line):
                return False
        else:
            if ref_line != out_line:
                return False

    return True

if __name__ == "__main__":
    if check():
        sys.exit(0)
    sys.exit(1)
