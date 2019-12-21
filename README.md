# mipt-caos-yacurl

## Feature status
- Option ```-O```: unsupported

## Build & Run

```
$ git clone https://github.com/bcskda/mipt-caos-yacurl
$ git checkout dev-cli
$ mkdir -p build && cd build
$ cmake --DCMAKE_BUILD_TYPE=Debug ..
$ make yacurl
$ ./yacurl [args] 2>/dev/null
```
