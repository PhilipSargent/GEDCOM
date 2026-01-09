// objects.cxx

#include <string.h>
#include <stdio.h>

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

// Statically allocate space for pointers to tags. Real values get set up
// by GEDCOM_base_taglist() at run time.

#include "tags.cxx"

GEDCOM_tag::GEDCOM_tag( char *newtag ) {

// default constructor is for adding tags we know we haven't seen before.
// this should only be used by initialisation code using a fixed list of
// tags as this avoids any risk of duplicates. For adding tags which might
// already be in the list, don't use new GEDCOM_tag directly, use
// GEDCOM_tagfromname( "WIBBLE" ) which will call new GEDCOM_tag if needed

  strcpy( tag, newtag );

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
// add it to the list first

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

#include "taginc.cxx"

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

  strcpy( id, newid );
  object = NULL;
  next = NULL;
}

GEDCOM_id::~GEDCOM_id() {

// should only ever be called from tree::drop_id, which should already
// have removed us from its linked list and have in hand the destruction
// of the object to which we point.
}

char *GEDCOM_id::GEDCOM_idname() const {
  return (char *) id; // discard its const-ness to stop ANSI C++ whinge
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
  thestring = (char*)malloc( strlen(callers_string)+1);
  if (thestring!=NULL) strcpy( thestring, callers_string );
}

GEDCOM_string::GEDCOM_string( size_t size ) {
  thestring = (char*)malloc( size );
}

GEDCOM_string::~GEDCOM_string() {
  if (thestring!=NULL) free(thestring);
}

char* GEDCOM_string::string() const {
  return (char*)thestring;
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
  next (NULL)
//
// never called directly. *Every* GEDCOM object needs a tag.
// Is this actually used ?
{ }

GEDCOM_object::GEDCOM_object(GEDCOM_tag *newtag):
  tag (newtag),
  val (NULL),
  id (NULL),
  ref (NULL),
  sub (NULL),
  next (NULL)
//
// called for tags with no value like       0 HEAD
{ }

GEDCOM_object::GEDCOM_object(GEDCOM_tag *newtag, char *value):
  tag (newtag),
  id (NULL),
  ref (NULL),
  sub (NULL),
  next (NULL) {
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
    strncpy( shortval, (const char*)valptr, MAX_GEDCOM_text );
    shortval[MAX_GEDCOM_text] = '\0';
    val = new GEDCOM_string( shortval );
    valptr += MAX_GEDCOM_text;
    while (strlen(valptr)>MAX_GEDCOM_text) {
      strncpy( shortval, (const char*)valptr, MAX_GEDCOM_text );
      shortval[MAX_GEDCOM_text] = '\0';
      this->add_subobject( new GEDCOM_object( CONC_tag, shortval ));
      valptr += MAX_GEDCOM_text;
    }
    this->add_subobject( new GEDCOM_object( CONC_tag, valptr ));
  } else val = new GEDCOM_string( value);
}

GEDCOM_object::GEDCOM_object(GEDCOM_tag *newtag, GEDCOM_id *objref ):
  tag (newtag),
  val (NULL),
  id (NULL),
  ref (objref),
  sub (NULL),
  next (NULL)
//
// called for tags with a cross reference like            1 CHIL @xref@
{ }

GEDCOM_object::GEDCOM_object(GEDCOM_id *objid, GEDCOM_tag *newtag ):
  tag (newtag),
  val (NULL),
  id (objid),
  ref (NULL),
  sub (NULL),
  next (NULL)
//
// called for tags with an identifier like                0 @id@ INDI
{ }

void GEDCOM_object::add_subobject( GEDCOM_object *newobj ) {

// adds newobj as the last subobject of this

  GEDCOM_object *subchain=sub;
//  printf("pointer to subobject of object on which add_subobject operates is %d\n",(int)sub);
  if (subchain!=NULL) {
    while ((subchain->next)!=NULL) {
      subchain = subchain->next;
    }
    subchain->next = newobj;
  } else sub = newobj;
}

bool GEDCOM_object::remove_subobject( GEDCOM_object *oldobj ) {
// find the object and remove it from the chain of subobjects of this. That's
// *all* we do, just detach it. What to do with the object then is up to the caller.
// return true if we found and removed the subobject, false if we didn't

  GEDCOM_object *subchain=sub;
  GEDCOM_object *sublast=NULL;
  // printf("Trying to remove a subobject of %ld\n", (long)this);
  if (subchain==NULL) return false; // no subobjects to remove !!
  while (subchain!=NULL) {
    if (subchain==oldobj) {
      if (sublast==NULL) sub = oldobj->next; else sublast->next = oldobj->next;
      oldobj->next = NULL;
      return true;
    }
    sublast = subchain;
    subchain = subchain->next;
  }
  return false;
}
      

