#if !defined(OWL_GMODULE_H)
#define OWL_GMODULE_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_DECLCLASS(TModule);

#if defined(BI_COMP_BORLANDC) && !defined(BI_NEW_GMODULE)
extern __OWL TModule* Module;   // Global Module ptr in each DLL/EXE
#define gModule ::Module
#else
extern __OWL TModule* gModule;  // Global Module ptr in each DLL/EXE
#endif

#endif  // OWL_GMODULE_H
