SHELL := /bin/bash
PWD := $(shell pwd)

build:
	docker build . --rm -t satellite-antop:latest
.PHONY: build

run: build
	docker run -it satellite-antop:latest
.PHONY: run

test: build
	docker run -it satellite-antop:latest sh -c "./build/tests/SatelliteAntopTest"
.PHONY: run

test-ci: build
	docker run -it satellite-antop:latest sh -c "ctest -T Test -T Coverage --test-dir build"
.PHONY: run

clean:
	docker system prune
.PHONY: clean

all: build run
