/* logout.c: All log handling functions go here */
#include <x3d/common.h>
#include <logout.h>


#define MAX_MESSAGE_LENGTH		1024

const char *FileName = "x3d_log";
const char *NormalPrefix = "x3d_log:";
const char *MildErrPrefix = "x3d_mild: *";
const char *SevereErrPrefix = "x3d_severe: **";
const char *CritiErrPrefix = "x3d_critical: ***";

struct log_output {
        FILE *file;
        int normal_bhv;		/* Sort of behavior is gonna purpose */
        int mild_err_bhv;
        int severe_err_bhv;
        int criti_err_bhv;
        int to_terminate;
        int tmp_bhv;
};

struct log_output g_log_inst;
int g_is_init = 0;


int init_log_output ( int to_terminate )
{
        if ( g_is_init ) {
                free_log_output ();
        }
        memset ( &g_log_inst, 0, sizeof ( g_log_inst ) );
        g_log_inst.normal_bhv = LOG_OUTPUT_TO_CONSOLE;
        g_log_inst.mild_err_bhv = LOG_OUTPUT_TO_CONSOLE;
        g_log_inst.severe_err_bhv = LOG_OUTPUT_TO_BOTH;
        g_log_inst.criti_err_bhv = LOG_OUTPUT_TO_BOTH;
        g_log_inst.to_terminate = to_terminate;

        time_t timeT;
        time ( &timeT );
        struct tm *gmtTime = gmtime ( &timeT );

        printf ( "x3drenderlibrary - %s\nx3d log start recording\t%d:%d:%d\n\n",
                 RENDER_LIB_VERSION_STRING, gmtTime->tm_hour, gmtTime->tm_min, gmtTime->tm_sec );
        if ( (g_log_inst.normal_bhv | g_log_inst.mild_err_bhv |
              g_log_inst.severe_err_bhv | g_log_inst.criti_err_bhv) &
             LOG_OUTPUT_TO_FILE ) {
                if ( !(g_log_inst.file = fopen ( FileName, "w" )) ) {
                        return 0;
                }
                fprintf ( g_log_inst.file, "x3drenderlibrary - %s\nx3d log start recording\t%d:%d:%d\n\n",
                          RENDER_LIB_VERSION_STRING, gmtTime->tm_hour, gmtTime->tm_min, gmtTime->tm_sec );
        }
        g_is_init = 1;
        return 1;
}


void free_log_output ( void )
{
        if ( g_log_inst.file ) {
                fprintf ( g_log_inst.file, "x3d log stop recording\n\n" );
                fclose ( g_log_inst.file );
        }
        printf ( "x3d log stop recording\n\n" );
        memset ( &g_log_inst, 0, sizeof ( g_log_inst ) );
        g_is_init = 0;
}

#define report_log(_functionName, _message, _logState, _prefix)	\
{ \
	if ( !g_is_init ) { \
		/* Initialize log reporter, if it is not */ \
		init_log_output ( 1 ); \
	} \
\
	va_list _args; \
	va_start ( _args, (_message) ); \
	char *_outputMessage; \
	if ( -1 == vasprintf ( &_outputMessage, (_message), _args ) ) { \
		printf ( "Error in allocating memory for Log Reporter\n" ); \
		abort (); \
	} \
\
	va_end ( _args ); \
\
	int _behavior; \
	if ( !g_log_inst.tmp_bhv ) { \
		_behavior = (_logState); \
	} else { \
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

void call_log_normal ( const char *functionName, const char *message, ... )
{
        report_log ( functionName, message, g_log_inst.normal_bhv, NormalPrefix );
}

void call_log_mild_err ( const char *functionName, const char *errorMessage, ... )
{
        report_log ( functionName, errorMessage, g_log_inst.mild_err_bhv, MildErrPrefix );
}

void call_log_severe_err ( const char *functionName, const char *errorMessage, ... )
{
        report_log ( functionName, errorMessage, g_log_inst.severe_err_bhv, SevereErrPrefix );
}

void call_log_critical_err ( const char *functionName, const char *errorMessage, ... )
{
        report_log ( functionName, errorMessage, g_log_inst.criti_err_bhv, CritiErrPrefix );
        if ( g_log_inst.to_terminate ) {
                puts ( "The program will be shutdown" );
                if ( g_log_inst.file ) {
                        fprintf ( g_log_inst.file, "The program will be shutdown..." );
                }
                abort ();
        }
}

void set_log_behavior ( int bhv )
{
        g_log_inst.tmp_bhv = bhv;
}
