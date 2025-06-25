/* CLASSDEF.H
*  Updated for Borland C++ 5.02 - Aggressive OLE2 exclusion
*/
#ifndef _CLASSDEF_H_
#define _CLASSDEF_H_

// --- AGGRESSIVE OLE2 EXCLUSION ---
#define _OLE2_H_               // Prevent ole2.h from being included
#define _OBJBASE_H_            // Prevent objbase.h from being included  
#define _UNKNWN_H_             // Prevent unknwn.h from being included
#define _OBJIDL_H_             // Prevent objidl.h from being included

// --- WIN32 DEFINES (Conservative for BCC 5.02) ---
#define NOOLE                   // Disable OLE APIs
#define NOMINMAX                // Disable min/max macros
#define _WINNETWK_             // Exclude problematic network headers
#define STRICT
#define WINVER 0x0400

// --- Other Global Project Defines ---
#define NOTOOLBUTTONS  1
#define NOSELECTORS    17
#define INIFILENAME    "RAILC.INI"
#define HELPFILENAME   "RAILC.HLP"
#define APPNAME        "Rail Control"

// --- OWL INCLUDES ---
#include <owl/compat.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dialog.h>
#include <owl/window.h>
#include <owl/dc.h>

// --- ADDITIONAL SYSTEM HEADERS ---
// Skip shellapi.h for now to avoid OLE2 conflicts
#include <ctl3d.h>             // ctl3d.dll library

// --- STANDARD C/C++ LIBRARY HEADERS ---
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <strstrea.h>

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
