#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED


#include <misc.h>


enum SerialResetMode {
        SerialResetReader = 0X1,
        SerialResetWriter = 0X2
};

enum SerialIOMode {
        SerialRead,
        SerialWrite
};

/*
 * structures
 */
struct byte_bit {
        uint32_t        byte;
        uint32_t        bit;
};

struct serializer {
        void*                   data_stream;
        struct byte_bit         read_pos;
        struct byte_bit         write_pos;
        int                     size;
        int                     total;
};

/*
 * functions' declaration
 */
void            serial_init(struct serializer* self);
void            serial_free(struct serializer* self);
void            serial_reset(struct serializer* self, enum SerialResetMode mode);
void            serial_read(struct serializer* self, void* data, int* size);
void            serial_write(struct serializer* self, void* data, int* size);
void            serial_import(struct serializer* self, uint8_t* stream);
uint8_t*        serial_export(struct serializer* self, int* size);


#endif // SERIALIZER_H_INCLUDED
