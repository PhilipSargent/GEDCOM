// trees.cxx

#include "fixes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
// docs are very unclear about what to #include to get the fltk namespace...
#include <FL/fl_ask.H>

#include "classes.h"
#include "family.h"
#include "structure.h"
#include "trees.h"
#include "gui.h"
#include "objects.h"
// beware that strings.h gets Posix routines whch are not available in standard C++ as in, say Visual Studio
//#include "strings.h"
#include "tags.h"

////////////////////////////////////////////////////////////////////////////////
// class treeinstance

treeinstance::treeinstance() :
  next (NULL),
  previous (NULL),
  UIcount (0),
  first_id (NULL),
  last_id (NULL),
  indicount (0),
  maxINDIid (0),
  famcount (0),
  maxFAMid (0),
  sourcount (0),
  maxSOURid (0),
  repocount (0),
  maxREPOid (0),
  linecount (0),
  modified (false)
{
#ifdef debugging
  printf("treeinstance constructor called to build minimal tree\n");
#endif
  filename = (char*)malloc(MAX_FILENAME * sizeof(char));
  // build the minimal empty tree structure
  rootobject = new GEDCOM_object( this ); // special form of constructor for root object
  rootobject->add_subobject( headlist = new GEDCOM_object( heads_tag ));
  rootobject->add_subobject( indilist = new GEDCOM_object( indis_tag ));
  rootobject->add_subobject( famslist = new GEDCOM_object( fams_tag ));
  rootobject->add_subobject( sourlist = new GEDCOM_object( sours_tag ));
  rootobject->add_subobject( repolist = new GEDCOM_object( repos_tag ));
  rootobject->add_subobject( trlrlist = new GEDCOM_object( trlrs_tag ));
  firstview = new mainUI(this); // is this required ?
  // well, if it is, then we need this:
  UIcount++;
  statsbox = new statsUI(this);
#ifdef debugging
  printf("treeinstance instanciated tree at %ld\n",(long)rootobject);
#endif
}

treeinstance::~treeinstance() {
mainUI * view;
mainUI * eyesore;
#ifdef destructorlogs
  printf("~treeinstance %ld, firstview %ld, statsbox %ld, rootobject %ld\n",(long)this,(long)firstview,(long)statsbox,(long)rootobject);
#endif
  eyesore = firstview;
  while (eyesore != NULL) {
    view = eyesore->getnext();
    delete eyesore;
    eyesore = view;
  }
  delete statsbox;
  delete rootobject; // which must call the destructors for the subobjects
  // and all the things those subobjects point to *uniquely*
}

void treeinstance::dump() {
  headlist->print(0);
  indilist->print(0);
  famslist->print(0);
  sourlist->print(0);
  repolist->print(0);
  trlrlist->print(0);
}

treeinstance * treeinstance::getnext() const {
  return next;
}

void treeinstance::setnext( treeinstance * newinstance ) {
  next = newinstance;
}

treeinstance * treeinstance::getprevious() const {
  return previous;
}

void treeinstance::setprevious( treeinstance * newinstance ) {
  previous = newinstance;
}

// methods for creating and destroying GEDCOM

void treeinstance::freshindi( GEDCOM_object* indi ) {
// initially when we create a new INDI we will have no name, so no point
// trying to insert it in collating sequence yet. Move it later...
  indilist->insert_first( indi );
}

void treeinstance::freshfam( GEDCOM_object* fam ) {
// FAM objects are mostly added in order of creation, so just insert at end
  famslist->add_subobject( fam );
}

void treeinstance::remove_indi( GEDCOM_object* person ) {
  if (person==NULL) {
    fl_alert("Alert! remove_indi called with NULL person\n");
    return;
  }
  GEDCOM_id* indi_id = person->getid();
  GEDCOM_object *famc = person->parental_family();
  GEDCOM_object *child = NULL;
  GEDCOM_object *chil;
  if (famc!=NULL) {
    chil = famc->child(&child, false);
    // the false indicates that we want the chil object, not the INDI it points to
    printf("Checking indi %s against chil %s\n",indi_id->GEDCOM_idname(),chil->getxref()->GEDCOM_idname());
    while (chil!=NULL) {
      if (indi_id==chil->getxref()) {
        printf("deleteindi_cb deleting CHIL from FAM\n");
        famc->delete_subobject( chil );
        break;
      }
      chil = famc->child(&child, false);
    }
  } else {
    // this is just debugging, not an error
    printf("remove_indi found no FAMC\n");
  }
  GEDCOM_object *fams=NULL;
  fams = person->own_family( fams );
  GEDCOM_object *spouse;
  spouse = NULL; // FAM without either spouse is possible is broken GEDCOM (and this stops the
                 // compiler whingeing that spouse might be undefined). In reality we know the
                 // indi we are removing must be either HUSB or WIFE in the FAMS referenced...
  while (fams!=NULL) {
    if (person==fams->thewife()) {
      spouse = fams->subobject(WIFE_tag);
    }
    if (person==fams->thehusband()) {
      spouse = fams->subobject(HUSB_tag);
    }
    if (fams->delete_subobject( spouse )) {
      if ((fams->garbage_fam( true ))<2) this->remove_fam( fams );
    }
    fams = person->own_family( fams );
  }
  notesUI* notebox;
  notebox = noteUIs->notesbox( person );
  while (notebox!=NULL) {
    noteUIs->close(notebox);
    notebox = noteUIs->notesbox( person );
  }
  indiUI* editbox;
  if ((editbox = (editUIs->editbox(person)))!=NULL) {
    editUIs->close( editbox );
  }
  if (!indilist->delete_subobject( person )) fl_alert("Failed trying to remove indi %ld;\n",(long)person);
}

