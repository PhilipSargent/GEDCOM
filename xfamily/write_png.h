/*
  * write_png.h
  *
  */

#ifndef WRITE_PNG_H_
#define WRITE_PNG_H_
 

extern int write_png(const char *file_name,             /* Name of file to be 
written */
                          unsigned char *datap,                         /* 
pointer to image data array */
                          int width,                                            
/* Image width */
                          int height,                                           
/* Image height */
                          int depth                                             
/* Image depth, valid values 1(mono), 3(RGB) */
                        );

// return zero for success, -1 for error

#endif /* WRITE_PNG_H_ */
