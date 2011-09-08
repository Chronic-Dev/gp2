#ifndef _LIBCRASHPARSER_H_
#define _LIBCRASHPARSER_H_

class CrashParser {
	public:
		/**
		 * The CrashParser Constructor
		 */
		CrashParser();
		/**
		 * The CrashParser Deconstructor
		 */
		~CrashParser();
		/**
		 * Reads the crash log from a char * parses it
		 * @param data char * providing the crash log
		 */
		void ParseLogFromString(char *data);
		/**
		 * Reads the crash log from a file and parses it
		 * @param file char * pointing to the location of the crash log
		 */
		void ParseLogFromFile(char *file);
		/**
		 * Gets the string value of the specified register
		 * @param reg char * Describing the register to get the value of. Must match the registers from the crash log. (R0, R5, etc)
		 * @returns char * containing the hex string without the leading 0x of the value in the provided register or NULL if not found
		 */
		char *getRegisterStringValue(char *reg);
		/**
		 * Gets the unsigned int value of the specified register
		 * @param reg char * Describing the register to get the value of. Must match the registers from the crash log. (R0, R5, etc)
		 * @returns unsigned int The atoi converted value in the register
		 */
		unsigned int getRegisterIntValue(char *reg);
		/**
		 * Gets the list of librarys that were loaded in the program
		 * @returns char ** An array of strings containing the libraries loaded. Must be freed.
		 */
		char **getLibraryList();
		/**
		 * Gets the string base address of the specified library
		 * @param lib char * Describing the library to get the base address of
		 * @returns char * containing the hex string wtihout the leading 0x of the base address for the specified library
		 */
		char *getLibBaseAddressString(char *lib);
		/**
		 * Gets the unsigned int base address of the specified library
		 * @param lib char * Describing the library to get the base address of
		 * @returns unsigned in containing the atoi converted base address of the library
		 */
		unsigned int getLibBaseAddressInt(char *lib);
	private:
		char reg[12][9]; /**< The value of each of the 11 Rx registers */
		char ip[9]; /**< The value of the IP register */
		char sp[9]; /**< The value of the SP register */
		char lr[9]; /**< The value of the LR register */
		char pc[9]; /**< The value of the PC register */
		char cpsr[9]; /**< The value of the CPSR register */
		char *libs[100]; /**< The library list */
		char libBA[100][9]; /**< The library base addresses */

};

#endif

