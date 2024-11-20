@echo off
echo Cloning submodules
git submodule update --init --recursive
echo Building preprocessor
cd ./Tools/PAPIPreprocessor/
dotnet build --configuration Release
cd ../../Scripts/
./GenerateProjectsWindows.bat
