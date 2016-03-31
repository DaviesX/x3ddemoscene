#ifndef FILESYSTEM_H_INCLUDED
#define FILESYSTEM_H_INCLUDED


#include <x3d/common.h>

/*
 * <directory> decl
 */
struct directory {
        int     uid;
        char*   path;
};
/*
 * <directory> public
 */
void direct_init(struct directory* self, int uid, const char* path);
int direct_get_uid(struct directory* self);
const char* direct_get_path(struct directory* self);


/*
 * <file> decl
 */
struct file {
        int     uid;
        char*   file_name;
};
/*
 * <file> public
 */
void file_init(struct file* self, int uid, const char* file_name);
int file_get_uid(struct file* self);
const char* file_get_name(struct file* self);


struct file_system;

typedef void (*f_FileSys_Free) (struct file_system* self);
typedef bool (*f_FileSys_Open_Directory) (struct file_system* self, const char* dir, struct directory* direct);
typedef bool (*f_FileSys_Base_Directory) (struct file_system* self, struct directory* direct);
typedef bool (*f_FileSys_Create_Subdirectory) (struct file_system* self, const char* path,
                                               struct directory* direct, struct directory* new_direct);
typedef bool (*f_FileSys_Open_Subdirectory) (struct file_system* self, const char* subpath, struct directory* direct);
typedef bool (*f_FileSys_Open_Parent) (struct file_system* self, struct directory* direct);
typedef bool (*f_FileSys_Close_Directory)(struct file_system* self, struct directory* direct);

typedef bool (*f_Open_File) (struct file_system* self, struct directory* direct, const char* file_name);
typedef struct file* (*f_Create_File) (struct file_system* self, struct directory* direct, const char* file_name);
typedef uint64_t (*f_File_Size) (struct file_system* self, struct file* file);
typedef uint64_t (*f_Read_File) (struct file_system* self, uint64_t bytes, struct file* file, char* var_buf);
typedef uint64_t (*f_Write_File) (struct file_system* self, uint64_t bytes, struct file* file, char* buf);
typedef bool (*f_Close_File) (struct file_system* self, struct file* file);

/*
 * <filesystem> decl
 */
struct file_system {
        f_FileSys_Free           f_free;
        f_FileSys_Open_Directory f_open_directory;
        f_FileSys_Base_Directory f_base_directory;
        f_FileSys_Create_Subdirectory f_create_subdirectory;
        f_FileSys_Open_Subdirectory   f_open_subdirectory;
        f_FileSys_Open_Parent    f_open_parent;
        f_FileSys_Close_Directory f_close_directory;

        f_Open_File             f_open_file;
        f_Create_File           f_create_file;
        f_File_Size             f_file_size;
        f_Read_File             f_read_file;
        f_Write_File            f_write_file;
        f_Close_File            f_close_file;
};
/*
 * <filesystem> public
 */
void filesys_init(struct file_system* self,
                  f_FileSys_Free           f_free,
                  f_FileSys_Open_Directory f_open_directory,
                  f_FileSys_Base_Directory f_base_directory,
                  f_FileSys_Create_Subdirectory f_create_subdirectory,
                  f_FileSys_Open_Subdirectory   f_open_subdirectory,
                  f_FileSys_Open_Parent    f_open_parent,
                  f_FileSys_Close_Directory f_close_directory,

                  f_Open_File             f_open_file,
                  f_Create_File           f_create_file,
                  f_File_Size             f_file_size,
                  f_Read_File             f_read_file,
                  f_Write_File            f_write_file,
                  f_Close_File            f_close_file);
void filesys_free(struct file_system* self);
bool filesys_open_directory(struct file_system* self, const char* dir, struct directory* direct);
bool filesys_base_directory(struct file_system* self, struct directory* direct);
bool filesys_create_subdirectory(struct file_system* self, const char* path,
                                 struct directory* direct, struct directory* new_direct);
bool filesys_open_subdirectory(struct file_system* self, const char* subpath, struct directory* direct);
bool filesys_open_parent(struct file_system* self, struct directory* direct);
bool filesys_close_directory(struct file_system* self, struct directory* direct);

bool filesys_open_file(struct file_system* self, struct directory* direct, const char* file_name);
struct file* filesys_create_file(struct file_system* self, struct directory* direct, const char* file_name);
uint64_t filesys_file_size(struct file_system* self, struct file* file);
uint64_t filesys_read_file(struct file_system* self, uint64_t bytes, struct file* file, char* var_buf);
uint64_t filesys_write_file(struct file_system* self, uint64_t bytes, struct file* file, char* buf);
bool filesys_close_file(struct file_system* self, struct file* file);


#endif // FILESYSTEM_H_INCLUDED
