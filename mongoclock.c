/* See LICENSE file for copyright and license details. */
#include <sys/ioctl.h>
#include <sys/timerfd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
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
static int exit_value = 0;

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-s]\n", argv0);
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
sigio(int signo)
{
	char c;
	ssize_t r;
	r = read(STDIN_FILENO, &c, 1);
	if (r <= 0) {
		if (r < 0)
			exit_value = 1;
		caught_sigterm = 1;
	} else if (c == 'q') {
		caught_sigterm = 1;
	}
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

static int
display_time(int timerfd)
{
	const char **digits[9];
	int small = 0;
	uint64_t _overrun;
	struct winsize winsize;
	size_t x = 0, y = 0;
	struct tm *now;
#ifdef USE_ADJTIMEX
	struct timex timex;
	int r;
#else
	time_t now_;
#endif

#ifdef USE_ADJTIMEX
	memset(&timex, 0, sizeof(timex));
#endif

	digits[8] = NULL;

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
		if (timex.time.tv_sec % (24 * 60 * 60) == 0) {
			if (r == TIME_INS) {
				timex.time.tv_sec -= 1;
				now = localtime(&timex.time.tv_sec);
				if (!now)
					goto fail;
				now->tm_sec += 1;
				goto now_checked;
			} else if (r == TIME_DEL) {
				timex.time.tv_sec += 1;
				now = localtime(&timex.time.tv_sec);
			} else {
				now = localtime(&timex.time.tv_sec);
			}
		} else if (r == TIME_OOP) {
			now = localtime(&timex.time.tv_sec);
			now->tm_sec += 1;
		} else {
			now = localtime(&timex.time.tv_sec);
		}
		if (!now)
			goto fail;
	now_checked:
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

		print_time(digits, y, x);

		if (read(timerfd, &_overrun, sizeof(_overrun)) < 0)
			if (errno != EINTR)
				goto fail;
	}

	return 0;

fail:
	return -1;
}

static int
display_posixtime(int timerfd)
{
	const char **digits[21];
	uint64_t _overrun;
	struct winsize winsize;
	size_t w = 0, h = 0, x, y;
	time_t now;
	size_t first_digit, ndigits;

	digits[20] = NULL;

	while (!caught_sigterm) {
		if (caught_sigwinch) {
			if (ioctl(STDOUT_FILENO, (unsigned long)TIOCGWINSZ, &winsize) < 0) {
				if (errno == EINTR)
					continue;
				goto fail;
			}
			caught_sigwinch = 0;
			h = winsize.ws_row;
			w = winsize.ws_col;
		}

		now = time(NULL);
		if (now < 0)
			goto fail;

		first_digit = 20;
		do {
			if (!first_digit)
				abort();
			digits[--first_digit] = mongo_ds[now % 10];
		} while (now /= 10);
		ndigits = 20 - first_digit;

		if (h < DY || w < ndigits * DX) {
			fprintf(stdout, "\033[H\033[2J%s\n", "Screen is too small");
			fflush(stdout);
			pause();
			continue;
		}

		y = (h - DY) / 2;
		x = (w - ndigits * DX) / 2;
		print_time(&digits[first_digit], y, x);

		if (read(timerfd, &_overrun, sizeof(_overrun)) < 0)
			if (errno != EINTR)
				goto fail;
	}

	return 0;

fail:
	return -1;
}

int
main(int argc, char *argv[])
{
	int timerfd = -1, old_flags = -1, tcset = 0;
	int posixtime = 0, old_sig = 0, owner_set = 0;
	struct itimerspec itimerspec;
	struct sigaction sigact;
	struct f_owner_ex old_owner, new_owner;
	struct termios stty, saved_stty;

	ARGBEGIN {
	case 's':
		posixtime = 1;
		break;
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
	timerfd = timerfd_create(CLOCK_REALTIME, 0);
	if (timerfd < 0)
		goto fail;
	if (timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &itimerspec, NULL))
		goto fail;

	memset(&sigact, 0, sizeof(sigact));

	sigact.sa_handler = sigterm;
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGINT, &sigact, NULL);

	sigact.sa_handler = sigwinch;
	sigaction(SIGWINCH, &sigact, NULL);

	sigact.sa_handler = sigio;
	sigaction(SIGIO, &sigact, NULL);
	sigaction(SIGURG, &sigact, NULL);

	if (fcntl(STDIN_FILENO, F_GETOWN_EX, &old_owner))
		goto fail;
	memset(&new_owner, 0, sizeof(new_owner));
	new_owner.type = F_OWNER_PID;
	new_owner.pid = getpid();
	if (fcntl(STDIN_FILENO, F_SETOWN_EX, &new_owner))
		goto fail;
	owner_set = 1;
	old_flags = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, old_flags | FASYNC | O_NONBLOCK);
	fcntl(STDIN_FILENO, F_GETSIG, &old_sig);
	if (old_sig)
		fcntl(STDIN_FILENO, F_SETSIG, 0);

	if (!tcgetattr(STDIN_FILENO, &stty)) {
		saved_stty = stty;
		stty.c_lflag &= (tcflag_t)~(ECHO | ICANON);
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &stty);
		tcset = 1;
	}

	if (posixtime ? display_posixtime(timerfd) : display_time(timerfd))
		goto fail;

	fprintf(stdout, "\033[?25h\n\033[?1049l");
	fflush(stdout);
	fcntl(STDIN_FILENO, F_SETOWN_EX, &old_owner);
	fcntl(STDIN_FILENO, F_SETFL, old_flags);
	fcntl(STDIN_FILENO, F_SETSIG, old_sig);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_stty);
	close(timerfd);
	return exit_value;

fail:
	perror(argv0 ? argv0 : "mongoclock");
	fprintf(stdout, "\033[?25h\n\033[?1049l");
	fflush(stdout);
	if (owner_set)
		fcntl(STDIN_FILENO, F_SETOWN_EX, &old_owner);
	if (old_flags != -1)
		fcntl(STDIN_FILENO, F_SETFL, old_flags);
	if (old_sig)
		fcntl(STDIN_FILENO, F_SETSIG, old_sig);
	if (tcset)
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_stty);
	if (timerfd >= 0)
		close(timerfd);
	return 1;
}
