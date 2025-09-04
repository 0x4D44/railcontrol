@echo off

cd owlcore
call bcc55owl.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

cd ocf
call bcc55ocf.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..