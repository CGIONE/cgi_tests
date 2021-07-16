//
//  CGIONE
//  14.07.2021
//  little endian, win32, gcc, c[99] 
//  test bmp output

//  moar info:

//  bmp structure idea and size
//  https://codereview.stackexchange.com/questions/196084/read-and-write-bmp-file-in-c
//  https://web.archive.org/web/20210507012023/https://engineering.purdue.edu/ece264/17au/hw/HW15

//  bmp file format
//  https://en.wikipedia.org/wiki/BMP_file_format
//  http://archive.retro.co.za/CDROMs/DrDobbs/CD%20Release%2012/articles/1995/9503/9503e/9503e.htm
//  http://archive.retro.co.za/CDROMs/DrDobbs/CD%20Release%2012/articles/1995/9504/9504c/9504c.htm

//  pixel plot
//  http://www.brackeen.com/vga/source/bc31/pixel.c.html
//  https://github.com/SamuliNatri/c-drawing-pixels/blob/master/main.c

//  gradient
//  casey's https://handmadehero.org/

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

#include <stdint.h>     // datatypes
#include <stdio.h>      // printf, FILE
#include <stdlib.h>     // malloc
#include <string.h>     // memset
#include <math.h>

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
typedef uint8_t     u8;
typedef int32_t     b32;

typedef int32_t     i32;    typedef uint32_t     u32; 

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

#define FILEWRITE "bin\\test1.bmp"

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// STRUCTS

// typeof output format
// enum not used directly
enum img_type
{
    BMP_32 = 0,
};

// generic img struct
struct img_proto
{
    i32 img_width; i32 img_height; u32 size_to_alloc;
    void *ptr_raster_data;

};

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

// FUNCTIONS

// malloc and fill pixel data
void fill_raster_data(struct img_proto *img)
{
    // allocation
    img->ptr_raster_data = malloc(img->size_to_alloc);
    if (img->ptr_raster_data == NULL) { puts("Malloc Failed!"); return; }

    // fill all (background-like) BUT gray-like
    memset(img->ptr_raster_data, 100, img->size_to_alloc);

#define fill_3_pixels 0
#if fill_3_pixels

    // u32 pixel format 
        u32 *pixels = (u32 *)img->ptr_raster_data;

        // fill 3 pixels only
        u32 red   = 0xFF0000;
        u32 green = 0x00FF00;
        u32 blue  = 0x0000FF;

        // first pixels
        *pixels = red;
        printf("P_ADDRS[1] = [0x%I64X]\n", (uintptr_t)pixels);

        // second pixel
        ++pixels;
        *pixels = green;
        printf("P_ADDRS[2] = [0x%I64X]\n", (uintptr_t)pixels);

        // third
        ++pixels;
        *pixels = blue;
        printf("P_ADDRS[3] = [0x%I64X]\n", (uintptr_t)pixels);

#endif

#define fill_at_location 0
#if fill_at_location

    //
    u32 *fill_loc = (u32 *)img->ptr_raster_data;
    i32 locationX = img->img_width / 2;
    i32 locationY = img->img_height / 2;

    i32 offset = locationY * img->img_width + locationX;
    fill_loc[offset] = 0x672673;

#endif

#define fill_gradient 0
#if fill_gradient
    
    //
    u8 *Row = (u8 *)img->ptr_raster_data;
    i32 BlueOffset = img->img_width / img->img_height;
    i32 GreenOffset = img->img_height / img->img_width;

	for (i32 Y = 0; Y < img->img_height; ++Y)
	{
		u32 *Pixel = (u32 *)Row;
		for (i32 X = 0; X < img->img_width; ++X)
		{
			u8 Blue = (X + BlueOffset);
			u8 Green = (Y + GreenOffset);
			*Pixel++ = ((Green << 16) | Blue >> 16);
		}

		Row += img->img_width * sizeof(u32);
	}
#endif

#define fill_pattern 0
#if fill_pattern

    //
    u32 *pattern = (u32 *) img->ptr_raster_data;
    for (i32 i = 0; i < img->img_width; ++i)
    {
        for (i32 j = 0; j < img->img_height; ++j)
        {
            u32 first_c  = 0x122563;
            u32 second_c = 0x481263;
            
            u32 final_c = 
            ( tan(i) > tan(j) ) && i < j*2 && j < i*2 
            ? first_c >> 2 : second_c;

            *pattern = final_c;
            ++pattern;
        }
        
    }
#endif  

}

