#
# documentation/makefile - Generates the documentation by calling Doxygen.
# Copyright (c) 2014 Vidar Hasfjord
# See OWLNext for license information (http://owlnext.sourceforge.net).
#
# Usage: nmake [targets]
#
# The following targets are available:
#
# html - Builds HTML documentation for online use.
# html-help - Builds HTML Help documentation for offline use.
# all - Builds both "html" and "html-help" targets.
# clean - Removes all output.

#-------------------------------------------------
# Parse the OWLNext version number from the source.

!if [..\source\parse_version.bat OWL ..\source\owlcore\version.h >"$(TEMP)\OWL_VERSION.tmp"]
! error Unable to detect the OWLNext version number.
!endif

!include "$(TEMP)\OWL_VERSION.tmp"

!if "$(OWL_VERSION)" == ""
! error OWL_VERSION is undefined (parsing failed).
!endif

!message OWL_VERSION = $(OWL_VERSION)

#-------------------------------------------------
# Targets

OUTPUT = html html-help

all:: $(OUTPUT)

$(OUTPUT)::
  @echo.
  @set OWL_VERSION=$(OWL_VERSION)
  doxygen $@.doxyfile

clean::
  -@del *.tmp 2>nul
  -@for %d in ($(OUTPUT)) do @if exist %d echo Removing %d... & rmdir /Q /S %d 2>nul
