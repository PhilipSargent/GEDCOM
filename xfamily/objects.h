// objects.h

#ifndef GEDCOMobjects_h
#define GEDCOMobjects_h

#include <string.h>
#include <stdio.h>

#include "fixes.h"
#include "classes.h"
#include "family.h"

/* A GEDCOM_object represents a line of GEDCOM with components

   <level> [@<id>@] <tag> [@<xref>@] [value]

   where <level> defines the position in a hierarchy of objects. We don't store
                 this explicitly, since we represent the structure itself
         <id>    is a unique identifier for this line
         <tag>   is an identifier for the type of object
         <xref>  is a (textual) pointer to the <id> of another line of GEDCOM
         value   is an arbitrary string

   we have specialist classes for the <tag> and <id>/<xref> and for strings  */

////////////////////////////////////////////////////////////////////////////////

class GEDCOM_tag {
// we need to be able to refer to our "known" tags by name, eg. INDItag
// but also be able to handle GEDCOM objects with any tag which we may not
// have seen before. So we need to be able to set up a list using things
// like NAMEtag = new GEDCOM_tag("NAME"); (although in practice we would set
// these up statically) but also do things like
// GEDCOM_tag *tag=GEDCOM_tagfromname(string);   which involves a search

// set up a linked list of all tagnames we have seen. last_tag->next==NULL
  char tag[MAX_TAG];                 // this is an arbitrary limit, but probably
                                     // one we can reasonably well justify
  GEDCOM_tag *next;
public:
  GEDCOM_tag( char * );
  char* GEDCOM_namefromtag() const;
  GEDCOM_tag* nexttag() const;
};

GEDCOM_tag *GEDCOM_tagfromname( char * );
void        GEDCOM_base_taglist();

////////////////////////////////////////////////////////////////////////////////

class GEDCOM_id {
//  static GEDCOM_id *first_id;
#ifdef fix0023
  // fix0023 removes the arbitrary 16 char limit on ID strings. In reality, the
  // GEDCOM spec says there is a 20-char limit, so we could have adopted that,
  // but people can break the spec. and a buffer overrun is an obvious exploit,
  // so it is better if we are able to cope with any length we are fed. The ones
  // we generate ourselves will not get anywhere near 20 chars.
  GEDCOM_string *id;
#else
  char id[16];                        // this is an arbitrary limit which worries me
#endif
  GEDCOM_object *object;
  GEDCOM_id     *next;
  GEDCOM_id     *previous;
public:
  GEDCOM_id( char *);
#ifdef fix0023
  GEDCOM_id( char*, int );
#endif
  ~GEDCOM_id();

  char* GEDCOM_idname() const;
  GEDCOM_id* getnext() const; // changed the name of this from nextid() for consistency
  void setnext( GEDCOM_id* );
  GEDCOM_id* getprevious() const;
  void setprevious( GEDCOM_id* );
  GEDCOM_object *GEDCOM_objectfromid( ) const;
  int GEDCOM_setobjectforid( GEDCOM_object* );
};

////////////////////////////////////////////////////////////////////////////////

class GEDCOM_string {
  char *thestring;
public:
  GEDCOM_string( char* );
#ifdef fix0023
  // we used to create IDs by sprintf-ing a number into a fixed buffer, but for safety
  // on import we wanted that to be a string on the heap. That now means we need a method
  // to generate a heap string from a letter and a number
  GEDCOM_string( char*, int );
#endif
  GEDCOM_string( char*, char* );
  GEDCOM_string( char*, char*, char* );
  GEDCOM_string( size_t );
  ~GEDCOM_string();
  char* string() const;
  char* conventionalsurname() const;
};

////////////////////////////////////////////////////////////////////////////////

class GEDCOM_object {
// protected: now private again - we will not subclass

/* GEDCOM_object is completely general and is not subclassed into specific
   object types. Some methods only make sense for specific objects, but GEDCOM
   is sufficiently "well-structured" that we will always be confident that it
   is appropriate to call these methods. They may check that they are being
   passed an appropriate object, for the purpose of reporting broken GEDCOM,
   and a reality check should probably always be done on a newly loaded tree.
   The type of object can be checked by reference to its GEDCOM_tag */

