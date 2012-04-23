//#ifdef DEBUG
#define SHOWDEBUGGING 1
//#else
//#define SHOWDEBUGGING 0
//#endif

#define debug(...) if (SHOWDEBUGGING) fprintf(stderr, __VA_ARGS__)
#define error(...) fprintf(stderr, __VA_ARGS__)
#define info(...) printf(__VA_ARGS__)
