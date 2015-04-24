#include <misc.h>
#include <system/allocator.h>
#include <codecs/serializer.h>

#define INIT_SIZE		1

static void     set_mem_size(struct serializer* self, int new_size);
static uint8_t  read_bit(int n_bit, struct byte_bit* pos, void* dest_mem);
static void     write_bit(uint8_t data, int n_bit, struct byte_bit* pos, void* dest_mem);


static void set_mem_size(struct serializer* self, int new_size)
{
        uint8_t* tmp = alloc_fix(new_size, 1);
        zero_array(tmp, new_size);
        memcpy(tmp, self->data_stream, self->size);

        free_fix(self->data_stream);
        self->data_stream       = tmp;
        self->total             = new_size;
}

static uint8_t read_bit(int n_bit, struct byte_bit* pos, void* dest_mem)
{
        uint8_t *byte_mem = dest_mem;
        uint8_t data = 0;

        /* scan bit by bit byte_mem
         * data:        1001 0101
         * mask:        0001 0000
         * result:      0001 0000
         * tmp:         0000 0001 */
        int i;
        for (i = 0; i < n_bit; i ++) {
                uint8_t mask    = 1 << pos->bit;
                uint8_t tmp     = (byte_mem[pos->byte] & mask) >> pos->bit;
                data            |= tmp << i;

                pos->bit ++;			/**< process next bit */
                pos->byte += (pos->bit == 8);	/**< if bit == 8，then "byte" add the carry bit */
                pos->bit %= 8;			/**< if bit == 8，then "bit" is set to 0 after byte add the carry bit，otherwise bit doesn't change */
        }
        return data;
}

static void write_bit(uint8_t data, int n_bit, struct byte_bit* pos, void* dest_mem)
{
        uint8_t *byte_mem = dest_mem;

        /* scan bit by bit on data
         * data:   1001 0101
         * mask:   0001 0000
         * result: 0001 0000
         * tmp:    0000 0001 */
        int i;
        for (i = 0; i < n_bit; i ++) {
                uint8_t mask            = 1 << i;
                uint8_t tmp             = (data & mask) >> i;
                byte_mem[pos->byte]     |= tmp << pos->bit;

                pos->bit ++;			/**< process next bit */
                pos->byte += (pos->bit == 8);	/**< if bit == 8，then "byte" add the carry bit */
                pos->bit %= 8;			/**< 如果bit == 8，then "bit" is set to 0 after byte add the carry bit，otherwise bit doesn't change */
        }
}

void serial_init(struct serializer* self)
{
        self->data_stream       = alloc_fix(INIT_SIZE, 1);
        self->read_pos.bit      = 0;
        self->read_pos.byte     = 0;
        self->write_pos.bit     = 0;
        self->write_pos.byte    = 0;
        self->size              = 0;
        self->total             = 1;
}

void serial_free(struct serializer* self)
{
        free_fix(self->data_stream);
        zero_obj(self);
}

void serial_reset(struct serializer* self, enum SerialResetMode mode)
{
        if (mode & SerialResetReader) {
                self->read_pos.bit      = 0;
                self->read_pos.byte     = 0;
        }
        if (mode & SerialResetWriter) {
                self->write_pos.bit     = 0;
                self->write_pos.byte    = 0;
                self->size              = 0;
        }
}

void serial_read(struct serializer* self, void* data, int* size)
{
        uint8_t *dest   = data;
        int n_read      = 0;
        int indicator;
        do {
                dest[n_read ++] = read_bit(8, &self->read_pos, self->data_stream);
                indicator       = read_bit(1, &self->read_pos, self->data_stream);
        } while (indicator == 1);
        if (size != nullptr) {
                *size = n_read;
        }
}

void serial_write(struct serializer* self, void* data, int* size)
{
        uint8_t* src    = data;
        int n_write     = *size;
        int dest_size   = ceil(n_write*9.0/8.0);

        if (self->size + dest_size > self->total) {
                set_mem_size (self, (self->size + dest_size)*2);
        }

        int i;
        for (i = 0; i < n_write; i ++) {
                write_bit(src[i], 8, &self->write_pos, self->data_stream);
                write_bit((i != n_write - 1), 1, &self->write_pos, self->data_stream);
        }
        self->size = self->write_pos.byte + 1;
}

void serial_import(struct serializer* self, uint8_t* stream)
{
        uint32_t* s = (uint32_t*) stream;
        int size = s[0];
        self->size = size;
        set_mem_size(self, size);
        memcpy(self->data_stream, s + 1, size);
}

uint8_t* serial_export(struct serializer* self, int* size)
{
        *size = self->size;
        uint32_t* stream = alloc_fix(self->size + sizeof(uint32_t), 1);
        stream[0]        = self->size;
        memcpy(stream + 1, self->data_stream, self->size);
        return (uint8_t*) stream;
}
