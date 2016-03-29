#ifndef HOSTIMAGE_H_INCLUDED
#define HOSTIMAGE_H_INCLUDED

#include <x3d/common.h>

enum ColorMode {
        Color8Mode,
        Color16AMode,
        Color16BMode,
        Color24Mode,
        Color32Mode,
        ColorHDR32Mode,
        ColorHDR96Mode
};

/** \brief utility for representing in-memory image
 */
struct host_image {
  	int                     nlevel;
  	enum ColorMode          cmode;
  	int                     width;
  	int                     height;
  	int                     pix_size;
  	int                     stride;
        void*                   data[14];
};

void hostimg_init(struct host_image* img, int nlevel, enum ColorMode cmode, int width, int height);
void hostimg_import(struct host_image* img, int nlevel, void* data);
void hostimg_alloc(struct host_image* img, int nlevel);
void hostimg_delete(struct host_image* img, int nlevel);
void hostimg_free(struct host_image* img);
void* hostimg_read(struct host_image* img, int nlevel, int x, int y);
void hostimg_write(struct host_image* img, int nlevel, int x, int y, void* pix_data);
void hostimg_get_level0_dimension(struct host_image* img, int* width, int* height);
void* hostimg_export_level0(struct host_image* self, enum ColorMode* cmode, int* w, int* h, int* stride, int* pix_size);
void hostimg_export_to_file(struct host_image* self, FILE* file);


#endif // HOSTIMAGE_H_INCLUDED
