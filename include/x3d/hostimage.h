#ifndef HOSTIMAGE_H_INCLUDED
#define HOSTIMAGE_H_INCLUDED

#include <x3d/common.h>

enum UtilImageFormat {
        UtilImgRGB24,
        UtilImgRGBA32,
        UtilImgRGBRadiance
};

/** \brief utility for representing in-memory image
 */
struct host_image {
  	int                     nlevel;
  	enum UtilImageFormat    format;
  	int                     width;
  	int                     height;
  	int                     pix_size;
  	int                     stride;
        void*                   data[14];
};

void hostimg_init(struct host_image* img, int nlevel, enum UtilImageFormat format, int width, int height);
void hostimg_import(struct host_image* img, int nlevel, void* data);
void hostimg_alloc(struct host_image* img, int nlevel);
void hostimg_delete(struct host_image* img, int nlevel);
void hostimg_free(struct host_image* img);
void* hostimg_read(struct host_image* img, int nlevel, int x, int y);
void hostimg_write(struct host_image* img, int nlevel, int x, int y, void* pix_data);
void hostimg_get_level0_dimension(struct host_image* img, int* width, int* height);
void hostimg_export_to_file(struct host_image* self, FILE* file);


#endif // HOSTIMAGE_H_INCLUDED
