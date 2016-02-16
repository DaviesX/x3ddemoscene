/* log.c: All log handling functions go here */
#include <x3d/common.h>
#include <container/linkedlist.h>
#include <system/allocator.h>
#include <system/log.h>
#include <system/panic.h>


#define MAX_MESSAGE_LENGTH		1024

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

const char* c_FileName          = "x3d_log";

// plain text
const char* c_NormalPrefix      = "x3d_log:";
const char* c_MildErrPrefix     = "x3d_mild:*";
const char* c_SevereErrPrefix   = "x3d_severe:**";
const char* c_CritiErrPrefix    = "x3d_critical:***";

// colored prefix
const char* c_CNormalPrefix      = ANSI_COLOR_GREEN"x3d_log:"ANSI_COLOR_RESET;
const char* c_CMildErrPrefix     = ANSI_COLOR_MAGENTA"x3d_mild:*"ANSI_COLOR_RESET;
const char* c_CSevereErrPrefix   = ANSI_COLOR_RED"x3d_severe:**"ANSI_COLOR_RESET;
const char* c_CCritiErrPrefix    = ANSI_COLOR_RED"x3d_critical:***"ANSI_COLOR_RESET;

struct log_output {
        FILE*                   file;
        int                     normal_bhv;
        int                     mild_err_bhv;
        int                     severe_err_bhv;
        int                     criti_err_bhv;
        int                     to_terminate;
        int                     tmp_bhv;
        d_alg_llist(char*)      last_ten_errors;
};

static struct log_output        g_log_inst;
static bool                     g_is_init = false;


bool log_init(int to_terminate)
{
        if (g_is_init) {
                goto finished;
        }
        zero_obj(&g_log_inst);
        g_log_inst.normal_bhv           = LOG_OUTPUT_TO_BOTH;
        g_log_inst.mild_err_bhv         = LOG_OUTPUT_TO_BOTH;
        g_log_inst.severe_err_bhv       = LOG_OUTPUT_TO_BOTH;
        g_log_inst.criti_err_bhv        = LOG_OUTPUT_TO_BOTH;
        g_log_inst.to_terminate         = to_terminate;

        time_t timeT;
        time(&timeT);
        struct tm* gmtTime = gmtime(&timeT);

        printf(ANSI_COLOR_BLUE"x3drenderlibrary - %s\nx3d log start recording\t%d:%d:%d\n\n"ANSI_COLOR_RESET,
               X3D_VERSION_STRING, gmtTime->tm_hour, gmtTime->tm_min, gmtTime->tm_sec);
        if ((g_log_inst.normal_bhv | g_log_inst.mild_err_bhv | g_log_inst.severe_err_bhv | g_log_inst.criti_err_bhv) & LOG_OUTPUT_TO_FILE) {
                if (!(g_log_inst.file = fopen(c_FileName, "w"))) {
                        return false;
                }
                fprintf(g_log_inst.file, "x3drenderlibrary - %s\nx3d log start recording\t%d:%d:%d\n\n",
                        X3D_VERSION_STRING, gmtTime->tm_hour, gmtTime->tm_min, gmtTime->tm_sec);
        }

        alg_init(llist, &g_log_inst.last_ten_errors, sizeof(char*), 10);
        g_is_init = true;
finished:
        return true;
}


void log_free(void)
{
        if (g_log_inst.file) {
                fprintf(g_log_inst.file, "\nx3d log stop recording\n\n");
                fclose(g_log_inst.file);
        }
        alg_free(llist, &g_log_inst.last_ten_errors);
        printf(ANSI_COLOR_BLUE"\nx3d log stop recording\n\n"ANSI_COLOR_RESET);
        zero_obj(&g_log_inst);
        g_is_init = 0;
}

