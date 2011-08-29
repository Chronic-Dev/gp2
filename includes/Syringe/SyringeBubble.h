#include <common.h>

class SyringeBubble {
	public:
		/**
		 * The SyringeBubble Constructor
		 * If this one is called, an unknown error occured
		 */
		SyringeBubble();
		/**
		 * The alternate SyringeBubble Constructor
		 * @param err The error message to display
		 */
		SyringeBubble(char *err);
		/**
		 * The SyringeBubble Deconstructor
		 */
		~SyringeBubble();
		/**
		 * Gets the error message for this error.
		 * @returns The error message for this SyringeBubble
		 */
		char *getError();
	private:
		char *error; /**< The error message */
};

