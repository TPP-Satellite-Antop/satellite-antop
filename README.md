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

## Running tests locally
```sh
mkdir -p build && cd build && cmake .. && make && ctest
```
## Running tests with docker
```sh
docker build -t satellite-antop-test . -f Dockerfile.test 
docker run satellite-antop-test:latest
```
