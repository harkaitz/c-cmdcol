#ifdef __unix__
#  define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#ifdef __unix__
#  include <sys/ioctl.h>
#endif


#define NL "\n"
#define URL_REPO "https://github.com/harkaitz/c-cmdcol"

char CMDCOL[] =
    "Usage: cmdcol [-RWc:] CMD [COLS] [CMD [COLS] [...]]"          NL
    ""                                                             NL
    "Display the output of commands in columns."                   NL
    ""                                                             NL
    "  -R     : Do not remove ANSI escape sequences when piping."  NL
    "  -W     : Do not cut the output to terminal width."          NL
    "  -c MAX : Output columns."                                   NL
    ""                                                             NL
    "Examples:"                                                    NL
    ""                                                             NL
    "  $ cmdcol \"cal\" 25 \"calendar | head -n 8\""               NL
    "  $ cmdcol \"sscal -c\" 25 \"rem | sed '3,${ /^$/d }'\""      NL
    ""                                                             NL
    "Copyright (c) 2026 Harkaitz Agirre Ezama, ISC licensed."      NL
    URL_REPO                                                       NL;

static bool opt_ansi = true;
static bool opt_wrap = true; 

char   *commands[10] = { "cal -c", "rem | sed '3,${ /^$/d }'", NULL };
int     commandsz    = 0;
FILE   *fps[]        = { NULL };
int     columns[10]  = { 0 };
int     columns_max  = 80;

#define errorf(FMT, ...) fprintf(stderr, "scal: error: " FMT "\n", ##__VA_ARGS__)

static bool print_column(char const _s[], int _max, bool _pad);

int
main(int _argc, char *_argv[])
{
	int opt;
	char buffer[1024] = {0};

	if (_argc == 1 || !strcmp(_argv[1], "--help") || !strcmp(_argv[1], "-h")) {
		fputs(CMDCOL, stdout);
		return 0;
	}

	#ifdef __unix__
	opt_ansi = isatty(1);
	if (opt_ansi) {
		opt_wrap = true;
		struct winsize w;
		ioctl(1, TIOCGWINSZ, &w);
		columns_max = w.ws_col;
	}
	#endif

	while((opt = getopt(_argc, _argv, "RWc:")) != -1) {
		switch (opt) {
		case 'R': opt_ansi = true; break;
		case 'W': opt_wrap = false; break;
		case 'c': columns_max = atoi(optarg); break;
		case '?':
		default:
			return 1;
		}
	}

	int left = columns_max;
	while (optind < _argc && commandsz < 10) {
		int ncol;
		char *arg1 = _argv[optind++];
		char *arg2 = (arg1)?_argv[optind++]:NULL;
		if (arg1 && arg2) {
			ncol = atoi(arg2);
			ncol = (ncol < columns_max && ncol > 0)?ncol:columns_max;
		} else if (arg1) {
			ncol = columns_max;
		} else {
			break;
		}
		if (!opt_wrap) {
		} else if (ncol < left) {
			left -= ncol;
		} else {
			ncol = left;
			left = 0;
		}
		commands[commandsz] = arg1;
		columns[commandsz] = ncol;
		commandsz++;
	}

	for (int i=0; i<commandsz; i++) {
		if (!(fps[i] = popen(commands[i], "r"))) {
			errorf("%s: %s.", commands[i], strerror(errno));
			return 1;
		}
	}

	bool keep = true;
	while (keep) {
		keep = false;
		for (int i=0; i<commandsz; i++) {
			bool esc = false;
			bool pad = (i+1)!=commandsz;
			if (!fps[i]) {
				esc = print_column("", columns[i], pad);
			} else if (!fgets(buffer, sizeof(buffer)-1, fps[i])) {
				fclose(fps[i]);
				fps[i] = NULL;
				esc = print_column("", columns[i], pad);
			} else {
				keep = true;
				esc = print_column(buffer, columns[i], pad);
			}
			if (esc && opt_ansi) {
				fputs("\x1b[0m", stdout);
			}
		}
		putc('\n', stdout);
	}
}

static bool
print_column(char const _s[], int _max, bool _pad)
{
	int   len = 0;
	bool  esc = false;
	bool  ret = false;
	for(;*_s;_s++) {
		bool is_escape;
		if (esc && *_s=='m') {
			esc = false;
			is_escape = true;
		} else if (esc) {
			is_escape = true;
		} else if (*_s=='\x1b' && *(_s+1) == '[') {
			esc = true;
			ret = true;
			is_escape = true;
		} else if (strchr("\n\r", *_s)) {
			continue;
		} else if (len == _max) {
			break;
		} else if ((*_s & 0xC0) == 0x80) {
			is_escape = false;
		} else {
			is_escape = false;
			len++;
		}
		if ((opt_ansi && is_escape) || !is_escape) {
			putc(*_s, stdout);
		}
	}
	while ((*_s & 0xC0) == 0x80) {
		putc(*_s, stdout);
		_s++;
	}
	if (_pad) {
		for (int i=len; i<_max; i++) {
			putc(' ', stdout);
		}
	}
	return ret;
}
