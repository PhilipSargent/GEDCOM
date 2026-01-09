// > display.h
#ifndef display_h
#define display_h

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scroll.H>

#include "classes.h"
#include "fixes.h"

#define SEX_UNKNOWN 0
#define SEX_MALE    1
#define SEX_FEMALE  2

#define COLOUR_UNKNOWN 59
// unknown gender is dark green (FL_GREEN is far too bright)
// there aren't fancy names for all the colours and using the
// fl_colourcube call uses code which we don't need.
#define COLOUR_MALE    FL_BLUE
#define COLOUR_FEMALE  FL_RED

#define V_OFFSET 2
// font 20 is a bad idea in that it limits the number of generations you can display
// before hitting the limiting X bitmap size of 32k pixels, but we're chossing this
// for a binary specifivally to run on washburn as otherwise it is too small to read
#define XF_FONT_SIZE 20

class treedisplay : public Fl_Button {
  mainUI* view;
  Fl_Scroll* scroller;
#ifdef fix0003
  indidisplay* currentindi;
#endif
  int fontface;
  int fontsize;
  void draw();

public:
               treedisplay( int, int, int, int );
void           setview( mainUI* );
void           setscroller( Fl_Scroll* );
#ifdef fix0011
GEDCOM_object *whoisat( int, int, indidisplay*& indid, famdisplay*& famd );
#else
GEDCOM_object *whoisat( int, int );
#endif
#ifdef fix0010
  void drawoffscreen();
#endif
};

class indidisplay {

  GEDCOM_object  *indi;
  famdisplay     *fams;
#ifdef fix0011
  famdisplay     *famc;
#endif
  indidisplay    *next;
  int             centrex;
  int             bottomy;
  int             widthn;
  int             widthd;
  int             xmin;
  int             xmax;
  GEDCOM_string  *showname;
  GEDCOM_string  *dates;
  int             gender;    // in fact an enumeration

public:
                 indidisplay( GEDCOM_object* );
                 ~indidisplay();
#ifdef fix0011
void             setfamc( famdisplay* );
bool             younger_valid();
#endif
GEDCOM_object   *getperson() const;
indidisplay     *sibling() const;
indidisplay     *last() const;
void             setsibling( indidisplay* );
famdisplay      *family() const;
void             setfamily( famdisplay* );
int              x() const;
//void             setx(int);
int              y() const;
void             sety(int);
int              namewidth() const;
int              datewidth() const;
char            *name() const;
char            *life() const;
//void             setxmin(int);
//void             setxmax(int);
void             setplace(int,int,int,int);
int              getgender() const;
bool             findfam( famdisplay*, GEDCOM_object* ) const;
void             displayindi( int, int, int ) const;
void             centre( int, int, int ) const;
#ifdef fix0011
GEDCOM_object   *whoisat( int, int, int, indidisplay*& indid, famdisplay*& famd ) const;
#else
GEDCOM_object   *whoisat(int,int,int) const;
#endif
bool             testat(int,int,int) const;

};

/////////////////////////////////////////////////////////////////////////

class famdisplay {

#ifdef fix0011
  GEDCOM_object  *indi;
#endif
  GEDCOM_object  *fam;
  indidisplay    *spouse;
  famdisplay     *next;
  int             centrex;
  int             bottomy;
  int             width;
  GEDCOM_string  *label;
  indidisplay    *issue;

public:
#ifdef fix0011
               famdisplay( GEDCOM_object*, GEDCOM_object*, indidisplay*, int );
#else
               famdisplay( GEDCOM_object*, indidisplay*, int );
#endif
               ~famdisplay();
#ifdef fix0011
GEDCOM_object *getindi() const;
#endif
GEDCOM_object *getfamily() const;
indidisplay   *getspouse() const;
famdisplay    *nextfam() const;
void           setnext( famdisplay* );
int            x() const;
void           setx( int );
int            y() const;
void           sety( int );
int            marrwidth() const;
char          *marrlabel() const;
indidisplay   *getissue() const;
void           setissue( indidisplay* );
#ifdef fix0011
bool           later_valid();
#endif
void           displayfam( int, int, int ) const;
void           centre( int, int, int ) const;
#ifdef fix0011
GEDCOM_object *whoisat( int, int, int, indidisplay*& indid, famdisplay*& famd ) const;
#else
GEDCOM_object *whoisat(int,int,int) const;
#endif
bool           testat(int,int,int) const;

};

/////////////////////////////////////////////////////////////////////////

class displaytree {

  indidisplay   *treetop;
#ifdef fix0003
  GEDCOM_object *treecurrent;
  indidisplay   *indicurrent;
#endif
  int            xMax[MAX_TREE_GENERATIONS];
  int            ymax;   // "ymin" would be zero
  int            xmin;   // xMin should be zero
  int            xmax;   //
  int            fontface;
  int            fontsize;
  int            lineheight;
  int            gap;
#ifdef fix0012
  void*          eventobj;
#endif

public:
#ifdef fix0003
               displaytree( GEDCOM_object*, GEDCOM_object* );
#else
               displaytree( GEDCOM_object* );
#endif
               ~displaytree();
indidisplay   *gettop() const;
#ifdef fix0003
indidisplay   *getcurrent() const;
#endif
void           buildtree();
void           addmarriages( indidisplay* );
void           adddescendants( famdisplay* );
void           calctree();
void           calcmarriages( indidisplay*, int, int, int&, int&, int& );
void           calcdescendants( famdisplay*, int, int, int&, int&, int& );
void           setfont() const;
int            getvint() const;
int            xsize() const;
int            ysize() const;
#ifdef fix0012
void           setevent( void* );
void*          getevent();
#endif
#ifdef fix0011
GEDCOM_object *whoisat( int, int, indidisplay*& indid, famdisplay*& famd ) const;
#else
GEDCOM_object *whoisat( int, int ) const;
#endif

};

#endif
