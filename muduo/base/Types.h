#ifndef _MUDUO_BASE_TYPES_H
#define _MUDUO_BASE_TYPES_H

#include<stdint.h>
#ifdef MUDUO_STD_STRING
#include<string>
#else
#include<ext/vstring.h>
#include<ext/vstring_fwd.h>
#endif

namespace muduo{
#ifdef MUDUO_STD_STRING
using std::string;
#else
typedef __gnu_cxx::__sso_string string;
#endif

template<typename To,typename From>
inline To implicit_cast(const From &F){
	return F;
}

template<typename To,typename From>
inline To down_cast(From* f){
  if (false) {
    implicit_cast<From*, To>(0);
  }
#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only
#endif
  return static_cast<To>(f);	
}
 
}
#endif
