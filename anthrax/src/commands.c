#include "commands.h"

int cmd_get(int wfd, int argc, char **args) {
	char *filename;
	size_t nr, nw, br, bsize;
	static unsigned char *buf = NULL;
	struct stat sbuf;
	unsigned long long tb = 0;
	off_t off;
	int fd;

	if (argc > 0) {
		filename = args[0];
	} else {
		return 1;
	}

	printf("sending %s...\n", filename);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		printf("ERROR: unable to open %s for reading: %s\n",
			filename, strerror(errno));
		goto FAIL;
	}

	if (fstat(fd, &sbuf)) {
		printf("ERROR: unable to fstat() file\n");
		goto FAIL;
	}

	bsize = sbuf.st_blksize;
	if ((buf = malloc(bsize)) == NULL) {
		printf("ERROR: malloc() failed\n");
		goto FAIL;
	}

	while ((nr = read(fd, buf, bsize)) > 0) {
		if (nr) {
			for (off = 0; nr; nr -= nw, off += nw) {
				if ((nw = send(wfd, buf + off, (size_t)nr, 0)) < 0)
				{
					printf("ERROR: send() to socket failed");
					goto FAIL;
				} else {
					tb += nw;
				}
			}
		}
	}

	printf("transmitted %llu bytes\n", tb);

	free(buf);
	close(fd);
	return 0;

FAIL:
	sleep(10);
	free(buf);
	if (fd >= 0) close(fd);
	return 1;

}

int cmd_put(int wfd, int argc, char **args) {
	//Put a file to a location on the fs
	return 0;
}

int cmd_mod(int wfd, int argc, char **args) {
	//Load a module that's been uploaded
	if (argc != 1)
		return 1;
	char *filename = args[0];
	void **tmp_gMods;
	int i;
	void *module = dlopen(filename, RTLD_LAZY);
	if (module == NULL)
		return 1;
	gModc++;
	tmp_gMods = malloc(sizeof(void *) * gModc);
	tmp_gMods[(gModc - 1)] = module;
	for (i = 0; i < (gModc - 1); i++) {
		if (gMods[i] != NULL) {
			tmp_gMods[i] = gMods[i];
		}
	}
	gMods = tmp_gMods;
	return 0;
}

