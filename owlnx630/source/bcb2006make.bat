@echo off

cd owlcore
call bcb2006owl.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

cd ocf
call bcb2006ocf.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..