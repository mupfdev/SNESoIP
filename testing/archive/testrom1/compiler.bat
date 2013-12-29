del testrom1.sfc
wla-65816.exe -vo testrom1.asm testrom1.o
wlalink.exe -vr testrom1.prj testrom1.sfc
del testrom1.o
pause
