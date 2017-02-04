all: out/forth test_roundtrip

run: out/forth
	out/forth

CFLAGS=-O2 -Wall -Werror $(F)
LIBS=-ldl
SOURCES=forth.c interpreter.c out/gen/boot.c
HEADERS=interpreter.h opcodes.h

out:
	mkdir -p out

out/gen:
	mkdir -p out/gen

out/forth: $(SOURCES) $(HEADERS) | out
	$(CC) $(CFLAGS) $(SOURCES) $(LIBS) -o $@

out/gen/boot.raw: out/text2bicol boot.fs | out/gen
	$^ $@

out/gen/boot_decoded.fs: out/bicol2text out/gen/boot.raw | out/gen
	$^ $@

out/gen/boot.c: out/gen/boot.raw out/binary2c | out/gen
	out/binary2c boot $< $@

out/text2bicol: text2bicol.c $(HEADERS) | out
	$(CC) $(CFLAGS) $< -o $@

out/bicol2text: bicol2text.c $(HEADERS) | out
	$(CC) $(CFLAGS) $< -o $@

out/binary2c: binary2c.c | out
	$(CC) $(CFLAGS) $< -o $@

test_roundtrip: out/gen/boot_decoded.fs boot.fs
	diff $^

reformat: out/gen/boot_decoded.fs
	cp $< boot.fs

size: all
	ls -l out out/gen
	$(MAKE) clean
	wc -l *

clean:
	rm -rf out