void treeinstance::remove_fam( GEDCOM_object* fam ) {
  // if we are being called as a result of garbage collection on the FAM after
  // all the cross-referencing subobjects have gone, we are safe to search for
  // such subobjects, as they are fully gone, not left to give us NULL pointers.
  // Equally, it is not an error if we don't find them.
  // if we are called as a result of an "Unmarry", we need to look for the HUSB
  // and WIFE tags and follow them to remove FAMS from those INDIs. I think we
  // made a decision not to allow the unmarrying of FAMs which still have CHIL
  // objects, so we should probably check for those first and return without
  // doing any damage if they exist (as well as printing a diagnostic).
  // It's actually a bit of a context-dependent nightmare but having fixed the ifdef,
  // we are no longer refusing to delete the FAM in either of the contexts where
  // we hoped it would work.

  GEDCOM_object* link;
  GEDCOM_object* indi;
  link = fam->subobject(CHIL_tag);
  // We were copping out and failing to remove the FAM leaving broken GEDCOM.
  // We need to follow every CHIL, remove the FAMC from the INDI it points
  // to, then remove the CHIL tag object from the FAM. After that, we can proceed
  // to delete the FAM. It is up to caller to ensure that deleting everything is
  // a sensible idea (Unmarry now greyed in many cases).
  GEDCOM_object* famc;
  while (link != NULL) {
    // the indi must be at link->followxref()
    indi = link->followxref();
    // so we need subobject(FAMC_tag) of that (GEDCOM is broken if there isn't one)
    famc=indi->subobject(FAMC_tag);
    // and remove that object
    if (famc!=NULL) {
      indi->delete_subobject(famc);
    } else {
      fl_alert("treeinstance::remove_fam followed a CHIL to an INDI with no corresponding FAMC - GEDCOM is broken !\n");
      return;
    }
    // then remove the CHIL tag from this FAM
    fam->delete_subobject(link);
    // and set link to the next subobject(CHIL_tag)
    // if we just deleted the CHIL tag we found, then the next one must now be first so is just fam->subobject(CHIL_tag) again
    link = fam->subobject(CHIL_tag);
  }
  // I think this is only going to destroy notesUIs for a NOTE on the FAM, unless there
  // is code that recurses and does similar checks for subobjects like MARR, ENGA and DIV
  notesUI* notebox;
  notebox = noteUIs->notesbox( fam );
  while (notebox!=NULL) {
    noteUIs->close(notebox);
    notebox = noteUIs->notesbox( fam );
  }
  famUI* fambox;
  if ((fambox = (famUIs->fambox(fam)))!=NULL) {
    famUIs->close(fambox);
  }
  link = fam->subobject(HUSB_tag);
  if (link != NULL) {
    indi = link->followxref();
    if (indi != NULL) {
      link = indi->subobject(FAMS_tag);
      while (link!=NULL) {
        if ((link->followxref())==fam) {
          indi->delete_subobject(link);
          break;
        }
        link = link->next_object(FAMS_tag);
      }
    }
  }
  link = fam->subobject(WIFE_tag);
  if (link != NULL) {
    indi = link->followxref();
    if (indi != NULL) {
      link = indi->subobject(FAMS_tag);
      while (link!=NULL) {
        if ((link->followxref())==fam) {
          indi->delete_subobject(link);
          break;
        }
        link = link->next_object(FAMS_tag);
      }
    }
  }
  if (!famslist->delete_subobject( fam )) fl_alert("Failed trying to remove fam %ld;\n",(long)fam);
}

// methods for loading and saving GEDCOM

