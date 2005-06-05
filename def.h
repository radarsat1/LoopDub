
#ifndef _DEF_H_
#define _DEF_H_

// Global definitions
#define N_LOOPS   8
#define MAX_TEMPO 260
#define MIN_TEMPO 20
#define MAX_KEYS  10
#define MAX_PARTS 16

// Commands
enum
{
	 CMD_LEARN=1,
	 CMD_SELECT,
	 CMD_BEATS,
	 CMD_NORMALIZE,
	 CMD_KEYS,
	 CMD_SPLIT,
	 CMD_HOLD,
	 CMD_SWITCH,
	 CMD_DIRCLICK, // all file browsers call this
	 CMD_FILECLICK,
	 CMD_PROGRAMCHANGE,
	 CMD_CLOSE=CMD_FILECLICK+N_LOOPS,
	 N_CMD=CMD_CLOSE+N_LOOPS
};

// Macros
#define max(a, b) (((a) > (b)) ? a : b)
#define min(a, b) (((a) < (b)) ? a : b)

#endif // _DEF_H_
