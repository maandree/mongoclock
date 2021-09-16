.POSIX:

CONFIGFILE = config.mk
include $(CONFIGFILE)

DIGITS =\
	mongo_0.h mongo_1.h mongo_2.h mongo_3.h mongo_4.h \
	mongo_5.h mongo_6.h mongo_7.h mongo_8.h mongo_9.h \
	mongo_c.h


all: mongoclock

mongoclock: mongoclock.o
	$(CC) -o $@ mongoclock.o $(LDFLAGS)

mongoclock.o: mongoclock.c arg.h $(DIGITS)
	$(CC) -c -o $@ mongoclock.c $(CPPFLAGS) $(CFLAGS)

mongo_0.h: digit.sh
	./digit.sh 0 > $@

mongo_1.h: digit.sh
	./digit.sh 1 > $@

mongo_2.h: digit.sh
	./digit.sh 2 > $@

mongo_3.h: digit.sh
	./digit.sh 3 > $@

mongo_4.h: digit.sh
	./digit.sh 4 > $@

mongo_5.h: digit.sh
	./digit.sh 5 > $@

mongo_6.h: digit.sh
	./digit.sh 6 > $@

mongo_7.h: digit.sh
	./digit.sh 7 > $@

mongo_8.h: digit.sh
	./digit.sh 8 > $@

mongo_9.h: digit.sh
	./digit.sh 9 > $@

mongo_c.h: digit.sh
	./digit.sh c > $@

install: mongoclock
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	mkdir -p -- "$(DESTDIR)$(MANPREFIX)/man1"
	cp -- mongoclock "$(DESTDIR)$(PREFIX)/bin/"
	cp -- mongoclock.1 "$(DESTDIR)$(MANPREFIX)/man1/"

uninstall:
	-rm -- "$(DESTDIR)$(PREFIX)/bin/mongoclock"
	-rm -- "$(DESTDIR)$(MANPREFIX)/man1/mongoclock.1"

clean:
	-rm -f -- *.o *.su mongoclock mongo_*.h

.PHONY: all install uninstall clean
