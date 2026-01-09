// family.h

#ifndef family_h
#define family_h

#include "classes.h"

/**********************************************************************************
***********************************************************************************
****                                                                           ****
****  W A R N I N G ! !   There are a series of arbitrary limits defined here  ****
****                      which have, in the real world, proven to be based    ****
****                      on bad assumptions (even those which are based on    ****
****  limits which are laid down in the GEDCOM specification). This runs the   ****
****  risk of various buffer overruns and other nasties which can (at best)    ****
****  cause a crash and core dump. At worst, there is a theoretical chance     ****
****  that they could allow the execution of arbitrary code (by whom ?)        ****
****                                                                           ****
***********************************************************************************
**********************************************************************************/

#define MAX_FILENAME 256

// maximum length of a line we expect (as specified in GEDCOM spec)
// note that real world GEDCOM files may not respect this limit, even though
// there is a perfectly good mechanism (the CONC tag) to avoid hitting it.
// mostly this seems to result in us binning stuff because the bit that wraps
// into the next buffer, when parsed for a level number, yields nonsense which
// we flag with a "bad level" error. But if the arbitrary text has a number in
// it small enough to interpret as a valid level number, we could completely
// break the GEDCOM structure. We need a mechanism to check for line overruns
// on input, so we can create CONC records internally (and on output)

#define MAX_GEDCOM_line 256

// maximum amount of text we will place on a line before splitting with CONC
// when we are putting arbitrary user-supplied strings into GEDCOM tag lines

// 2013-01-14 this is causing us to split lines in existing GEDCOM files,
// which makes doing diff between loaded and saved files get filled with
// diffs between unsplit and split lines. This means that useful debugging
// differences are getting lost in the noise, so temporarily increase this
// to a safe value which is longer than any lines in our test file. The
// original value was 80.

#define MAX_GEDCOM_text 200

// maximum depth of GEDCOM structure we expect - GEDCOM generating software
// will need to be being deliberately perverse to exceed this, although it
// is perfectly permissible in theory...

#define MAX_GEDCOM_levels 20

// this is an unpleasant and unreasonable arbitrary limit:
// (although, in practice, a tree this deep will exceed the X limit on bitmaps when
// you try to display it - an arbitrary limit we do hit and can't circumvent )-:

#define MAX_TREE_GENERATIONS 35

#define MAX_TAG 16

// longest tag we are likely to encounter - longest I have seen
// are 11 + '\0' (eg. _MARR_CMLAW ). These are likely to vanish
// if GEDCOM 5.5 deprecates them ...

// window titles depend on the pathname of the file we are browsing, which
// could potentially be arbitrarily long. In practice, we should truncate
// such long filenames with an ellipsis as in "/usr/local ... /test/test2.ged"
// such that the title actually fits on the title bar !

#define MAX_indititle  256
#define MAX_notestitle 256
#define MAX_famtitle   256

// all infinity has to do is be bigger than the pixel width/height of
// any tree we might want to draw, and be representable as both + and
// - infinity in an int. Some big trees might well be more than sixteen
// times the width of a screen, I'm not sure that a 16-bit int is up to
// this job... It turns out that INFINITY is defined in <math.h> anyway,
// but not everything uses that ... so

#ifndef INFINITY
#define INFINITY 32767
#endif

#define GAP 20

// static storage defined in main.cxx: in practice these are things that we only
// ever have one of. Heads of lists or graph structures, for example, or once-only
// windows. It may prove to be the case that we need more than one completionsUI
// window if they are used in different contexts (such as having a "goto person"
// completions window open whilst trying to fill in a parent in an editUI by
// generating a completionsUI for possible matches ...)

extern infoUI       *aboutbox;
extern prefUI       *choicebox;
extern indifindUI        *indigotobox;
extern indicompletionsUI *indicompletionsbox;
extern famfindUI         *famgotobox;
extern famcompletionsUI  *famcompletionsbox;
extern editlist     *editUIs;
extern famedlist    *famUIs;
extern noteslist    *noteUIs;
extern instancelist *trees;
extern char          person_buffer[];

extern GEDCOM_tag   *first_tag;
extern GEDCOM_tag   *last_tag;

extern int xMax[];
extern int GEDCOMcount;
#endif
