/* log.c: All log handling functions go here */
#include <x3d/common.h>
#include <container/linkedlist.h>
#include <system/allocator.h>
#include <system/log.h>


#define MAX_MESSAGE_LENGTH		1024

const char* c_FileName          = "x3d_log";
const char* c_NormalPrefix      = "x3d_log:";
const char* c_MildErrPrefix     = "x3d_mild:*";
const char* c_SevereErrPrefix   = "x3d_severe:**";
const char* c_CritiErrPrefix    = "x3d_critical:***";

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

        printf("x3drenderlibrary - %s\nx3d log start recording\t%d:%d:%d\n\n",
               X3D_VERSION_STRING, gmtTime->tm_hour, gmtTime->tm_min, gmtTime->tm_sec);
        if ((g_log_inst.normal_bhv | g_log_inst.mild_err_bhv | g_log_inst.severe_err_bhv | g_log_inst.criti_err_bhv) & LOG_OUTPUT_TO_FILE) {
                if (!(g_log_inst.file = fopen(c_FileName, "w" ))) {
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


void log_free ( void )
{
        if (g_log_inst.file) {
                fprintf(g_log_inst.file, "\nx3d log stop recording\n\n");
                fclose(g_log_inst.file);
        }
        alg_free(llist, &g_log_inst.last_ten_errors);
        printf("\nx3d log stop recording\n\n");
        zero_obj(&g_log_inst);
        g_is_init = 0;
}

#define report_log(_functionName, _message, _logState, _prefix)	\
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
	switch ( _behavior ) { \
	case LOG_OUTPUT_TO_CONSOLE: { \
		printf ( "%s<%s> %s\n", (_prefix), (_functionName), (_outputMessage) ); \
		break; \
	} \
\
	case LOG_OUTPUT_TO_FILE: { \
		fprintf ( g_log_inst.file, "%s<%s> %s\n", \
			 (_prefix), (_functionName), (_outputMessage) ); \
		break; \
	} \
\
	case LOG_OUTPUT_TO_BOTH: { \
		printf ( "%s<%s> %s\n", (_prefix), (_functionName), (_outputMessage) ); \
		fprintf ( g_log_inst.file, "%s<%s> %s\n", \
			 (_prefix), (_functionName), (_outputMessage) ); \
		break; \
	} \
	} \
\
	free ( _outputMessage ); \
}

void call_log_normal ( const char* functionName, const char* message, ... )
{
        report_log ( functionName, message, g_log_inst.normal_bhv, c_NormalPrefix );
}

void call_log_mild_err ( const char* functionName, const char* errorMessage, ... )
{
        report_log ( functionName, errorMessage, g_log_inst.mild_err_bhv, c_MildErrPrefix );
}

void call_log_severe_err ( const char* functionName, const char* errorMessage, ... )
{
        report_log ( functionName, errorMessage, g_log_inst.severe_err_bhv, c_SevereErrPrefix );
}

void call_log_critical_err ( const char* functionName, const char* errorMessage, ... )
{
        report_log ( functionName, errorMessage, g_log_inst.criti_err_bhv, c_CritiErrPrefix );
        if ( g_log_inst.to_terminate ) {
                puts ( "The program will be shutdown" );
                if ( g_log_inst.file ) {
                        fprintf ( g_log_inst.file, "The program will be shutdown..." );
                }
                abort ();
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