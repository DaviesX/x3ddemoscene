#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <allocator.h>
#include <math.h>
#include <stdbool.h>
#include "serializer.h"

#define INIT_SIZE		1

static void set_mem_size ( int new_size, struct serializer *serial );
static uint8_t read_bit ( int n_bit, struct byte_bit *pos, void *dest_mem );
static void write_bit ( uint8_t data, int n_bit, struct byte_bit *pos, void *dest_mem );


/** @brief 按照参数设定data_stream的内存大小
 *
 * @param new_size 	[i]要设定的大小
 * @param serial 	[m]结构struct serializer
 * @return void
 *
 */
static void set_mem_size ( int new_size, struct serializer *serial )
{
        void *tmp = malloc ( new_size );
        memset ( tmp, 0, new_size );
        memcpy ( tmp, serial->data_stream, serial->size );

        free ( serial->data_stream );
        serial->data_stream = tmp;
        serial->total = new_size;
}

/** @brief 读取指定数量的binary位
 *
 * @param n_bit 	[i]要读取的bit的数量，从低位开始数
 * @param pos 		[m]当前读取的内存位置（精确到bit），pos在读取完毕之后会自动增加
 * @param dest_mem 	[i]目标内存的首地址
 * @return 读取出的数据
 * @note 参数pos会随之而改变
 */
static uint8_t read_bit ( int n_bit, struct byte_bit *pos, void *dest_mem )
{
        uint8_t *byte_mem = dest_mem;
        uint8_t data = 0;

        /* 逐位扫描 byte_mem
         * 数据：1001 0101
         * 掩码：0001 0000
         * 结果：0001 0000
         * tmp：0000 0001 */
        int i;
        for ( i = 0; i < n_bit; i ++ ) {
                uint8_t mask = 1 << pos->bit;
                uint8_t tmp = (byte_mem[pos->byte] & mask) >> pos->bit;
                data |= tmp << i;

                pos->bit ++;			/**< 处理下一位 */
                pos->byte += (pos->bit == 8);	/**< 如果bit == 8，那么byte进1 */
                pos->bit %= 8;			/**< 如果bit == 8，那么byte进1位之后的bit是0，否则bit不改变 */
        }
        return data;
}

/** @brief 写入指定数量的binary位
 *
 * @param data 		[i]要写入的数据
 * @param n_bit 	[i]要写入的bit的数量，从低位开始数
 * @param pos 		[m]要写入的内存位置（精确到bit），pos在写入完毕之后会自动增加
 * @param dest_mem 	[o]目标内存的首地址
 * @return void
 * 注意：参数pos会随之而改变
 */
static void write_bit ( uint8_t data, int n_bit, struct byte_bit *pos, void *dest_mem )
{
        uint8_t *byte_mem = dest_mem;

        /* 逐位扫描 data
         * 数据：1001 0101
         * 掩码：0001 0000
         * 结果：0001 0000
         * tmp：0000 0001 */
        int i;
        for ( i = 0; i < n_bit; i ++ ) {
                uint8_t mask = 1 << i;
                uint8_t tmp = (data & mask) >> i;
                byte_mem[pos->byte] |= tmp << pos->bit;

                pos->bit ++;			/**< 处理下一位 */
                pos->byte += (pos->bit == 8);	/**< 如果bit == 8，那么byte进1 */
                pos->bit %= 8;			/**< 如果bit == 8，那么byte进1位之后的bit是0，否则bit不改变 */
        }
}

/** @brief 初始化序列化器
 *
 * @param serial 	[m]结构struct serializer
 * @return void
 * 调用serial使用完以后需要调用serial_free来释放内存
 */
void serial_init ( struct serializer *serial )
{
        serial->data_stream = malloc ( INIT_SIZE );
        serial->read_pos.bit = 0;
        serial->read_pos.byte = 0;
        serial->write_pos.bit = 0;
        serial->write_pos.byte = 0;
        serial->size = 0;
        serial->total = 1;
}

/** @brief 释放序列化器
 *
 * @param serial 	[m]结构struct serializer
 * @return void
 * serial必须由serial_init初始化，否则结果未定义
 */
void serial_free ( struct serializer *serial )
{
        free ( serial->data_stream );
        memset ( serial, 0, sizeof ( *serial ) );
}