GEDCOM_object* treeinstance::getGEDCOMline(
    FILE        *in,
    GEDCOM_tag **tag,
    int         *level,
    const int    maxlevel) {

char tmpline[MAX_GEDCOM_line];
char line[MAX_GEDCOM_line];
char *line_ref;
char *tok;
char id[32];
char xref[32];
char value[MAX_GEDCOM_line];
GEDCOM_object* object;

  GEDCOMcount++;
  if (fgets( tmpline, MAX_GEDCOM_line-1, in ) == NULL) {
    // already checked eof, so could mean an error,
    // or simply a non-terminated last line ?
    return NULL;
  }
#ifdef debugging
  printf("Line %d is '%s'\n",GEDCOMcount,tmpline);
#endif
  
  line_ref = strtok(tmpline, "\r"); //ignore trailing <cr>s from broken software
  tok = strtok(line_ref, " "); // this ignores leading spaces which GEDCOM
  // shouldn't have, but which we should be able to cope with (and ignore).

  // return a null pointer for a blank line
  if (tok == NULL)
  {
#ifdef debugging
      printf("returning NULL pointer for blank line %ld\n", (long)line);
#endif
      return NULL;
  }

  // first token should always be a level number. If we can't decode a
  // number from it, choose to ignore the line
  { int i;
    if (sscanf(tok, "%d", &i) != 1) {
        fl_alert("Can't decode a level from line %s\n", tmpline);
        return NULL;
    }
    *level = i;
  }

  // next token might be an id or the tag
  tok = strtok(NULL, " \n");

  // is it an id ? starts with '@' if it is
  if (*tok == '@') {
    // drop a null over the trailing '@', then copy minus the first char
    *(tok + strlen(tok) - 1) = '\0';
    strcpy(id, tok + 1);
    // get another token, which must be the tag
    tok = strtok(NULL, " \n");
  }
  else id[0] = '\0';

// here we are making arbitrary assumptions that an id starting with one of
// these letters will be of the form <letter><integer>. This is fine, for the
// purpose of ensuring that any ids we generate will have larger integers than
// any we have seen, which is why we are doing it. However, it may be somewhat
// misleading. Some GEDCOMs may contain fairly arbitrary strings for these ids,
// which may give us "false positives" in this code. For example, if you have
// an INDI with id I27999 who married I25321 and the GEDCOM was generated by
// "Gene", you'll find that the FAM object is labelled "F27999X25321", which
// is perfectly reasonable and sensible, but will cause this code to set
// maxFAMid to 27999 ... It's not clear that this can ever do any harm, but
// all arbitrary assumptions are lurking traps for the unwary later on.

  switch ( id[0] ) {
    case 'I': if (atoi(&id[1])>maxINDIid) maxINDIid = atoi(&id[1]);
              break;
    case 'F': if (atoi(&id[1])>maxFAMid ) maxFAMid  = atoi(&id[1]);
              break;
    case 'S': if (atoi(&id[1])>maxSOURid) maxSOURid = atoi(&id[1]);
              break;
    case 'R': if (atoi(&id[1])>maxREPOid) maxREPOid = atoi(&id[1]);
              break;
  }
  printf("ID (might be empty) is %s\n", id);

  // at this point we must have the tag for this line
  // look it up (or add it to the list) returning a GEDCOM_tag pointer

  if (tok==NULL) {
    fl_alert("Bad tag at line %d, '%s'\n",GEDCOMcount,tmpline);
    return NULL;
  }
  *tag = GEDCOM_tagfromname( tok );
  if (tag==NULL) {
    fl_alert("Bad tag at line %d, '%s'\n",GEDCOMcount,tmpline);
    return NULL;
  }
  // both of those are probably broken GEDCOM and it is going to end badly
  // if we have a bad tag which has subobjects. Indeed, I don't think we are
  // rigourously testing that levels are kosher. We ought to run some broken
  // GEDCOM tests...

  // any more on this line ?
  tok = strtok(NULL, "\n");
  if (tok == NULL) {
    // no cross-reference and no text value
    if ((*tag==HUSB_tag)||(*tag==WIFE_tag)||(*tag==CHIL_tag)||(*tag==FAMC_tag)||(*tag==FAMS_tag)) {
      fl_alert("Expected a cross reference at line %d\n",GEDCOMcount);
    }
    xref[0] = '\0';
    value[0] = '\0';
  } else {

    // rest of line could be a cross-reference or a string value
    if (*tok != '@') xref[0]='\0';
    else {
      // still not that simple - could be an escape to processing, is next an '#' ?
      if (*(tok+1) == '#') xref[0]='\0';
      else {
        // for an xref, drop a null on the last '@' and copy without the first
        *(tok+strlen(tok)-1)='\0';
        strcpy( xref, tok+1);
        // I think there should never be a text value here, but no harm to
        // check (I've seen SOUR objects that have text...)
        tok = strtok(NULL, "\n");
      }
    }
    if (tok == NULL) value[0] = '\0'; else strcpy( value, tok );
  }
  if (*level>maxlevel) {
    fl_alert("Error: bad level in %d @%s@ %s @%s@ %s\n",
      (*level), id, (*tag)->GEDCOM_namefromtag(), xref, value);
    return NULL; // ignore that line
  }
  if (xref[0]!='\0') {
      // printf("found a GEDCOM line with an xref of %s:\n", xref);
      object = new GEDCOM_object( *tag, this->GEDCOM_idfromref( xref ) );
      // object->outline( stdout, *level );
      }
  else {
    if (id[0]!='\0') {
      object = new GEDCOM_object( this->GEDCOM_idfromref( id ), *tag );
    }
    else
      object = new GEDCOM_object( *tag, value );
      // the constructor will spot if value is a null string
  }
  if (id[0] != '\0') {
    if (this->GEDCOM_objectforref( id, object ) != 0) {
      fl_alert("object has same id %s as a previous object\n",id);
      // but carry on anyway - is this wise ? at least we get to report
      // all such errors before bombing out ...
    }
  // potentially that error could happen if someone managed to import
  // eg. 0 @I345@ FAM  or 0 @F123@ INDI but all IDs are unique in a
  // given GEDCOM, and internal checks in GEDCOM_id should ensure we
  // never create a conflict, so I reckon we are OK. The other situation
  // would be if we merged-on-load, but X!Family is intending to prevent
  // this by only allowing merging of two trees in memory in separate
  // main window instanciations - it would allocate new ids for all
  // the objects in one of the trees, thus avoiding possible clashes
  }
  return object;
}

void treeinstance::loadGEDCOMfile( const char * newfile ) {

FILE* in;

  if (newfile == NULL) {
    fl_alert("No filename given");
    strcpy_s(filename, MAX_FILENAME,"");
  }
  strcpy_s(filename, MAX_FILENAME,newfile);
#ifdef debugging
   printf("set filename for tree to %s\n",filename);
#endif
  if ((fopen_s(&in, filename, "r")) != 0) {
    fl_alert("Failed to open file");
    // no harm - just return with an empty tree - what do we mean "no harm ?" We will now crash !!
    strcpy_s(filename, MAX_FILENAME,"");
  }

  int level;
  int levelnow[6] = { -1, -1, -1, -1, -1, -1 };
  int currlist = 0;
  char id[32];
//  char tag[MAX_TAG];
  char xref[32];
  char value[MAX_GEDCOM_line];

  GEDCOM_object *SubTl[6][MAX_GEDCOM_levels];
  for (int i=0;i<MAX_GEDCOM_levels;i++) {
    for (int j=0;j<6;j++) SubTl[j][i] = NULL; }
  SubTl[0][0] = headlist;
  SubTl[1][0] = indilist;
  SubTl[2][0] = famslist;
  SubTl[3][0] = sourlist;
  SubTl[4][0] = repolist;
  SubTl[5][0] = trlrlist; // not really the trailer, but anything we
         // don't understand which will get put at the end just before TRLR
  GEDCOM_object *object;
  GEDCOM_tag *thistag;
  while (!feof( in )) {
    object = this->getGEDCOMline( in, &thistag, &level, levelnow[currlist]+1 );
    if (object==NULL) continue; // ignore blank or incomprehensible lines

    if (level == 0) {
      // can't use switch 'cos cases must be int constants, how pathetic
      if (thistag==HEAD_tag)   currlist = 0; else
      if (thistag==INDI_tag) { currlist = 1; indicount++; } else
      if (thistag==FAM_tag)  { currlist = 2;  famcount++; } else
      if (thistag==SOUR_tag) { currlist = 3; sourcount++; } else
      if (thistag==REPO_tag) { currlist = 4; repocount++; } else
       currlist = 5;
    }
    if (level == levelnow[currlist] + 1)
      SubTl[currlist][levelnow[currlist]+1]->add_subobject( object );
    else
      object->insert_after( SubTl[currlist][level] );
    levelnow[currlist] = level;
    SubTl[currlist][level] = object;  // object->next will get set for same level
    SubTl[currlist][level+1] = object;// object->sub will get set for a sub
    SubTl[currlist][level+2] = NULL;  // we will spot an error for an oversub
  } // endwhile
  // now at end of file, so we are effectively finished
  fclose( in );
}

