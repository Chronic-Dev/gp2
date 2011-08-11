#include <common.h>

class SyringeBubble {
	public:
		SyringeBubble(char *err);
		~SyringeBubble();
		char *getError();
	private:
		char *error;
};

