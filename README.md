# C++ exercise

This is a template for the C++ exercise. You can either fork it to your GitHub and then clone it, or clone it directly from here.

## Getting the code ##

The tests are written using GTest environment, which is attached as a submodule to this repo. With Git 1.6.5 and later, you can clone the exercise and Google Test in one step:

```sh
git clone --recursive https://github.com/mzdun/cxx-exercise.git
```

Prior to that, to get the submodules, you must call `submodule update` by hand:

```sh
git clone https://github.com/mzdun/cxx-exercise.git
cd cxx-exercise
git submodule update --init --recursive
```

## Building and running tests ##

CMake supports out-of-directory builds, which I strongly recommend. They keep the objects file away from the source code and cleaning the project and removing the build directory are almost the same operations.

```sh
mkdir build
cd build
cmake ..
```

CMake will use the best guess as to which compiler to use. On Windows it will consult registry and target newest Visual Studio/MSBuild combo. On Linux, it will use the `cc` and `c++` programs. In case you are not happy with the results of `c++ --version`, you can force the compiler with `CC` and `CXX` environment variables. E.g. on Linux, where the shells support ad-hoc environment setting, you can force GCC 9 replacing the last line of the code above with:

```sh
CC=gcc-9 CXX=g++-9 cmake ..
```

Once the CMake finishes the configuration, it should leave you with set of build files. On Windows it will create a Visual Studio solution, `cxx-exercise.sln` and a `.vcxproj` for each target. On Linux it will create quite a number of `Makefile` files, so you can build and run the tests with:

```sh
make && ./tests/tests
```

## Further reading ##

Further information, including the text of the exercise itself, can be found in the [docs](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/mzdun/cxx-exercise/master/docs/spec.bs) ([raw file](docs/spec.bs)).
