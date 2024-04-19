<div align="center">

# 📝2DO

![Version](https://img.shields.io/badge/version-0.0.1-blue.svg)
![Build Status](https://img.shields.io/badge/build-passing-green.svg)
![Tests Status](https://img.shields.io/badge/tests-passing-green.svg)

</div>

<div align="center">
  <video width="640" height="360" controls>
    <source src="./Resource/app-presentation.mp4" type="video/mp4">
    Your browser does not support the video tag.
  </video>
</div>

## About the project
Yet another simple old-school to-do list as single-executable with some additional features for companies with only one PC and no connection to the internet.

## Dependencies
- clang with C++20
- CMake
- SQLiteCpp
- fmt
- googletest
  
## Building
```
cd 2DO
git submodule update --recursive
mkdir build
cd build
cmake .
```
