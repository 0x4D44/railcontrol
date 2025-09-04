//----------------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
// Include for OWLFX Extensions, gets core.h or all.h when precompiled
// headers are enabled.
//----------------------------------------------------------------------------
# include <owl/pch.h>
# include <owl/owlcore.h>

#if defined(_OWLFXPCH) && !defined(OWLFX_CORE_H)
#	include <owlfx/core.h>
#endif
#if defined(_OWLFXALLPCH) && !defined(OWLFX_ALL_H)
#	include <owlfx/all.h>
#endif

