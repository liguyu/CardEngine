@echo off

set mode=
set /p mode=Please choose deploy mode(1-Debug 2-Release):
set dirName=
if "%mode%"=="1" (set dirName=Debug) else (set dirName=Release) 

set dir=.\CardEngine_%date:~0,2%%date:~3,2%%date:~6,4%
echo %dir%
md %dir%

copy /y ..\%dirName%\CardEngine.dll          %dir%
copy /y ..\%dirName%\CPUUserCard.dll         %dir%
copy /y ..\%dirName%\Commons.dll             %dir%
copy /y ..\%dirName%\Decard.dll              %dir%
copy /y ..\%dirName%\dcrf32.dll              %dir%
copy /y ..\%dirName%\PsamCard.dll            %dir%
copy /y ..\%dirName%\CardEngine.lib          %dir%
copy /y ..\%dirName%\log4cplus.properties    %dir%
copy /y ..\%dirName%\RedCard.properties      %dir%
copy /y ..\CardEngine\CardEngine.h           %dir%
copy /y ..\Commons\Header.h                  %dir%

if "%mode%"=="1" (copy /y ..\%dirName%\log4cplusUD.dll  %dir%) else (copy /y ..\%dirName%\log4cplusU.dll  %dir%) 

pause

