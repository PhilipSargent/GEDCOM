// flags.h

#ifndef flags_h
#define flags_h

#include "fixes.h"

#ifdef fix0021

#define ged_delete    1
#define ged_ephemeral 2
// probably don't need this - it's only a UI representation that should be immutable
#define ged_immutable 4

#define ind_ephemeral 2
#define ind_immutable 4

#define fam_ephemeral 2
#define fam_immutable 4

#endif

#endif
