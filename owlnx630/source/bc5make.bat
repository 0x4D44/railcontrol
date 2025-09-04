@echo off

cd owlcore
call bc5owl.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

cd ocf
call bc5ocf.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..