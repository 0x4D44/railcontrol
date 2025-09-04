//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.1.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
//
//----------------------------------------------------------------------------

#if !defined(OWLFX_OWLFXLNKB2_H)
#define OWLFX_OWLFXLNKB2_H

#if defined(__BORLANDC__) && __BORLANDC__ < 0x530
# if __BORLANDC__ < 0x520
#  error Unsupported version
# else

#  if (__DEBUG > 0)
	  // Debug
#   if defined(INC_VCL)
     // VCL Multi-thread
#    if defined(_OWLFXDLL)
			// DLL import library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded Unicows OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dvusc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
		  // Debug VCL multi-threaded Unicows OWL2 compatibility import library
#       pragma comment(lib, "owlfx-630-b2-dvusc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug VCL multi-threaded Unicows OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dvusc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded Unicows import library 
#       pragma comment(lib, "owlfx-630-b2-dvusi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded Unicode OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dvuc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug VCL multi-threaded Unicode OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dvuc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug VCL multi-threaded Unicode OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dvuc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded Unicode import library 
#       pragma comment(lib, "owlfx-630-b2-dvui.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dvc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug VCL multi-threaded OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dvc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug VCL multi-threaded OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dvc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded import library 
#       pragma comment(lib, "owlfx-630-b2-dvi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // DLL import library end
#    else 	                    // #if defined(_OWLFXDLL)
			// static library
#     if defined(USE_UNICOWS)
	   // Unicows
#      if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded Unicows OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvusc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug VCL multi-threaded Unicows OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvusc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug VCL multi-threaded Unicows OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvusc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded Unicows static library 
#       pragma comment(lib, "owlfx-630-b2-dvus.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded Unicode OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvuc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug VCL multi-threaded Unicode OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvuc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug VCL multi-threaded Unicode OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvuc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded Unicode static library 
#       pragma comment(lib, "owlfx-630-b2-dvu.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug VCL multi-threaded OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dvc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug VCL multi-threaded static library 
#       pragma comment(lib, "owlfx-630-b2-dv.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // static library end
#    endif	                    // #if defined(_OWLFXDLL)	
     // VCL Multi-thread end
#   elif defined(__MT__)        //#if defined(INC_VCL)
	 // Multi-thread
#    if defined(_OWLFXDLL)
			// DLL import library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Debug multi-threaded Unicows OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtusc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug multi-threaded Unicows OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtusc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug multi-threaded Unicows OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtusc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug multi-threaded Unicows import library 
#       pragma comment(lib, "owlfx-630-b2-dtusi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Debug multi-threaded Unicode OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtuc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug multi-threaded Unicode OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtuc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug multi-threaded Unicode OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtuc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug multi-threaded Unicode import library 
#       pragma comment(lib, "owlfx-630-b2-dtui.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Debug multi-threaded OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug multi-threaded OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug multi-threaded OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dtc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug multi-threaded import library 
#       pragma comment(lib, "owlfx-630-b2-dti.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // DLL import library end
#    else 	                    // #if defined(_OWLFXDLL)
			// static library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Debug multi-threaded Unicows OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtusc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug multi-threaded Unicows OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtusc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug multi-threaded Unicows OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtusc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug multi-threaded Unicows static library 
#       pragma comment(lib, "owlfx-630-b2-dtus.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Debug multi-threaded Unicode OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtuc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug multi-threaded Unicode OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtuc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug multi-threaded Unicode OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtuc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug multi-threaded Unicode static library 
#       pragma comment(lib, "owlfx-630-b2-dtu.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Debug multi-threaded OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug multi-threaded OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug multi-threaded OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dtc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug multi-threaded static library 
#       pragma comment(lib, "owlfx-630-b2-dt.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // static library end
#    endif	                    // #if defined(_OWLFXDLL)	
     // Multi-thread end
#   else	                    // #if defined(INC_VCL)
     // Single-thread
#    if defined(_OWLFXDLL)
			// DLL import library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Debug single-threaded Unicows OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dsusc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
		  // Debug single-threaded Unicows OWL2 compatibility import library
