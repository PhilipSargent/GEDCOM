// objects.cxx

#include "fixes.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <FL/fl_ask.H>

#include "classes.h"
#include "family.h"

#include "objects.h"
#include "tags.h"

// only for debugging:
#include <stdio.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////
// class GEDCOM_tag:  all the tags we "know" about and any new ones we
//                    find in a GEDCOM file get kept in a linked list
// which should be global to the whole program.
// We don't define a destructor - we should never need to lose a tag
// once we've seen it (and being able to dump a list of every tag we've
// ever seen might help debug typos in GEDCOM files)

// Statically allocate space for pointers to tags. Real values get set up
// by GEDCOM_base_taglist() at run time.

GEDCOM_tag* DATE_tag = NULL;
GEDCOM_tag* NAME_tag = NULL;
GEDCOM_tag* INDI_tag = NULL;
GEDCOM_tag* SEX_tag = NULL;
GEDCOM_tag* CONT_tag = NULL;
GEDCOM_tag* FAMC_tag = NULL;
GEDCOM_tag* CHIL_tag = NULL;
GEDCOM_tag* FAMS_tag = NULL;
GEDCOM_tag* BIRT_tag = NULL;
GEDCOM_tag* DEAT_tag = NULL;
GEDCOM_tag* NOTE_tag = NULL;
GEDCOM_tag* FAM_tag = NULL;
GEDCOM_tag* HUSB_tag = NULL;
GEDCOM_tag* WIFE_tag = NULL;
GEDCOM_tag* PLAC_tag = NULL;
GEDCOM_tag* SOUR_tag = NULL;
GEDCOM_tag* MARR_tag = NULL;
GEDCOM_tag* LVG_tag = NULL;
GEDCOM_tag* BURI_tag = NULL;
GEDCOM_tag* SITE_tag = NULL;
GEDCOM_tag* CEME_tag = NULL;
GEDCOM_tag* TITL_tag = NULL;
GEDCOM_tag* TIME_tag = NULL;
GEDCOM_tag* BAPM_tag = NULL;
GEDCOM_tag* CHR_tag = NULL;
GEDCOM_tag* CREM_tag = NULL;
GEDCOM_tag* CAUS_tag = NULL;
GEDCOM_tag* PLOT_tag = NULL;
GEDCOM_tag* AGE_tag = NULL;
GEDCOM_tag* FIDE_tag = NULL;
GEDCOM_tag* QUAY_tag = NULL;
GEDCOM_tag* ORIG_tag = NULL;
GEDCOM_tag* CLAS_tag = NULL;
GEDCOM_tag* OCCU_tag = NULL;
GEDCOM_tag* EVEN_tag = NULL;
GEDCOM_tag* PUBL_tag = NULL;
GEDCOM_tag* TEXT_tag = NULL;
GEDCOM_tag* PHOTO_tag = NULL;
GEDCOM_tag* TYPE_tag = NULL;
GEDCOM_tag* PUBR_tag = NULL;
GEDCOM_tag* CONC_tag = NULL;
GEDCOM_tag* PAGE_tag = NULL;
GEDCOM_tag* REPO_tag = NULL;
GEDCOM_tag* CNTC_tag = NULL;
GEDCOM_tag* ADDR_tag = NULL;
GEDCOM_tag* ENGA_tag = NULL;
GEDCOM_tag* DIV_tag = NULL;
GEDCOM_tag* INFT_tag = NULL;
GEDCOM_tag* MARL_tag = NULL;
GEDCOM_tag* VERS_tag = NULL;
GEDCOM_tag* GEDC_tag = NULL;
GEDCOM_tag* HEAD_tag = NULL;
GEDCOM_tag* TRLR_tag = NULL;
GEDCOM_tag* root_tag = NULL;
GEDCOM_tag* heads_tag = NULL;
GEDCOM_tag* indis_tag = NULL;
GEDCOM_tag* fams_tag = NULL;
GEDCOM_tag* sours_tag = NULL;
GEDCOM_tag* repos_tag = NULL;
GEDCOM_tag* trlrs_tag = NULL;

GEDCOM_tag::GEDCOM_tag( char *newtag ) {

// default constructor is for adding tags we know we haven't seen before.
// this should only be used by initialisation code using a fixed list of
// tags as this avoids any risk of duplicates. For adding tags which might
// already be in the list, don't use new GEDCOM_tag directly, use
// GEDCOM_tagfromname( "WIBBLE" ) which will call new GEDCOM_tag if needed

  strcpy_s( tag, newtag );

  if (first_tag == NULL)
    first_tag = this;
  else
    last_tag->next = this;
  last_tag = this;
  next = NULL;
}

// we will also need a "namefromtag" to be able to write out GEDCOM files
// from our internal structure

char* GEDCOM_tag::GEDCOM_namefromtag() const {
  return (char *) tag; // discard its const-ness to stop ANSI C++ whinge
}

GEDCOM_tag* GEDCOM_tag::nexttag() const {
  return next;
}

//char* GEDCOM_tag::tagname() const {
//  return tag;
//}

GEDCOM_tag* GEDCOM_tagfromname( char *tagname ) {

// return pointer to tag in list - if its a tag we haven't seen before,
// add it to the list first. We do need to ensure that the tag is not
// going to cause a buffer overflow (we've had broken GEDCOM with random
// text stuck on the end of a tag)
int overflow;

  overflow = strlen(tagname);
  if (overflow>MAX_TAG-1) {
    printf("Found tag longer than maximum permitted: '%s'\n",tagname);
    return NULL;
  } 
  if (first_tag == NULL) return new GEDCOM_tag( tagname );
  GEDCOM_tag *link = first_tag;
  while ( link!=last_tag ) {
    if ( strcmp(link->GEDCOM_namefromtag(),tagname) == 0) return link;
    link = link->nexttag();
  }
  // now at last tag, but still might be a match:

  if ( strcmp(link->GEDCOM_namefromtag(),tagname) == 0) return link;
  return new GEDCOM_tag( tagname );
}

