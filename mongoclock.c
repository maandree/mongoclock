/* See LICENSE file for copyright and license details. */
#include <sys/ioctl.h>
#include <sys/timerfd.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifdef USE_ADJTIMEX
# include <sys/timex.h>
#endif

#include "arg.h"

#define DX 16
#define DY 12
#define DC 16

#include "mongo_0.h"
#include "mongo_1.h"
#include "mongo_2.h"
#include "mongo_3.h"
#include "mongo_4.h"
#include "mongo_5.h"
#include "mongo_6.h"
#include "mongo_7.h"
#include "mongo_8.h"
#include "mongo_9.h"
#include "mongo_c.h"

static const char **mongo_ds[] = {
	mongo_0, mongo_1, mongo_2, mongo_3, mongo_4,
	mongo_5, mongo_6, mongo_7, mongo_8, mongo_9
};

static volatile sig_atomic_t caught_sigterm = 0;
static volatile sig_atomic_t caught_sigwinch = 1;

char *argv0;

static void
usage(void)
{
	fprintf(stderr, "usage: %s\n", argv0);
	exit(1);
}

static void
sigterm(int signo)
{
	caught_sigterm = 1;
	(void) signo;
}

static void
sigwinch(int signo)
{
	caught_sigwinch = 1;
	(void) signo;
}

static void
print_time(const char ***str, size_t y, size_t x)
{
	size_t r, c;

	fprintf(stdout, "\033[%zu;1H\033[1J", y + 1);

	for (r = 0; r < DY; r++) {
		fprintf(stdout, "\033[%zu;%zuH\033[1K", y + r + 1, x + 1);
		for (c = 0; str[c]; c++)
			fprintf(stdout, "%s", str[c][r]);
		fprintf(stdout, "\033[0K");
	}

	fprintf(stdout, "\033[0J");
	fflush(stdout);
}

int
main(int argc, char *argv[])
{
	struct tm *now;
	const char **digits[9];
	size_t x = 0, y = 0;
	struct winsize winsize;
	int small = 0;
	int fd = -1;
	struct itimerspec itimerspec;
	uint64_t _overrun;
	struct sigaction sigact;
#ifdef USE_ADJTIMEX
	struct timex timex;
	int r;
#else
	time_t now_;
#endif

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if (argc)
		usage();

	fprintf(stdout, "\033[?1049h\033[?25l");

	if (clock_gettime(CLOCK_REALTIME, &itimerspec.it_value))
		goto fail;
	itimerspec.it_interval.tv_sec = 1;
	itimerspec.it_interval.tv_nsec = 0;
	itimerspec.it_value.tv_sec += 1;
	itimerspec.it_value.tv_nsec = 0;
	fd = timerfd_create(CLOCK_REALTIME, 0);
	if (fd < 0)
		goto fail;
	if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &itimerspec, NULL))
		goto fail;

	memset(&sigact, 0, sizeof(sigact));

	sigact.sa_handler = sigterm;
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGINT, &sigact, NULL);

	sigact.sa_handler = sigwinch;
	sigaction(SIGWINCH, &sigact, NULL);

#ifdef USE_ADJTIMEX
	memset(&timex, 0, sizeof(timex));
#endif

	while (!caught_sigterm) {
		if (caught_sigwinch) {
			if (ioctl(STDOUT_FILENO, (unsigned long)TIOCGWINSZ, &winsize) < 0) {
				if (errno == EINTR)
					continue;
				goto fail;
			}
			caught_sigwinch = 0;
			y = winsize.ws_row;
			x = winsize.ws_col;
			if      (y < DY)              small = 2;
			else if (x < 4 * DX + DC)     small = 2;
			else if (x < 6 * DX + 2 * DC) small = 1;
			else                          small = 0;
			y -= DY;
			x -= 4 * DX + DC;
			if (!small)
				x -= 2 * DX + DC;
			y /= 2;
			x /= 2;
		}

		if (small == 2) {
			fprintf(stdout, "\033[H\033[2J%s\n", "Screen is too small");
			fflush(stdout);
			pause();
			continue;
		}

#ifdef USE_ADJTIMEX
		r = adjtimex(&timex);
		if (r == -1)
			goto fail;
		now = localtime(&timex.time.tv_sec);
		if (now == NULL)
			goto fail;
		if (r == TIME_OOP)
			now->tm_sec += 1;
#else
		now_ = time(NULL);
		if (now_ == -1)
			goto fail;
		now = localtime(&now_);
		if (now == NULL)
			goto fail;
#endif

		digits[0] = mongo_ds[now->tm_hour / 10];
		digits[1] = mongo_ds[now->tm_hour % 10];
		digits[2] = mongo_c;
		digits[3] = mongo_ds[now->tm_min / 10];
		digits[4] = mongo_ds[now->tm_min % 10];
		digits[5] = small ? NULL : mongo_c;
		digits[6] = mongo_ds[now->tm_sec / 10];
		digits[7] = mongo_ds[now->tm_sec % 10];
		digits[8] = NULL;

		print_time(digits, y, x);

		if (read(fd, &_overrun, sizeof(_overrun)) < 0)
			if (errno != EINTR)
				goto fail;
	}

	fprintf(stdout, "\033[?25h\n\033[?1049l");
	fflush(stdout);
	close(fd);
	return 0;
  
fail:
	perror(argc ? *argv : "mongoclock");
	fprintf(stdout, "\033[?25h\n\033[?1049l");
	fflush(stdout);
	if (fd >= 0)
		close(fd);
	return 1;
}