bool treeinstance::integritycheck() {
  // at this point we should do an integrity check on the loaded tree - mainly looking
  // for references with no targets - but for every reference which finds a target, we
  // could check that the target is the right sort of object and contains an appropriate
  // object with a reference back to the starting object
  GEDCOM_object *person;
  GEDCOM_object *family;
  bool fail=false;
#ifdef debugging
  printf("Running integrity check on tree: indilist\n");
#endif
  person = indilist->subobject();
#ifdef debugging
  printf("Start of indilist object %s\n",person->objtype()->GEDCOM_namefromtag());
#endif
  //while (person->objtype()==INDI_tag) {
  while (person!=NULL) {
    fail = fail || integritycheck_indi(person);
    person = person->next_object();
  } // endwhile of going through entire INDI list of INDI objects
  // should now start at famlist and go through FAMs checking all HUSB, WIFE and CHIL objects.
  // obviously if the file integrity is OK, that will be all and exactly the same links we
  // checked above. But if the GEDCOM is *broken*, we need both sets of checks.

#ifdef debugging
  printf("Running integrity check on tree: famslist\n");
#endif
  family = famslist->subobject();
  while (family!=NULL) {
    fail = fail || integritycheck_fam( family );
    family = family->next_object();
  } // end while famslist (ie. at this point family has become NULL)
#ifdef debugging
  printf("Reached the end of FAM list\n");
#endif
  return fail;
}

// bool treeinstance::integritycheck() was huge and monolithic with a great nesting depth of tests and loops
// at the risk of a small amount of excess overhead, we have now factored all the code into more comprehensible
// small functions. There are one or two extra tests, too.

bool treeinstance::integritycheck_indi(GEDCOM_object *person) {
  GEDCOM_object *check;
  GEDCOM_object *subcheck;
  GEDCOM_object *family;
  bool links;
  bool fail=false;
  int famccount=0;

// called by integrity_check for each INDI in the indilist

#ifdef debugging
  printf("Start of indilist object %s\n",person->objtype()->GEDCOM_namefromtag());
#endif
  check = person->subobject();
#ifdef debugging
  printf("INDI %s, check = person->subobject() %s\n",person->getidname(),check->objtype()->GEDCOM_namefromtag());
#endif
  while (check!=NULL) {
    if (check->objtype()==FAMS_tag) {
#ifdef debugging
      printf("INDI %s, FAMS %s\n",person->getidname(),check->getxrefname());
#endif
      family = check->followxref();
      if ( (subcheck=(person->subobject(SEX_tag))) ==NULL) {
        fl_alert("Ungendered person %s (%s) has a FAMS tag %s\n",person->getidname(),person->subobject(NAME_tag)->value(),family->getxrefname());
        fail = true;
      } else {
#ifdef debugging
        printf("gender %s\n",subcheck->value());
#endif
        if (family==NULL) {
          fl_alert("Person %s (%s) has a FAMS tag with no target %s\n",person->getidname(),person->subobject(NAME_tag)->value(),family->getxrefname());
          fail = true;
        } else {
          if ((family->objtype())==FAM_tag) {
#ifdef debugging
            printf("xref leads to FAM %s\n", family->getidname());
#endif
            if (strncmp(subcheck->value(),"M",1)==0) {
              fail = fail || integritycheck_parent( HUSB_tag, person, family);
            } else {
              if (strncmp(subcheck->value(),"F",1)==0) {
                fail = fail || integritycheck_parent( WIFE_tag, person, family);
              } else {
                fl_alert("INDI %s seems to have invalid SEX tag %s\n",person->getidname(),subcheck->value());
                fail = true;
              }
            }
          } else {
            fl_alert("person %s (%s) has a FAMS tag which points at a non-FAM object\n",person->getidname(),person->subobject(NAME_tag)->value());
            fail = true;
          }
        }
      }
    } else {
      if (check->objtype()==FAMC_tag) {
        famccount++;
        // same stuff, but checking through FAM's CHIL tags...
        fail = fail || integritycheck_famc(check, person);
      } else {
#ifdef debugging
        printf("INDI %s, not FAMS or FAMC: %s\n",person->getidname(),check->objtype()->GEDCOM_namefromtag());
#endif
      }
    }
    check = check->next_object();
#ifdef debugging
    printf("Endwhile of this INDI, %ld\n",(long)check);
#endif
  } // endwhile going through list of INDI's subobjects looking for FAMS and FAMC
  if (famccount>1) {
    fl_alert("person %s (%s), has %d FAMC tags\n",person->getidname(),person->subobject(NAME_tag)->value(),famccount);
    return true;
  }
  return fail;
}

bool treeinstance::integritycheck_parent( GEDCOM_tag *spousetag, GEDCOM_object *person, GEDCOM_object *family) {
  GEDCOM_object *subcheck;
#ifdef debugging
  printf("Checking back for %s\n",spousetag->GEDCOM_namefromtag());
#endif
  subcheck = family->subobject(spousetag);
  if (subcheck==NULL) {
    fl_alert("INDI %s FAMS tag points to a FAM %s with no %s subobject\n",person->getidname(),family->getidname(),spousetag->GEDCOM_namefromtag());
    return true;
  } else {
    // check spouse obj points back to INDI
#ifdef debugging
    printf("FAM->HUSB/WIFE %s\n",subcheck->getxrefname());
#endif
    if (person!=subcheck->followxref()) {
      fl_alert("Person %s's FAMS points to FAM whose appropriate gendered spouse doesn't point back\n",person->getidname());
      return true;
    }
  }
  return false;
}

