// family.h

#ifndef family_h
#define family_h

#include "classes.h"

// maximum length of a line we expect (as specified in GEDCOM spec)
#define MAX_GEDCOM_line 256
// maximum amount of text we will place on a line before splitting with CONC
#define MAX_GEDCOM_text 80
// maximum depth of GEDCOM structure we expect
#define MAX_GEDCOM_levels 20
#define MAX_TREE_GENERATIONS 35

#define MAX_TAG 12
// longest tag we are likely to encounter - longest I have seen
// are 11 + '\0' (eg. _MARR_CMLAW ). These are likely to vanish
// if GEDCOM 5.5 deprecates them ...

#define MAX_indititle  256
#define MAX_notestitle 256
#define MAX_famtitle   256

// all infinity has to do is be bigger than the pixel width/height of
// any tree we might want to draw, and be representable as both + and
// - infinity in an int.

#define INFINITY 32767
#define GAP 20

// static storage defined in main.cxx:

extern infoUI       *aboutbox;
extern prefUI       *choicebox;
extern findUI       *gotobox;
extern editlist     *editUIs;
extern famedlist    *famUIs;
extern noteslist    *noteUIs;
extern instancelist *trees;
extern char          person_buffer[];

extern GEDCOM_tag   *first_tag;
extern GEDCOM_tag   *last_tag;

extern int xMax[];
#endif
