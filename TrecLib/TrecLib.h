#pragma once



#ifdef _WINDOWS

#ifndef _TREC_LIB_DLL
#define _TREC_LIB_DLL __declspec(dllexport)
#else
#define _TREC_LIB_DLL __declspec(dllimport)
#endif

#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
#define _TREC_LIB_DLL

#endif