bool treeinstance::integritycheck_famc(GEDCOM_object *check, GEDCOM_object *person) {
  GEDCOM_object *family;
  GEDCOM_object *subcheck;
  bool links;
  bool fail=false;
#ifdef debugging
  printf("INDI %s, FAMC %s\n",person->getidname(),check->getxrefname());
#endif
  if ((family = (check->followxref()))==NULL) {
    fl_alert("Person %s (%s) has a FAMC tag with no target %s\n",person->getidname(),person->subobject(NAME_tag)->value(),check->getxrefname());
    fail = true;
  } else {
    if ((family->objtype())==FAM_tag) {
      subcheck = family->subobject(CHIL_tag);
      links = false;
      while (subcheck!=NULL) {
        if (person==subcheck->followxref()) {
          links = true;
          break;
        }
        subcheck = subcheck->next_object(CHIL_tag);
      }
      if (!links) {
        fl_alert("Person %s (%s) has a FAMC tag whose target has no corresponding CHIL tag\n",person->getidname(),person->subobject(NAME_tag)->value());
        fail = true;
      }
    } else {
      fl_alert("person %s (%s) has a FAMC tag which points at a non-FAM object\n",person->getidname(),person->subobject(NAME_tag)->value());
      fail = true;
    }
  }
  return fail;
}

bool treeinstance::integritycheck_fam(GEDCOM_object *family) {

// called by integrity_check for each FAM in the famlist
  GEDCOM_object *person;
  GEDCOM_object *check;
  GEDCOM_object *subcheck;
  bool links;
  bool fail=false;
  int count=0;

  check = family->subobject();
  while (check!=NULL) {
#ifdef debugging
    printf("FAM %s sub %s\n",family->getidname(),check->objtype()->GEDCOM_namefromtag());
#endif
    if (check->objtype()==HUSB_tag) {
      count++;
      fail = fail || integritycheck_spouse( HUSB_tag, check, family);
    }
    if (check->objtype()==WIFE_tag) {
      count++;
      fail = fail || integritycheck_spouse( WIFE_tag, check, family);
    }
    // similar for CHIL tag (should only ever be one FAMC to check points back)
    if (check->objtype()==CHIL_tag) {
      fail = fail || integritycheck_child( check, family );
    }
    check = check->next_object();
  } // end while subobjects of the FAM (ie. at this point check has become NULL)
  if (count<1) {
    fail = true;
    fl_alert("FAM %s has neither HUSB nor WIFE\n",family->getidname());
  }
  return fail;
}

bool treeinstance::integritycheck_spouse( GEDCOM_tag *spousetag, GEDCOM_object *check, GEDCOM_object *family) {
  GEDCOM_object *person;
  GEDCOM_object *gender;
  GEDCOM_object *subcheck;
  bool links;
  if ((person = check->followxref())==NULL) {
    fl_alert("FAM %s has HUSB tag with no target\n",family->getidname());
    return true;
  }
  if ((person->objtype())!=INDI_tag) {
    fl_alert("FAM %s has HUSB tag with non-INDI target\n",family->getidname());
    return true;
  }
  gender = person->subobject(SEX_tag);
  if (gender==NULL) {
    fl_alert("FAM %s has HUSB/WIFE which points to ungendered INDI\n",family->getidname());
    return true;
  }
  // it appears that we raise no eyebrow if HUSB is female - well, we do have at least one single-sex
  // civil partnership, so that's not entirely unreasonable (GEDCOM doesn't really cope with single-sex FAMs)
  subcheck = person->subobject(FAMS_tag);
  if (subcheck==NULL) {
    fl_alert("FAM %s has HUSB/WIFE with no FAMS\n",family->getidname());
    return true;
  } else {
    links = false;
    while (subcheck!=NULL) {
#ifdef debugging
      printf("Checking FAMS %s\n",subcheck->getxrefname());
#endif
      if (family==subcheck->followxref()) {
        links = true;
        break;
      } else {
        subcheck = subcheck->next_object(FAMS_tag);
      }
    }
    if (!links) {
     fl_alert("FAM %s HUSB/WIFE points to INDI %s whose FAMS doesn't point back\n",family->getidname(),person->getidname());
     return true;
    }
  }
#ifdef debugging
  printf("integritycheck_spouse believes FAM %s HUSB/WIFE points to INDI %s whose FAMS points back correctly\n",family->getidname(),person->getidname());
#endif
  if (strncmp(gender->value(),"M",1)==0) {
    if (spousetag==WIFE_tag) {
      fl_alert("FAM %s WIFE points to male INDI\n",family->getidname());
      return true;
    }
    return false;
  }
  if (strncmp(gender->value(),"F",1)==0) {
    if (spousetag==HUSB_tag) {
      fl_alert("FAM %s HUSB points to female INDI\n",family->getidname());
      return true;
    }
    return false;
  }
  fl_alert("FAM %s %s tag points to INDI with unknown gender %s\n",family->getidname(),spousetag->GEDCOM_namefromtag());
  return true;
}

bool treeinstance::integritycheck_child(GEDCOM_object *check, GEDCOM_object *family) {
  GEDCOM_object *person;
  GEDCOM_object *subcheck;
  if ((person = check->followxref())==NULL) {
    fl_alert("FAM %s has CHIL tag with no target\n",family->getidname());
    return true;
  }
  if ((person->objtype())!=INDI_tag) {
    fl_alert("FAM %s has CHIL tag with non-INDI target\n",family->getidname());
    return true;
  }
  subcheck = person->subobject(FAMC_tag);
  if (subcheck==NULL) {
    fl_alert("FAM %s has CHIL %s with no FAMC\n",family->getidname(), check->getxrefname());
    return true;
  } else {
    if (family!=(subcheck->followxref())) {
      fl_alert("FAM %s CHIL points to INDI whose FAMC doesn't point back\n",family->getidname());
      return true;
    }
    // there's an argument that this check ought to be under the INDI chain,
    // but, in fact, it's slightly less code to do it here, I think...
    if ((subcheck->next_object(FAMC_tag))!=NULL) {
      fl_alert("FAM %s CHIL points to INDI with more than one FAMC\n",family->getidname());
      return true;
    }
  }
  return false;
}

