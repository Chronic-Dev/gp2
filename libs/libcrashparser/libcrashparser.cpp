#include <libcrashparser/libcrashparser.h>
#include <sstream>
#include <iostream>
#include <cstdlib>

using namespace std;
CrashParser::CrashParser() {
	int i = 0;
	for (i = 0; i < 100; i++) {
		libs[i] = NULL;
		for (int j = 0; j < 9; j++)
			libBA[i][j] = '\0';
	}
}

CrashParser::~CrashParser() {
	int i = 0;
	for (i = 0; i < 100; i++) {
		if (libs[i] != NULL)
			free(libs[i]);
	}
}

//I know this function is a clusterfuck of everything that is terrible in the world
//Forgive me... If you want to write some crazy awesome regex to do this and handle 
//all the boost deps, be my guest. Just don't screw up what works!
void CrashParser::ParseLogFromString(char *data) {
	int i = 0;
	int j = 0;
	char regdata[10];
	char *datastart = strstr(data,"ARM Thread State:");

	//Parse the registers	
	for (i = 0; i < 12; i++) {
		sprintf(regdata, "r%d: 0x", i);
		datastart = strstr(datastart, regdata); //Find the start of Ri
		if (i < 10) { //If it's < 10, we start at 6
			for (j = 6; j < 14; j++) {
				reg[i][(j - 6)] = datastart[j]; //Store the reg data
			}
		} else { //Otherwise we start at 7 since we have an extra digit
			for (j = 7; j < 15; j++) {
				reg[i][(j - 7)] = datastart[j]; //Store the reg data
			}
		}
		reg[i][8] = '\0'; //Make sure it terminates
	}

	//Same stuff for ip, sp, lr, pc, and cpsr
	datastart = strstr(datastart, "ip: 0x");
	for (j = 6; j < 14; j++) {
		ip[(j - 6)] = datastart[j];
	}
	ip[8] = '\0';

	datastart = strstr(datastart, "sp: 0x");
	for (j = 6; j < 14; j++) {
		sp[(j - 6)] = datastart[j];
	}
	sp[8] = '\0';

	datastart = strstr(datastart, "lr: 0x");
	for (j = 6; j < 14; j++) {
		lr[(j - 6)] = datastart[j];
	}
	lr[8] = '\0';

	datastart = strstr(datastart, "pc: 0x");
	for (j = 6; j < 14; j++) {
		pc[(j - 6)] = datastart[j];
	}
	pc[8] = '\0';

	datastart = strstr(datastart, "cpsr: 0x");
	for (j = 8; j < 16; j++) {
		cpsr[(j - 8)] = datastart[j];
	}
	cpsr[8] = '\0';

	//Now for the address spaces and libs
	datastart = strstr(datastart, "Binary Images:");
	datastart += 15;
	i = 0;
	int lib = 0;
	
	while (datastart[i] != '<' && lib < 100) {
		//Ignore all the spaces if there are any
		while (datastart[i] == ' ') {
			i++;
		}
		//Skip past the 0x
		i += 2;

		//Set j to 0 to store our BA properly
		j = 0;
		while (datastart[i] != ' ') {
			libBA[lib][j] = datastart[i];
			i++;
			j++;
			libBA[lib][j] = '\0'; //Make sure we null terminate our last one
		}

		//Ignore everything up to the lib path listing...
		while (datastart[i] != '/') {
			i++;
		}
		
		//Store where we were to j...
		j = i;

		//Count the path size
		while (datastart[i] != '\n') {
			i++;
		}
		
		libs[lib] = (char *)malloc(sizeof(char) * ((i - j) + 1)); //allocate the proper size to store the data
		i = j; //Restore where we were, now that we know the size

		j = 0;
		while (datastart[i] != '\n') {
			//Store it...
			libs[lib][j] = datastart[i];
			i++;
			j++;
			libs[lib][j] = '\0'; //Make sure we null terminate our last one
		}
		i++;
		lib++;
	}
}

void CrashParser::ParseLogFromFile(char *file) {
	FILE *fp = fopen(file, "r");
	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		rewind(fp);
		char *data = (char *)malloc(sizeof(char) * size + 1);
		fread(data, 1, size, fp);
		if (!ferror(fp)) {
			ParseLogFromString(data);
		} else {
			//Error
		}
		fclose(fp);
	}

}

char *CrashParser::getRegisterStringValue(char *reg) {
	char regnum[3];
	if (reg[0] == 'r' || reg[0] == 'R') {
		regnum[0] = reg[1];
		regnum[1] = reg[2];
		regnum[3] = '\0';
		return this->reg[atoi(regnum)];
	} else if (!strcmp(reg,"IP") || !strcmp(reg,"ip")) {
		return ip;
	} else if (!strcmp(reg,"SP") || !strcmp(reg,"sp")) {
		return sp;
	} else if (!strcmp(reg,"LR") || !strcmp(reg,"lr")) {
		return lr;
	} else if (!strcmp(reg,"PC") || !strcmp(reg,"pc")) {
		return pc;
	} else if (!strcmp(reg,"CPSR") || !strcmp(reg,"cpsr")) {
		return cpsr;
	} else {
		return NULL;
	}
}

unsigned int CrashParser::getRegisterIntValue(char *reg) {
	unsigned int ret;
	char *strval = getRegisterStringValue(reg);
	if (strval == NULL)
		throw 
	std::stringstream ss;
	ss << std::hex << strval;
	ss >> ret;
	return ret;
}

char **CrashParser::getLibraryList() {
	return libs;
}

char *CrashParser::getLibBaseAddressString(char *lib) {
	int i = 0;
	while (libs[i] != NULL && strcmp(lib,libs[i])) {
		i++;
	}
	if (libs[i] == NULL)
		return NULL;
	return libBA[i];
}

unsigned int CrashParser::getLibBaseAddressInt(char *lib) {
	unsigned int ret;
	char *strval = getLibBaseAddressString(lib);
	if (strval == NULL)
		throw 
	std::stringstream ss;
	ss << std::hex << strval;
	ss >> ret;
	return ret;
}

