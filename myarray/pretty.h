#pragma once

#ifdef _MSC_VER 
    #define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
  
#define CHECK_BOUNDS(idx, size) { \
  if ((idx) >= (size))          \
  throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": index " + std::to_string(idx) + " is out of bound"); \
}
