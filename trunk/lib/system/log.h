#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


#include <x3d/common.h>

/*
 * Definitions
 */
#define LOG_OUTPUT_NONDEFINED		0X0
#define LOG_OUTPUT_TO_CONSOLE		0X1
#define LOG_OUTPUT_TO_FILE		0X2
#define LOG_OUTPUT_TO_BOTH		(LOG_OUTPUT_TO_CONSOLE | LOG_OUTPUT_TO_FILE)

/*
 * Functions' declaration
 */

bool log_init ( int to_terminate );
void log_free ( void );

#define log_normal( _message, ... )		call_log_normal ( __func__, _message, ##__VA_ARGS__ )
#define log_mild_err( _message, ... )		call_log_mild_err ( __func__, _message, ##__VA_ARGS__ )
#define log_severe_err( _message, ... )		call_log_severe_err ( __func__, _message, ##__VA_ARGS__ )
#define log_critical_err( _message, ... )	call_log_critical_err ( __func__, _message, ##__VA_ARGS__ )

void call_log_normal ( const char *functionName, const char *message, ... );
void call_log_mild_err ( const char *functionName, const char *errorMessage, ... );
void call_log_severe_err ( const char *functionName, const char *errorMessage, ... );
void call_log_critical_err ( const char *functionName, const char *errorMessage, ... );
void set_log_behavior ( int bhv );


/*
 * Definitions
 */

// Reference names for debug mode
#ifdef X3D_DEBUG_MODE
#define log_normal_dbg				log_normal
#define log_mild_err_dbg			log_mild_err
#define log_severe_err_dbg			log_severe_err
#define log_critical_err_dbg 			log_critical_err
#else
#define log_normal_dbg( ... )
#define log_mild_err_dbg( ... )
#define log_severe_err_dbg( ... )
#define log_critical_err_dbg( ... )
#endif


#endif // LOG_H_INCLUDED
