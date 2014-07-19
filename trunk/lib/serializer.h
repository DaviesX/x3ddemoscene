#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED


#include <x3d/common.h>


enum SER_RESET_MODE {
        SER_RESET_READER = 0X1,
        SER_RESET_WRITER = 0X2
};

enum SERIAL_IO_MODE {	/* 为下一次改进准备 */
        SERIAL_READ,
        SERIAL_WRITE
};

/*
 * structures
 */
struct byte_bit {
        uint32_t byte;
        uint32_t bit;
};

struct serializer {
        void *data_stream;
        struct byte_bit read_pos;
        struct byte_bit write_pos;
        int size;	/* 已使用的大小 */
        int total;	/* 总大小 */
};

/*
 * interfaces
 */
void serial_init ( struct serializer *serial );
void serial_free ( struct serializer *serial );
void serial_reset ( struct serializer *serial, enum SER_RESET_MODE mode );
void serial_read ( void *data, int *size, struct serializer *serial );
void serial_write ( void *data, int *size, struct serializer *serial );
bool serial_import ( char *filename, struct serializer *serial );
bool serial_export ( char *filename, struct serializer *serial );


#endif // SERIALIZER_H_INCLUDED
