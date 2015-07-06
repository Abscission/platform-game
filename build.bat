@echo off

setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64_x86\vcvarsamd64_x86.bat"
set PATH=%PATH%;C:\Program Files (x86)\Git\cmd;C:\Program Files (x86)\Git\bin;C:\Program Files\7-Zip;

set buildnumber=%1

echo Starting build %buildnumber%

if exist build rmdir /s /q build
mkdir build

echo Building 32 bit
mkdir build\x86
msbuild .\platform-game\platform-game\platform-game.vcxproj /m /p:Configuration=Release /p:Platform=win32 > .\build\x86\build.log
copy platform-game\Release\platform-game.exe build\x86
copy lib\x86\* build\x86 > NUL

echo Building 64 bit
mkdir build\x64
msbuild .\platform-game\platform-game\platform-game.vcxproj /m /p:Configuration=Release /p:Platform=x64 > .\build\x64\build.log
copy platform-game\x64\Release\platform-game.exe build\x64
copy lib\x64\* build\x64 > NUL

pushd build
cd x86
7z a platform-game-32-%buildnumber%.zip *
cd ..
cd x64
7z a platform-game-64-%buildnumber%.zip *
cd ..
popd

endlocal