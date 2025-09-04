@echo off

cd owlcore
call bcb6owl.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

cd ocf
call bcb6ocf.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..