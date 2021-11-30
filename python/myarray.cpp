#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_myarray(py::module&);

namespace mcl {

  PYBIND11_MODULE(myarray, m) {
    // Optional docstring
    m.doc() = "Myarray library";

    init_myarray(m);
  }
}