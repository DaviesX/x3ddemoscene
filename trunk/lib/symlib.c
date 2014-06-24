#include <algorithm.h>
#include <logout.h>
#include <symlib.h>


#if defined(X3D_PLATFORM_POSIX)
#include <dlfcn.h>

enum ELF_TYPE {
        ELF_UNDEF,
        ELF_EXE,
        ELF_DYN
};

static bool match_type ( char* str, enum ELF_TYPE* type )
{
        if ( !strncmp ( "Type: DYN (Shared object file)", str,
                        strlen ( "Type: DYN (Shared object file)" ) ) ) {
                *type = ELF_DYN;
                return true;
        } else if ( !strncmp ( "Type: EXEC (Executable file)", str,
                               strlen ( "Type: EXEC (Executable file)" ) ) ) {
                *type = ELF_EXE;
                return true;
        } else {
                *type = ELF_UNDEF;
                return false;
        }
}

static bool match_symbol ( char* str, uint32_t* addr, int* size, enum SYMBOL_IDR* type,
                           char** symbol_name )
{
        /* Num:    Value    Size    Type    Bind   Vis       Ndx  Name
         *  29: 000003c0       0    FUNC    LOCAL  DEFAULT    9   deregister_tm_clones */
        enum STATE {
                NUM,
                VALUE,
                SIZE,
                TYPE,
                BIND,
                VIS,
                NDX,
                NAME
        };
        int state = NUM;
        char buf[64] = {0};
        int ibuf = 0;

        while ( *str != '\0' ) {
                switch ( state ) {
                case NUM: {
                        if ( isdigit ( *str ) ) {
                                str ++;
                        } else if ( *str == ':' ) {
                                str ++;
                        } else if ( isspace ( *str ) ) {
                                state = VALUE;
                                str ++;
                        } else {
                                return false;
                        }
                        break;
                }
                case VALUE: {
                        if ( isdigit ( *str ) ) {
                                buf[ibuf ++] = *str;
                                str ++;
                        } else if ( isspace ( *str ) ) {
                                buf[ibuf] = '\0';
                                if ( ibuf == 0 )
                                        return false;
                                ibuf = 0;
                                sscanf ( buf, "%x", addr );

                                state = SIZE;
                                str ++;
                        } else {
                                return false;
                        }
                        break;
                }
                case SIZE: {
                        if ( isdigit ( *str ) ) {
                                buf[ibuf ++] = *str;
                                str ++;
                        } else if ( isspace ( *str ) ) {
                                buf[ibuf] = '\0';
                                if ( ibuf == 0 )
                                        return false;
                                ibuf = 0;
                                sscanf ( buf, "%u", size );

                                state = TYPE;
                                str ++;
                        } else {
                                return false;
                        }
                        break;
                }
                case TYPE: {
                        if ( isalpha ( *str ) ) {
                                buf[ibuf ++] = *str;
                                str ++;
                        } else if ( isspace ( *str ) ) {
                                buf[ibuf] = '\0';
                                if ( ibuf == 0 )
                                        return false;

                                if ( !strncmp ( "NOTYPE", buf, ibuf ) )
                                        *type = SYMBOL_MISC;
                                else if ( !strncmp ( "FUNC", buf, ibuf ) )
                                        *type = SYMBOL_ABI;
                                else
                                        *type = SYMBOL_MISC;

                                ibuf = 0;
                                state = BIND;
                                str ++;
                        } else {
                                return false;
                        }
                        break;
                }
                case BIND: {
                        if ( isalpha ( *str ) ) {
                                str ++;
                        } else if ( isspace ( *str ) ) {
                                state = VIS;
                                str ++;
                        } else {
                                return false;
                        }
                        break;
                }
                case VIS: {
                        if ( isalpha ( *str ) ) {
                                str ++;
                        } else if ( isspace ( *str ) ) {
                                state = NDX;
                                str ++;
                        } else {
                                return false;
                        }
                        break;
                }
                case NDX: {
                        if ( isdigit ( *str ) || isalpha ( *str ) ) {
                                str ++;
                        } else if ( isspace ( *str ) ) {
                                state = NAME;
                                str ++;
                        } else {
                                return false;
                        }
                        break;
                }
                case NAME: {
                        if ( isalpha ( *str ) || isdigit ( *str ) ||
                             *str == '_' || *str == '@' ) {
                                buf[ibuf ++] = *str;
                                str ++;
                        } else if ( isspace ( *str ) || *str == '\0' ) {
                                buf[ibuf] = '\0';
                                if ( ibuf == 0 ) {
                                        *symbol_name = nullptr;
                                        return true;
                                }
                                ibuf = 0;

                                *symbol_name = alg_alloc_string ( buf );
                                return true;
                        } else {
                                return false;
                        }
                        break;
                }
                }
        }
        return false;
}

