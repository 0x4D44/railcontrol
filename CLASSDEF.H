/* CLASSDEF.H
 *  Updated for OWLNext 7.x / MSVC toolchain
 */
#ifndef _CLASSDEF_H_
#define _CLASSDEF_H_

// --- OWLNEXT CONFIGURATION ---
#ifndef OWL5_COMPAT
# define OWL5_COMPAT            // Enable OWL 5 compatibility mode for legacy APIs
#endif
#define BI_SUPPRESS_OLE         // Suppress OWL's OLE functionality to avoid conflicts

// --- WIN32 DEFINES ---
#define NOOLE                   // Disable OLE APIs
#ifndef NOMINMAX
# define NOMINMAX               // Disable min/max macros
#endif
#ifndef STRICT
# define STRICT
#endif
#ifndef WINVER
# define WINVER 0x0600
#endif
#ifndef _WIN32_WINNT
# define _WIN32_WINNT 0x0600
#endif
#define _WINNETWK_             // Exclude problematic network headers

// --- Other Global Project Defines ---
#define NOTOOLBUTTONS  1
#define NOSELECTORS    17
#define INIFILENAME    "RAILC.INI"
#define HELPFILENAME   "RAILC.HLP"
#define APPNAME        "Rail Control"

#include <owl/defs.h>

// --- OWL INCLUDES ---
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dialog.h>
#include <owl/window.h>
#include <owl/dc.h>

using namespace owl;

#ifdef GetClassName
# undef GetClassName
#endif
#ifndef HWindow
# define HWindow GetHandle()
#endif

// --- ADDITIONAL SYSTEM HEADERS ---
// Skip shellapi.h for now to avoid OLE2 conflicts
#include <ctl3d.h>             // ctl3d.dll library
#include <commdlg.h>
#include <mmsystem.h>

// --- STANDARD C/C++ LIBRARY HEADERS ---
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sstream>
#include <fstream>

#ifndef _CLASSDEF
# define _CLASSDEF(type) class type;
#endif

// --- PROJECT-SPECIFIC HEADERS ---
#include "general.h"    
#include "section.h"
#include "platdata.h"
#include "ovlpdata.h"
#include "routes.h"
#include "locos.h"
#include "timetabl.h"
#include "toolbutt.h"
#include "toolbar.h"
#include "statbar.h"
#include "selector.h"
#include "arrivals.h"
#include "departur.h"
#include "platform.h"
#include "locoyard.h"
#include "configur.h"
#include "layout.h"
#include "railc.h"
#include "finish.h"
#include "start.h"
#include "startup.h"
#include "about.h"

#endif // _CLASSDEF_H_
