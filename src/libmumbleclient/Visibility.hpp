#ifndef _LIBMUMBLECLIENT_VISIBILITY_H_
#define _LIBMUMBLECLIENT_VISIBILITY_H_

// Linux, use visibility __attribute__ if there for both static and dynamic
#if __GNUC__ >= 4
        #define DLL_PUBLIC __attribute__ ((visibility("default")))
        #define DLL_LOCAL  __attribute__ ((visibility("hidden")))
    #else
        #define DLL_PUBLIC
        #define DLL_LOCAL
    #endif
#endif

