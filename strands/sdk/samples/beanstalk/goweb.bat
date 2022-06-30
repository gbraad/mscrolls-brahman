xcopy /E /I /Y ..\..\bin\web web
copy /Y *.str web
copy /Y images\* web\images
copy /Y audio\* web\audio
start http://localhost:8080/index.html?story=beanstalk.str
..\..\bin\civetweb -document_root web