#       pragma comment(lib, "owlfx-630-b2-dsusc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug single-threaded Unicows OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dsusc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug single-threaded Unicows import library 
#       pragma comment(lib, "owlfx-630-b2-dsusi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Debug single-threaded Unicode OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dsuc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug single-threaded Unicode OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dsuc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug single-threaded Unicode OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dsuc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug single-threaded Unicode import library 
#       pragma comment(lib, "owlfx-630-b2-dsui.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Debug single-threaded OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dsc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug single-threaded OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dsc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug single-threaded OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-dsc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug single-threaded import library 
#       pragma comment(lib, "owlfx-630-b2-dsi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // DLL import library end
#    else 	                    // #if defined(_OWLFXDLL)
			// static library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Debug single-threaded Unicows OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsusc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug single-threaded Unicows OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsusc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug single-threaded Unicows OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsusc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug single-threaded Unicows static library 
#       pragma comment(lib, "owlfx-630-b2-dsus.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Debug single-threaded Unicode OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsuc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug single-threaded Unicode OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsuc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug single-threaded Unicode OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsuc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug single-threaded Unicode static library 
#       pragma comment(lib, "owlfx-630-b2-dsu.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Debug single-threaded OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Debug single-threaded OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Debug single-threaded OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-dsc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Debug single-threaded static library 
#       pragma comment(lib, "owlfx-630-b2-ds.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // static library end
#    endif	                    // #if defined(_OWLFXDLL)	
     // Single-thread end
#   endif                     // #if defined(INC_VCL) 
    // Debug end
#  else                     // #if (__DEBUG > 0)
	  // Release
#   if defined(INC_VCL)
     // VCL Multi-thread
#    if defined(_OWLFXDLL)
			// DLL import library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded Unicows OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vusc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release VCL multi-threaded Unicows OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vusc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release VCL multi-threaded Unicows OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vusc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded Unicows import library 
#       pragma comment(lib, "owlfx-630-b2-vusi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded Unicode OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vuc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release VCL multi-threaded Unicode OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vuc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release VCL multi-threaded Unicode OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vuc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded Unicode import library 
#       pragma comment(lib, "owlfx-630-b2-vui.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release VCL multi-threaded OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release VCL multi-threaded OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-vc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded import library 
#       pragma comment(lib, "owlfx-630-b2-vi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // DLL import library end
#    else 	                    // #if defined(_OWLFXDLL)
			// static library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded Unicows OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vusc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release VCL multi-threaded Unicows OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vusc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release VCL multi-threaded Unicows OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vusc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded Unicows static library 
#       pragma comment(lib, "owlfx-630-b2-vus.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded Unicode OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vuc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release VCL multi-threaded Unicode OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vuc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release VCL multi-threaded Unicode OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vuc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded Unicode static library 
#       pragma comment(lib, "owlfx-630-b2-vu.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release VCL multi-threaded OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-vc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release VCL multi-threaded static library 
#       pragma comment(lib, "owlfx-630-b2-v.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // static library end
#    endif	                    // #if defined(_OWLFXDLL)	
     // VCL Multi-thread end
#   elif defined(__MT__)        //#if defined(INC_VCL)
     // Multi-thread
#    if defined(_OWLFXDLL)
			// DLL import library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Release multi-threaded Unicows OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tusc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release multi-threaded Unicows OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tusc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release multi-threaded Unicows OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tusc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release multi-threaded Unicows import library 
#       pragma comment(lib, "owlfx-630-b2-tusi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Release multi-threaded Unicode OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tuc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release multi-threaded Unicode OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tuc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release multi-threaded Unicode OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tuc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release multi-threaded Unicode import library 
#       pragma comment(lib, "owlfx-630-b2-tui.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Release multi-threaded OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release multi-threaded OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release multi-threaded OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-tc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release multi-threaded import library 
#       pragma comment(lib, "owlfx-630-b2-ti.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // DLL import library end
#    else 	                    // #if defined(_OWLFXDLL)
			// static library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Release multi-threaded Unicows OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tusc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release multi-threaded Unicows OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tusc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release multi-threaded Unicows OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tusc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release multi-threaded Unicows static library 
