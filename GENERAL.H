/* GENERAL.H
*  =========
*
*  PROGRAM DESCRIPTION
*  ===================
*  This header file is responsible for general (non-module specific) symbol
*  definitions. It contains menu item definitions and general train associated
*  definitions which are mainly used in TLayout (although not exclusively).
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 01/11/1994
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#ifndef GENERAL_H_INCLUDED
#define GENERAL_H_INCLUDED

#include <cstddef>
#include <cstdio>
#include <cstring>

// Main menu definitions
#define CM_MNUFILNEW       100
#define CM_MNUFIPAUS       101
#define CM_MNUFISTOP       102
#define CM_MNUFISETD       103
#define CM_MNUFIEXIT       110
#define CM_OPTOPTIMI       200
#define CM_OPTCONFIG       201
#define CM_WINARRIVA       300
#define CM_WINDEPART       301
#define CM_WINPLATFO       302
#define CM_WINLOCOYA       303
#define CM_MNUHECTNT       900
#define CM_MNUHEHELP       901
#define CM_MNUHEABOT       999


// Other definitions
#define CM_SELECT          500
#define ID_HELP_BUTTON     998
#define ID_TIMER           2100

// Resource definitions
#define HR_DIGITAL         800

// Configuration dialog box definitions
#define ID_CONFSLOWS       3000
#define ID_CONFNORMA       3001
#define ID_CONFFASTS       3002
#define ID_CONFDELAY       3003
#define ID_CONFREFUE       3004
#define ID_CONFSEXIT       3005
#define ID_CONFAUOPT       3006
#define ID_CONFSOUND       3007

// Finish dialog box definitions
#define ID_FINIACHIE       1200
#define ID_FINIARRDE       1201
#define ID_FINIDEPDE       1202
#define ID_FINIRATIN       1203

// Start dialog box definitions
#define ID_STRTTEXT1       1300
#define ID_STRTTEXT2       1301
#define ID_STRTTEXT3       1302

// Delay flag definitions
#define DLF_NODELAYS           0
#define DLF_DELAYS             1
#define DLF_DELAYS_MAINT       2
#define DLF_DELAYS_THRU        3
#define DLF_ONLYLATE           4
#define DLF_ONLYLATE_THRU      5

// Selector types
#define SEL_INPUT              1
#define SEL_OUTPUT             2
#define SEL_PLAT               3
#define SEL_ELECTRIC_PLAT      4
#define SEL_HOLD               5
#define SEL_LOCOYARD           6


// Status defines used in tracking trains
// The following codes are used:
//
//  1: Due
//  2: Approaching
//  3: Held
//  4: First held    
//  5: Set to platform
//  6: Stage A of approach
//  7: Stage B of approach
//  8: Stage C of approach
//  9: Stage D of approach
// 10: Stage E of approach
// 11: Stage F of approach
// 12: In platform
// 13: Stock released and needing new loco
// 14: Stock ready to depart
// 15: Ready to depart (ie loco attached and 5min to DepTime)
// 16: Starting to depart
// 17: Stage A of departure
// 18: Stage B of departure
// 19: Stage C of departure
// 20: Stage D of departure
// 21: Stage E of departure
// 22: Stage F of departure
// 30: Twin unit departure - second unit 

#define ST_NONE        0
#define ST_DUE         1
#define ST_APPROACH    2
#define ST_HELD        3
#define ST_FIRSTHELD   4
#define ST_SETPLAT     5
#define ST_ARRA        6
#define ST_ARRB        7
#define ST_ARRC        8
#define ST_ARRD        9
#define ST_ARRE       10
#define ST_ARRF       11
#define ST_INPLAT     12
#define ST_RELEASE    13
#define ST_STOCKOK    14
#define ST_READYDEP   15
#define ST_STARTDEP   16
#define ST_DEPA       17
#define ST_DEPB       18
#define ST_DEPC       19
#define ST_DEPD       20
#define ST_DEPE       21
#define ST_DEPF       22

#define ST_TWINASSOC  30


// Define locomotive flags (used in <locoinfo> struct)
#define LF_UNASSIGN 1                // Not assigned to anything
#define LF_ASSIGNED 2                // Assigned to a train
#define LF_INPLAT   3                // Unassigned in a platform
#define LF_MAINTAIN 4                // Needs maintainance
#define LF_NEEDFUEL 5                // Loco requires to be refuelled
#define LF_REFUEL   6                // Loco is refuelling in locoyard
#define LF_LOCOYARD 7                // In locoyard
#define LF_LIGHT    8                // Light engine movement

// Number of arrival / departure entries to display
#define DISP_NUM_ARRIVAL   8
#define DISP_NUM_DEPART    8

// Define locomotive stock codes
// The following stock codes are defined:
//
// 1:  ECS stock             - Class 31
// 2:  HST set               - no associated engine
// 3:  EMU set               - no associated locomotive
// 4:  Deltic express        - Class 55
// 5:  Standard passenger    - Class 40/45/46/47
// 6:  Relief passenger      - Class 40/45/46/47
// 7:  Light engine for ECS  - Class 31
// 8:  Light normal engine   - Class 40/45/46/47
// 9:  Light Deltic engine   - Class 55
// 10: Push/pull train       - Class 47/7 - locked to set
// 11: DMU set               - no associated engine (old 101/107 types)
// 12: Class 37 loco         - Class 37/4
// 13: Light relief loco     - Class 40/45/46/47
// 14: Light class 37 loco   - Class 37/4
// 15: Heavy freight loco    - Class 60/66
// 16: Class 156 DMU         - Class 156
// 17: Class 158 DMU         - Class 158
// 18: Class 170 DMU         - Class 170
// 19: Twin 156 DMU          - Twin class 156
// 20: Twin 158 DMU          - Twin class 158
// 21: Twin 170 DMU          - Twin class 170
// 22: Class 150 DMU         - no associated engine (class 150 Sprinter)

#define SC_ECS           1
#define SC_HST           2
#define SC_EMU           3
#define SC_DELTIC        4
#define SC_NORMAL        5
#define SC_RELIEF        6
#define SC_LIGHTECS      7
#define SC_LIGHTNORM     8
#define SC_LIGHTDELT     9
#define SC_PUSHPULL      10
#define SC_DMU1          11
#define SC_CLASS37       12
#define SC_LIGHTRLF      13
#define SC_LIGHT37       14
#define SC_HEAVYFREIGHT  15
#define SC_DMU156        16
#define SC_DMU158        17
#define SC_DMU170        18
#define SC_TWIN156       19
#define SC_TWIN158       20
#define SC_TWIN170       21
#define SC_DMU150        22

/****************************************************************************/
/* Debugging information.                                                   */
/****************************************************************************/
//#define MDDEBUG
#ifdef MDDEBUG
#define TB mdDebugStringBuffer, sizeof(mdDebugStringBuffer)
#define TRC_ERR(X)  TRC_INT(X)
#define TRC_ALT(X)  TRC_INT(X)
#define TRC_NRM(X)  TRC_INT(X)
#define TRC_DBG(X)  TRC_INT(X)
#define TRC_INT(X)                                                           \
        {                                                                    \
           char mdDebugStringBuffer[256];                                    \
           const int trcWritten = std::snprintf X;                           \
           if (trcWritten < 0 ||                                             \
               trcWritten >= static_cast<int>(sizeof(mdDebugStringBuffer)))  \
           {                                                                 \
             mdDebugStringBuffer[sizeof(mdDebugStringBuffer) - 1] = '\0';    \
           }                                                                 \
           OutputDebugString(mdDebugStringBuffer);                           \
        }                                                                    
