#!/bin/sh
echo Running preprocessor...
cd ../Tools/PAPIPreprocessor/bin/Release/net8.0/linux-x64/
./PAPIPreprocessor "../../../../../../" $1
