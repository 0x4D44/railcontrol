/* CLASSDEF.H
*  ==========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 01/11/1994
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

/* Header file for Class definitions */

//#define WIN31
//#define STRICT
#define NOTOOLBUTTONS  1
#define NOSELECTORS    17
#define INIFILENAME    "RAILC.INI"
#define HELPFILENAME   "RAILC.HLP"
#define APPNAME        "Rail Control"

#include <owl\applicat.h>
#include <owl\framewin.h>
#include <owl\dialog.h>
#include <owl\dc.h>
#include <owl\gdiobjec.h>
#include <owl\checkbox.h>
#include <owl\radiobut.h>
//#include <owl\owlpch.h>
#include <alloc.h>
#include <ctl3d.h>
#include <dos.h>
#include <stdlib.h>
#include <strstrea.h>
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <commdlg.h>
#include <time.h>

#include "section.h"
#include "platdata.h"
#include "ovlpdata.h"
#include "routes.h"
#include "locos.h"
#include "timetabl.h"
#include "general.h"
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


