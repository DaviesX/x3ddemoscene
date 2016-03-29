#include <math/math.h>
#include <system/allocator.h>
#include <x3d/colorspectrum.h>
#include <x3d/hostimage.h>


void hostimg_init(struct host_image* img, int nlevel, enum UtilImageFormat format,
                                int width, int height)
{
        zero_obj(img);
        img->nlevel = nlevel;
        img->format = format;
        img->width = width;
        img->height = height;

        switch (img->format) {
        case UtilImgRGB24:
                {
                img->pix_size = 3;
                break;
                }
        case UtilImgRGBA32:
                {
                img->pix_size = 4;
                break;
                }
        case UtilImgRGBRadiance:
                {
                img->pix_size = 12;
                break;
                }
        }
        img->stride = img->width*img->pix_size;
}

void hostimg_import(struct host_image* img, int nlevel, void* data)
{
}

void hostimg_alloc(struct host_image* img, int nlevel)
{
        img->data[nlevel] = alloc_fix(1, img->width*img->height*img->pix_size);
}

void hostimg_delete(struct host_image* img, int nlevel)
{
        free_fix(img->data[nlevel]);
}

void hostimg_free(struct host_image* img)
{
        int i;
        for(i = 0; i < 14; i ++) {
                free_fix(img->data[i]);
        }
        zero_obj(img);
}

void* hostimg_read(struct host_image* img, int nlevel, int x, int y)
{
        return &((uint8_t*) img->data[nlevel])[x*img->pix_size + y*img->stride];
}

void hostimg_write(struct host_image* img, int nlevel, int x, int y, void* pix_data)
{
        memcpy(&((uint8_t*) img->data[nlevel])[x*img->pix_size + y*img->stride], pix_data, img->pix_size);
}

void hostimg_get_level0_dimension(struct host_image* img, int* width, int* height)
{
        *width  = img->width;
        *height = img->height;
}

void hostimg_export_to_file(struct host_image* self, FILE* file)
{
        int width, height;
        hostimg_get_level0_dimension(self, &width, &height);
        fprintf(file, "P3\n%d %d\n%d\n", width, height, 255);
        int i, j;
        for (j = 0; j < height; j ++) {
                for (i = 0; i < width; i ++) {
                        switch (self->format) {
                        case UtilImgRGB24: {
                                unsigned char* px = hostimg_read(self, 0, i, j);
                                fprintf(file, "%d %d %d ", px[2], px[1], px[0]);
                                break;
                        }
                        case UtilImgRGBA32: {
                                unsigned char* px = hostimg_read(self, 0, i, j);
                                fprintf(file, "%d %d %d ", px[2], px[1], px[0]);
                                break;
                        }
                        case UtilImgRGBRadiance: {
                                struct float_color3* px = hostimg_read(self, 0, i, j);
                                int r, g, b;
                                r = clamp(px->r*255, 0, 255);
                                g = clamp(px->g*255, 0, 255);
                                b = clamp(px->b*255, 0, 255);
                                fprintf(file, "%d %d %d ", r, g, b);
                                break;
                        }
                        }
                }
        }
        fclose(file);
}
