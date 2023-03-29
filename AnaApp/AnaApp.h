#pragma once
#ifdef _WINDOWS

#ifndef _ANA_APP
#define _ANA_APP __declspec(dllexport)
#else
#define _ANA_APP __declspec(dllimport)
#endif

#else
#define _ANA_APP
#endif