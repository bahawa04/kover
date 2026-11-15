.PHONY: build clean test

exec = kover

build:
	$(MAKE) build -C src
	mkdir -p bin
	cp src/$(exec) bin/$(exec)

clean:
	$(MAKE) clean -C src
	rm -rf bin

test: build
	$(MAKE) -C bats
