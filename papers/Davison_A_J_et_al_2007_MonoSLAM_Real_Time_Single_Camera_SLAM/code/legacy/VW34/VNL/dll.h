#ifndef vnl_dll_h_
#define vnl_dll_h_
// This is vxl/vnl/dll.h



#define 

#if defined(_WIN32) && !defined(BUILDING_VNL_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef 
#  define  _declspec(dllimport)
# endif // BUILD_DLL
#endif // _WIN32 and !Building_*_dll

#endif // vnl_dll_h_
