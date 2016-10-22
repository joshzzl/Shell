/*
 * Strings used in myshell
 * Author: ZHANG Zhili
 * UID: 3035141243
 */


#define TITLE "\nPID       CMD           RTIME     UTIME     STIME      \n"
#define FORMAT_STR "%-10d%-14s%-3.3lfs    %3.3lfs    %-3.3lfs    \n"
#define COMPLETE_MSG "[%d] done.\n"
#define USGMSG "## myShell $ "
#define TERM_MSG "myshell: terminated\n"
#define PROC_STAT "/proc/%d/stat"
#define SCAN_F "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu"

#define ERROR_MSG "Executing: %s\nError Message: \n\t%s\n"
#define ERR_SPLIT "Error in splitting\n"
#define ERR_REALLOC "Error in realloc\n"
#define ERR_5_ARG "More than 5 commands were entered!\n"
#define ERR_ALLOC_SERIES "Error in alloc space for series\n"
#define ERR_SPLIT_N "Error when splitting %d-th command, exiting the program\n"
#define ERR_INCOM_PIPE "Incomplete | sequence.\n"
#define ERR_EXIT_ARG "\"exit\" with other arguments!!!\n"
#define ERR_TIMEX_ALONE "\"timeX\" cannot be a standalone command\n"
#define ERR_AND_MIDDLE "\'&\' should not appear in the middle of the command line\n"
#define ERR_TIMEX_BCG "Error dectected: timeX can be used on foreground only\n"
#define ERR_MAL_PID "Error in malloc pids\n"
#define ERR_ALLOC_PFD "Error in alloc pfds\n"
#define ERR_EXECVP "Execvp: error no = %s\n"
#define ERR_FGETS "Something wrong in fgets()\n"
#define ERR_WAITID "Error in (PID: %d) waitid: %s\n"
#define ERR_OPEN_PROC "Error in opening proc file\n"