  GEDCOM_tag *tag;      // tag for this object
  GEDCOM_string *val;   // -> string value for this object (same line as tag)
  GEDCOM_id *id;        // @identifier@ string
  GEDCOM_id *ref;       // @xref@ string
  GEDCOM_object *sub;   // -> object representing first subobject of this object
  GEDCOM_object *next;  // -> next subobject of our parent object
  GEDCOM_object *myparent;
  unsigned int flags;   // we (probably) need some flags to indicate things like
                        // this is an ephemeral object which we need to garbage collect
                        //   if not made permanent
                        // this is an object which is flagged for deletion and should be
                        //   freed as soon as all its subobjects have gone

// to make that clearer:
//   the @ref@ in         1 CHIL @I960@  refers to a 0 @I960@ INDI
// the CHIL object has .ref pointing to the "I960" identifier   <level> [@<id>@] <tag> [@<xref>@] [value]

// the INDI object has .id  pointing to the same "I960" identifier
// the identifier has a pointer back to the INDI object, and of course,
// the INDI object would (in this case) have a FAMC reference whose
// @ref@ would point to an identifier which would itself have a pointer back
// to the FAM object of which the CHIL object is a subobject.
// Confused ? You will be ....

  GEDCOM_object();
  // we need parent for internal maintenance, but prefer it
  void setparent(GEDCOM_object*);        // not to be available to the public
public:
  GEDCOM_object* parent() const;  // inevitably we end up needing to reveal it ...
  // for creating objects:
  GEDCOM_object(treeinstance*); // ONLY for creating the root object
  GEDCOM_object(GEDCOM_tag*);
  GEDCOM_object(GEDCOM_tag*, char* );
  GEDCOM_object(GEDCOM_tag*, GEDCOM_string* );
  GEDCOM_object(GEDCOM_id*, GEDCOM_tag* ); // for level @id@ tag
  GEDCOM_object(GEDCOM_tag*, GEDCOM_id* ); // for level tag @xref@
  // by analogy with the other routines, add_subobject ought to have been insert_last()
  void add_subobject( GEDCOM_object* );
  bool swap_subobject( GEDCOM_object*, GEDCOM_object* );
  bool delete_subobject( GEDCOM_object* );
  void insert_after( GEDCOM_object* );
  void insert_before( GEDCOM_object* );
  void insert_first( GEDCOM_object* );

// for destroying objects
  ~GEDCOM_object();


// for changing objects
  void setid( GEDCOM_id*);
  void setvalue( GEDCOM_string*);
  void update( GEDCOM_tag*, char* );
  void update_event( GEDCOM_tag*, char*, char*, char*, char*, char*, char* );

// for navigating - these will return NULL if no appropriate object
  GEDCOM_object* next_object() const;     // the next object at this level
  GEDCOM_object* next_object( GEDCOM_tag* ) const; // next with this tag
  GEDCOM_object* subobject() const;       // the first subobject
  GEDCOM_object* subobject( GEDCOM_tag* ) const; // first with given tag
  treeinstance* root();                   // treeinstance containing this object

  char* value() const;                    // the text value of this object
  GEDCOM_tag* objtype() const;            // the tag
  char* tagname() const;                  // the text string for the tag of this object
  void output( FILE*, int ) const;        // output a chain of objects (recursive)
  void outline( FILE*, int ) const;       // output a line of the object
  void print( int ) const;
  void printline( int ) const;

// methods appropriate for objects which have an @id@ such as INDI, SOUR, REPO, FAM
  GEDCOM_id* getid() const;               // pointer to @id@ for this object
  char* getidname() const;                // the string value of the id (without @..@)

// methods appropriate for objects which use a cross reference, such as WIFE, HUSB, CHIL
  GEDCOM_id *getxref() const;             // pointer to @xref@ for this object
  GEDCOM_object *followxref() const;      // pointer to object referenced by xref
  char *getxrefname() const;              // the string value of the xref (without @..@)

// methods appropriate only for FAM objects:
  bool garbage_fam();                     // returns true if FAM may be deleted as empty
  GEDCOM_object* thewife() const;         // the INDI pointed to by a WIFE subobject
  GEDCOM_object* thehusband() const;      // the INDI pointed to by a HUSB subobject
  GEDCOM_object* thespouseof( GEDCOM_object* ) const; //
  GEDCOM_object* child(GEDCOM_object**, bool) const;  // successive CHILs or the INDIs pointed to

// methods appropriate only for INDI objects:
  GEDCOM_object* parental_family() const; // the FAM pointed to by a FAMC subobject
  GEDCOM_object* own_family( GEDCOM_object* ) const; // next FAM implied by FAMS subobjects

// we will also need destructors - note that we cannot just destroy a subobject
// in the middle of a chain, since we have to find its predecessor in order to
// update its next pointer. You could get round this with a doubly linked list,
// at the expense of more memory. This would save a very large amount of
// processing if you do something like deleting a person in the middle of a
// very large database, where you might have to perambulate a very long next
// chain to get to the relevant subobject. This might also make insertion
// quicker - you will often be inserting a new person as the child of a
// person whose object you have to hand - with the same surname. So you
// could look at the father and then go either forwards or backwards along
// the list to find the alphabetically correct place to insert (all INDI
// objects are subobjects of the indi subobject of root in a very long list :-)
};
#endif
