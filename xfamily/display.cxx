// > display.cxx

#include "fixes.h"

#include <string>
#include <stdio.h>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_draw.H>

#include "classes.h"
#include "family.h"
#include "display.h"
#include "trees.h"
#include "gui.h"
#include "objects.h"
#include "strings.h"
#include "tags.h"

// The canvas upon which we draw our family tree is a subclass of Fl_Button,
// which provides us with the callbacks to make popup menus happen, and
// know about where their context is on the canvas itself.

// There is an issue on X displays in that the canvas pixelmap is limited
// to 32kx32k which we can easily exceed in the x-direction with a biggish
// tree. Sometime we need to implement the canvas we present to X as being
// a window into an arbitralily large bitmap we store internally - but this
// will probably lose us access to the FLTK drawing and font code :-( But,
// in fact, from fltk 1.3, this issue seems to have gone away.

treedisplay::treedisplay( int x, int y, int w, int h ) :
  Fl_Button( x,y, w,h ),
  view (NULL),
// eventually the fontface and size of the main display will be configurable from
// the preferences box (as will the printing face and size, of course)
  fontface (FL_HELVETICA_BOLD),
  fontsize (XF_FONT_SIZE)
{
}

void treedisplay::setview( mainUI* theview ) {
  view = theview;
}

void treedisplay::setscroller( Fl_Scroll* thescroller ) {
  scroller = thescroller;
}

GEDCOM_object *treedisplay::whoisat( int x, int y ) {

int            xoffset;
int            yoffset;

  xoffset = scroller->xposition();
  yoffset = scroller->yposition();

  x += xoffset;
  y += yoffset;
  return view->whattodraw()->whoisat(x,y);
}

void treedisplay::draw() {

// currently not tested - we just show names anyway: char          *showname;
// but it is something we are intending to set with an options dbox, so don't just forget it...
displaytree   *display;
indidisplay   *person;
int            xoffset;
int            yoffset;

  // get the current size and position from Fl_Button's methods - we may
  // have been resized in response to the tree being recalculated:
  fl_push_clip(x(), y(), w(), h());
  // whatever else we may cock up, we should now draw only where we are
  // supposed to ! Lets clear our background to white so we can see:
  fl_color(FL_WHITE);
  // this demonstrably overwrites the scroll bars, so we must deduce that
  // the fl_clip above is not restrictive enough. I suspect that the clip
  // rectangle is inherited when the window is resized and that we just
  // overrode it with something too big. But even if this is untrue and we
  // really do need to set it ourselves, then we should get it right !
  fl_rectf(x(), y(), w(), h());

  xoffset = scroller->xposition();
  yoffset = scroller->yposition();

  // The coordinate we pass to fl_draw turns
  // out to be the bottom left of the bounding box of the string, and the
  // offsets are within the *window*, not the treedisplay widget, so we
  // always need to add 25 for the height of the menu bar. This is done
  // by setting the initial y to addmarriages to 25 - everything then falls out

  // this shouldn't happen, but we had better check:
  if (view==NULL) {
#ifdef debugging
    printf("treedisplay::draw got a null view/n");
#endif
    fl_pop_clip(); return; }

  fl_font( fontface, fontsize );
  display = view->whattodraw();
  // on the other hand, it is theoretically possible to have a display
  // with no current person set (it might be an empty tree, for instance)
  if (display == NULL) { fl_pop_clip(); return; }

  person = display->gettop();
  if (person == NULL ) { fl_pop_clip(); return; }

  display->setfont();

  person->displayindi( display->getvint(), xoffset, yoffset );
  switch ( person->getgender() ) {
    case SEX_MALE:   fl_color(COLOUR_MALE); break;
    case SEX_FEMALE: fl_color(COLOUR_FEMALE); break;
    case SEX_UNKNOWN:fl_color(COLOUR_UNKNOWN); break;
  }
  fl_pop_clip(); // AERW added 2003-12-01 - shouldn't this be here, or have I
  // misunderstood horribly ? Nope, adding that fixes a longstanding bug :-)

  // However, having an empty tree still causes a segfault, which may be
  // something in here.
}

/////////////////////////////////////////////////////////////////////////

