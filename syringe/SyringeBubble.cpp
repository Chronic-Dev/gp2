#include <Syringe/SyringeBubble.h>
#include <cstdlib>
#include <cstring>
using namespace std;

SyringeBubble::SyringeBubble() {
	SyringeBubble("");
}

SyringeBubble::SyringeBubble(char *err) {
	error = NULL;
	int len = strlen(err);
	if (len > 0) {
		error = (char *)malloc(len * sizeof(char));
		strcpy(error, err);
	} else {
		error = (char *)malloc(26 * sizeof(char));
		strcpy(error, "An unknown error occured\0");

	}
}

SyringeBubble::~SyringeBubble() {
	//if (error != NULL)
	//	free(error);
}

char *SyringeBubble::getError() {
	return error;
}