// create image file
b32 create_image_output(char *output_path, i32 output_width, i32 output_height, struct img_proto *img, enum img_type output_type)
{
    // bmp_header
        #pragma pack(push)  // save the original data alignment
        #pragma pack(1)     // Set data alignment to 1 byte boundary

            struct bmp_header {
                
                // sizeof == 54 bytes
                // uint16_t is a 16-bit unsigned integer
                // uint32_t is a 32-bit unsigned integer

                uint16_t type;              // Magic identifier: 0x4d42
                uint32_t size;              // File size in bytes == (image_size + header_size)
                uint16_t reserved1;         // Not used
                uint16_t reserved2;         // Not used
                uint32_t offset;            // Offset to image data in bytes from beginning of file
                uint32_t dib_header_size;   // DIB Header size in bytes
                int32_t  width_px;          // Width of the image
                int32_t  height_px;         // Height of image
                uint16_t num_planes;        // Number of color planes
                uint16_t bits_per_pixel;    // Bits per pixel
                uint32_t compression;       // Compression type
                uint32_t image_size_bytes;  // Image size in bytes == (filesize - sizeof(header))
                int32_t  x_resolution_ppm;  // Pixels per meter
                int32_t  y_resolution_ppm;  // Pixels per meter
                uint32_t num_colors;        // Number of colors
                uint32_t important_colors;  // Important colors
            } ;

        #pragma pack(pop)  // restore the previous pack setting

    // file open
        FILE *ptr_file = fopen64(output_path, "wb");
        if (ptr_file == NULL) { puts("File open failed!\n"); return FALSE; }
        else { puts("\n*******************\n"); printf("OPENED     = [%s]\n", output_path); }

    // write img sizes to img struct
        img->img_width  = output_width;
        img->img_height = output_height;
        img->size_to_alloc = img->img_width * img->img_height * sizeof(u32);

    // fill pixel data
        fill_raster_data(img);

    //
    switch (output_type)
    {
        //
        case BMP_32:
        {
            //
            puts("CASE       = [BMP_32]");
            struct bmp_header bmp = {0};

            // fill header
                bmp.type = 0x4D42;
                bmp.width_px  = output_width;
                bmp.height_px = -output_height; // start from top-left
                bmp.bits_per_pixel = 32; 
                bmp.num_planes = 1;
                bmp.compression = 0;
                bmp.dib_header_size = 40;
                bmp.offset = sizeof(bmp);   // 54

                // not important for use with vscode, mspaint or acdsee under win32
                    //bmp.num_colors = 0;
                    //bmp.important_colors = 0;
                    //bmp.image_size_bytes = 0; // if compression == 0;
                    //bmp.size = bmp.image_size_bytes + sizeof(bmp);

            // write
                i32 write_header_result = fwrite(&bmp, sizeof(bmp), 1, ptr_file); 
                if (write_header_result == 0) { puts("0 on write_header!"); return FALSE; }
                else { printf("WR_HEADER  = [%d] elements\n", write_header_result); }

                i32 write_pixel_result = fwrite(img->ptr_raster_data, img->size_to_alloc, 1, ptr_file);
                if (write_pixel_result == 0) { puts("0 on write_pixels!"); return FALSE; }
                else { printf("WR_PIXELS  = [%d] elements\n", write_pixel_result); }

        }
            break;
        
        default:
            break;
    }

    // file close
        i32 close_result = fclose(ptr_file);
        if (close_result != 0) { perror("Error on fclose!\n"); return FALSE; }
        else { printf("CLOSED     = [%s]\n", output_path); puts("\n*******************\n"); }

    // free memory
    free(img->ptr_raster_data);


    //
    return TRUE;
}

/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/

//
i32 main(void)
{
    //
    struct img_proto imgOne = {0};

    //
    create_image_output(FILEWRITE, 32, 32, &imgOne, BMP_32);

    

    //
    return 0;
}
