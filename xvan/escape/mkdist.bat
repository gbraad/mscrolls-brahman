rm -rf dist
mkdir dist
cp ../release/ifigame.dll dist
cp ../../app/release/*.dll dist
cp ../../app/release/klif.exe dist/escape.exe
mkdir dist\assets
cp escape.dat dist/assets
cp -r images dist/assets
cp -r icons dist/assets



