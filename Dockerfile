FROM alpine:3.21.2 AS builder

RUN apk update && \
    apk add --no-cache \
        gcc \
        g++ \
        cmake \
        make \
        gtest \
        libtool \
        musl-dev

WORKDIR /app

COPY . .

RUN cmake -S cmake -B build && cmake --build build

ENTRYPOINT ["./build/SatelliteAntop"]