void GEDCOM_object::chain_object( GEDCOM_object *newobj ) {

// adds newobj as the next subobject of this's parent
// ie. inserts in chain after this (and before this->next if there is one)

  if (next!=NULL) {
    newobj->next = next;
  }
  next = newobj;
}

GEDCOM_object::~GEDCOM_object() {
  if (val!=NULL) delete val;
}

// for changing objects: note that changing the id of an object is a *dangerous*
// thing. Are we sure there is no different GEDCOM_id pointing at this object ?
// The call will be needed when merging trees, as conflicting ids will need to be
// changed, but *extreme care* will be required :-)

void GEDCOM_object::setid( GEDCOM_id *newid ) {
  // should we catch the error that object->id!=NULL ?
  id = newid;
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

  if ((nextsub=sub)==NULL) return NULL;
  while ((nextsub->tag)!=scantag) {
    nextsub = nextsub->next;
    if (nextsub==NULL) return NULL;
  }
  return nextsub;
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

  if (level>=0) this->outline( out, level );
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

// methods appropriate for objects which have an @id@ such as INDI, SOUR, REPO, FAM

GEDCOM_id* GEDCOM_object::getid() const {
  return id;
}

char* GEDCOM_object::getidname() const {
  if (id!=NULL) return id->GEDCOM_idname();
  return NULL;
}

// methods appropriate for objects which use a cross reference, such as WIFE, HUSB, CHIL

GEDCOM_id *GEDCOM_object::getxref() const {
  return ref;
}

GEDCOM_object *GEDCOM_object::followxref() const {
  return ref->GEDCOM_objectfromid();
}

char *GEDCOM_object::getxrefname() const {
  if (ref!=NULL)
    return ref->GEDCOM_idname();
  return NULL;
}

// methods appropriate only for FAM objects:

GEDCOM_object* GEDCOM_object::thewife() const {
// to be called to follow the WIFE pointer of a FAM
GEDCOM_object *wife;
 if (tag!=FAM_tag) return NULL; //bad call !
  wife = this->subobject( WIFE_tag );
  if (wife==NULL) return NULL;
  wife = wife->followxref();
  return wife; // again, a NULL here is a broken GEDCOM structure
}

GEDCOM_object* GEDCOM_object::thehusband() const {
// to be called to follow the HUSB pointer of a FAM
GEDCOM_object *husb;
 if (tag!=FAM_tag) return NULL; //bad call !
  husb = this->subobject( HUSB_tag );
  if (husb==NULL) return NULL;
  husb = husb->followxref();
  return husb; // again, a NULL here is a broken GEDCOM structure
}

GEDCOM_object* GEDCOM_object::thespouseof( GEDCOM_object* person ) const {
// to be called to find the other spouse, given one of unspecified gender
// only returns a real object when both WIFE and HUSB are in this FAM
GEDCOM_object* spouse;
 if (tag!=FAM_tag) { printf("thespouseof bad call\n"); return NULL; } //bad call !
  spouse = this->subobject( WIFE_tag );
  if (spouse==NULL) return NULL;
  if ((spouse->followxref())==person) spouse = this->subobject( HUSB_tag );
  if (spouse==NULL) return NULL;
  return spouse->followxref();
}

GEDCOM_object* GEDCOM_object::child( GEDCOM_object **older ) const {
// to be called to follow the CHIL pointers of a FAM
// call initially with the argument set to NULL as in
  //  GEDCOM_object *chil = NULL; GEDCOM_object *fam;
  //  GEDCOM_object *child,*nextchild;
  //  child = fam->child( &chil );
  //  if (child!=NULL)
  //    nextchild = fam->child( &chil );

GEDCOM_object *chil;
 if (tag!=FAM_tag) return NULL;// bad call !
  if (*older==NULL)
    chil = this->subobject( CHIL_tag );
  else
    chil = (*older)->next_object( CHIL_tag );
  if (chil == NULL) return NULL;
  *older = chil;
  return chil->followxref();
}

// methods appropriate only for INDI objects:

GEDCOM_object* GEDCOM_object::parental_family() const {
// to be called to follow the FAMC pointer of an INDI
GEDCOM_object* famc;
// if (tag!=INDI_tag) return NULL; // bad call test not needed in subclass
  famc = this->subobject( FAMC_tag );
  if (famc==NULL) return NULL;
  famc = famc->followxref();
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