void treeinstance::setfilename( char* newfile ) {
  strcpy_s(filename, MAX_FILENAME,newfile);
}

void treeinstance::save() const {

FILE* out;
#ifdef debugging
  printf("About to try to save to %s\n", filename );
#endif
  // move this into the debugging section if not needed after initial testing
  ((treeinstance*) this)->integritycheck();
  if ((fopen_s( &out, filename, "w" )) != 0) {
    fl_alert("Failed to open for output");
  }
  else {
  int level = 0;
  GEDCOM_object* trlritem;
  GEDCOM_object* oldnote;
  GEDCOM_object* newnote;
  GEDCOM_object* person;
  mainUI* view;

    if ((headlist->subobject())!=NULL) headlist->subobject()->output( out, level );
    printf("end headlist\n");
    if ((indilist->subobject())!=NULL) indilist->subobject()->output( out, level );
    printf("end indilist\n");
    if ((famslist->subobject())!=NULL) famslist->subobject()->output( out, level );
    printf("end famslist\n");
    if ((sourlist->subobject())!=NULL) sourlist->subobject()->output( out, level );
    printf("end sourlist\n");
    if ((repolist->subobject())!=NULL) repolist->subobject()->output( out, level );
    printf("end repolist\n");
    trlritem = trlrlist->subobject();
    // first, remove the old NOTE objects saying who were current person(s)
    while (trlritem!=NULL) {
      if (trlritem->objtype()==NOTE_tag) {
        oldnote = trlritem;
        trlritem = trlritem->next_object();
        trlrlist->delete_subobject(oldnote);
      } else {
        trlritem = trlritem->next_object();
      }
    }

    trlritem = trlrlist->subobject( TRLR_tag );
    // now insert a single 0 NOTE Person <name> for the current person on the
    // first view we have open
    // then insert 0 NOTE View <indi-id> for the curent person on EACH view we have open
    person = firstview->getcurrent();
    // we should never get a view with no current person - unless we have an empty tree,
    // but we need to code defensively at this stage..
    char at[] = "@";
    char v[] = "View @";
    if (person!=NULL) {
      char p[] = "Person ";
      newnote = new GEDCOM_object( NOTE_tag, (new GEDCOM_string(p, person->subobject( NAME_tag )->value() ))->string());
      newnote->insert_before( trlritem );
      // this needs a way to insert the "@"s
      newnote = new GEDCOM_object( NOTE_tag, (new GEDCOM_string(v, person->getidname(), at ))->string());
      newnote->insert_before( trlritem );
    }
    view = firstview->getnext();
    while ( view!=NULL ) {
      person = view->getcurrent();
      if (person!=NULL) {
        newnote = new GEDCOM_object( NOTE_tag, (new GEDCOM_string(v, person->getidname(), at ))->string());
        newnote->insert_before( trlritem );
      }
      view = view->getnext();
    }
    trlrlist->subobject()->output( out, level );
    //fprintf(out,"end trlrlist\n");
        
    fclose(out);
  }
}

char* treeinstance::getfilename() const {
  return filename;
}

// methods for extracting stats

int treeinstance::getindicount() const {
  return indicount;
}

int treeinstance::getmaxindi() const {
  return maxINDIid;
}

int treeinstance::getfamcount() const {
  return famcount;
}

int treeinstance::getmaxfam() const {
  return maxFAMid;
}

int treeinstance::getsourcount() const {
  return sourcount;
}

int treeinstance::getmaxsour() const {
  return maxSOURid;
}

int treeinstance::getrepocount() const {
  return repocount;
}

int treeinstance::getmaxrepo() const {
  return maxREPOid;
}

int treeinstance::getnextindi() {
  maxINDIid++;
  return maxINDIid;
}

int treeinstance::getnextfam() {
  maxFAMid++;
  return maxFAMid;
}

int treeinstance::getnextsour() {
  maxSOURid++;
  return maxSOURid;
}

int treeinstance::getnextrepo() {
  maxREPOid++;
  return maxREPOid;
}

// methods for traversing/maintaining GEDCOM structures

GEDCOM_id* treeinstance::add_id( char *ref ) {
// program code should never need to call this - always use idfromref
// or objectforref to create a new id

GEDCOM_id* newid = new GEDCOM_id( ref );
  if (first_id==NULL)
    first_id = newid;
  else
    last_id->setnext( newid );
  last_id = newid;
  return newid;
// but we also need to keep track of the highest number used for IDs
// of the form Innn, Snnn, Fnnn, Rnnn used for individuals, sources,
// families and repositories. We must support arbitrary references
// in imported GEDCOM, but generate our own on that scheme and not
// create any duplicates (though we may leave gaps). None of which
// is relevant just here... this just keeps a list of what we've seen
}

// creating id for newly added INDIs or FAMs is a different process, and
// we create the id before the object
void treeinstance::add_id( GEDCOM_id* id ) {
  if (first_id==NULL)
    first_id = id;
  else
    last_id->setnext( id );
  last_id = id;
}

void treeinstance::drop_id( GEDCOM_id* oldid ) {
GEDCOM_id *previd,*tryid;
  if (first_id==NULL) { /* empty list - an error */ }
  if (first_id==oldid) {
    first_id = oldid->getnext();
  } else {
    previd = first_id;
    while ((tryid = previd->getnext())!=oldid) {
      previd = tryid;
      if (previd==NULL) { /* id not found - an error */ }
    }
    previd->setnext( oldid->getnext() );
  }
  delete oldid;
}

