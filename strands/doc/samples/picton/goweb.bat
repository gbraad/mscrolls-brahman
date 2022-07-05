rem https://github.com/civetweb/civetweb/blob/master/docs/UserManual.md
set SDK=..\..\..\sdk
xcopy /E /I /Y %SDK%\bin\web web
copy /Y b:\strands\imgui\web\index.js web
copy /Y b:\strands\imgui\web\index.wasm web
copy /Y b:\strands\imgui\web\index.html web
copy /Y *.str web
copy /Y images\* web\images
copy /Y audio\* web\audio
start http://localhost/index.html
%SDK%\bin\civetweb -listening_ports 80 -document_root web

