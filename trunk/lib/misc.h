#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef long long       uuid_t;
typedef unsigned char   untyped;
typedef unsigned int    address_t;
typedef void            (*f_Generic)();

#if defined(X3D_COMPILER_GCC)
#  define nullptr                       0
#  define cast(_var)			(typeof(_var))
#  define get_file_name()               (__FILE__)
#  define get_function_name()           (__func__)
#  define get_line_number()             (__LINE__)
#endif // defined X3D_COMPILER_GCC

int alg_match_pattern(const char* cstr, const char* format, ...);

uint32_t alg_hash_str0(char* str);
uint32_t alg_hash_str1(char* str);
uint32_t alg_hash_str2(char* str);
uint32_t alg_hash_str3(char* str);
uint32_t alg_hash_str4(char* str);
uuid_t alg_hash_str_uuid(char* str);

char* alg_str_simplify(char* str );
bool alg_match_suffix(char* str, char* suffix );
char *alg_alloc_string(const char *string );

int alg_match_substring(char *str, char *sub );
void alg_init_kmp_substring(char *sub, char *kmp_array[] );
int alg_match_substring_kmp(char *str, char *sub, char *kmp_array[] );

uuid_t alg_gen_uuid();
void alg_use_uuid(uuid_t id );

int strn_to_int(const char* str, int* value, int n);
int strn_to_float(const char* str, float* value, int n);

char* uint_to_str_tens(unsigned int n, char* str, int len);
char* uint_to_str_hex(unsigned int n, char* str, int len);

bool ishexdigit(int value);


#endif // MISC_H_INCLUDED