#       pragma comment(lib, "owlfx-630-b2-tus.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Release multi-threaded Unicode OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tuc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release multi-threaded Unicode OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tuc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release multi-threaded Unicode OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tuc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release multi-threaded Unicode static library 
#       pragma comment(lib, "owlfx-630-b2-tu.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Release multi-threaded OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release multi-threaded OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release multi-threaded OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-tc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release multi-threaded static library 
#       pragma comment(lib, "owlfx-630-b2-t.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // static library end
#    endif	                    // #if defined(_OWLFXDLL)	
     // Multi-thread end
#   else	                    // #if defined(INC_VCL)
     // Single-thread
#    if defined(_OWLFXDLL)
			// DLL import library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Release single-threaded Unicows OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-susc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release single-threaded Unicows OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-susc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release single-threaded Unicows OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-susc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release single-threaded Unicows import library 
#       pragma comment(lib, "owlfx-630-b2-susi.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Release single-threaded Unicode OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-suc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release single-threaded Unicode OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-suc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release single-threaded Unicode OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-suc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release single-threaded Unicode import library 
#       pragma comment(lib, "owlfx-630-b2-sui.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Release single-threaded OWL1 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-sc1i.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release single-threaded OWL2 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-sc2i.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release single-threaded OWL5 compatibility import library 
#       pragma comment(lib, "owlfx-630-b2-sc5i.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release single-threaded import library 
#       pragma comment(lib, "owlfx-630-b2-si.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Single-bye end
#     endif                       //#if defined(UNICODE)
      // DLL import library end
#    else 	                    // #if defined(_OWLFXDLL)
			// static library
#     if defined(USE_UNICOWS)
       // Unicows
#      if defined(OWL1_COMPAT)
	      // Release single-threaded Unicows OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-susc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release single-threaded Unicows OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-susc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release single-threaded Unicows OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-susc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release single-threaded Unicows static library 
#       pragma comment(lib, "owlfx-630-b2-sus.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicows end
#     elif defined(UNICODE)   //#if defined(USE_UNICOWS)
       // Unicode
#      if defined(OWL1_COMPAT)
	      // Release single-threaded Unicode OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-suc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
		  // Release single-threaded Unicode OWL2 compatibility static library
#       pragma comment(lib, "owlfx-630-b2-suc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release single-threaded Unicode OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-suc5.lib")
#      else                         //if defined(OWL1_COMPAT)
	      // Release single-threaded Unicode static library 
#       pragma comment(lib, "owlfx-630-b2-su.lib")
#      endif		                    //if defined(OWL1_COMPAT)
       // Unicode end
#     else                        //#if defined(USE_UNICOWS)
       // Single-byte
#      if defined(OWL1_COMPAT)
	      // Release single-threaded OWL1 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-sc1.lib")
#      elif defined(OWL2_COMPAT)   	//if defined(OWL1_COMPAT)		
	      // Release single-threaded OWL2 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-sc2.lib")
#      elif defined(OWL5_COMPAT)	 	//if defined(OWL1_COMPAT)	
	      // Release single-threaded OWL5 compatibility static library 
#       pragma comment(lib, "owlfx-630-b2-sc5.lib")
#      else                         //if defined(OWL1_COMPAT)
		  // Release single-threaded static library
#       pragma comment(lib, "owlfx-630-b2-s.lib")
#      endif		                    //if defined(OWL1_COMPAT)
	   // Single-bye end
#     endif                       //#if defined(UNICODE)
	  // static library end
#    endif	                    // #if defined(_OWLFXDLL)
	 // Single-thread end
#   endif                     // #if defined(INC_VCL)
	// Release end
#  endif	                  // #if (__DEBUG > 0)

# endif

#else   //#if defined(__BORLANDC__) && __BORLANDC__ < 0x530

#  error this file must be included only when building with Borland C++ 5.02

#endif  //#if defined(__BORLANDC__) && __BORLANDC__ < 0x530

#endif // OWLFX_OWLFXLNKB2_H
