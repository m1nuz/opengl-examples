#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <glcore_450.h>

enum TARGA_DATA_TYPE
{
    TARGA_DATA_NO = 0,
    TARGA_DATA_COLOR_MAPPED = 1,        // indexed
    TARGA_DATA_TRUE_COLOR = 2,          // RGB
    TARGA_DATA_BLACK_AND_WHITE = 3,     // grayscale
    TAGRA_DATA_RLE_COLOR_MAPPED = 9,
    TARGA_DATA_RLE_TRUE_COLOR = 10,
    TARGA_DATA_RLE_BLACK_AND_WITE = 11
};

#pragma pack(push, tga_header_align)
#pragma pack(1)
struct tga_header
{
    uint8_t     length;
    uint8_t     color_map;
    uint8_t     data_type;
    uint16_t    colormap_index;
    uint16_t    colormap_length;
    uint8_t     colormap_entry_size;
    uint16_t    x;
    uint16_t    y;
    uint16_t    width;
    uint16_t    height;
    uint8_t     bpp;
    uint8_t     decription;
};
#pragma pack(pop, tga_header_align)

void* load_targa(const char *filepath, GLuint *iformat, GLenum *format, GLsizei *width, GLsizei *height);

extern void* load_targa(const char *filepath, GLuint *iformat, GLenum *format, GLsizei *width, GLsizei *height)
{
    FILE *fp = fopen(filepath, "rb");

    if(!fp)
        return NULL;

    fseek(fp, 0, SEEK_END);
    int lenght = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    struct tga_header header = {0};
    fread(&header, sizeof(header), 1, fp);

    const uint8_t bytesperpixel = header.bpp / 8;
    uint8_t *data = NULL;

    if((header.data_type == TARGA_DATA_RLE_TRUE_COLOR) || (header.data_type == TARGA_DATA_RLE_BLACK_AND_WITE))
    {
        data = (uint8_t*)malloc(header.width * header.height * (bytesperpixel + 1)); // todo : why +1??? it works
        uint8_t *pdata = data;

        uint8_t block = 0;

        for(int i = 0; (i < header.width * header.height) && !feof(fp); i++)
        {
            fread(&block, 1, 1, fp);

            uint8_t count = (block & 0x7f) + 1;

            if(block & 0x80)
            {
                uint8_t bytes[4] = {0};
                fread(bytes, bytesperpixel, 1, fp);

                for(int j = 0; j < count; j++)
                {
                    memcpy(pdata, bytes, bytesperpixel);
                    pdata += bytesperpixel;
                }
            }
            else
            {
                fread(pdata, bytesperpixel * count, 1, fp);
                pdata += bytesperpixel * count;
            }
        }
    }
    else if((header.data_type == TARGA_DATA_TRUE_COLOR) || (header.data_type == TARGA_DATA_BLACK_AND_WHITE))
    {
        data = (uint8_t*)malloc(header.width * header.height * (bytesperpixel + 1));

        if(!fread(data, lenght - sizeof(header), 1, fp))
        {
            fclose(fp);
        }
    }

    fclose(fp);

    switch(header.bpp)
    {
    case 8:
        *iformat = GL_R8;
        *format = GL_RED;
        break;
    case 24:
        *iformat = GL_RGB8;
        *format = GL_BGR;
        break;
    case 32:
        *iformat = GL_RGBA8;
        *format = GL_BGRA;
        break;
    }

    *width = header.width;
    *height = header.height;

    return data;
}
