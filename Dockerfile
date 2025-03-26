FROM alpine:3.21.2 AS builder

RUN apk update && \
    apk add --no-cache \
        gcc \
        g++ \
        cmake \
        make \
        gtest \
        libtool \
        musl-dev \
        git \
        doxygen

WORKDIR /app

COPY . .

RUN cmake -S . -B build && cmake --build build

CMD ["./build/SatelliteAntop"]
