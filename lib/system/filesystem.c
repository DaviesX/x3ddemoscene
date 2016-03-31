#include <misc.h>
#include <system/filesystem.h>


/*
 * <directory> public
 */
void direct_init(struct directory* self, int uid, const char* path)
{
        self->uid = uid;
        self->path = alg_alloc_string(path);
}

int direct_get_uid(struct directory* self)
{
        return self->uid;
}

const char* direct_get_path(struct directory* self)
{
        return self->path;
}


/*
 * <file> public
 */
void file_init(struct file* self, int uid, const char* file_name)
{
        self->uid = uid;
        self->file_name = alg_alloc_string(file_name);
}

int file_get_uid(struct file* self)
{
        return self->uid;
}

const char* file_get_name(struct file* self)
{
        return self->file_name;
}



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
                  f_Close_File            f_close_file)
{
        self->f_free = f_free;
        self->f_open_directory = f_open_directory;
        self->f_base_directory = f_base_directory;
        self->f_create_subdirectory = f_create_subdirectory;
        self->f_open_subdirectory = f_open_subdirectory;
        self->f_open_parent = f_open_parent;
        self->f_close_directory = f_close_directory;

        self->f_open_file = f_open_file;
        self->f_create_file = f_create_file;
        self->f_file_size = f_file_size;
        self->f_read_file = f_read_file;
        self->f_write_file = f_write_file;
        self->f_close_file = f_close_file;
}

void filesys_free(struct file_system* self)
{
        self->f_free(self);
}

bool filesys_open_directory(struct file_system* self, const char* dir, struct directory* direct)
{
        return self->f_open_directory(self, dir, direct);
}

bool filesys_base_directory(struct file_system* self, struct directory* direct)
{
        return self->f_base_directory(self, direct);
}

bool filesys_create_subdirectory(struct file_system* self, const char* path,
                                 struct directory* direct, struct directory* new_direct)
{
        return self->f_create_subdirectory(self, path, direct, new_direct);
}

bool filesys_open_subdirectory(struct file_system* self, const char* subpath, struct directory* direct)
{
        return self->f_open_subdirectory(self, subpath, direct);
}

bool filesys_open_parent(struct file_system* self, struct directory* direct)
{
        return self->f_open_parent(self, direct);
}

bool filesys_close_directory(struct file_system* self, struct directory* direct)
{
        return self->f_close_directory(self, direct);
}


bool filesys_open_file(struct file_system* self, struct directory* direct, const char* file_name)
{
        return self->f_open_file(self, direct, file_name);
}

struct file* filesys_create_file(struct file_system* self, struct directory* direct, const char* file_name)
{
        return self->f_create_file(self, direct, file_name);
}

uint64_t filesys_file_size(struct file_system* self, struct file* file)
{
        return self->f_file_size(self, file);
}

uint64_t filesys_read_file(struct file_system* self, uint64_t bytes, struct file* file, char* var_buf)
{
        return self->f_read_file(self, bytes, file, var_buf);
}

uint64_t filesys_write_file(struct file_system* self, uint64_t bytes, struct file* file, char* buf)
{
        return self->f_write_file(self, bytes, file, buf);
}

bool filesys_close_file(struct file_system* self, struct file* file)
{
        return self->f_close_file(self, file);
}

