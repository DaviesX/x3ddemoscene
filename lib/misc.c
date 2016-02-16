#include <system/thread.h>
#include <system/allocator.h>
#include <math/math.h>
#include <misc.h>

// Definitions for alg_match_pattern function
#define PATTERN_FORMAT_PREFIX		'['
#define PATTERN_FORMAT_LENGTH		3
#define PATTERN_FORMAT_INT		'i'
#define PATTERN_FORMAT_FLOAT		'f'
#define PATTERN_FORMAT_STRING		's'
#define PATTERN_TYPE_INIT		0X7F
#define PATTERN_MAX_COUNTS		16

volatile static uuid_t          g_uuid = 0;
static struct thread_trap          g_uuid_trap = {0};

/* contains translated pattern */
typedef struct PATTERN_DATA_TYP {
        struct {
                int type;
                int pStart;
                int pEnd;
        } region[PATTERN_MAX_COUNTS];
        int nPatterns;
} PATTERN_DATA, *PATTERN_DATA_PTR;


static int GetNPrefix ( const char *str, const char **prefix, int *n );
static void GetNSuffix ( const char *str, const char **suffix, int *n );


/* Compute prefix string ( string before PATTERN_FORMAT_PREFIX occurs ) */
static int GetNPrefix ( const char *str, const char **prefix, int *n )
{
        int pos = 0;
        while ( str[pos] != 0 ) {
                if ( str[pos] == PATTERN_FORMAT_PREFIX ) {
                        *n = pos;
                        *prefix = str;
                        return 1;
                }// End If

                pos ++;
        }// End While

        return 0;
}

/* Compute suffix string ( string before next PATTERN_FORMAT_PREFIX occurs ) */
static void GetNSuffix ( const char *str, const char **suffix, int *n )
{
        int pos = 0;
        while ( str[pos] != 0 ) {
                if ( str[pos] == PATTERN_FORMAT_PREFIX ) {
                        break;
                }
                pos ++;
        }// End While
        *n = pos;
        *suffix = str;
}

/* Convert c string to integer */
int strn_to_int ( const char *str, int *iValue, int n )
{
        int temp = 0;
        int pos;
        int sign;
        if ( str[0] == '-' ) {
                pos = 1;
                sign = -1;
        } else {
                pos = 0;
                sign = 1;
        }// End If

        while ( pos < n ||
                (n == 0 && str[pos] != 0) ) {
                if ( str[pos] < '0' ||
                     str[pos] > '9' ) {
                        return 0;
                }
                temp *= 10;
                temp += str[pos] - '0';
                pos ++;
        }// End While

        *iValue = sign*temp;
        return 1;
}

/* Convert c string to float */
int strn_to_float ( const char *str, float *fValue, int n )
{
        unsigned int iTemp = 0;
        unsigned int fTemp = 0;
        int pos;
        float sign;
        unsigned int floatCount = 1;
        int exponent = 0;
        int exSign;

        // Process sign
        if ( str[0] == '-' ) {
                pos = 1;
                sign = -1.0f;
        } else {
                pos = 0;
                sign = 1.0f;
        }// End If

        // Convert integer part
        while ( pos < n ||
                (n == 0 && str[pos] != 0) ) {
                if ( str[pos] < '0' || str[pos] > '9' ) {
                        if ( str[pos] == '.' ||
                             str[pos] == 'e' || str[pos] == 'E' ) {
                                break;
                        } else {
                                return 0;
                        }
                }// End If

                iTemp *= 10;
                iTemp += str[pos] - '0';
                pos ++;
        }// End While

        // Convert float part
        if ( str[pos] == '.' ) {
                pos ++;
                while ( pos < n ||
                        (n == 0 && str[pos] != 0) ) {
                        if ( str[pos] < '0' || str[pos] > '9' ) {
                                if ( str[pos] == 'e' || str[pos] == 'E' ) {
                                        // Convert exponent
                                        pos ++;
                                        if ( str[pos] == '-' ) {
                                                exSign = -1;
                                                pos ++;
                                        } else {
                                                exSign = 1;
                                        }

                                        while ( pos < n ||
                                                (n == 0 && str[pos] != 0) ) {
                                                if ( str[pos] < '0' || str[pos] > '9' ) {
                                                        return 0;
                                                }
                                                exponent *= 10;
                                                exponent += str[pos] - '0';
                                                pos ++;

                                        }// End While

                                        exponent *= exSign;
                                        float fMerged = (float)iTemp + (float)fTemp/(float)floatCount;
                                        *fValue = sci_to_float ( fMerged, exponent )*sign;
                                        return 1;
                                } else {
                                        return 0;
                                }// End If

                        }// End If

                        fTemp = fTemp*10 + (str[pos] - '0');
                        floatCount *= 10;
                        pos ++;
                }// End While
        }// End If ( Float part )

        *fValue = (float) iTemp + (float)fTemp/(float)floatCount;
        return 1;

}

