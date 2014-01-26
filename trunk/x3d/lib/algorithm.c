/* algorithm.c: commonly used algorithms */
#include <algorithm.h>
#include <math/math.h>


// Definitions for GetPatternValue function
#define PATTERN_FORMAT_PREFIX		'['
#define PATTERN_FORMAT_LENGTH		3
#define PATTERN_FORMAT_INT		'i'
#define PATTERN_FORMAT_FLOAT		'f'
#define PATTERN_FORMAT_STRING		's'
#define PATTERN_TYPE_INIT		0X7F
#define PATTERN_MAX_COUNTS		16

// A structure that contains translated pattern
typedef struct PATTERN_DATA_TYP {
	struct {
		int type;
		int pStart;
		int pEnd;
	} region[PATTERN_MAX_COUNTS];
	int nPatterns;
} PATTERN_DATA, *PATTERN_DATA_PTR;


int GetNPrefix ( const char *str, const char **prefix, int *n );
void GetNSuffix ( const char *str, const char **suffix, int *n );


// Compute prefix string ( string before PATTERN_FORMAT_PREFIX occurs )
int GetNPrefix ( const char *str, const char **prefix, int *n )
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

}// End Function GetNPrefix

// Compute suffix string ( string before next PATTERN_FORMAT_PREFIX occurs )
void GetNSuffix ( const char *str, const char **suffix, int *n )
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

}// End Function GetNSuffix

// Convert c string to integer
int StrNToInt ( const char *str, int *iValue, int n )
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

}// End Function StrNToInt

// Convert c string to float
int StrNToFloat ( const char *str, float *fValue, int n )
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

}// End Function StrNToFloat

// Compute value of a c string according to the format template
int GetPatternValue ( const char *cString, const char *format, ... )
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
		assert ( cString[cStrPos] == 0 &&
			 n != 0 );
		if ( !strncmp ( &cString[cStrPos], temp, n ) ||
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
				if ( !StrNToInt ( &cString[startPos], iDest, diff ) ) {
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
				if ( !StrNToFloat ( &cString[startPos], fDest, diff ) ) {
					goto FUNCTION_FAILED;
				}
				break;
			}// End Case

			case PATTERN_FORMAT_STRING: {
				char *sDest = va_arg ( argList, char* );
				assert ( !sDest );
				if ( n != 0 ) {
					strncpy ( sDest, &cString[startPos], cStrPos - startPos );
				} else {
					strcpy ( sDest, &cString[startPos] );
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

}// End Function GetPatternValue

uint32_t hash_str0 ( char *str )
{
	int hash = 0;
	while ( *str != 0 ) {
		hash += 33*(*str);
		str ++;
	}
	return hash;
}

uint32_t hash_str1 ( char *str )
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

uint32_t hash_str2 ( char *str )
{
	uint64_t hash = 0xcbf29ce484222325;
	while ( *str != 0 ) {
		hash ^= *str;
		hash *= 0x100000001b3;
		str ++;
	}
	return hash;
}

uint32_t hash_str3 ( char *str )
{
	uint32_t hash = 0;
	while ( *str ) {
		hash = *str + (hash << 6) + (hash << 16) - hash;
		str ++;
	}
	return hash;
}

uint32_t hash_str4 ( char *str )
{
	uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
	uint32_t hash = 0;
	while ( *str ) {
		hash = hash*seed + *str;
		str ++;
	}
	return hash;
}

// Check if the main string has a substring of subStr
int MatchSubStr ( char *str, char *subStr )
{
	char *oriStr = str;
	char *strBt = str;
	char *subStrBt = subStr;
	while ( *str && *subStr ) {
		if ( *str != *subStr ) {
			subStr = subStrBt;
		} else {
			subStr ++;
		}
		str ++;
	}// End While

	if ( *subStr == 0 ) {
		return str - oriStr;
	} else {
		return -1;
	}// End If

}// End Function MatchSubStr

// Pre-process the substring's next array
void GetKmpNextArr ( char *subStr, char *nextArr[] )
{
}// End Function GetKmpNextArr

// Check if the main string has a substring of subStr using kmp algorithm
int MatchSubStrKmp ( char *str, char *subStr, char *nextArr[] )
{
}// End Function MatchSubStrKmp

// Converts an integer to string in tens
char *UIntToStrTens ( unsigned int n, char *str, int len )
{
	char *strBack = &str[len - 1];
	*strBack = 0;
	while ( n != 0 ) {
		strBack --;
		*strBack = '0' + n%10;
		n /= 10;
	}// End While

	return strBack;

}// End Function IntToStrTens

// Converts an unsigned integer to string in hexidecimal
char *UIntToStrHex ( unsigned int n, char *str, int len )
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

}// End Function IntToStrHex

void set_container_operations ( void *container,
				void (*init_callback) ( void *elm ),
				void (*destroy_callback) ( void *elm ) )
{
	struct container_operations *cont_ops = container;
	cont_ops->init_callback = init_callback;
	cont_ops->destroy_callback = destroy_callback;
}

void create_alg_list ( struct alg_list *list, int elm_size, int init_count )
{
	memset ( list, 0, sizeof ( *list ) );
	if ( init_count < 0 ) {
		list->list = alloc_var ( elm_size, 0 );
	} else {
		list->list = alloc_var ( elm_size, init_count );
	}
	list->elm_size = elm_size;
	list->num_elm = 0;

	if ( list->ops.init_callback ) {
		list->ops.init_callback ( list );
	}
}

void free_alg_list ( struct alg_list *list )
{
	if ( list->ops.destroy_callback ) {
		int i;
		untyped *t = list->list;
		for ( i = 0; i < list->num_elm; i ++ ) {
			list->ops.destroy_callback ( t );
			t += list->elm_size;
		}
	}
	free_var ( list->list );
	memset ( list, 0, sizeof ( *list ) );
}

void add_element_alg_list ( void *elm, struct alg_list *list )
{
	list->list = add_var ( list->list, list->elm_size );
	if ( list->ops.init_callback ) {
		list->ops.init_callback ( elm );
	}
	memcpy ( &list->list[list->num_elm*list->elm_size], elm, list->elm_size );
	list->num_elm ++;
}

void expand_alg_list ( int count, struct alg_list *list )
{
	list->list = expand2_var ( list->list, count );
}

void flush_alg_list ( struct alg_list *list )
{
	flush_var ( list->list );
	list->num_elm = 0;
}

/* Both lists must have the same element type */
void copy_alg_list ( struct alg_list *list0, struct alg_list *list1 )
{
	untyped *t = list1->list;
	memcpy ( list1, list0, sizeof ( *list1 ) );
	list1->list = t;
	list1->list = expand_var ( list1->list, list0->num_elm );
	memcpy ( list1->list, list0->list, list0->num_elm*list0->elm_size );
}

/* swaps the list's pointer respectively */
void swap_alg_list ( struct alg_list *list0, struct alg_list *list1 )
{
	untyped *t =list0->list;
	list0->list = list1->list;
	list1->list = t;
}