void GEDCOM_base_taglist() {

// since we add new tags at the end of the list, an optimisation would be to
// declare the least frequently used tags later on, as they are searched from
// the head when matching. Some tags (like DATE) will be looked up far more
// often than others (like ones we come across in files, not on our
// "well-known" list). Its really only important to get those tags that occur
// hundreds of times near the start. The order of ones which only appear a few
// times is far less critical. This "well-known" list should match the list of
// extern tag names in the header, so is now generated from a master tag
// list (ordered fairly optimally) by a noddy build tool and just #included here:
    char a[] = "DATE"; DATE_tag = new GEDCOM_tag(a);
    char b[] = "NAME"; NAME_tag = new GEDCOM_tag(b);
    char c[] = "INDI"; INDI_tag = new GEDCOM_tag(c);
    char d[] = "SEX"; SEX_tag = new GEDCOM_tag(d);
    char e[] = "CONT"; CONT_tag = new GEDCOM_tag(e);
    char f[] = "FAMC"; FAMC_tag = new GEDCOM_tag(f);
    char g[] = "CHIL"; CHIL_tag = new GEDCOM_tag(g);
    char h[] = "FAMS"; FAMS_tag = new GEDCOM_tag(h);
    char i[] = "BIRT"; BIRT_tag = new GEDCOM_tag(i);
    char j[] = "NOTE"; NOTE_tag = new GEDCOM_tag(j);
    char k[] = "FAM"; FAM_tag = new GEDCOM_tag(k);
    char l[] = "HUSB"; HUSB_tag = new GEDCOM_tag(l);
    char m[] = "WIFE"; WIFE_tag = new GEDCOM_tag(m);
    char n[] = "PLAC"; PLAC_tag = new GEDCOM_tag(n);
    char o[] = "SOUR"; SOUR_tag = new GEDCOM_tag(o);
    char p[] = "MARR"; MARR_tag = new GEDCOM_tag(p);
    char q[] = "BURI"; BURI_tag = new GEDCOM_tag(q);
    char r[] = "SITE"; SITE_tag = new GEDCOM_tag(r);
    char s[] = "CEME"; CEME_tag = new GEDCOM_tag(s);
    char t[] = "TITL"; TITL_tag = new GEDCOM_tag(t);
    char u[] = "TIME"; TIME_tag = new GEDCOM_tag(u);
    char v[] = "BAPM"; BAPM_tag = new GEDCOM_tag(v);
    char w[] = "CHR"; CHR_tag = new GEDCOM_tag(w);
    char x[] = "CREM"; CREM_tag = new GEDCOM_tag(x);
    char y[] = "CAUS"; CAUS_tag = new GEDCOM_tag(y);
    char z[] = "PLOT"; PLOT_tag = new GEDCOM_tag(z);
    char A[] = "AGE"; AGE_tag = new GEDCOM_tag(A);
    char B[] = "FIDE"; FIDE_tag = new GEDCOM_tag(B);
    char C[] = "QUAY"; QUAY_tag = new GEDCOM_tag(C);
    char D[] = "ORIG"; ORIG_tag = new GEDCOM_tag(D);
    char E[] = "CLAS"; CLAS_tag = new GEDCOM_tag(E);
    char F[] = "OCCU"; OCCU_tag = new GEDCOM_tag(F);
    char G[] = "EVEN"; EVEN_tag = new GEDCOM_tag(G);
    char H[] = "PUBL"; PUBL_tag = new GEDCOM_tag(H);
    char I[] = "TEXT"; TEXT_tag = new GEDCOM_tag(I);
    char J[] = "PHOTO"; PHOTO_tag = new GEDCOM_tag(J);
    char K[] = "TYPE"; TYPE_tag = new GEDCOM_tag(K);
    char L[] = "PUBR"; PUBR_tag = new GEDCOM_tag(L);
    char M[] = "CONC"; CONC_tag = new GEDCOM_tag(M);
    char N[] = "PAGE"; PAGE_tag = new GEDCOM_tag(N);
    char O[] = "REPO"; REPO_tag = new GEDCOM_tag(O);
    char P[] = "CNTC"; CNTC_tag = new GEDCOM_tag(P);
    char Q[] = "ADDR"; ADDR_tag = new GEDCOM_tag(Q);
    char R[] = "ENGA"; ENGA_tag = new GEDCOM_tag(R);
    char S[] = "DIV"; DIV_tag = new GEDCOM_tag(S);
    char T[] = "INFT"; INFT_tag = new GEDCOM_tag(T);
    char U[] = "MARL"; MARL_tag = new GEDCOM_tag(U);
    char V[] = "VERS"; VERS_tag = new GEDCOM_tag(V);
    char W[] = "GEDC"; GEDC_tag = new GEDCOM_tag(W);
    char X[] = "HEAD"; HEAD_tag = new GEDCOM_tag(X);
    char Y[] = "TRLR"; TRLR_tag = new GEDCOM_tag(Y);
    char aa[] = "root"; root_tag = new GEDCOM_tag(aa);
    char ab[] = "heads"; heads_tag = new GEDCOM_tag(ab);
    char ac[] = "indis"; indis_tag = new GEDCOM_tag(ac);
    char ad[] = "fams"; fams_tag = new GEDCOM_tag(ad);
    char ae[] = "sours"; sours_tag = new GEDCOM_tag(ae);
    char af[] = "repos"; repos_tag = new GEDCOM_tag(af);
    char ag[] = "trlrs"; trlrs_tag = new GEDCOM_tag(ag);

// the last tag type "root" is for our internal purposes only (same as !Family
// we might want to handle this a bit differently here eventually)
// The remaining tags here are ones we might need to create in the program
// although I've included all the ones I've used in GEDCOM files except user-
// defined type, which is a bigger list than the ones !Family actually deals with.

}

/////////////////////////////////////////////////////////////////////////////
// class GEDCOM_id: ID strings are held in a linked list much akin to the
//                  tag list, but with the addition of a pointer to the
// object to which the ID applies. We should also have one instance of the
// list for each GEDCOM file we have loaded (as you will soon fall over if
// you look up a reference from one GEDCOM file in the list for another...)

// this probably means that we have to have a class to represent the list,
// with a non-static pointer to the first ID, rather than having the
// first_id pointer as a static member of the GEDCOM_id class, as at present

