// menucodes.h

#ifndef menucodes_h
#define menucodes_h

#include "fixes.h"

#define imenubase 0
#define imenunotes   imenubase+1
#define imenumarry   imenunotes+1
#define imenuyounger imenumarry+1
#define imenuchild   imenuyounger+1
#ifdef fix0015
#define imenunewfam  imenuchild+1
#define imenuremove  imenunewfam+1
#else
#define imenuremove  imenuchild+1
#endif
#define imenuindreps imenuremove+1
#define imenuancs    imenuindreps+1
#define imenuendreps imenuancs+1
#define imenuend     imenuendreps+1
#define imenusize    imenuend+1



#define fmenubase 0
#define fmenunotes   fmenubase+1
#define fmenulater   fmenunotes+1
#define fmenuchild   fmenulater+1
#define fmenuunmarry fmenuchild+1
#define fmenuend     fmenuunmarry+1
#define fmenusize    fmenuend+1

#endif
