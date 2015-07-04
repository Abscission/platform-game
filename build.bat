@echo off

setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64_x86\vcvarsamd64_x86.bat"
set PATH=%PATH%;C:\Program Files (x86)\Git\cmd;C:\Program Files (x86)\Git\bin;C:\Program Files\7-Zip;

set /p buildnumber=<BuildNumber.txt
set /a next="buildnumber+1"
echo %next% > BuildNumber.txt

echo Starting build %buildnumber%

git pull

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

echo Building Assets
if not exist ..\platform-game-art-assets git clone https://github.com/rilwal/platform-game-art-assets ..\platform-game-art-assets
pushd ..\platform-game-art-assets
git pull
call build.bat
popd

mkdir build\x64\assets
copy ..\platform-game-art-assets\build\* build\x64\assets > NUL

mkdir build\x86\assets
copy ..\platform-game-art-assets\build\* build\x86\assets > NUL

echo Packaging build
set file_32=platform-game-x86-%buildnumber%.zip
set file_64=platform-game-x64-%buildnumber%.zip

set upload_dest=root@rilwal.com:/srv/www/fs.abscission.net/builds

pushd build
cd x86
7z a %file_32% *
copy %file_32% Z:\
scp %file_32% %upload_dest%
cd ..
cd x64
7z a %file_64% *
copy %file_64% Z:\
scp %file_64% %upload_dest%
cd ..
popd

sh notify.sh "A new build is available! Get it at:\n32-bit: http://fs.abscission.net/builds/%file_32%\n64-bit: http://fs.abscission.net/builds/%file_64%"

endlocal