@echo off

cd owlcore
call msc8owl.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..

cd ocf
call msc8ocf.bat %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..