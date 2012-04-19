#include "syscalls.h"
#include "utils.h"
#include "firmware.h"

int main(int argc, char* argv[], char* env[]) {
	int ret = 0;
	int dev = 0;
	char model[10];
	char root_disk[20];
	char user_disk[20];
	struct stat status;

	console = open("/dev/console", O_WRONLY);
	dup2(console, 1);
	char **envp = env;

	puts("Searching for disk...\n");
	while (stat("/dev/disk0s1", &status) != 0) {
		sleep(1);
	}
	puts("\n\n\n\n\n");
	puts("Pois0nDisk - by Chronic-Dev Team\n");

	//fsexec();
	while (1) {
	}
	puts("Rebooting device...\n");
	close(console);
	reboot(1);

	return 0;
}
