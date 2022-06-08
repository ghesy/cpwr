VERSION = 0.1

CFLAGS = -O2 -DVERSION='"$(VERSION)"'

cpwr: cpwr.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f cpwr

.PHONY: clean
