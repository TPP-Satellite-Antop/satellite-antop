SHELL := /bin/bash
PWD := $(shell pwd)

build:
	docker build . --rm -t satellite-antop:latest
.PHONY: build

run:
	docker run -it satellite-antop:latest
.PHONY: run

clean:
	docker system prune
.PHONY: clean

all: build run