/** @brief 复位读取/写入状态
 *
 * @param serial 	[m]结构struct serializer
 * @param mode 		[i]复位方式，查看enum SER_RESET_MODE，不同的方式可以用OR或者add来组合
 * @return void
 *
 */
void serial_reset ( struct serializer *serial, enum SER_RESET_MODE mode )
{
        if ( mode & SER_RESET_READER ) {
                serial->read_pos.bit = 0;
                serial->read_pos.byte = 0;
        }
        if ( mode & SER_RESET_WRITER ) {
                serial->write_pos.bit = 0;
                serial->write_pos.byte = 0;
                serial->size = 0;
        }
}

/** @brief 读取下一个区块的字节流到data，直到遇到0指示符
 *
 * @param data 		[o]data的大小必须要能够容纳要读取的区块，否则将会内存越界
 * @param size 		[o]该区块的字节大小
 * @param serial 	[m]结构struct serializer
 * @return void
 *
 */
void serial_read ( void *data, int *size, struct serializer *serial )
{
        uint8_t *dest = data;
        int n_read = 0;
        int indicator;
        do {
                dest[n_read ++] =
                        read_bit ( 8, &serial->read_pos, serial->data_stream );
                indicator =
                        read_bit ( 1, &serial->read_pos, serial->data_stream );
        } while ( indicator == 1 );
        if ( size != NULL ) {
                *size = n_read;
        }
}

/** @brief 写入指定size的data到序列化器
 *
 * @param data 		[i]数据内存
 * @param size 		[i]数据的字节大小
 * @param serial 	[m]结构struct serializer
 * @return void
 *
 */
void serial_write ( void *data, int *size, struct serializer *serial )
{
        uint8_t *src = data;
        int n_write = *size;
        int dest_size = ceil ( n_write*9.0/8.0 );

        if ( serial->size + dest_size > serial->total ) {
                set_mem_size ( (serial->size + dest_size)*2, serial );
        }

        int i;
        for ( i = 0; i < n_write; i ++ ) {
                write_bit ( src[i], 8, &serial->write_pos, serial->data_stream );
                write_bit ( (i != n_write - 1), 1, &serial->write_pos,
                            serial->data_stream );
        }
        serial->size = serial->write_pos.byte + 1;
}

/** @brief 导入序列文件到序列化器
 *
 * @param filename 	[i]序列文件的相对/绝对路径
 * @param serial 	[o]结构struct serializer
 * @return true: 成功; false: 失败
 * 失败原因请看控制台输出
 */
bool serial_import ( char *filename, struct serializer *serial )
{
        /* 用二进制读取模式打开文件 */
        FILE *f_serial = fopen ( filename, "rb" );
        if ( !f_serial ) {
                /* 无法打开文件 */
                printf ( "failed in opening: %s", filename );
                goto fail;
        }

        fseek ( f_serial, 0, SEEK_END );
        uint32_t size = ftell ( f_serial );
        fseek ( f_serial, 0, SEEK_SET );
        set_mem_size ( size, serial );

        fread ( serial->data_stream, 1, size, f_serial );

        if ( ferror ( f_serial ) ) {
                /* 读取出错 */
                goto fail;
        } else {
                serial->size = size;
                printf ( "%d bytes has been read from %s\n", size, filename );
                goto succeed;
        }
succeed:
        fclose ( f_serial );
        return true;
fail:
        if ( f_serial ) {
                fclose ( f_serial );
        }
        return false;
}

/** @brief 导出序列化器到序列文件
 *
 * @param filename 	[i]序列文件的相对/绝对路径
 * @param serial 	[i]结构struct serializer
 * @return true: 成功; false: 失败
 * 失败原因请看控制台输出
 */
bool serial_export ( char *filename, struct serializer *serial )
{
        /* 用二进制写入模式打开文件 */
        FILE *f_serial = fopen ( filename, "wb" );
        if ( !f_serial ) {
                /* 无法打开文件 */
                printf ( "failed in opening: %s", filename );
                goto fail;
        }

        uint32_t size = fwrite ( serial->data_stream, 1, serial->size, f_serial );

        if ( ferror ( f_serial ) ) {
                /* 写入出错 */
                goto fail;
        } else {
                printf ( "%d bytes has been written to %s\n", size, filename );
                goto succeed;
        }
succeed:
        fclose ( f_serial );
        return true;
fail:
        if ( f_serial ) {
                fclose ( f_serial );
        }
        return false;
}
