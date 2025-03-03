#!/bin/bash
SDK_VERSION=$(grep 'sdkVersion' version.properties | cut -d'=' -f2)

./gradlew clean
./gradlew bundleForUpload
rm -rf package
mkdir -p package/io/github/supperai/parse_sdk
cp -r build/repo/io/github/supperai/parse_sdk/* package/io/github/supperai/parse_sdk/

rm -rf package/io/github/supperai/parse_sdk/maven-metadata*
rm -rf package/io/github/supperai/parse_sdk/${SDK_VERSION}/*.asc.*
rm -rf package/io/github/supperai/parse_sdk/${SDK_VERSION}/*.sha256
rm -rf package/io/github/supperai/parse_sdk/${SDK_VERSION}/*.sha512

cd package
zip -r parse_sdk_${SDK_VERSION}.zip io # 压缩包包含版本号