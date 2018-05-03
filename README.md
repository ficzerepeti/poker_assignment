# poker_assignment
Purpose of this tool is to help you in decision making while playing texas holdem poker.
This was given as an assignment when I applied to a company.

## Checkout
Git clone with submodules under external.
```
git clone --recurse-submodules https://github.com/ficzerepeti/poker_assignment.git
```
## Build
Use CMake to configure and build on your platform (should work on both Linux and Windows).
Make sure you have a C++17 or newer compiler.
It's recommended to build a release binary to speed things up otherwise calculating equity can a while.
Note that on Windows the tests don't link properly which is described in this [stackoverflow post](https://stackoverflow.com/questions/14714877/mismatch-detected-for-runtimelibrary).
```
mkdir build_root
cd build_root
cmake /path/to/poker_assignment -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```