/* Compute value of a c string according to the format template */
int alg_match_pattern ( const char *cstr, const char *format, ... )
{
        va_list argList;
        va_start ( argList, format );

        const char *temp;
        char type = PATTERN_TYPE_INIT;
        int currentPos = 0;
        int cStrPos = 0;
        int startPos;
        int n;

        if ( !GetNPrefix ( format, &temp, &n ) ) {
                goto FUNCTION_FAILED;
        }
        currentPos += n + PATTERN_FORMAT_LENGTH;
        while ( 1 ) {
                assert ( cstr[cStrPos] == 0 &&
                         n != 0 );
                if ( !strncmp ( &cstr[cStrPos], temp, n ) ||
                     n == 0 ) {
                        switch ( type ) {
                        case PATTERN_FORMAT_INT: {
                                int diff;
                                if ( n == 0 ) {
                                        diff = 0;
                                } else {
                                        diff = cStrPos - startPos;
                                }
                                int *iDest = va_arg ( argList, int* );
                                assert ( !iDest );
                                if ( !strn_to_int ( &cstr[startPos], iDest, diff ) ) {
                                        goto FUNCTION_FAILED;
                                }
                                break;
                        }// End Case

                        case PATTERN_FORMAT_FLOAT: {
                                int diff;
                                if ( n == 0 ) {
                                        diff = 0;
                                } else {
                                        diff = cStrPos - startPos;
                                }
                                float *fDest = va_arg ( argList, float* );
                                assert ( !fDest );
                                if ( !strn_to_float ( &cstr[startPos], fDest, diff ) ) {
                                        goto FUNCTION_FAILED;
                                }
                                break;
                        }// End Case

                        case PATTERN_FORMAT_STRING: {
                                char *sDest = va_arg ( argList, char* );
                                assert ( !sDest );
                                if ( n != 0 ) {
                                        strncpy ( sDest, &cstr[startPos], cStrPos - startPos );
                                } else {
                                        strcpy ( sDest, &cstr[startPos] );
                                }
                                break;
                        }// End Case

                        case PATTERN_TYPE_INIT: {
                                break;
                        }// End Case

                        default:
                                goto FUNCTION_FAILED;
                        }// End Switch

                        cStrPos += n;
                        startPos = cStrPos;

                        if ( format[currentPos - PATTERN_FORMAT_LENGTH] == 0 ) {
                                goto FUNCTION_SUCCESS;
                        } else {
                                type = format[currentPos - 2];
                                GetNSuffix ( &format[currentPos], &temp, &n );
                                currentPos += n + PATTERN_FORMAT_LENGTH;
                        }// End If
                }// End If

                cStrPos ++;
        }// End While

FUNCTION_SUCCESS:
        va_end ( argList );
        return 1;

FUNCTION_FAILED:
        va_end ( argList );
        return 0;

}

/* a series of string hashing functions */
uint32_t alg_hash_str0 ( char *str )
{
        uint32_t hash = 0;
        while ( *str != 0 ) {
                hash += 33*(*str);
                str ++;
        }
        return hash;
}

