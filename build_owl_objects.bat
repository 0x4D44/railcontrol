@echo off
REM Build essential OWL framework objects for RailControl
echo Building missing OWL framework objects...
set path=c:\apps\bc5\bin;%path%

cd C:\Apps\owlnx630\source\owlcore

echo Compiling core OWL framework files...

REM Essential OWL objects that RailControl needs
bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -DOWL1_COMPAT -DOWL2_COMPAT -DOWL5_COMPAT -DBI_SUPPRESS_OLE -D_OLE2_H_ -D_OBJBASE_H_ -D_UNKNWN_H_ -D_OBJIDL_H_ -DNOOLE -DNOMINMAX -D_WINNETWK_ -DSTRICT -DWINVER=0x0400 -c applicat.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c framewin.cpp  
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c window.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c dialog.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c module.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c eventhan.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c dispatch.cpp  
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c exbase.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c except.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c dc.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c paintdc.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c brush.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c color.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c menu.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c button.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c checkbox.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c control.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c static.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c radiobut.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c system.cpp
if errorlevel 1 goto error

bcc32 -IC:\Apps\owlnx630\include -IC:\Apps\BC5\INCLUDE -H=owlcore.csm -i32 -W -w -D_OWLALLPCH -DBI_MULTI_THREAD_RTL -DBI_SUPPRESS_AUTOLINK -c thread.cpp
if errorlevel 1 goto error

echo Copying object files to RailControl directory...
if not exist ..\..\..\language\railcontrol\owlcore_objs mkdir ..\..\..\language\railcontrol\owlcore_objs
copy *.obj ..\..\..\language\railcontrol\owlcore_objs\
if errorlevel 1 goto error

echo Successfully built OWL framework objects!
goto end

:error
echo Failed to build OWL objects
exit /b 1

:end
echo Build complete.
