Remote control messages

Run parameter:
[%address%:%port%] - start server on IPv4 address %address% and port %port%. E.g.: 127.0.0.1:1025. If not specified, use default 0.0.0.0:0 - all IPv4 network interfaces and port is chosen automatically).

Create text file %AppName%.cfg (default RemoteMessage.cfg) with rules line by line:
%Name%|%RunApplication%

E.g.:
Calc|Calc.exe
MyMprogram|"C:\My Programs\Show.exe" /alert

Also to show screenshot of desktop go to link:
%address%:%port%/screen/%n% - show screenshot. If n between 1 and 100 take JPG screenshot, 1 - low quality, 100 - best quality, else take PNG screenshot
E.g.:
192.168.1.2:3456/screen/75
10.20.30.40:10000/screen/png
