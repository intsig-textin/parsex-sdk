#!/bin/bash

set -e

# PyPI API Token
API_TOKEN="pypi-AgEIcHlwaS5vcmcCJDc0ZjQ1MDA3LWE1YTMtNDIwNy1hZTI0LTYxMjczNjM4NmI3MQACHVsxLFsidGV4dC1pbi1wYXJzZXItZW5naW5lIl1dAAIsWzIsWyI3NzAxZjA0OC0yNzY2LTRhZDgtODgyZS0wNmMxOTRhYjYzNjAiXV0AAAYghDQVolZDka1rvT7KbK11yRzsowbHfT1abgSB5L3RTrs"

echo "Cleaning up old builds..."
rm -rf dist/*

echo "Building the package..."
python3 setup.py sdist bdist_wheel
echo "Uploading the package to PyPI..."
twine upload dist/* -u __token__ -p "$API_TOKEN"

echo "Package successfully uploaded to PyPI!"
