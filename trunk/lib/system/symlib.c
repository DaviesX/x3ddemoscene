#include <misc.h>
#include <system/log.h>
#include <system/symlib.h>
#include <system/allocator.h>
#include <x3d/init.h>


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

static bool match_symbol ( char* str, address_t* addr, int* size, enum SYMBOL_IDR* type,
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
        char buf[1024] = {0};
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
                        if (ishexdigit(*str)) {
                                buf[ibuf ++] = *str;
                                str ++;
                        } else if(isspace(*str)) {
                                buf[ibuf] = '\0';
                                if(ibuf == 0) return false;
                                ibuf = 0;
                                // parses the hex-address
                                errno = 0;
                                *addr = strtoull(buf, NULL, 16);
                                if (errno == EINVAL || errno == ERANGE) {
                                        kernel_panic();
                                }

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
                                else if ( !strncmp ( "OBJECT", buf, ibuf ) )
                                        *type = SYMBOL_VARIABLE;
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
                             *str == '_' || *str == '@' || *str == '.' ||
                             *str == '(' || *str == ')' ) {
                                buf[ibuf ++] = *str;
                                str ++;
                        } else if ( isspace ( *str ) || *str == '\n' ) {
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

static char* make_temp_file_name ( char* filename )
{
        static char tempname[512];
        strcpy ( tempname, "sym-" );

        char* s = filename;
        while ( *filename != '\0' ) {
                if ( *filename == '/' ) {
                        s = filename + 1;
                }
                filename ++;
        }
        strcat ( tempname, s );
        return tempname;
}

static bool dump ( char* filename, struct symbol_set *symbols )
{
        const char* commandfile = "./etc/readelf";
        const char* option = " -a --wide ";
        char command[512] = {0};
        char tempfile[128] = {0};

        strcat ( tempfile, make_temp_file_name ( filename ) );

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
                                void* handle = dlopen ( filename, RTLD_NOW | RTLD_DEEPBIND );
                                if ( handle == nullptr ) {
                                        log_severe_err_dbg ( "failed to open dynamic linked: %s",
                                                             dlerror () );
                                        fclose ( elf_info );
                                        return false;
                                }
                                symbols->handle = handle;
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
                struct dlsymbol         s_temp;
                address_t               addr;
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
                                alg_push_back ( list, &symbols->symbol[SYMBOL_ABI], s_temp );
                                break;
                        }
                        case SYMBOL_VARIABLE: {
                                s_temp.value = *(void**) &addr;
                                s_temp.name = name;
                                s_temp.size = size;
                                alg_push_back ( list, &symbols->symbol[SYMBOL_MISC], s_temp );
                                break;
                        }
                        case SYMBOL_MISC: {
                                s_temp.value = *(void**) &addr;
                                s_temp.name = name;
                                s_temp.size = size;
                                alg_push_back ( list, &symbols->symbol[SYMBOL_MISC], s_temp );
                                break;
                        }
                        default: {
                                log_mild_err_dbg ( "symbol type: %d not supported yet", type );
                                break;
                        }
                        }
                }
        }
        /* relink the address for dynamic linked */
        if ( type == ELF_DYN ) {
                dlerror();
                int i;
                for ( i = 0; i < cNumSymbolCate; i ++ ) {
                        struct dlsymbol* syms = alg_array(list, &symbols->symbol[i]);
                        int num_sym = alg_n(list, &symbols->symbol[i]);
                        int j;
                        for ( j = 0; j < num_sym; j ++ ) {
                                if (syms[j].name) {
                                        syms[j].func_ptr = dlsym(symbols->handle, syms[j].name);
                                        syms[j].reason   = dlerror();
                                        syms[j].value    = dlsym(symbols->handle, syms[j].name);
                                }
                        }
                }
        }

        fclose ( elf_info );
#if !defined(X3D_DEBUG_MODE)
        remove ( tempfile );
#endif
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


void symlib_init ( struct symbol_set *symbols )
{
        int i;
        for ( i = 0; i < cNumSymbolCate; i ++ ) {
                alg_init ( list, &symbols->symbol[i],
                           sizeof (struct dlsymbol), 100 );
        }
        symbols->filename = nullptr;
        symbols->handle = nullptr;
        symbols->is_loaded = false;
}

void symlib_free(struct symbol_set* symbols)
{
        symlib_unload(symbols);
        int i;
        for (i = 0; i < cNumSymbolCate; i ++) {
                alg_free(list, &symbols->symbol[i]);
        }
}

bool symlib_load ( struct symbol_set* symbols, char* filename )
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