#define report_log(_functionName, _message, _logState, _prefix, _prefix_colored)	\
{ \
	if (!g_is_init) { \
		/* Initialize log reporter, if it is not */ \
		log_init(true); \
	} \
\
	va_list _args; \
	va_start(_args, _message); \
	char* _outputMessage; \
	if (-1 == vasprintf(&_outputMessage, _message, _args)) { \
		printf("Error in allocating memory for Log Reporter\n"); \
		abort(); \
	} \
\
	va_end(_args); \
       /* @fixme (davis#2#): <log> get last ten errors doesn't work yet */\
/*        if (_prefix != c_NormalPrefix) {                                              \
                if (alg_n(llist, &g_log_inst.last_ten_errors) >= 10) {                  \
                        alg_iter(char*) _iter;                                          \
                        alg_pop_back(llist, &g_log_inst.last_ten_errors, &_iter);       \
                }                                                                       \
                alg_push_front(llist, &g_log_inst.last_ten_errors, _outputMessage);     \
        }*/                                                                               \
                                                                                        \
	int _behavior;                                                          \
	if (!g_log_inst.tmp_bhv) {                                              \
		_behavior = (_logState);                                        \
	} else {                                                                \
		_behavior = g_log_inst.tmp_bhv; \
		g_log_inst.tmp_bhv = LOG_OUTPUT_NONDEFINED; \
	} \
\
	switch(_behavior) { \
	case LOG_OUTPUT_TO_CONSOLE: { \
		printf("%s%s<%s>%s %s\n", \
                       (_prefix_colored), ANSI_COLOR_CYAN, (_functionName), ANSI_COLOR_RESET, (_outputMessage)); \
		break; \
	} \
\
	case LOG_OUTPUT_TO_FILE: { \
		fprintf(g_log_inst.file, "%s<%s> %s\n", \
			 (_prefix), (_functionName), (_outputMessage)); \
		break; \
	} \
\
	case LOG_OUTPUT_TO_BOTH: { \
		printf("%s%s<%s>%s %s\n", \
                       (_prefix_colored), ANSI_COLOR_CYAN, (_functionName), ANSI_COLOR_RESET, (_outputMessage)); \
		fprintf(g_log_inst.file, "%s<%s> %s\n", \
			 (_prefix), (_functionName), (_outputMessage)); \
		break; \
	} \
	} \
\
	free(_outputMessage); \
}

void call_log_normal(const char* functionName, const char* message, ...)
{
        report_log(functionName, message, g_log_inst.normal_bhv, c_NormalPrefix, c_CNormalPrefix);
}

void call_log_mild_err(const char* functionName, const char* errorMessage, ...)
{
        report_log(functionName, errorMessage, g_log_inst.mild_err_bhv, c_MildErrPrefix, c_CMildErrPrefix);
}

void call_log_severe_err(const char* functionName, const char* errorMessage, ...)
{
        report_log(functionName, errorMessage, g_log_inst.severe_err_bhv, c_SevereErrPrefix, c_CSevereErrPrefix);
}

void call_log_critical_err(const char* functionName, const char* errorMessage, ...)
{
        report_log(functionName, errorMessage, g_log_inst.criti_err_bhv, c_CritiErrPrefix, c_CCritiErrPrefix);
        if(g_log_inst.to_terminate) {
                puts("The program will be shutdown");
                if(g_log_inst.file) {
                        fprintf(g_log_inst.file, "The program will be shutdown...");
                }
                panic();
        }
}

char** log_get_last_ten_errors()
{
        return alg_array(llist, &g_log_inst.last_ten_errors);
}

void log_set_behavior(int bhv)
{
        g_log_inst.tmp_bhv = bhv;
}

// @todo(davis#9#): implement <log_guard_frequency_for_next_message>
void log_guard_frequency_for_next_message(int min_interval)
{
}

// Test Cases:
#include <container/paramset.h>
#include <x3d/debug.h>
// variable_memory_test0
__dlexport void __callback                  log_report_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  log_report_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   log_report_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_KernelStart
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = true;
        return pos;
}
__dlexport void __callback                  log_report_test(struct alg_var_set* envir)
{
        /* Test the error report code in log.h/c */
        log_init(false);
        log_normal("This is a normal message");
        log_mild_err("This is a mild error message");
        log_severe_err("This is a severe error message");
        log_critical_err("This is a critical error message");
        int para = 10, para1 = 15;
        log_set_behavior(LOG_OUTPUT_TO_FILE);
        log_normal("There are two numbers: %d %d", para, para1);
}