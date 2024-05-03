# MAMBO Fuzz

Simple plugin that allows fuzzing unmodified binaries with MAMBO and honggfuzz.

## Set up

1. Clone and build honggfuzz from [here](https://github.com/google/honggfuzz).

2. Copy `fuzz.c` into `plugins` directory in MAMBO.

3. Update MAMBO `makefile`:

```
PLUGINS+=fuzz.c

...

LIBS+=-L<honggfuzz-roo>/libhfuzz/ -lhfuzz -lm -lunwind-ptrace -lunwind-aarch64 -lunwind -llzma -L<honggfuzz-root>/libhfcommon/ -lhfcommon -lrt -ldl -lpthread
```

4. Build MAMBO with `make`.

5. Run binary with MAMBO and honggfuzz, for example:

```
<honggfuzz-root>/honggfuzz -i corpus/ -s -- <mambo-root>/dbm ./a.out
```
