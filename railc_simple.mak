#
# Simplified Borland C++ 5.02 makefile for railcontrol
#
.AUTODEPEND

# Borland C++ tools
BCC32   = C:\Apps\BC5\bin\Bcc32 +BccW32.cfg
TLINK32 = C:\Apps\BC5\bin\TLink32
BRC32   = C:\Apps\BC5\bin\Brc32

# Object files
OBJS = railc.obj departur.obj locoyard.obj arrivals.obj finish.obj about.obj startup.obj start.obj configur.obj layout.obj selector.obj platform.obj toolbar.obj toolbutt.obj statbar.obj section.obj platdata.obj ovlpdata.obj routes.obj locos.obj timetabl.obj

# Main target
railc.exe: $(OBJS) resource\railc.res
	$(TLINK32) -Tpe -aa -c -LC:\Apps\BC5\LIB @&&|
C:\Apps\BC5\LIB\c0w32.obj $(OBJS)
railc.exe

C:\Apps\BC5\lib\ctl3d32.lib+C:\Apps\BC5\lib\owlwf.lib+C:\Apps\BC5\lib\bidsf.lib+C:\Apps\BC5\lib\import32.lib+C:\Apps\BC5\lib\cw32.lib
railc.def
resource\railc.res
|

# Generic rule for .cpp files
.cpp.obj:
	$(BCC32) -c $<

# Resource file
resource\railc.res: resource\railc.rc
	$(BRC32) -IC:\Apps\BC5\INCLUDE -w32 -R -FO$@ resource\railc.rc

# Clean target
clean:
	del *.obj
	del railc.exe
	del resource\railc.res
