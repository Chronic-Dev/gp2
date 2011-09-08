#include <stdio.h>

/*
dummy binary, insert kernel exploit here
security.mac.vnode_enforce=0
security.mac.proc_enforce=0
*/
int main()
{
    syslog(4, "kern_sploit starting !");
    if( fork())
    {
        int one = 1;
        sysctlbyname("security.mac.vnode_enforce", NULL, 0, &one, sizeof(one));
        setenv("DYLD_INSERT_LIBRARIES", "", 1);
        setenv("DYLD_FORCE_FLAT_NAMESPACE", "", 1);
        //dont need that since we didnt hijack the first launchd
        //execl("/sbin/launchd", "/sbin/launchd", NULL);
    }

    while(1)
    {
        syslog(4, "hey !!!");
        sleep(5);
    }
    return 0;
}
