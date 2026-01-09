/*
  * write_png.cpp
  *
  */

#include "fixes.h"

#ifndef WIN32
#include <fstream>
        using std::fstream;
        using std::ios_base;

#include <stdlib.h>
#include <zlib.h>
#include <png.h>
// using memcpy now seems to need the string library
#include <cstring>

#include "write_png.h"

typedef unsigned char uchar;

//===================================================================
/* structure to store PNG image bytes */
typedef struct
{
   uchar *buffer;
   size_t size;
   size_t size_allocated;
} mem_encode;


static void my_png_write_data(png_structp png_ptr, png_bytep data, 
png_size_t length)
{
   mem_encode* p=(mem_encode*)png_get_io_ptr(png_ptr);
   size_t nsize = p->size + length;

   /* allocate or grow buffer */
   if(nsize > p->size_allocated){
       int alloc_size = nsize * 2;
       if(p->buffer)
         p->buffer = (uchar*)realloc(p->buffer, alloc_size);
       else
         p->buffer = (uchar*)malloc(alloc_size);

       if(!p->buffer) {
         png_error(png_ptr, "Write Error");
         return;
       }
       p->size_allocated = alloc_size;
   }

   /* copy new bytes to end of buffer */
   memcpy(p->buffer + p->size, data, length);
   p->size += length;
}

/* This is optional but included to show how png_set_write_fn() is called */
static void my_png_flush(png_structp png_ptr){}

int write_png(const char *file_name,            /* Name of file to be written */
                          unsigned char *datap,                 /* pointer to 
image data array */
                          int imgw,                                             
/* Image width */
                          int imgh,                                             
/* Image height */
                          int imgd                                              
/* Image depth, valid values 1(mono), 3(RGB) */
                        )
{
     //    outlen = imgw * imgh * imgd;

     png_structp                        pp;                     // PNG data
     png_infop                  info;           // PNG image info
     mem_encode state;
     /* initialise - put this before png_write_png() call */
     state.buffer = NULL;
     state.size = 0;
     state.size_allocated = 0;


     if( !((imgd == 1) || (imgd ==3) || (imgd ==4))) return -1;
     pp = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
     if(!pp) return -1;

     info = png_create_info_struct(pp);
     if (!info){
         png_destroy_write_struct(&pp, 0);
         return -1;
     }

     if (setjmp(png_jmpbuf(pp))){
         png_destroy_write_struct(&pp, &info);
         return -1;
     }

     //png_init_io(pp, fp);
     /* if my_png_flush() is not needed, change the arg to NULL */
     png_set_write_fn(pp, &state, my_png_write_data, my_png_flush);


     png_set_compression_level(pp, Z_BEST_COMPRESSION);
     png_set_IHDR(pp, info, imgw, imgh, 8,
                imgd == 1 ? PNG_COLOR_TYPE_GRAY :
                imgd == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT);
     png_set_sRGB(pp, info, PNG_sRGB_INTENT_PERCEPTUAL);
     png_set_sRGB_gAMA_and_cHRM(pp, info, PNG_INFO_sRGB);

     png_write_info(pp, info);

     int iwd = imgw * imgd;
     for (int y = 0; y < imgh; y++)
     {
         png_write_row(pp, (png_byte *)datap);
         datap += iwd;
     }

     png_write_end(pp, info);
     png_destroy_write_struct(&pp, 0);

     if (state.buffer && state.size)
     {
                fstream Out(file_name, ios_base::out | ios_base::binary);
                if (Out.good()) Out.write((const char*) state.buffer, 
state.size);
                Out.close();
     }

     /* cleanup */
     if (state.buffer) free(state.buffer);
        return 0;
}
#endif

  //
  // End of "$Id$".
  //
