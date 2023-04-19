#ifndef ANDRE_GLOBAL_H
#define ANDRE_GLOBAL_H

#ifdef _WIN32
    #ifdef ANDRE_LIBRARY
		#define ANDRESHARED_EXPORT __declspec(dllexport)
    #else
		#define ANDRESHARED_EXPORT __declspec(dllimport)
    #endif
#else
		#define ANDRESHARED_EXPORT __attribute__((visibility("default")))
#endif

#endif // ANDRE_GLOBAL_H