GEDCOM_id::GEDCOM_id( char *newid ) {

// should only ever need to be called from tree's add_id method.

  id = new GEDCOM_string( newid );
  object = NULL;
  next = NULL;
}

GEDCOM_id::GEDCOM_id( char *prefix, int suffix ) {
  id = new GEDCOM_string( prefix, suffix );
  object = NULL;
  next = NULL;
}

GEDCOM_id::~GEDCOM_id() {

// should only ever be called from tree::drop_id, which should already
// have removed us from its linked list and have in hand the destruction
// of the object to which we point.
#ifdef destructorlogs
  printf("~GEDCOM_id %ld, object %ld, next %ld (ought to be NULL already)\n",(long)this,(long)object,(long)next);
#endif

  delete id;
}

char *GEDCOM_id::GEDCOM_idname() const {
  return (char *) id->string();
}

GEDCOM_id *GEDCOM_id::getnext() const {
  return next;
}

void GEDCOM_id::setnext( GEDCOM_id* thisid ) {
  next = thisid;
}

GEDCOM_object *GEDCOM_id::GEDCOM_objectfromid() const {
  return object;
}

int GEDCOM_id::GEDCOM_setobjectforid( GEDCOM_object* obj ) {
  if (object!=NULL) {
    if (object!=obj) return 1; }
  object = obj;
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// this class is for holding arbitrary length data on the heap - but as the
// class has to be a fixed size, all we do is hold a pointer to some malloc
// caller needs to check that the value of new GEDCOM_string != NULL

GEDCOM_string::GEDCOM_string( char* callers_string ) {
  int buflen = strlen(callers_string) + 1;
  thestring = (char*)malloc( buflen );
  if (thestring!=NULL) strcpy_s( thestring, buflen, callers_string );
}

// often we will generate an id by passing a prefix letter and a serial number
GEDCOM_string::GEDCOM_string( char* prefix, int suffix ) {
  int l;
  l = int(log10(suffix));
  int buflen = strlen(prefix) + l + 2;
  thestring = (char*)malloc( buflen );
  if (thestring!=NULL) {
    sprintf_s( thestring, buflen, "%s%i", prefix, suffix );
  }
}

GEDCOM_string::GEDCOM_string( char* prefix, char* suffix ) {
  int buflen = strlen(prefix) + strlen(suffix) + 1;
  thestring = (char*)malloc( buflen );
  if (thestring!=NULL) {
    strcpy_s( thestring, buflen, prefix );
    strcat_s( thestring, buflen, suffix );
  }
}

GEDCOM_string::GEDCOM_string( char* prefix, char* meat, char* suffix ) {
// sometimes we need to create a value with a reference, so prefix/suffix will typically be "@"
  int buflen = strlen(prefix) + strlen(meat) + strlen(suffix) + 1;
  thestring = (char*)malloc( buflen );
  if (thestring!=NULL) {
    strcpy_s( thestring, buflen, prefix );
    strcat_s( thestring, buflen, meat );
    strcat_s( thestring, buflen, suffix );
  }
}

GEDCOM_string::GEDCOM_string( size_t size ) {
  thestring = (char*)malloc( size );
}

GEDCOM_string::~GEDCOM_string() {
#ifdef destructorlogs
  printf("~GEDCOM_string %ld, string %ld\n",(long)this,(long)this->thestring);
#endif
  if (thestring!=NULL) free(thestring);
}

char* GEDCOM_string::string() const {
  return (char*)thestring;
}

// I'm pretty unconvinced this achieves what I hoped for - but fortunately, nothing ever calls it :-(
char* GEDCOM_string::conventionalsurname() const {
  char *p;
  p = strtok(thestring,"/");
  if (p!=NULL) {
    p = strtok(NULL,"/");
  }
  return p;
}


/////////////////////////////////////////////////////////////////////////////
// class GEDCOM_object: pretty much all the other data we use is held in
//                      these structures, or as simple strings on the heap

// we need several constructors, for example
// 0 HEAD          needs new GEDCOM_object( HEAD_tag );
// 1 SOUR !Family  needs new GEDCOM_object( SOUR_tag, "!Family" );
// 0 @I3@ INDI     needs new GEDCOM_object( GEDCOM_id::GEDCOM_idfromname("I3"), INDI_tag);
// 1 FAMS @F2@     needs new GEDCOM_object( FAMS_tag, GEDCOM_id::GEDCOM_objectfromname("F2"));
// note that the order of the GEDCOM_id* and GEDCOM_tag* args is all that distinguish the
// signatures of the latter two constructors: the GEDCOM_id is headed for different members

GEDCOM_object::GEDCOM_object():
  val (NULL),
  id (NULL),
  ref (NULL),
  sub (NULL),
  next (NULL),
  myparent(NULL)
//
// never called directly. *Every* GEDCOM object needs a tag.
// Is this actually used ? Since it is private, I imagine not...
{ }

GEDCOM_object* GEDCOM_object::parent() const {
  // printf("About to return parent %d of %s object at %d\n",(int)myparent,this->tag->GEDCOM_namefromtag(),(int)this);
  return myparent;
}

void GEDCOM_object::setparent( GEDCOM_object* descendedfrom ) {
  printf("setparent is a one-liner\n");
  //myparent->print(0);
  //descendedfrom->print(1);
  myparent = descendedfrom;
}

GEDCOM_object::GEDCOM_object(treeinstance* whichtree):
  tag ( root_tag ),
  val (NULL),
  id (NULL),
  ref (NULL),
  sub (NULL),
  next (NULL),
  myparent ((GEDCOM_object*)whichtree) // something of a kludge.
  // but is that kludge needed ? myparent((GEDCOM_object*)NULL) better ?
// called for the root object in any treeinstance
{ }

GEDCOM_object::GEDCOM_object(GEDCOM_tag *newtag ):
  tag (newtag),
  val (NULL),
  id (NULL),
  ref (NULL),
  sub (NULL),
  next (NULL),
  myparent(NULL) // gets set by add_subobject
//
// called for tags with no value like       0 HEAD
{ }

GEDCOM_object::GEDCOM_object(GEDCOM_tag *newtag, char *value ):
  tag (newtag),
  id (NULL),
  ref (NULL),
  sub (NULL),
  next (NULL),
  myparent(NULL) {
//
// called for tags with a string value like         1 NAME Fred
// note that it is also used for things like TEXT, NOTE, TITL which in some
// contexts are quite likely to have strings longer than we want to put on one line.
// This is why the CONC tag exists, and we need to split such lines using it.
  /* val  = (*value == '\0') ? NULL : new GEDCOM_string( value ); */
  if (*value=='\0') { val=NULL;return;}
  char shortval[MAX_GEDCOM_text+1];
  char* valptr=value;
  if (strlen(value)>MAX_GEDCOM_text) {
    strncpy_s( shortval, (const char*)valptr, MAX_GEDCOM_text );
    shortval[MAX_GEDCOM_text] = '\0';
    val = new GEDCOM_string( shortval );
    valptr += MAX_GEDCOM_text;
    while (strlen(valptr)>MAX_GEDCOM_text) {
      strncpy_s( shortval, (const char*)valptr, MAX_GEDCOM_text );
      shortval[MAX_GEDCOM_text] = '\0';
      this->add_subobject( new GEDCOM_object( CONC_tag, shortval ));
      valptr += MAX_GEDCOM_text;
    }
    this->add_subobject( new GEDCOM_object( CONC_tag, valptr ));
  } else val = new GEDCOM_string( value);
}

GEDCOM_object::GEDCOM_object(GEDCOM_tag *newtag, GEDCOM_string* value ):
  tag (newtag),
  val (value),
  id (NULL),
  ref (NULL),
  sub (NULL),
  next (NULL),
  myparent(NULL) {
}


GEDCOM_object::GEDCOM_object(GEDCOM_tag *newtag, GEDCOM_id *objref ):
  tag (newtag),
  val (NULL),
  id (NULL),
  ref (objref),
  sub (NULL),
  next (NULL),
  myparent(NULL)
//
// called for tags with a cross reference like            1 CHIL @xref@
{
  printf("Constructor for %s, @%s@\n",tag->GEDCOM_namefromtag(),ref->GEDCOM_idname());
}

GEDCOM_object::GEDCOM_object(GEDCOM_id *objid, GEDCOM_tag *newtag ):
  tag (newtag),
  val (NULL),
  id (objid),
  ref (NULL),
  sub (NULL),
  next (NULL),
  myparent(NULL)
//
// called for tags with an identifier like                0 @id@ INDI
{ 
  printf("Constructor for %s, @%s@\n",id->GEDCOM_idname(),tag->GEDCOM_namefromtag());
}

void GEDCOM_object::add_subobject( GEDCOM_object *newobj ) {

// adds newobj as the last subobject of this

  GEDCOM_object *subchain=sub;
#ifdef debugging
  printf("add_subobject ");
  //newobj->print(1);
  printf("pointer %ld of object %s",(long)sub,tag->GEDCOM_namefromtag());
  if (subchain!=NULL) {
    printf(" whose first subobject %s",sub->tag->GEDCOM_namefromtag());
  }
  printf("\n");
#endif
  if (subchain!=NULL) {
    while ((subchain->next)!=NULL) {
      //subchain->print(1);
      subchain = subchain->next;
      printf("Next in subobject chain %ld\n",(long)subchain);
    }
    subchain->next = newobj;
  } else {
    sub = newobj;
    printf("What we are adding is first subobject\n");
  }
  printf("add_subobject setting parent of newobj to\n");
  //this->print(1);
  newobj->setparent(this);
}

bool GEDCOM_object::swap_subobject( GEDCOM_object *first, GEDCOM_object *second ) {
// given two subobjects of this, swap their positions in the chain
// how much should we assume ? is it safe to assume that we are given
// the objects in the right order ? We are expecting this to be used
// on subobjects of the same type (CHILs in a FAM or FAMS in an INDI),
// should we enforce that ?
// OK, we return false if first doesn't precede second or if either isn't a
// subobject of this. but we are being wholly general and they could be
// any type of subobject.

  GEDCOM_object *a1; // will be the object whose next->first
  GEDCOM_object *a2; // will be first->next
  GEDCOM_object *a3; // will be object whose next->second (may be same as a2)
  GEDCOM_object *a4; // will be second->next (may be NULL)

  a1 = NULL; // we only ever use a1 under the same conditions as we set it up, but the compiler
  // thinks it might be undefined, so giving it a nominal value avoids that whinge
  if (first != this->sub ) {
    a1 = this->sub;
    while (first != a1->next) {
      a1 = a1->next;
      if (a1 == NULL) return false;
    }
    // a1 is the object whose next is first
#ifdef debugging
  printf("a1 is the object whose next is first:\n");
#endif
  //a1->print(1);
  }
  a2 = first->next;
  if (a2==NULL) return false;
#ifdef debugging
  printf("a2 is first->next\n");
#endif
  //a2->print(1);
  if (a2==second) {
    a3=first;
#ifdef debugging
    printf("a3 is first\n");
#endif
  } else {
    a3 = a2;
#ifdef debugging
    printf("Looking for a3\n");
#endif
    //a3->print(1);
#ifdef debugging
    printf("testing equal to second\n");
#endif
    second->print(1);
    while (second!=a3->next) {
      a3 = a3->next;
#ifdef debugging
      printf("Looking for a3\n");
#endif
      if (a3==NULL) return false;
      //a3->print(1);
    }
#ifdef debugging
    printf("a3 is object whose next->second\n");
#endif
    //a3->print(1);
  }
  a4 = second->next; // this one's allowed to be NULL
#ifdef debugging
  printf("a4 is second->next %ld\n",(long)a4);
#endif
  //if (a4!=NULL) a4->print(1);

  if (first == this->sub ) {
    this->sub = second;
  } else {
    a1->next = second;
  }
  // need to dry run this for cases when first->next is and is not second
  // so if first->next is second. Yep, think that's OK... no, it wasn't
  // we think it's OK now, but leave debug code in 
  if (a2==second) {
    second->next = first;
  } else {
    second->next = a2;
    a3 -> next = first;
  }
  first->next = a4;
  return true;
}

bool GEDCOM_object::delete_subobject( GEDCOM_object *oldobj ) {
// find the object and remove it from the chain of subobjects of this. Then
// destroy it utterly, including all subobjects

// a complication here is that certain types of objects have a user interface
// associated with them, in particular INDI, FAM and NOTE. INDI and FAM are
// never deeper subobjects that we will recurse to, so we are safe in assuming
// that our caller is already on top of removing indiUI and famUIs, but NOTEs
// can theoretically attach to anything, at any level, so we need to check if
// any object we are called on has a NOTE tag, and then check if there is a
// notesUI open on that NOTE. If so, we need to close that before deleting the
// NOTE.

// FIXME what happens if we are currently traversing the chain ? - it's OK as
// long as we don't retain a pointer to the item we're deleting. If we have a
// pointer to it's preceding object, the next pointer will be updated to point
// after the deleted object, so that's OK; if we have a pointer to the following
// object, nothing that changes will affect us.

// clarification: oldobj can never be the subobject of more than one object, so it
// is always safe to destroy a subobject once it has been removed from its parent
// (the fact that other objects may still point to it by reference is not our concern
// here - we assume that the caller is aware of this and has already flagged those
// other objects to be deleted, too - that *must* occur at a higher level because of
// the potential ramifications...)

// sometimes we want to reorder things, so we'll want to remove a subobject,
// but not destroy it as we wish to reinsert it in the chain. That suggests
// we need a remove_subobject method, and if we have that, we should probably
// implement delete_subobject to use it

  if (oldobj==NULL) {
    fl_alert("Bad call to delete_subobject - NULL passed!\n");
    return false;
  }
  printf("delete subobject called for\n");
  oldobj->outline( stdout, 1);
  GEDCOM_object *subchain=sub;
  GEDCOM_object *sublast=NULL;
  GEDCOM_object *cull;
  // current thought is that there is no occasion when we remove a subobject without wanting
  // to remove all *its* subobjects, too, so call recursively - ah but this then leaves them
  // inaccessible to deletion, so we should rename this code
  while ((cull=(oldobj->subobject()))!=NULL) {
    printf("Subobject to cull - delete_subobject should tell you what\n");
    oldobj->delete_subobject(cull);
    // no, no, you daft bugger, don't "delete cull;" 'cos the delete_subobject call does "delete oldobj;" already
  }
  if (oldobj->subobject()!=NULL) fl_alert("Removing an object (tag %s) which still has subobjects (first with tag %s), even though we have just supposedly removed them\n",oldobj->tagname(),oldobj->subobject()->tagname());
  printf("Trying to remove a subobject of %ld\n", (long)this);
  // industrial strength code could check that (oldobj->parent() == this, and probably
  // ought to set oldobj's parent to NULL)
  if ((oldobj->myparent)!=this) {
    fl_alert("Bad remove %ld (tag %s) parent not this %ld (tag %s)\n",(long)oldobj,oldobj->tagname(),(long)this, this->tagname());
    printf("Bad remove %ld (tag %s) parent not this %ld (tag %s)\n",(long)oldobj,oldobj->tagname(),(long)this, this->tagname());
  }
  if (subchain==NULL) {
    fl_alert("Bad remove: this %ld (tag %s) has no subobjects\n",(long)this, this->tagname());
    printf("Bad remove: this %ld (tag %s) has no subobjects\n",(long)this, this->tagname());
    return false; // no subobjects to remove !! which is a bug
  }
  while (subchain!=NULL) {
    if (subchain==oldobj) {
      // sublast will be null if this is the first object in the list:
      printf("Apparently valid remove subobject %ld of %ld\n",(long)oldobj,(long)this);
      if (sublast==NULL) {
        // this implies we haven't set sublast, so oldobj is first in the chain
        sub = oldobj->next;
      } else {
        sublast->next = oldobj->next;
      }
      oldobj->next = NULL;
      oldobj->myparent =  NULL; // these stop the destructor from complaining
      printf("Believe we removed oldobj %ld from chain, now deleting\n",(long)oldobj);
      delete oldobj;
      return true;
    }
    sublast = subchain;
    subchain = subchain->next;
  }
  // if subchain ever becomes NULL and the loop ends, this means we reached the end of
  // the linked list without having found oldobj
  fl_alert("Bad remove: this %ld (tag %s) has no subobjects which match %ld (tag %s)\n",(long)this,this->tagname(),(long)oldobj, oldobj->tagname());
  printf("Bad remove: this %ld (tag %s) has no subobjects which match %ld (tag %s)\n",(long)this,this->tagname(),(long)oldobj, oldobj->tagname());
  return false; // also means we were called in error
}

// implement insert_before() and insert_after() as being a little more
// human-readable, and should not actually be more work ?
// In these routines 'this' is the object to insert and oldobj is an existing
// subobject which we want to follow or precede in the list.

// FIXME, we are using an illogical mix of methods and direct access to variables

void GEDCOM_object::insert_after( GEDCOM_object *oldobj ) {

  this->next = oldobj->next;
  oldobj->next = this;
  this->setparent( oldobj->parent() );
}

void GEDCOM_object::insert_before( GEDCOM_object *oldobj ) {
GEDCOM_object *preobj;
  preobj = oldobj->parent()->subobject();
  while (preobj!=oldobj) {
    if ((preobj->next)==oldobj) {
      preobj->next = this;
      this->next = oldobj;
      this->setparent( oldobj->parent() );
      return;
    } else
      preobj = preobj->next;
  }
  // only get here if oldobj was the first subobject
  oldobj->parent()->sub = this;
  this->next = oldobj;
  this->setparent( oldobj->parent() );
}

void GEDCOM_object::insert_first( GEDCOM_object *newobj ) {

// adds newobj as the first subobject of this
  newobj->setparent(this);
  newobj->next=sub;
  sub = newobj;
}


GEDCOM_object::~GEDCOM_object() {
#ifdef destructorlogs
  printf("~GEDCOM_object %ld, val %ld, next %ld, sub %ld (last two ought to be NULL already)\n",(long)this,(long)val,(long)next,(long)sub);
#endif
  if (val!=NULL) {
   delete val;
  }
  // AERW 2003-12-24 what about subobjects ? if we evaporate whilst subobjects
  // exist, we have a memory leak. We shouldn't rely on the caller always to
  // have checked before destroying us - at least send a message to stderror so
  // that the developer spots trouble ahead !!
  if (myparent!=NULL) printf("Drat! GEDCOM_object %ld (tag %s) being killed still has link to parent at %ld (tag %s)\n",(long)this,this->tagname(),(long)myparent,myparent->tagname());
  if (sub!=NULL) printf("Help, GEDCOM_object %ld (tag %s) being killed with extant subobjects!!\n",(long)this,this->tagname());
  // we should also have been removed from a chain, so we should have no next (but does remove subobject do that ?)
  if (next!=NULL) printf("GEDCOM_object %ld (tag %s) being killed with extant next pointer!!\n",(long)this,this->tagname());
}

// for changing objects: note that changing the id of an object is a *dangerous*
// thing. Are we sure there is no different GEDCOM_id pointing at this object ?
// The call will be needed when merging trees, as conflicting ids will need to be
// changed, but *extreme care* will be required :-)

void GEDCOM_object::setid( GEDCOM_id *newid ) {
  // once an object (which must be an INDI, FAM, SOR or REPO) has an ID, it should never change
  // So should we catch the error that this->id!=NULL ?
  id = newid;
}

void GEDCOM_object::setvalue( GEDCOM_string *newvalue ) {
  // only here can we see if the existing value was non-null, so it must
  // be our job to free that
  if (val!=NULL) {
    delete val;
  }
  val = newvalue;
}

void GEDCOM_object::update( GEDCOM_tag* tag, char* value ) {

  printf("update called on %ld for tag %s value %s\n", (long)this, tag->GEDCOM_namefromtag(), value);
  GEDCOM_object *thing;
  if ((thing=(this->subobject(tag)))!=NULL) {
#ifdef debugging
    printf("Update thinks thing is non-null %ld\n",(long)thing);
#endif
    if (strlen(value)>0) {
      if ((thing->value())!=NULL) {
#ifdef debugging
        printf("So we're comparing %s with %s\n",thing->value(),value);
#endif
        if (strcmp(thing->value(),value)) {
          thing->setvalue(new GEDCOM_string(value));
        } // else nothing to do, object is unchanged
      } else {
        thing->setvalue(new GEDCOM_string(value));
      }
    } else {
      // remove the subobject (or remove its value if it has subobjects)
      if (thing->subobject()!=NULL) {
#ifdef debugging
        printf("update thinks thing %ld has at least one subobject %ld\n",(long)thing,(long)thing->subobject());
#endif
        thing->setvalue(NULL);
      } else {
#ifdef debugging
        printf("update thinks thing %ld can be deleted\n",(long)thing);
#endif
        this->delete_subobject(thing);
      }
    }
  } else {
    if (strlen(value)>0) {
      thing = new GEDCOM_object( tag, value );
      this->add_subobject(thing);
    }
  }
}

void GEDCOM_object::update_event( GEDCOM_tag* tag, char* date, char* time, char* place, char* site, char* plot, char* cause, char* age ) {
/* 'this' is the indi or fam object for which the event is defined
   'tag' is the tag for the event we are updating, typically BIRT, CHR, BAPM, DEAT, CREM, BURI, ENGA, MARR, DIV (but would work
    just as well for any event with a DATE/TIME; PLACE/SITE|CEME[/PLOT] structure, and we already deal wth special cases like an
    AGE tag for DEAT).
   'object[4]' is an array of subtags for this event, starting at object[0] which is the event tag, object[1] is the PLAC tag,
    object[2] is the SITE or CEME tag, and (for BURI only), object[3] is the PLOT tag.
    When we udate an event, we *always* ensure that the full set of subobjects exist, even if they are blank. Empty structures
     are only culled when the GEDCOM is saved.  */

  GEDCOM_object* object[4];
  char emptystring[] = { 0 };
  if ((object[0]=(this->subobject(tag)))==NULL) {
    object[0] = new GEDCOM_object(tag);
    this->add_subobject( object[0] );
  }

  // DATE - TIME structure
  if ((object[1]=(object[0]->subobject(DATE_tag)))==NULL) {
    object[1] = new GEDCOM_object(DATE_tag, emptystring);
    object[0]->add_subobject( object[1] );
  }
  if ((object[2]=(object[1]->subobject(TIME_tag)))==NULL) {
    object[2] = new GEDCOM_object(TIME_tag, emptystring);
    object[1]->add_subobject( object[2] );
  }
  object[1]->update(TIME_tag, time );
  object[0]->update(DATE_tag, date );
  if ((object[1]=(object[0]->subobject(PLAC_tag)))==NULL) {
    object[1] = new GEDCOM_object(PLAC_tag, emptystring);
    object[0]->add_subobject( object[1] );
  }

  // PLAC - SITE/CEME - PLOT structure
  GEDCOM_tag* site_tag=SITE_tag;
  if (tag==BURI_tag) site_tag = CEME_tag;
  if ((object[1]=(object[0]->subobject(PLAC_tag)))==NULL) {
    object[1] = new GEDCOM_object(PLAC_tag,emptystring);
    object[0]->add_subobject( object[1] );
  }
  if ((object[2]=(object[1]->subobject(site_tag)))==NULL) {
    object[2] = new GEDCOM_object(site_tag, emptystring);
    object[1]->add_subobject( object[2] );
    if (tag==BURI_tag) {
#ifdef debugging
      printf("Dealing with extra tag for burial event PLOT %s\n", plot );
#endif
      if ((object[3]=(object[2]->subobject(PLOT_tag)))==NULL) {
        object[3] = new GEDCOM_object(PLOT_tag, emptystring);
        object[2]->add_subobject( object[3] );
      }
    //2023-03-24, shouldn't this be [3] not [2] ? apparently not. I'm struggling a bit here...
    object[2]->update(PLOT_tag,plot);
    }
  }
  object[1]->update(site_tag, site );
  object[0]->update(PLAC_tag,place );

  if (tag==DEAT_tag) {
#ifdef debugging
    printf("Dealing with extra tags for death event AGE %s CAUS %s\n", age, cause );
#endif
    if ((object[1]=(object[0]->subobject(AGE_tag)))==NULL) {
      object[1] = new GEDCOM_object(AGE_tag, emptystring);
      object[0]->add_subobject( object[1] );
    }
    object[0]->update(AGE_tag, age );
    if ((object[1]=(object[0]->subobject(CAUS_tag)))==NULL) {
      object[1] = new GEDCOM_object(CAUS_tag, emptystring);
      object[0]->add_subobject( object[1] );
    }
    object[0]->update(CAUS_tag, cause );
  }    
  this->update(tag, emptystring);
}

////////////////// navigation primitives ///////////////////

GEDCOM_object* GEDCOM_object::next_object() const {
  return next;
}

GEDCOM_object* GEDCOM_object::next_object( GEDCOM_tag* scantag ) const {

GEDCOM_object* nextsub;

  if ((nextsub=next)==NULL) return NULL;
  while ((nextsub->tag)!=scantag) {
    nextsub = nextsub->next;
    if (nextsub==NULL) return NULL;
  }
  return nextsub;
}

GEDCOM_object* GEDCOM_object::subobject() const {
  return sub;
}

GEDCOM_object* GEDCOM_object::subobject( GEDCOM_tag* scantag ) const {

GEDCOM_object* nextsub;

#ifdef debugging
  printf("We're looking for a subobject %s\n",scantag->GEDCOM_namefromtag());
#endif
  if ((nextsub=sub)==NULL) {
#ifdef debugging
  printf("But this %ld has no subjects at all - returning NULL\n",(long)this);
#endif
    return NULL;
  }
#ifdef debugging
  printf("Yes, object %ld has at least one subobject %ld\n",(long)this,(long)nextsub);
#endif
  while ((nextsub->tag)!=scantag) {
#ifdef debugging
    printf("current tag is %s next would be %ld\n",nextsub->tag->GEDCOM_namefromtag(),(long)nextsub->next);
#endif
    nextsub = nextsub->next;
    if (nextsub==NULL) return NULL;
  }
#ifdef debugging
  printf("Believe we matched the tag %s\n",nextsub->tag->GEDCOM_namefromtag());
#endif
  return nextsub;
}

treeinstance* GEDCOM_object::root() {
  GEDCOM_object* ancestor;
  GEDCOM_object* object;
  //if (this==NULL) {
  //  printf("About to return NULL from root() - expect to bomb shortly\n");
  //  return NULL; // which will ensure that object isn't used uninitialised
  //}
  // even if we have an extremely broken tree or bugs elsewhere in the code.
  ancestor = this;
  while (ancestor->objtype() != root_tag) {
    object = ancestor;
    ancestor = object->parent();
  }
#ifdef debugging
  printf("Ascended until ancestor is root, now about to cast object as treeinstance*\n");
#endif
  return (treeinstance*) ancestor->parent();
}

char* GEDCOM_object::value() const {
  if (val==NULL) return NULL;
  return val->string();
}

GEDCOM_tag* GEDCOM_object::objtype() const {
  return tag;
}

char* GEDCOM_object::tagname() const {
  return tag->GEDCOM_namefromtag();
}

void GEDCOM_object::output( FILE* out, int level ) const {

GEDCOM_object *then;

// output every tag line at this level or lower. Typically called on the
// first subobject of an object, so you don't get the actual object line
// (and if you did, then you get the rest of the file...)

// if we have an object with no value, reference or subobjects, we should not
// output it at all - in all probability it is an ephmeral object which only
// exists whilst we have an indiUI or famUI open, or it is an error in the
// GEDCOM. This is certainly true for the events, should never happen for
// level 0 things except TRLR, of which we make a special case.
  if (level>=0) {
    if ((sub==NULL)&&(val==NULL)&&(id==NULL)&&(ref==NULL)) {
      if ( tag == TRLR_tag) this->outline( out, level );
	// there may be other tags which are allowed to be empty
        // otherwise do NOT output a line
    }
    else this->outline( out, level );
  }
  // OK, here we need an else and do some checking on which object we
  // are about to deal with. at level -2 it should be root, whilst
  // at level -1 it will be one of the main list objects. trlrlist
  // should contain a number of 0 NOTE objects, and these will reflect
  // the current person at the time the GEDCOM was loaded. These will
  // be out of date, and we will want to generate a new trlr_list before
  // dropping back to output the objects. However, we probably can't see
  // enough to identify trlr_list, which is a private variable of treeinstance

  // we are being called from treeinstance::save() as rootobject->output()
  // maybe we need to set level one higher and call output once each for
  // headlist, indilist, famslist,sourlist,repolist and trlrlst
  // both headlist and trlrlist will need to be modified - need to write
  // another devel-doc ...
  
  if ((then=sub)!=NULL) then->output( out, level+1 );
  if ((then=next)!=NULL) then->output( out, level );
}

void GEDCOM_object::outline( FILE* out, int level ) const {

    fprintf( out,"%d",level);
    if (id!=NULL) fprintf( out," @%s@",id->GEDCOM_idname());
    fprintf( out," %s",tag->GEDCOM_namefromtag());
    if (ref!=NULL) fprintf( out," @%s@",ref->GEDCOM_idname());
    if (val!=NULL) fprintf( out," %s",val->string());
    fprintf( out,"\n");

}

void GEDCOM_object::print( int level ) const {

// when debugging, we want to print everything, even if it has no value, reference or subobjects
// but we probably only want to print one object (and its subobjects). To do this, call with level
// 0.

GEDCOM_object *then;
  this->printline( level );
  if ((then=sub)!=NULL) then->print( level+1 );
  if (level>0) { if ((then=next)!=NULL) then->print( level ); }
}

void GEDCOM_object::printline( int level ) const {
  printf( "%d",level);
  if (id!=NULL) printf(" @%s@",id->GEDCOM_idname());
  printf(" %s",tag->GEDCOM_namefromtag());
  if (ref!=NULL) printf(" @%s@",ref->GEDCOM_idname());
  if (val!=NULL) printf(" %s",val->string());
  printf( "\n");
}

// methods appropriate for objects which have an @id@ such as INDI, SOUR, REPO, FAM

GEDCOM_id* GEDCOM_object::getid() const {
  return id;
}

char* GEDCOM_object::getidname() const {
  if (id!=NULL) return id->GEDCOM_idname();
  printf("About to return a NULL for idname in a GEDCOM Object type %s\n",tag->GEDCOM_namefromtag());
  return NULL;
}

// methods appropriate for objects which use a cross reference, such as WIFE, HUSB, CHIL

GEDCOM_id *GEDCOM_object::getxref() const {
  return ref;
}

GEDCOM_object *GEDCOM_object::followxref() const {
#ifdef debugging
  printf("followxref starting with ref %ld %s\n",(long)ref, ref->GEDCOM_idname());
#endif
  return ref->GEDCOM_objectfromid();
}

char *GEDCOM_object::getxrefname() const {
  if (ref!=NULL)
    return ref->GEDCOM_idname();
  return NULL;
}

// methods appropriate only for FAM objects:

int GEDCOM_object::garbage_fam(bool includechildren) {
// to be called to see if a FAM object is empty of people
 if (tag!=FAM_tag) { fl_alert("garbage_fam bad call\n"); return false; } //bad call !
  int i=0;
  if ((this->subobject( WIFE_tag ))!= NULL) i++;
  if ((this->subobject( HUSB_tag ))!= NULL) i++;
  if (includechildren) {
    if ((this->subobject( CHIL_tag ))!= NULL) i++;
  }
  return i;
}

// the way these functions work militates against same-sex marriages (as does
// GEDCOM itself, of course). In effect GEDCOM forces us to label one of the
// couple as the wrong-gendered role.

GEDCOM_object* GEDCOM_object::thewife() const {
// to be called to follow the WIFE pointer of a FAM
GEDCOM_object *wife;
 if (tag!=FAM_tag) { fl_alert("thewife bad call\n"); return NULL; } //bad call !
  wife = this->subobject( WIFE_tag );
  if (wife==NULL) return NULL;
  wife = wife->followxref();
  return wife; // again, a NULL here is a broken GEDCOM structure
}

GEDCOM_object* GEDCOM_object::thehusband() const {
// to be called to follow the HUSB pointer of a FAM
GEDCOM_object *husb;
 if (tag!=FAM_tag) { fl_alert("thehusband bad call\n"); return NULL; } //bad call !
  husb = this->subobject( HUSB_tag );
  if (husb==NULL) return NULL;
  husb = husb->followxref();
  return husb; // again, a NULL here is a broken GEDCOM structure
}

GEDCOM_object* GEDCOM_object::thespouseof( GEDCOM_object* person ) const {
// to be called to find the other spouse, given one of unspecified gender
// only returns a real object when both WIFE and HUSB are in this FAM.
// We could probably recode this to cope with same-sex MARRs and possibly
// make it easier to understand, too !

GEDCOM_object* spouse;
 if (tag!=FAM_tag) { fl_alert("thespouseof bad call\n"); return NULL; } //bad call !
  spouse = this->subobject( WIFE_tag );
  if (spouse==NULL) return NULL;
  if ((spouse->followxref())==person) spouse = this->subobject( HUSB_tag );
  if (spouse==NULL) return NULL;
  return spouse->followxref();
}

GEDCOM_object* GEDCOM_object::child( GEDCOM_object **older, bool follow ) const {
// to be called to follow the CHIL pointers of a FAM
// call initially with the argument set to NULL as in
  //  GEDCOM_object *chil = NULL; GEDCOM_object *fam;
  //  GEDCOM_object *child,*nextchild;
  //  child = fam->child( &chil );
  //  if (child!=NULL)
  //    nextchild = fam->child( &chil );

GEDCOM_object *chil;
  if (tag!=FAM_tag) return NULL;// bad call !
  if (*older==NULL) {
    printf("child looking for first CHIL subobject\n");
    chil = this->subobject( CHIL_tag );
  } else {
    printf("child looking for next CHIL subobject\n");
    chil = (*older)->next_object( CHIL_tag );
  }
  if (chil == NULL) return NULL;
#ifdef debugging
  printf("child found a CHIL object, saving state and returning %ld\n",(long)chil->followxref());
#endif
  *older = chil;
  if (follow) {
    return chil->followxref();
  } else {
    return chil;
  }
}

// methods appropriate only for INDI objects:

GEDCOM_object* GEDCOM_object::parental_family() const {
// to be called to follow the FAMC pointer of an INDI
   printf("seeking parental family of %ld\n",(long)this);
   if (this==NULL) return NULL;
GEDCOM_object* famc;
// if (tag!=INDI_tag) return NULL; // bad call test not needed in subclass
  famc = this->subobject( FAMC_tag );
#ifdef debugging
//  printf("parental_family FAMC found %ld @%s@\n",(long)famc,famc->getidname());
  printf("parental_family FAMC found %ld\n",(long)famc);
#endif
  if (famc==NULL) return NULL;
  famc = famc->followxref();
#ifdef debugging
  printf("parental_family found %ld\n",(long)famc );
#endif
  if (famc==NULL) return NULL; // this is broken GEDCOM - every @xref@ should
  //have a corresponding @id@ in the file
  return famc;
}

GEDCOM_object* GEDCOM_object::own_family( GEDCOM_object* fams ) const {
// to be called to find the FAMSs of an INDI
  if (fams == NULL) {
    fams = this->subobject( FAMS_tag );
    if (fams == NULL) return NULL;
    fams = fams->followxref();
    return fams; // which may also be NULL if GEDCOM is broken
  }
  // note that we don't actually use the this-> pointer
  // for industrial strength, we should perhaps check that fams is a subobject of this
  fams = fams->next_object( FAMS_tag );
  if (fams == NULL) return NULL;
  fams = fams->followxref();
  return fams; // which may also be NULL if GEDCOM is broken
}