indidisplay::indidisplay( GEDCOM_object *person ) :
  indi (person),
  fams (NULL),
  next (NULL),
  centrex (INFINITY),
  bottomy (INFINITY)
{
  GEDCOM_object *obj;
  std::string tempstring;
  tempstring = "";
  int i;
  char* val;

  obj = indi->subobject( SEX_tag );
  if (obj == NULL) gender = SEX_UNKNOWN;
  else {
    char* sex = obj->value();
    if ((*sex)=='M') gender = SEX_MALE;
    else {
      if ((*sex)=='F') gender = SEX_FEMALE;
      else gender = SEX_UNKNOWN;
    }
  }

  /* if we are showing titles... (with present code, we always are) */
  obj = indi->subobject( TITL_tag );
  if (obj!=NULL) {
    tempstring += obj->value();
    tempstring += " ";
  }
  /* end of if */
  obj = indi->subobject( NAME_tag );
  if (obj==NULL) tempstring += msg_unnamed;
    else tempstring += obj->value();
  showname = new GEDCOM_string( (char *)tempstring.c_str() );

  tempstring = "?";
  obj = indi->subobject( BIRT_tag );
  if (obj != NULL) {
    obj = obj->subobject( DATE_tag );
    if (obj != NULL ) {
      val = obj->value();
      if ((*val)=='@') {
        i=2; while ((*(val+i))!='@') i++;i+=2;
        tempstring = (val+i);
      }
      else tempstring = val;
    }
  }
  // ideally, in the absence of a birth date, we'd like to show a CHR or BAPM
  // date in a different colour. We'd also like to provide a colour key to the
  // situation in which BIRT has more than one DATE - if we have different dates
  // from different sources, for example. Same applies to DEAT - we may have a
  // CREM or BURI date in the absence of a DEAT date, or again, may have alternatives
  tempstring += " - ";
  // a date string really ought to be provided by a method somewhere - code like
  // this here has this class knowing rather a lot about GEDCOM internals
  obj = indi->subobject( DEAT_tag );
  if (obj != NULL) {
    obj = obj->subobject( DATE_tag );
    if (obj != NULL ) {
      val = obj->value();
      if ((*val)=='@') {
        i=2; while ((*(val+i))!='@') i++;i+=2;
        tempstring += (val+i);
      }
      else tempstring += val;
    }
  }
  dates = new GEDCOM_string( (char *)tempstring.c_str() );
  // ah... bollox. We should get the font values from our displaytree,
  // but we don't keep a pointer to that, which is a bit of a blow :-(
  // FIXME need to add to the class, otherwise we won't understand
  // when we implement configurability and it fails to work...
  fl_font( FL_HELVETICA_BOLD , XF_FONT_SIZE );
  widthn = (int)(1+fl_width( showname->string() ));
  widthd = (int)(1+fl_width( dates->string() ));
}

indidisplay::~indidisplay() {
  delete dates;
  delete showname;
  if (fams != NULL) delete fams;
  if (next != NULL) delete next;
}

GEDCOM_object* indidisplay::getperson() const { return indi; }

#ifdef fix0002
// this code now obsoleted by fix0003 - see fixes.h (we hope)
indidisplay *indidisplay::findindi( GEDCOM_object* person ) {

indidisplay *indicheck;
indidisplay *obj;
famdisplay  *famcheck;

  if (this==NULL) return NULL;
#ifdef debugging
  printf("indidisplay::findindi Starting to check with %s\n", this->name());
#endif
  if (this->indi == person) return this;
  indicheck = this->sibling();
  if (indicheck != NULL) {
#ifdef debugging
    printf("Checking %s\n", indicheck->name());
#endif
    if ((obj = indicheck->findindi( person )) != NULL) {
#ifdef debugging
      printf("obj = indicheck->findindi( person ) is %s\n",obj->name());
#endif
      return obj;
    }
  }
  famcheck = this->family();
  while (famcheck != NULL ) {
    if ((indicheck = famcheck->findindi( person )) != NULL) return indicheck;
    famcheck = famcheck->nextfam();
  }
  return NULL;
}
#endif

