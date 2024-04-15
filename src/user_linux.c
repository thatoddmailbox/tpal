#include "tpal/user.h"

// required for setregid, setreuid
// TODO: does everyone have that? is it safe to use/portable?
#define _GNU_SOURCE

#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//
// helper functions from https://stackoverflow.com/a/53545658/2178519
//

#define ULONG_BITS (CHAR_BIT * sizeof(unsigned long))

static inline int devnullfd(const int fd) {
	int tempfd;

	if (fd == -1) {
		errno = EINVAL;
		return errno;
	}

	do {
		tempfd = open("/dev/null", O_RDWR | O_NOCTTY);
	} while (tempfd == -1 && errno == EINTR);
	if (tempfd == -1) {
		return errno;
	}

	if (tempfd != fd) {
		if (dup2(tempfd, fd) == -1) {
			const int saved_errno = errno;
			close(tempfd);
			errno = saved_errno;
			return errno;
		}
		if (close(tempfd) == -1) {
			return errno;
		}
	}

	return 0;
}

static void closeall(const unsigned long mask) {
	DIR *dir;
	struct dirent *ent;
	int dfd;

	dir = opendir("/proc/self/fd/");
	if (!dir) {
		// cannot list open descriptors, just try and close all
		const long fd_max = sysconf(_SC_OPEN_MAX);
		long fd;

		for (fd = 0; fd < ULONG_BITS; fd++) {
			if (!(mask & (1uL << fd))) {
				close(fd);
			}
		}

		for (fd = ULONG_BITS; fd <= fd_max; fd++) {
			close(fd);
		}

		return;
	}

	dfd = dirfd(dir);

	while ((ent = readdir(dir))) {
		if (ent->d_name[0] >= '0' && ent->d_name[0] <= '9') {
			const char *p = &ent->d_name[1];
			int fd = ent->d_name[0] - '0';

			while (*p >= '0' && *p <= '9') {
				fd = (10 * fd) + *(p++) - '0';
			}

			if (*p) {
				continue;
			}

			if (fd == dfd) {
				continue;
			}

			if (fd < ULONG_MAX && (mask & (1uL << fd))) {
				continue;
			}

			close(fd);
		}
	}

	closedir(dir);
}

char * tpal_user_get_temp_path() {
	return strdup("/tmp/");
}

void tpal_user_shell_open(const char * path) {
	int child = fork();
	if (child == -1) {
		// failure, oof
		return;
	}

	if (child == 0) {
		// child process

		uid_t uid = getuid();
		gid_t gid = getgid();

		// close all file descriptors
		closeall(0);

		// redirect stdin, stdout, stderr to /dev/null
		devnullfd(0);
		devnullfd(1);
		devnullfd(2);

		// drop elevated privileges
		if (setresgid(gid, gid, gid) == -1 || setresuid(uid, uid, uid) == -1) {
			_exit(98);
		}

		// run in a separate process group
		setsid();

		// execute the command
		execlp("xdg-open", "xdg-open", path, NULL);

		// if we get here, something went wrong
		_exit(1);
	}

	pid_t p;
	int status;
	do {
		status = 0;
		p = waitpid(child, &status, 0);
	} while (p == -1 && errno == EINTR);

	// TODO: report errors or failures?
}