static bool dump ( char* filename, struct symbol_set *symbols )
{
        const char* tempfile = "temp";
        const char* commandfile = "./etc/readelf";
        const char* option = " -a ";
        char command[512] = {0};

        strcat ( command, commandfile );
        strcat ( command, option );
        strcat ( command, filename );
        strcat ( command, ">" );
        strcat ( command, tempfile );
        int ret = system ( command );

        if ( ret != 0 ) {
                log_severe_err_dbg ( "cannot execute command: %s", command );
                FILE* test = fopen ( filename, "rb" );
                if ( test == nullptr )
                        log_severe_err_dbg ( "cannot find executable: %s", filename );
                else {
                        log_severe_err_dbg ( "elf file exist: %s", filename );
                        fclose ( test );
                        test = fopen ( commandfile, "rb" );
                        if ( test == nullptr )
                                log_severe_err_dbg ( "cannot find readelf util from: %s", commandfile );
                        else {
                                log_severe_err_dbg ( "unknown error" );
                                fclose ( test );
                        }
                }
                return false;
        }

        FILE* elf_info = fopen ( tempfile, "r" );
        if ( elf_info == nullptr ) {
                log_severe_err_dbg ( "elf info file: %s not found", tempfile );
                return false;
        }

        char buf[1024] = {0};
        enum ELF_TYPE type = ELF_UNDEF;
        while ( nullptr != fgets ( buf, sizeof buf, elf_info ) ) {
                alg_str_simplify ( buf );

                /* identify the type of the elf file */
                if ( type == ELF_UNDEF ) {
                        bool is_matched = match_type ( buf, &type );
                        switch ( type ) {
                        case ELF_EXE: {
                                log_normal_dbg ( "dumping elf executable: %s", filename );
                                symbols->handle = nullptr;
                                break;
                        }
                        case ELF_DYN: {
                                log_normal_dbg ( "dumping elf dynamic linked: %s", filename );
                                void* handle = dlopen ( filename, RTLD_NOW );
                                if ( handle == nullptr ) {
                                        log_severe_err_dbg ( "failed to open dynamic linked: %s",
                                                             dlerror () );
                                        fclose ( elf_info );
                                        return false;
                                }
                                break;
                        }
                        default: {
                                if ( is_matched ) {
                                        log_severe_err_dbg ( "unknown file type: %d", type );
                                        fclose ( elf_info );
                                        return false;
                                } else
                                        break;
                        }
                        }
                }
                /* load in the symbols */
#define cmp_symbol( _info, _elm )       ()
                struct dlsymbol         s_temp;
                unsigned int            addr;
                int                     size;
                enum SYMBOL_IDR         type;
                char*                   name;
                if ( match_symbol ( buf, &addr, &size, &type, &name ) ) {
                        log_normal_dbg ( "found: %x\t%u\t%u\t%s", addr, size, type, name );
                        switch ( type ) {
                        case SYMBOL_ABI: {
                                s_temp.func_ptr = *(f_Generic *) &addr;
                                s_temp.name = name;
                                s_temp.size = size;
                                alg_push_back ( list, &s_temp, &symbols->symbol[SYMBOL_ABI] );
                                break;
                        }
                        case SYMBOL_MISC: {
                                s_temp.value = *(void**) &addr;
                                s_temp.name = name;
                                s_temp.size = size;
                                alg_push_back ( list, &s_temp, &symbols->symbol[SYMBOL_MISC] );
                                break;
                        }
                        default: {
                                log_mild_err_dbg ( "symbol type: %d not supported yet", type );
                                break;
                        }
                        }
                }
#undef cmp_symbol
        }

        fclose ( elf_info );
        remove ( tempfile );
        return true;
}

static bool unload_dyn ( struct symbol_set *symbols )
{
        if ( !symbols->handle ) {
                log_normal_dbg ( "symbol file: %s is not dynamic linked", symbols->filename );
                return true;
        }
        if ( !dlclose ( symbols->handle ) ) {
                symbols->handle = nullptr;
                return true;
        } else {
                log_mild_err_dbg ( "failed to unload dynamic linked: %s", symbols->filename );
                log_mild_err_dbg ( "system prompted error: %s", dlerror () );
                return false;
        }
}
#elif defined(X3D_PLATFORM_WIN32)
#endif


void init_symlib ( struct symbol_set *symbols )
{
        int i;
        for ( i = 0; i < cNumSymbolCate; i ++ ) {
                alg_init ( list, sizeof (struct dlsymbol), 100,
                           &symbols->symbol[i] );
        }
        symbols->filename = nullptr;
        symbols->handle = nullptr;
        symbols->is_loaded = false;
}

void free_symlib ( struct symbol_set *symbols )
{
        int i;
        for ( i = 0; i < cNumSymbolCate; i ++ ) {
                alg_free ( list, &symbols->symbol[i] );
        }
        symlib_unload ( symbols );
}

bool symlib_load ( char *filename, struct symbol_set *symbols )
{
        if ( symbols->is_loaded ) {
                log_mild_err_dbg ( "symbol set has been occupied by file: %s",
                                   symbols->filename );
                log_mild_err_dbg ( "the specified file: %s won't be loaded", filename );
                return false;
        }
        log_normal_dbg ( "loading file %s", filename );
        symbols->filename = alg_alloc_string ( filename );
        symbols->handle = nullptr;
        symbols->is_loaded = false;

        if ( !dump ( filename, symbols ) ) {
                log_severe_err_dbg ( "error occurred when loading symbols from %s", filename );
                return false;
        }
        symbols->is_loaded = true;
        return true;
}

bool symlib_unload ( struct symbol_set *symbols )
{
        if ( !symbols->is_loaded ) {
                log_mild_err_dbg ( "symbol set wasn't loaded" );
                return false;
        }
        log_normal_dbg ( "unloading file %s", symbols->filename );
        unload_dyn ( symbols );
        symbols->is_loaded = false;
        symbols->handle = nullptr;
        free_fix ( symbols->filename );
        return true;
}

void symlib_add_abi ( char *name, f_Generic func, struct symbol_set *symbols )
{
}

void symlib_add_cstring ( char *name, char *string, struct symbol_set *symbols )
{
}

void symlib_add_variable ( char *name, void *value, int size, struct symbol_set *symbols )
{
}

f_Generic symlib_ret_abi ( char *sym_name, struct symbol_set *symbols )
{
        return nullptr;
}

struct dlsymbol* symlib_ret_misc ( char *sym_name, struct symbol_set *symbols )
{
        return nullptr;
}