indidisplay*   indidisplay::sibling() const { return next; }
indidisplay*   indidisplay::last() const {
indidisplay* sib;
  sib = (indidisplay*)this; // discard const-ness to keep pedantic ANSI C++ happy
  while (sib->sibling() != NULL) sib=sib->sibling();
  return sib;
}
void           indidisplay::setsibling( indidisplay* sib ) { next=sib; }
famdisplay*    indidisplay::family() const { return fams; }
void           indidisplay::setfamily( famdisplay* fam ) { fams=fam; }
int            indidisplay::x() const { return centrex; }
int            indidisplay::y() const { return bottomy; }
void           indidisplay::sety( int y ) { bottomy = y; }
int            indidisplay::namewidth() const { return widthn; }
int            indidisplay::datewidth() const { return widthd; }
char*          indidisplay::name() const { return showname->string(); }
char*          indidisplay::life() const { return dates->string(); }
void           indidisplay::setplace( int xc, int yb, int xl, int xr ) {
                centrex=xc; bottomy=yb; xmin=xl; xmax=xr; }
int            indidisplay::getgender() const { return gender; }

bool indidisplay::findfam( famdisplay* self, GEDCOM_object* fam ) const {
// parse the structure below this-> (which, on the first call, is treetop)
// to see if this family has already been created in the displaytree - in
// this case we won't want to display all the issue again.

// WATCHIT ! the nature of our recursion means we may be called on a NULL

// hmmm, this is costly - it is order N squared for the number of famdislays
// which becomes order N cubed over the number of times it is called
// an order N method would be to keep a linked list of all the famdisplays ?
// that's order N to build, order N to search, and order N squared overall

famdisplay* check;

  if (this==NULL) return false;

  check = fams; // pointer to the famdisplay for the FAM object will be
  // NULL for unmarried individuals with no children
  while (check != NULL) {
    if (check != self) { // otherwise we will always find ourselves !
      if (fam == check->getfamily()) return true;}
    if ( (check->getspouse())->findfam(self, fam) ) return true;
    if ( (check->getissue())->findfam(self, fam) ) return true;
    check = check->nextfam(); // will become NULL for last family
  }
  return (this->next)->findfam(self, fam);
}

void indidisplay::displayindi( int h, int xoffset, int yoffset ) const {

int x, y;

  x = centrex - xoffset;
  y = V_OFFSET + bottomy - h - yoffset;
  fl_color( FL_BLACK );
  fl_line( x, y, x, y - (h>>1));

  this->centre( h, xoffset, yoffset );

  if (next != NULL) next->displayindi( h, xoffset, yoffset );
  if (fams != NULL) fams->displayfam( h, xoffset, yoffset );
}

void indidisplay::centre( int h, int xoffset, int yoffset ) const {

int x, y;

  switch (gender) {
    case SEX_MALE:   fl_color(COLOUR_MALE); break;
    case SEX_FEMALE: fl_color(COLOUR_FEMALE); break;
    case SEX_UNKNOWN:fl_color(COLOUR_UNKNOWN); break;
  }
  // first do the [title] name

  x = centrex - (widthn >> 1 ) - xoffset;
  y = bottomy - yoffset;
  fl_draw( showname->string(), x, y );

  /* if we are showing dates : */
  x = centrex - (widthd >> 1 ) - xoffset;
  y += h;
  fl_color( FL_BLACK );
  fl_draw( dates->string(), x, y);
  /* end of if */
}

