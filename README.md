# mipt-caos-yacurl

## Build & Run

```
$ git clone https://github.com/bcskda/mipt-caos-yacurl
$ git checkout dev-cli
$ mkdir -p build && cd build
$ cmake --DCMAKE_BUILD_TYPE=Debug ..
$ make yacurl
$ valgrind ./yacurl [args]
```