int treeinstance::GEDCOM_objectforref( char *ref, GEDCOM_object *obj ) {
// we have an object and an @id@ which should point to it (*not* an @ref@)
// so we want to find the GEDCOM_id  (or create it if necessary) and
// point it at the object. We also point the object at the id.
// but not if the id is already pointing at another object

  GEDCOM_id *found = this->GEDCOM_idfromref( ref );
  // that can never return NULL, as it would simply create a new id
  // but it might find an id which already references an object:
  int i = found->GEDCOM_setobjectforid( obj );
  if (i==0) obj->setid( found );
  return i;
}

GEDCOM_id* treeinstance::GEDCOM_idfromref( char *ref ) {
  GEDCOM_id *link = first_id;
  while (link!=NULL) {
    if ( strcmp(link->GEDCOM_idname(),ref) == 0) return link;
    link = link->getnext();
  }
return this->add_id( ref );
}

GEDCOM_object* treeinstance::GEDCOM_objectfromref( char *ref ) {
  return this->GEDCOM_idfromref( ref )->GEDCOM_objectfromid();
}

GEDCOM_object* treeinstance::Lookup_INDI( char* name ) const {

// Lookup_INDI with just a string argument assumes we will find a unique
// INDI to match, so we can use a hash algorithm to find within the list.
// At least, eventually we could, but its not written yet ...

GEDCOM_object *trythis;
GEDCOM_object *nameobj;

// first try the hash, which we will write shortly ...

// then try scanning the whole list. Since we have no control over GEDCOM
// files from outside, we can't assume the INDIs are sorted alphabetically
// (unless we manage to sort them on loading - we do need a way to insert
// INDIs into the structure in the right place, for when we create a new
// INDI, so the code will need to exist eventually...)

  //printf("Lookup_INDI called for %s\n",name );
//  printf("WE *know* Lookup_INDI works: it thinks the treeinstance is %ld\n",(long)this);
  trythis = indilist->subobject();
//  printf("and it gets the first subobject at %ld\n",(long)trythis);
  while (trythis!=NULL) {
    nameobj = trythis->subobject( NAME_tag );
    if (nameobj != NULL) {
      if (strcoll( name, nameobj->value() ) == 0 ) {
//        printf("Ended up at nameobj %ld\n",(long)nameobj);
        return trythis;
      }
    } else {
      // strictly, this is broken GEDCOM. We have two possible strategies:
      // 1) ignore it
      // 2) raise some sort of error
      //  in practice we will do (1) but alert the user, for now
      // you might also like to think what happens if someone has two NAME
      // objects. I think this is the correct way to do aliases ?
      fl_alert("Didn't find the name object for '%s'\n", name);
    }
    trythis = trythis->next_object();  // shouldn't need to specify ( INDI_tag )
    // because all the objects chained from indilist are INDI
  }
  fl_alert("Lookup_INDI() failed to find person '%s'\n",name);
  return NULL; // up to caller to decide what to use instead
}

GEDCOM_object* treeinstance::Lookup_INDI( GEDCOM_object* last_hit, char* name ) const {

// Lookup_INDI with two arguments assumes we will find more than one match for
// name - which becomes possible with GEDCOMs that don't expect every INDI to
// be identified purely by a unique NAME string. It is therefore possible that
// we will be raising an alert rather gratuitously when we have run out of
// matches (ie. every time we use this method).

GEDCOM_object *trythis;
GEDCOM_object *nameobj;

  if (last_hit == NULL) trythis = indilist->subobject();
  else trythis = last_hit->next_object();

  while (trythis!=NULL) {
    nameobj = trythis->subobject( NAME_tag );
    if (nameobj != NULL) {
      if (strcoll( name, nameobj->value() ) == 0 ) return trythis;
    } else { 
      fl_alert("Didn't find the name object '%s'\n", name);
    }
    trythis = trythis->next_object();  // shouldn't need to specify ( INDI_tag )
    // because all the objects chained from indilist are INDI
  }
  fl_alert("Lookup_INDI(,) failed to find person %s\n",name);
  return NULL; // no more matches in the tree
}

GEDCOM_object* treeinstance::INDI_fromleft( GEDCOM_object* last_hit, char* name ) const {
GEDCOM_object *trythis;
GEDCOM_object *nameobj;
int testlen;

//  printf("INDI_fromleft called for %s on tree at %ld\n",name, (long)this );
  testlen = strlen( name );
//  printf("length is %d, indilist is %ld\n",testlen, (long)indilist);
  if (last_hit == NULL) {
//    printf("find initial subobject of indilist\n");
    trythis = indilist->subobject();
//    printf("which produced %ld\n",(long)trythis);
  }
  else {
//    printf("find next object in indilist\n");
    trythis = last_hit->next_object();
  }

  while (trythis!=NULL) {
    nameobj = trythis->subobject( NAME_tag );
    if (nameobj != NULL) {
//      printf("trying to match with %s\n",nameobj->value());
      // we're supposed to be doing strncasecamp, but that's posix, not standard C and Windows doesn't have it
      if ( strncmp( nameobj->value(), name, testlen ) == 0 ) return trythis;
    }
    else {
      fl_alert("Didn't find the name object '%s'\n", name);
    }
    trythis = trythis->next_object();
  }
//  printf("run out of tree\n");
  return NULL; // no more matches in the tree
}

GEDCOM_object* treeinstance::INDI_fromright( GEDCOM_object* last_hit, char* name ) const {
// for testing:
//  return this->Lookup_INDI( last_hit, name );
GEDCOM_object *trythis;
GEDCOM_object *nameobj;
int testlen, trylen;
char* val;

  testlen = strlen( name );
  if (last_hit == NULL) {
    trythis = indilist->subobject();
  }
  else {
    trythis = last_hit->next_object();
  }

  while (trythis!=NULL) {
    nameobj = trythis->subobject( NAME_tag );
    if (nameobj != NULL) {
      val = nameobj->value();
      trylen = strlen( val );
      if ( trylen>=testlen ) {
          // we're supposed to be doing strcasecamp, but that's posix, not standard C and Windows doesn't have it
          if ( strcmp( val+trylen-testlen, name ) == 0 ) return trythis;
      }
    }
    else
      { printf("didn't find the name object\n"); }
    trythis = trythis->next_object();
  }
//  printf("run out of tree\n");
return NULL; // no more matches in the tree
}

