@echo off

cd owlcore
call msc7owl.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

cd ocf
call msc7ocf.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..