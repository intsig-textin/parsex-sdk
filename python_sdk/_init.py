# _init.py
import sys
import os

import ctypes
lib = ctypes.CDLL('textin_parser.cpython-36m-x86_64-linux-gnu.so')

current_dir = os.path.dirname(os.path.abspath(__file__))

sys.path.append(current_dir)

print(f"Added {current_dir} to PYTHONPATH")

print(f"Current dir {sys.path} of PYTHONPATH")

try:
    import textin_parser
    print("Successfully imported textin_parser")
except ImportError as e:
    print(f"Error importing textin_parser: {e}")
