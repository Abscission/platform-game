@echo off

setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64_x86\vcvarsamd64_x86.bat"
set PATH=%PATH%;C:\Program Files (x86)\Git\cmd;C:\Program Files (x86)\Git\bin;C:\Program Files\7-Zip;

git pull

if exist build rmdir /s /q build
mkdir build

echo Building 32 bit
msbuild .\platform-game\platform-game\platform-game.vcxproj /m /p:Configuration=Release /p:Platform=win32
mkdir build\x86
copy platform-game\Release\platform-game.exe build\x86
copy lib\x86\* build\x86

echo Building 64 bit
msbuild .\platform-game\platform-game\platform-game.vcxproj /m /p:Configuration=Release /p:Platform=x64
mkdir build\x64
copy platform-game\x64\Release\platform-game.exe build\x64
copy lib\x64\* build\x64

echo Building Assets
if not exist ..\platform-game-art-assets git clone https://github.com/rilwal/platform-game-art-assets ..\platform-game-art-assets
pushd ..\platform-game-art-assets
git pull
call build.bat
popd

mkdir build\x64\assets
copy ..\platform-game-art-assets\build\* build\x64\assets

mkdir build\x86\assets
copy ..\platform-game-art-assets\build\* build\x86\assets

echo Packaging build

@For /F "tokens=2,3,4 delims=/ " %%A in ('Date /t') do @( 
	Set MM=%%A
	Set DD=%%B
	Set YYYY=%%C
)

pushd build
cd x86
7z a platform-game-x86-%YYYY%%MM%%DD%.zip *
copy platform-game-x86-%YYYY%%MM%%DD%.zip Z:\
cd ..
cd x64
7z a platform-game-x64-%YYYY%%MM%%DD%.zip *
copy platform-game-x64-%YYYY%%MM%%DD%.zip Z:\
cd ..

popd

endlocal