#else
#define TRC_ERR(X)
#define TRC_ALT(X)
#define TRC_NRM(X)
#define TRC_DBG(X)
#endif

template <std::size_t BufferSize, typename... Args>
inline void FormatBuffer(char (&buffer)[BufferSize], const char* format, Args... args)
{
  static_assert(BufferSize > 0, "FormatBuffer requires a non-empty buffer");
  const int written = std::snprintf(buffer, BufferSize, format, args...);
  if (written < 0 || static_cast<std::size_t>(written) >= BufferSize)
  {
    buffer[BufferSize - 1] = '\0';
  }
}

template <std::size_t BufferSize>
inline void CopyString(char (&buffer)[BufferSize], const char* value)
{
  FormatBuffer(buffer, "%s", value ? value : "");
}

inline void CopyString(char* buffer, std::size_t bufferSize, const char* value)
{
  if (!buffer || bufferSize == 0)
  {
    return;
  }

  const char* source = value ? value : "";
  const int written = std::snprintf(buffer, bufferSize, "%s", source);
  if (written < 0 || static_cast<std::size_t>(written) >= bufferSize)
  {
    buffer[bufferSize - 1] = '\0';
  }
}

inline void CopyString(char* buffer, const char* value)
{
  if (!buffer)
  {
    return;
  }

#if defined(_DEBUG)
  if (::IsBadWritePtr(buffer, 1))
  {
    ::DebugBreak();
  }
#endif
  const char* source = value ? value : "";
  std::strcpy(buffer, source);
}

template <std::size_t BufferSize>
inline void AppendString(char (&buffer)[BufferSize], const char* value)
{
  if (!value)
  {
    return;
  }

  const std::size_t currentLength = std::strlen(buffer);
  if (currentLength >= BufferSize - 1)
  {
    return;
  }

  const int written = std::snprintf(buffer + currentLength,
                                    BufferSize - currentLength,
                                    "%s",
                                    value);
  if (written < 0)
  {
    buffer[BufferSize - 1] = '\0';
  }
}

template <std::size_t BufferSize, typename... Args>
inline void AppendFormat(char (&buffer)[BufferSize], const char* format, Args... args)
{
  const std::size_t currentLength = std::strlen(buffer);
  if (currentLength >= BufferSize - 1)
  {
    return;
  }

  const int written = std::snprintf(buffer + currentLength,
                                    BufferSize - currentLength,
                                    format,
                                    args...);
  if (written < 0)
  {
    buffer[BufferSize - 1] = '\0';
  }
}

/****************************************************************************/
/* Utility functions.                                                       */
/****************************************************************************/
int checkisspace(char c);
char* lTrim(char* p);
char* rTrim(char* p);
char* Trim(char* p);

#endif // GENERAL_H_INCLUDED





