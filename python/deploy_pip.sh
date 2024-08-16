#!/bin/bash

set -e

# PyPI API Token
API_TOKEN="pypi-AgEIcHlwaS5vcmcCJDIxYzUwMTNhLWYxOTItNGU3NC1iZWI2LTdiZWVkOTY1YWE2ZgACKlszLCI0NzViNGJkNC05YTRmLTRiNjktYmM1MS0wNWQ0NzA2YzU4MGIiXQAABiBTyPjZd52L8KGwqm_DLlhbAtFLcoQmwvbvHcfjOfDjnw"

echo "Cleaning up old builds..."
rm -rf dist/*

echo "Building the package..."
python3 setup.py sdist bdist_wheel
echo "Uploading the package to PyPI..."
twine upload dist/* -u __token__ -p "$API_TOKEN"

echo "Package successfully uploaded to PyPI!"
