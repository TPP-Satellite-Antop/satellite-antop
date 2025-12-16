# Satellite Antop
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](./LICENSE)

## Building with Docker

Build the image
```sh
docker build -t satellite-antop .
```

Run the image
```sh
docker run satellite-antop:latest
```

### Building locally
```sh
./local-build.sh
```
And then run with
```sh
cd build
./SatelliteAntop
```

## Running tests locally
```sh
mkdir -p build && cd build && cmake .. && make && ctest -T Test -T Coverage
```
## Running tests with docker
```sh
docker build -t satellite-antop-test . -f Dockerfile.test 
docker run satellite-antop-test:latest
```

### Building the library for dtnsim
```sh
./lib-build.sh
```
