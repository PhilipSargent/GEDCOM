// menucodes.h

#ifndef menucodes_h
#define menucodes_h

#include "fixes.h"

#define imenubase 0
#define imenunotes   imenubase+1
#define imenuyounger imenunotes+1
#define imenuolder   imenuyounger+1
#define imenunewfam  imenuolder+1
#define imenustructure  imenunewfam+1
#define imenuremove  imenustructure+1
#define imenudetach  imenuremove+1
#define imenumove    imenudetach+1
#define imenuattach  imenumove+1
#define imenuendstruct imenuattach+1
#define imenuindreps imenuendstruct+1
#define imenuancs    imenuindreps+1
#define imenuendreps imenuancs+1
#define imenuend     imenuendreps+1
#define imenusize    imenuend+1



#define fmenubase 0
#define fmenunotes   fmenubase+1
#define fmenulater   fmenunotes+1
#define fmenuearlier fmenulater+1
#define fmenuchild   fmenuearlier+1
#define fmenuunmarry fmenuchild+1
#define fmenuend     fmenuunmarry+1

#define fmenusize    fmenuend+1

#endif
