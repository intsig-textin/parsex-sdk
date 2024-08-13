import subprocess

try:
    python3_6_installed = subprocess.check_output(["which", "python3.6"])
    python3_6_installed = python3_6_installed.decode('utf-8').strip() != ""
except subprocess.CalledProcessError:
    python3_6_installed = False

if python3_6_installed:
    print("Python 3.6 is installed.")
else:
    print("Python 3.6 is not installed.")

