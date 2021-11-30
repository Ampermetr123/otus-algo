/**
 *  Опредление интерфейса для массивов 
 */

#pragma once

namespace myarray {

  template<typename T>
  class IArray {
  public:
    virtual ~IArray() = default;
    virtual size_t count() const = 0;
    virtual void add(const T& it) = 0;
    //virtual void add(T&& it) = 0;
    virtual void removeLast() = 0;
    virtual T& get(size_t idx) = 0;
    virtual const T& get(size_t idx) const = 0;
    virtual void insert(const T& it, size_t idx) = 0;
   // virtual void insert(T&& it, size_t idx) = 0;
    virtual void remove(size_t idx) = 0;
    virtual size_t memory_usage() const = 0;
    virtual void  release() = 0;  // remove all elements and free memory 
  };

}
