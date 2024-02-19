<div align="center">

# 2DO

***"To do or not to do? That is the question"***

![Work](https://img.shields.io/badge/work-in_progress-yellow.svg)
![Version](https://img.shields.io/badge/version-0.0.1-blue.svg)
![Build Status](https://img.shields.io/badge/build-passing-green.svg)
![Tests Status](https://img.shields.io/badge/tests-passing-green.svg)

</div>

## About the project

2DO is a simple oldschool todo list with some additional features for start-up's with only one PC and no connection to the internet.

## How to build this

After cloning this repo (in terminal):

```
cd 2DO
git submodule update --recursive
mkdir build
cd build
cmake .
```

## Dependencies
- googletest
- SQLiteCpp
- fmt