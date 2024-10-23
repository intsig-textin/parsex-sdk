#!/bin/bash
# ./gradlew clean
# ./gradlew bundleForUpload
rm -rf package
mkdir -p package/io/github/supperai/parse_sdk
cp -r build/repo/io/github/supperai/parse_sdk/* package/io/github/supperai/parse_sdk/

rm -rf package/io/github/supperai/parse_sdk/maven-metadata*
rm -rf package/io/github/supperai/parse_sdk/1.0.2/*.asc.*
rm -rf package/io/github/supperai/parse_sdk/1.0.2/*.sha256
rm -rf package/io/github/supperai/parse_sdk/1.0.2/*.sha512

cd package
zip -r parse_sdk.zip io