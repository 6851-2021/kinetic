# kinetic

`kinetic` is a C++17 header-only library for two kinetic data structures:

* Kinetic successor
* Kinetic heap

Points must follow affine trajectories, of the form *a* + *b* *t*
where *t* represents time.

## Usage

* Copy `heap.h` and/or `successor.h` into your project.
* Add `#include "heap.h"` and/or `#include "successor.h"`
  into your list of includes.
* Now you can use the data structures provided by the library
  by simply compiling your files normally with no extra work.
* You must compile with the C++17 standard or above.
