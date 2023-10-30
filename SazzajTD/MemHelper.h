#ifndef MEM_HELPER_H
#define MEM_HELPER_H

#ifdef _DEBUG
    #define snew new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
    #define snew new
#endif

#endif