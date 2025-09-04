@echo off

cd owlcore
call cb2009owl.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

cd ocf
call cb2009ocf.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..