GEDCOM_object* treeinstance::INDI_fuzzymatch( GEDCOM_object* last_hit, char* name ) const {
// for testing:
//  return this->Lookup_INDI( last_hit, name );
GEDCOM_object *trythis;
GEDCOM_object *nameobj;
std::string val;

  if (last_hit == NULL) {
    trythis = indilist->subobject();
  }
  else {
    trythis = last_hit->next_object();
  }

  while (trythis!=NULL) {
    nameobj = trythis->subobject( NAME_tag );
    if (nameobj != NULL) {
      val = nameobj->value();
      if ((val.find(name,0))!=std::string::npos ) return trythis;
      // aha ! that's how you do it ... took a lot of goes :-(
    }
    else
      { printf("didn't find the name object\n"); }
    trythis = trythis->next_object();
  }
//  printf("run out of tree\n");
return NULL; // no more matches in the tree
}

// !Family compatability

GEDCOM_object* treeinstance::noted_person() const {

// a GEDCOM file loaded from !Family will have a level 0 NOTE (which will
// therefore be in the TRLR list from TRLRs[instance]) containing the word
// "Person" followed by a name, which we should look up using Lookup_INDI
// This routine returns a pointer to the GEDCOM object holding that person,
// or, if there was no such note, the first person in the file. It is up to
// the caller to decide whether to set as current person the value returned

GEDCOM_object* trythis;
char* searchname;

  trythis = trlrlist->subobject( NOTE_tag );
  while (trythis != NULL) {
    if ( strncmp( searchname=(trythis->value()), "Person ", 7)==0 )
      return this->Lookup_INDI( searchname+7 );
    trythis = trythis->next_object( NOTE_tag );
  }
  return indilist->subobject();
  // default (since we must have a current person) is first in file
  // hmmm. What about when we have an empty tree ? That is how we start off...
  // perhaps we should now create the level 0 NOTE for that person ? Otherwise,
  // there is trouble in store when we do a save, trying to destroy the 0 NOTE
  // we imagine we have, in order to create the new one. That would lose our
  // const-ness
}

// xfamily replacement idiom

GEDCOM_object* treeinstance::noted_view( GEDCOM_object* start) const {

// a GEDCOM file saved by xfamily should have one or more level 0 NOTE
// object(s) followed by the word "View" and an indi id, representing
// the current person on each view that was open at the time the file
// was saved.
// We need to be able to return more than one GEDCOM_object pointer,
// but we can't return a linked list - the links are part of the object,
// and INDI objects will already have links.
// So we have the caller supply a GEDCOM_object in trlrlist as a parameter
// and traverse that linked list finding the next 0 NOTE View object and
// returning it. The caller can then extract the reference to an INDI,
// open a view, and call us again with the 0 NOTE View object for us to return
// the next one (or NULL). Since the caller doesn't have access to trlrlist,
// he must call us passing NULL the first time - we have it as a private
// variable, so we can use it...
// If there are no (more) NOTE View objects, we return NULL

GEDCOM_object* trythis;
char* searchref;

  trythis = start;
  if (trythis == NULL) {
    trythis = trlrlist->subobject( NOTE_tag );
  } else {
    trythis = trythis->next_object( NOTE_tag );
  }
  while (trythis != NULL) {
#ifdef debugging
    if (trythis!=NULL) {
      printf("Found another 0 NOTE\n");
      trythis->print( 0 );
    }
#endif
    if ( strncmp( searchref=(trythis->value()), "View ", 5)==0 ) {
      return trythis;
    }
    trythis = trythis->next_object( NOTE_tag );
  }
  return trythis;
}


// access to windows dealing with this tree:

mainUI* treeinstance::mainui() const {
  return firstview;
}

statsUI* treeinstance::statsui() const {
  return statsbox;
}

void treeinstance::addview( mainUI* newview ) {
  newview->setnext( firstview );
  UIcount++;
  firstview = newview;
}

int treeinstance::viewcount() {
  return UIcount;
}

void treeinstance::removeview( mainUI* view ) {
// we think there may be a method provided which would simplfy this function a great deal FIXME !
mainUI* thisview;
mainUI* maybeview;
  if (firstview==view) {
    firstview=view->getnext();
    delete view;
    UIcount--;
    return;
  }
  thisview = firstview;
  maybeview = thisview->getnext();
  while (maybeview!=view) {
    thisview = maybeview;
    maybeview = thisview->getnext();
  }
  if (maybeview==NULL) {
    fl_alert("Failed to find the view to delete\n");
    return;
  }
  printf("Assert maybeview==view %ld==%ld\n",maybeview, view);
  thisview->setnext(maybeview->getnext());
  delete maybeview;
  UIcount--;
  return;
}

void treeinstance::redraw() {
mainUI * view;
  view = firstview;
  printf("treeinstance redraw firstview %ld\n",(long)view);
  while (view != NULL) {
    view->settitle();
    view->settop();
    view->newdisplay();
    //view->centre_current();
    printf("Done newdisplay(), will call redraw on %ld\n",(long)view->window);
    view->window->redraw();
    view = view->getnext();
    printf("treeinstance redraw next view %ld\n",(long)view);
  }
}

void treeinstance::reopen() {
mainUI * view;
bool redraw;

  view = firstview;
  while (view != NULL) {
    if ( (redraw=(view->window->shown())) ) view->hide();
    // FIXME: window should not be public
    view->settitle();
    if (redraw) view->show();
    view = view->getnext();
  }
  if (statsbox->shown()) {
    statsbox->hide();
    // should we set the values for the statsbox here ? No ! we need
    // to be setting them every time we change them, and we don't change
    // them here, so it would be inappropriate
    statsbox->show();
  }
}

void treeinstance::setevent( GEDCOM_object * eventobject ) {
  lastevent = eventobject;
}

GEDCOM_object * treeinstance::geteventobject() const {
  return lastevent;
}