bool symlib_unload ( struct symbol_set* symbols )
{
        if ( !symbols->is_loaded ) {
                log_mild_err_dbg ( "symbol set wasn't loaded" );
                return false;
        }
        log_normal_dbg ( "unloading file %s", symbols->filename );
        int i;
        for (i = 0; i < cNumSymbolCate; i ++) {
                struct dlsymbol* syms   = alg_array(list, &symbols->symbol[i]);
                int num_sym             = alg_n(list, &symbols->symbol[i]);
                int j;
                for (j = 0; j < num_sym; j ++) {
                        free_fix(syms[j].name);
                }
        }
        unload_dyn(symbols);
        symbols->is_loaded      = false;
        symbols->handle         = nullptr;
        free_fix(symbols->filename);
        return true;
}

void symlib_add_abi ( struct symbol_set* symbols, char* name, f_Generic func )
{
        struct dlsymbol         s_temp;
        s_temp.func_ptr = func;
        s_temp.name = alg_alloc_string ( name );
        s_temp.size = 0;
        alg_push_back ( list, &symbols->symbol[SYMBOL_ABI], s_temp );
}

void symlib_add_cstring ( struct symbol_set* symbols, char* name, char* string  )
{
        struct dlsymbol         s_temp;
        s_temp.const_char = alg_alloc_string ( string );
        s_temp.name = alg_alloc_string ( name );
        s_temp.size = strlen ( string ) + 1;
        alg_push_back ( list, &symbols->symbol[SYMBOL_CONST_CSTRING], s_temp );
}

void symlib_add_variable ( struct symbol_set* symbols, char* name, void* value, int size )
{
        struct dlsymbol         s_temp;
        s_temp.value = value;
        s_temp.name = alg_alloc_string ( name );
        s_temp.size = size;
        alg_push_back ( list, &symbols->symbol[SYMBOL_VARIABLE], s_temp );
}

f_Generic symlib_ret_abi ( struct symbol_set* symbols, char* sym_name )
{
        struct dlsymbol* sym = alg_array(list, &symbols->symbol[SYMBOL_ABI]);
        int num_sym = alg_n(list, &symbols->symbol[SYMBOL_ABI]);
        int i;
        for ( i = 0; i < num_sym; i ++ ) {
                if (sym[i].name && !strcmp(sym_name, sym[i].name)) {
                        log_normal_dbg("detailed information about abi - %s:", sym[i].name, sym[i].reason);
                        return sym[i].func_ptr;
                }
        }
        log_mild_err_dbg("couldn't find such symbol as: %s", sym_name);
        return nullptr;
}

f_Generic symlib_ret_abi2(struct symbol_set* symbols, void* data, f_Match_Name f_match_name)
{
        struct dlsymbol* sym = alg_array(list, &symbols->symbol[SYMBOL_ABI]);
        int num_sym = alg_n(list, &symbols->symbol[SYMBOL_ABI]);
        int i;
        for (i = 0; i < num_sym; i ++) {
                if (sym[i].name && f_match_name(data, sym[i].name)) {
                        log_normal_dbg("detailed information about abi - %s:", sym[i].name, sym[i].reason);
                        return sym[i].func_ptr;
                }
        }
        log_mild_err_dbg("couldn't find symbol by such matcher: %x", f_match_name);
        return nullptr;
}

void* symlib_ret_variable ( struct symbol_set* symbols, char* sym_name, int* size )
{
        struct dlsymbol* sym = alg_array ( list, &symbols->symbol[SYMBOL_ABI] );
        int num_sym = alg_n(list, &symbols->symbol[SYMBOL_ABI]);
        int i;
        for ( i = 0; i < num_sym; i ++ ) {
                if ( sym[i].name && !strcmp ( sym_name, sym[i].name ) )
                        if ( size != nullptr )
                                *size = sym[i].size;
                return sym[i].value;
        }
        log_mild_err_dbg ( "couldn't find such symbol as: %s", sym_name );
        return nullptr;
}

struct dlsymbol* symlib_ret_misc ( struct symbol_set* symbols, char* sym_name )
{
        struct dlsymbol* sym = alg_array ( list, &symbols->symbol[SYMBOL_MISC] );
        int num_sym = alg_n(list, &symbols->symbol[SYMBOL_MISC]);
        int i;
        for ( i = 0; i < num_sym; i ++ ) {
                if ( sym[i].name && !strcmp ( sym_name, sym[i].name ) )
                        return &sym[i];
        }
        log_mild_err_dbg ( "couldn't find such symbol as: %s", sym_name );
        return nullptr;
}
