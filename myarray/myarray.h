#pragma once

#include "expand_array.h"
#include "matrix_array.h"
#include "stdvector_array.h"

namespace myarray {

  template<typename T>
  using SingleArray = ExpandArray<T, ExpandConstSize<1>>;

  template<typename T>
  using VectorArray = ExpandArray<T, ExpandConstSize<100>>;

  template<typename T>
  using FactorArray = ExpandArray<T, ExpandFactorSize<3, 2>>; // factor = 1.5

  

}