GEDCOM_object *indidisplay::whoisat( int h, int x, int y ) const {

GEDCOM_object *obj;
indidisplay *indicheck;
famdisplay  *famcheck;

  if (this == NULL) return NULL;
  // if this indidisplay encloses the point, return the pointer to the INDI
  if (this->testat(h,x,y)) return this->getperson();
  indicheck = this->sibling();
#ifndef test0002
  while (indicheck != NULL) {
#else
  if (indicheck != NULL) {
#endif
    if ((obj = (indicheck->whoisat( h, x, y ))) != NULL ) return obj;
#ifndef test0002
    // think this is gratuitously testing everything extra times
    indicheck = indicheck->sibling();
#endif
  }
  famcheck = this->family();
  while (famcheck != NULL) {
    if ((obj = (famcheck->whoisat( h, x, y ))) != NULL) return obj;
    famcheck = famcheck->nextfam();
  }
  return NULL;
}

bool indidisplay::testat( int h, int x, int y ) const {

  if (y > (bottomy + h)) return false;
  if (y < (bottomy - h)) return false;
  int w = widthn; if (widthd>w) w=widthd;
  w = w>>1;
  if (x > (centrex + w)) return false;
  if (x < (centrex - w)) return false;
  return true;
}

/////////////////////////////////////////////////////////////////////////

famdisplay::famdisplay( GEDCOM_object *family, indidisplay *spousedisplay, int sequence ) :
  fam (family),       // the FAM object
  spouse (spousedisplay),
  next (NULL),
  centrex (INFINITY),
  bottomy (INFINITY),
  issue (NULL)
{
GEDCOM_object *object;
std::string         labeltext;
char * val;
int i;

  labeltext = "=";
  /* FIXME: we should only dereference to the date if we are actually
     meant to be showing marriage dates */
  object = family->subobject( MARR_tag );
  if (object!=NULL) object = object->subobject( DATE_tag );
  if (object!=NULL) {
    labeltext += " ";
    val = object->value();
    if ((*val)=='@') {
      i=2; while ((*(val+i))!='@') i++;i+=2;
      labeltext += (val+i);
    }
    else labeltext += val;
  } else {
    if (sequence != 0) {
      char seq[3]; // no-one marries more than 99 times :-)
      labeltext += " ";
      // not under gnu labeltext += itoa( sequence, seq, 10 );
      sprintf( seq, "%d", sequence );
      labeltext += seq;
    }
  }
  label = new GEDCOM_string((char *)labeltext.c_str()); // discard const-ness for pedantic ANSI
  // this was commented out, and things didn't seem to be changing ...
  fl_font( FL_HELVETICA_BOLD , XF_FONT_SIZE );
  width = (int)(1+fl_width( label->string() ));
}

famdisplay::~famdisplay() {
  delete label;
  if (spouse != NULL) delete spouse;
  if (issue != NULL) delete issue;
  if (next != NULL) delete next;
}

GEDCOM_object* famdisplay::getfamily() const { return fam; }
indidisplay*   famdisplay::getspouse() const { return spouse; }
famdisplay*    famdisplay::nextfam() const { return next; }
void           famdisplay::setnext( famdisplay *nextfam ) { next=nextfam; }
int            famdisplay::x() const { return centrex; }
void           famdisplay::setx(int x) { centrex = x; }
int            famdisplay::y() const { return bottomy; }
void           famdisplay::sety(int y) { bottomy = y; }
int            famdisplay::marrwidth() const { return width; }
char*          famdisplay::marrlabel() const { return label->string(); }
indidisplay*   famdisplay::getissue() const { return issue; }
void           famdisplay::setissue( indidisplay* child ) { issue = child; }

#ifdef fix0002
indidisplay *famdisplay::findindi( GEDCOM_object *person ) {

indidisplay *indicheck;
indidisplay *obj;

  if (this == NULL) return NULL;
  if ((indicheck = this->getspouse()) != NULL) {
    if (indicheck->getperson() == person ) return indicheck;
  }
  if ((indicheck = this->getissue()) != NULL) {
    if ((obj = indicheck->findindi( person )) != NULL ) return obj;
  }
  return NULL;
}
#endif

void famdisplay::displayfam( int h, int xoffset, int yoffset ) const {

  this->centre( h, xoffset, yoffset );

  if (spouse != NULL) spouse->centre( h, xoffset, yoffset );
  if ( next  != NULL) next->displayfam( h, xoffset, yoffset );
  if (issue != NULL) {
    issue->displayindi( h, xoffset, yoffset );
    int x1,x2,y;
    x1 = issue->x() - xoffset;
    x2 = issue->last()->x() - xoffset;
    if (x1 != x2) {
      y = V_OFFSET + bottomy + ((5*h) >>1) - yoffset;
      fl_color( FL_BLACK );
      fl_line( x1, y, x2, y);
    }
  }
}

void famdisplay::centre( int h, int xoffset, int yoffset ) const {

int x, y;

  fl_color( FL_BLACK );
  x = centrex - (width >> 1) - xoffset;
  y = bottomy - yoffset;
  fl_draw( label->string(), x, y);
  if (issue==NULL) return;
  y += h + V_OFFSET;
  /* if we are showing spouses dates */
  y += h;
  /* end of if */
  x = centrex - xoffset;
  fl_line( x, y, x, y + (h>>1));
}

GEDCOM_object *famdisplay::whoisat( int h, int x, int y ) const {

GEDCOM_object *obj;
// not sure what these were going to be for
//indidisplay *indicheck;
//famdisplay  *famcheck;

  if (this == NULL) return NULL;
  if (this->testat(h,x,y)) return this->getfamily();
  if ((obj = (this->getspouse()->whoisat( h, x, y ))) != NULL) return obj;
  if ((obj = (this->getissue()->whoisat( h, x, y ))) != NULL) return obj;
  return NULL;
}

bool famdisplay::testat( int h, int x, int y ) const {

  if (y > bottomy) return false;
  if (y < (bottomy - h)) return false;
  int w = width>>1;
  if (x > (centrex + w)) return false;
  if (x < (centrex - w)) return false;
  return true;
}

/////////////////////////////////////////////////////////////////////////

indidisplay* displaytree::gettop() const { return treetop; }

#ifdef fix0002
indidisplay *displaytree::findindi( GEDCOM_object* person ) {
#ifdef debugging
  printf("displaytree::gettop() looking for %s\n",person->subobject(NAME_tag)->value());
#endif
  return treetop->findindi( person );
}
#endif

#ifdef fix0003
indidisplay* displaytree::getcurrent() const { return indicurrent; }
#endif

#ifdef fix0003
displaytree::displaytree( GEDCOM_object *treeroot, GEDCOM_object *current ) :
  treecurrent(current),
#else
displaytree::displaytree( GEDCOM_object *treeroot ) :
#endif
  ymax (0),
  xmin (0),
  xmax (0)  // or some minimum width
{
  { int i; for (i=0; i<MAX_TREE_GENERATIONS; i++) xMax[i]=0; }
  treetop = new indidisplay( treeroot );
  fontface = FL_HELVETICA_BOLD; // eventually these will be configurable
  fontsize = XF_FONT_SIZE;
  fl_font( fontface , fontsize );
  gap = (int)(1+fl_width( "XX" ));
  lineheight = fl_height();
}

displaytree::~displaytree() {
  delete treetop;
}

void displaytree::buildtree() {

/* the buildtree process needs to have access to the whole tree built so
   far, in order to prevent sets of descendants appearing twice. This
   means it cannot be part of the displaytree constructor, since the
   pointer to the top of the new tree would not be available until the
   constructor exits */

  addmarriages( treetop );
}

void displaytree::addmarriages( indidisplay *newdisplay ) {

/* on entry: we expect that the indidisplay is already constructed, so
             indi, widthn, widthd, showname & dates will be set up
   purpose:  to create an indidisplay for each spouse and a famdisplay
             for each marriage and set up pointers. For each famdisplay,
             call adddescendants to create child structures */

GEDCOM_object *person;  // the INDI object represented by this display node
GEDCOM_object *fams;    // first or subsequent FAMS object for person
GEDCOM_object *family;  // FAM object dereferenced from FAMS
GEDCOM_object *spouse;  // person's spouse in this FAM
indidisplay   *spousedisplay;
famdisplay    *newfam;newfam=NULL;  // display object for the FAM
famdisplay    *oldfam;oldfam=NULL;  // needed to set up a next pointer
int famnumber = 0;      // zero if we don't need to sequence-number marriages, either
			// because there are 0 or 1

  person = newdisplay->getperson(); // look at the INDI object
#ifdef fix0003
  if (person==treecurrent) indicurrent = newdisplay;
#endif
  
  fams = person->subobject( FAMS_tag ); // see if it contains a FAMS subobject
  // WARNING! fams is a FAMS object - not the FAM object to which it points

  if (fams == NULL) return; // no known spouse or issue

  if ((fams->next_object( FAMS_tag )) != NULL) famnumber = 1;
  // if there are more than one FAMS subobjects, then this is number 1

  while (fams != NULL) { // for each FAMS subobject, we need to dereference to the FAM

    family = fams->followxref(); // get the FAM object
    // this should never return null, and the object should always be a FAM. However
    // it can happen for broken GEDCOM and we should cope, not core dump ...
    if (family==NULL) {
      printf("display.cxx displaytree::addmarriages: fams->followxref() dereferenced to NULL\n");
      printf("Trying to dereference from this FAMS:\n");
      fams->outline( stdout, 1 );
    }

    spouse = family->thespouseof( person );
    if (spouse != NULL) {
      spousedisplay = new indidisplay( spouse );
      if (spousedisplay==NULL) printf("displaytree::addmarriages out of heap ?\n");
    } else spousedisplay = NULL;
    oldfam = newfam;
    newfam = new famdisplay( family, spousedisplay, famnumber );
    if (newfam==NULL) printf("displaytree::addmarriages out of heap ?\n");
    if (oldfam == NULL) newdisplay->setfamily( newfam );
     else oldfam->setnext( newfam );
    this->adddescendants( newfam );
    famnumber++;
    fams = fams->next_object( FAMS_tag );
  }
}

void displaytree::adddescendants( famdisplay* newdisplay ) {

/* on entry: we expect that the famdisplay is already constructed, so
             fam, spouse, label and width will be set up
   purpose:  to create an indidisplay for each child and, for each,
             *if those descendants have not already appeared*
             call addmarriages to create family structures */

GEDCOM_object *family;
GEDCOM_object *chil;
GEDCOM_object *child;
indidisplay   *childdisplay = NULL;
indidisplay   *oldersib;

/* the famdisplay constructor has allocated pointers to the FAM object
   and the spouse's display object (if there is a spouse) */

  family = newdisplay->getfamily(); 

  if ( treetop->findfam( newdisplay, family ) ) {
#ifdef debugging
    printf("displaytree::adddescendants nothing to do - this lot already shown\n");
#endif
    return; // nothing else to do, as
            // we don't wish descendants to be shown twice
  }
  chil = family->subobject( CHIL_tag );
  while (chil != NULL) {
    child = chil->followxref();
    oldersib = childdisplay;
    childdisplay = new indidisplay( child );
    if (childdisplay==NULL) printf("displaytree::adddescendants out of heap ?\n");
    if (oldersib == NULL)
      newdisplay->setissue( childdisplay );
    else oldersib->setsibling( childdisplay );
    this->addmarriages( childdisplay );
    chil = chil->next_object( CHIL_tag );
  }
}

void displaytree::calctree() {
int z=0; // only needed recursively - no use when finally returned to us
  calcmarriages( treetop, 25, 0, z, xmin, xmax );
}

void displaytree::calcmarriages( indidisplay* display, int y, int gen,
  int &x, int &xlt, int &xrt ) {

// on entry: y   is *top* of this indi
//           gen starts at 0 for the top of display and spouse(s)

/* we should not need to look at any of the GEDCOM_objects again */

int OxMax[MAX_TREE_GENERATIONS];
int width; int w; int yindi;
int ydate;
famdisplay  *family;
// don't seem to use: indidisplay *spouse;
int xf, xlf, xrf;  // passed by reference to/from calcdescendants
int xr, x1, pass;

  width = display->namewidth();
  yindi = (y += lineheight);
  /* if we are showing dates: */
  if ((w = display->datewidth()) > width ) width = w;
// we don't seem to be using ydate (and the compiler whinges), but we *do* need the side-effect of incrementing y !!
  ydate = (y += lineheight); // y for lifedates
  /* end of the if */
  if (width>0) width += gap;
  y += lineheight; // y for marriage
  if (y > ymax) ymax = y;

  { int i; for (i=0;i<MAX_TREE_GENERATIONS;i++) OxMax[i]=xMax[i]; }

  for (pass=0;pass<2;pass++) {
    x = OxMax[gen]; // just now, x is person's left edge
    xr = x + width;
    xlt = x;
    xrt = xr;
    x += width/2;   // now person's centre
    x1 = 0; xf = x1; 
    family = display->family();
    while (family != NULL) {
      family->sety( y );
      this->calcdescendants( family, y, gen, xf, xlf, xrf );
      family->setx(xf);
      if (x1==0) {
        x1=xf;
        if (xlf < xlt ) xlt = xlf;
      }
      if (xrf > xrt) xrt = xrf;
      family = family->nextfam();
    }
    if (x1 != 0) {
      xf = (x1 + xf) >> 1;
      if (xf >= x) pass++;
      switch (pass) {
        case 0: {
          // 1st pass, when second is needed
          int i;
          x -= xf;
          for ( i=0; i<gen;i++ )                 xMax[i]=OxMax[i];
          for (i=gen;i<MAX_TREE_GENERATIONS;i++) xMax[i]=OxMax[i]+x;
        }
        case 1: {
          // 2nd pass, or only one pass needed
          x = xf;
          xr = xf + (width>>1);
        }
      } // end switch
    }   // end if x1!=0
    else pass ++;
  }     // end loop in pass
  if (xr > xMax[gen]) xMax[gen] = xr;
  display->setplace(x,yindi,xlt,xrt);
}

void displaytree::calcdescendants( famdisplay* display, int y, int gen,
  int &x, int &xlt, int &xrt ) {

// on entry: y   is *top* of spouse for this family
//           gen is same as gen for the indi

/* we should not need to look at any of the GEDCOM_objects again */

int OxMax[MAX_TREE_GENERATIONS];
int width; int w; int yindi; 
int ydate;
indidisplay *childdisplay;
indidisplay *spousedisplay;
int xc, xlf, xrf; // passed to/from adddescendants by reference
int xr, x1, pass;
bool nodisplay=false;

  spousedisplay = display->getspouse();
  yindi = (y += lineheight); // y correct for spouse
  if (spousedisplay != NULL) {
    width = spousedisplay->namewidth();
    /* if we are showing dates: */
    if ((w =spousedisplay->datewidth()) > width ) width = w;
    ydate = (y += lineheight); // y for spousedates
    /* end of if */
    if (width>0) width += gap;
  } else {
    /* if we were showing dates */
    y += lineheight;
    /* end of if */
    width = 0;
  }

  { int i; for (i=0;i<MAX_TREE_GENERATIONS;i++) OxMax[i]=xMax[i]; }
  y += lineheight; // y for *top* of next generation
  if (y > ymax) ymax = y;

  for (pass=0;pass<2;pass++) {
    x = xMax[gen]; // just now, x is spouse's left edge
    xr = x + width;
    xlt = x;
    xrt = xr;
    x += width/2;   // now spouse's centre
    x1 = 0;
    if (gen<MAX_TREE_GENERATIONS) {
      childdisplay = display->getissue();
      nodisplay = ((spousedisplay == NULL) && (childdisplay == NULL));
      while (childdisplay != NULL) {
        this->calcmarriages( childdisplay, y, gen+1, xc, xlf, xrf );
        if (x1 == 0) {
          x1 = xc;
          if (xlf < xlt) xlt = xlf;
        }
        if (xrf > xrt) xrt = xrf;
        childdisplay = childdisplay->sibling();
      }
      if (x1 != 0) {
        xc = (x1 + xc) >> 1;
        if (xc > x) pass++;
        switch (pass) {
          case 0: {
            // 1st pass, when second is needed
            int i;
            x -= xc;
            for ( i=0;   i<=gen;i++ )                xMax[i]=OxMax[i];
            for (i=gen+1;i<MAX_TREE_GENERATIONS;i++) xMax[i]=OxMax[i]+x;
          }
          case 1: {
            // 2nd pass, or only one pass needed
            x = xc;
            xr = x + (width >> 1);
          }
        } // end switch
      }   // end if x1!=0
    }     // end if gen<MAX
    if (nodisplay) { x=0; pass++; }
  }     // end loop in pass
  if (xr > xMax[gen]) xMax[gen] = xr;
  if (spousedisplay!=NULL) {
    spousedisplay->setplace(x,yindi,0,0);
  }
}

void displaytree::setfont() const {
  fl_font( fontface, fontsize );
}

int displaytree::getvint() const { return lineheight; }
int displaytree::xsize() const { return xmax; }
int displaytree::ysize() const { return ymax; }

GEDCOM_object *displaytree::whoisat( int x, int y ) const {
  return treetop->whoisat( lineheight, x, y );
}


