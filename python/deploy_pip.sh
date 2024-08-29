#!/bin/bash

set -e

# PyPI API Token
API_TOKEN="if you want to upload use your PyPI API Token not mine"

echo "Cleaning up old builds..."
rm -rf dist/*

echo "Building the package..."
python3 setup.py sdist bdist_wheel
echo "Uploading the package to PyPI..."
python3 -m twine upload dist/* -u __token__ -p "$API_TOKEN"

echo "Package successfully uploaded to PyPI!"