uint32_t alg_hash_str1 ( char *str )
{
        uint32_t hash = 0;
        while ( *str ) {
                hash += *str;
                hash += (hash << 10);
                hash ^= (hash >> 6);
                str ++;
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
}

uint32_t alg_hash_str2 ( char *str )
{
        uint64_t hash = 0xcbf29ce484222325;
        while ( *str != 0 ) {
                hash ^= *str;
                hash *= 0x100000001b3;
                str ++;
        }
        return (uint32_t) ((hash >> 32) ^ hash);
}

uint32_t alg_hash_str3 ( char *str )
{
        uint32_t hash = 0;
        while ( *str ) {
                hash = *str + (hash << 6) + (hash << 16) - hash;
                str ++;
        }
        return hash;
}

uint32_t alg_hash_str4 ( char *str )
{
        uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
        uint32_t hash = 0;
        while ( *str ) {
                hash = hash*seed + *str;
                str ++;
        }
        return hash;
}

uuid_t alg_hash_str_uuid ( char *str )
{
        uint64_t hash = 0xcbf29ce484222325;
        while ( *str != 0 ) {
                hash ^= *str;
                hash *= 0x100000001b3;
                str ++;
        }
        return (uuid_t) hash;
}

char* alg_str_simplify ( char* str )
{
        char* t = str;
        char* dest = str;
        int state = 0;
        while ( *str != '\0' ) {
                switch ( state ) {
                case 0: {       /* eat spaces and convert tabs to spaces */
                        if ( *str == ' ' || *str == '\t' ) {
                                state = 0;
                                str ++;
                        } else
                                state = 1;
                        break;
                }
                case 1: {
                        if ( *str == '\t' )
                                *dest = ' ';
                        *dest = *str;
                        if ( *str == ' ' || *str == '\t' ) {
                                state = 0;
                        }
                        str ++;
                        dest ++;
                        break;
                }
                }
        }
        *dest = '\0';
        return t;
}

bool alg_match_suffix ( char* str, char* suffix )
{
        char *oe = str + strlen ( str );
        char *se = suffix + strlen ( suffix );
        do {
                if ( *oe != *se )
                        return false;
                oe ++;
                se ++;
        } while ( oe != str && se != suffix );
        return true;
}

char* alg_alloc_string ( const char *string )
{
        int l = strlen ( string ) + 1;
        char* s = alloc_fix ( 1, l );
        memcpy ( s, string, l );
        return s;
}

// Check if the main string has a substring of sub
int alg_match_substring ( char *str, char *sub )
{
        char *ori = str;
        char *restart = sub;
        while ( *str && *sub ) {
                if ( *str != *sub ) {
                        sub = restart;
                } else {
                        sub ++;
                }
                str ++;
        }
        if ( *sub == 0 ) {
                return str - ori;
        } else {
                return -1;
        }
}

/* Pre-process the substring's next array */
void alg_init_kmp_substring ( char *sub, char *kmp_array[] )
{
}

/* Check if the main string has a substring of subStr using kmp algorithm */
int alg_match_substring_kmp ( char *str, char *sub, char *kmp_array[] )
{
        return -1;
}

/* Converts an integer to string in tens */
char *uint_to_str_tens ( unsigned int n, char *str, int len )
{
        char *back = &str[len - 1];
        *back = 0;
        while ( n != 0 ) {
                back --;
                *back = '0' + n%10;
                n /= 10;
        }
        return back;
}

// Converts an unsigned integer to string in hexidecimal
char *uint_to_str_hex ( unsigned int n, char *str, int len )
{
        char *strBack = &str[len - 1];
        *strBack = 0;
        while ( n != 0 ) {
                strBack --;
                *strBack = "012345678ABCDEF"[n%16];
                n /= 16;
        }// End While

        *(-- strBack) = 'X';
        *(-- strBack) = '0';
        return strBack;
}


bool ishexdigit ( int value )
{
        if ( (value >= '0' && value <= '9') ||
             (value >= 'a' && value <= 'f') ||
             (value >= 'A' && value <= 'F') )
                return true;
        else
                return false;
}


uuid_t alg_gen_uuid ( void )
{
        thread_trap_on_task(&g_uuid_trap);
        uuid_t x = g_uuid ++;
        thread_untrap_task(&g_uuid_trap);
        return x;
}

void alg_use_uuid ( uuid_t id )
{
        thread_trap_on_task(&g_uuid_trap);
        g_uuid = max(id, g_uuid);
        thread_untrap_task(&g_uuid_trap);
}

char** misc_get_command_params(char* command, int argc, char* argv[], int* n_matched)
{
        char** params = alloc_fix(sizeof(char*), argc);
        int j = 0;
        int i;
        for (i = 0; i < argc - 1; i ++) {
                if (!strcmp(command, argv[i]) && i + 1 < argc) {
                        params[j ++] = argv[i + 1];
                }
        }
        *n_matched = j;
        return params;
}
