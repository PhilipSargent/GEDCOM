#include <stdio.h>
#include <string.h>

#include <FL/Fl.H>
#include <FL/fl_message.H>

#include "xlate.h"
#include "classes.h"
#include "family.h"
#include "strings.h"

void fgetstringonly( FILE *msgs, char *message, int maxsize ) {

static char msgbuffer[MSG_BUFLEN];

  if (feof( msgs )) return;
  if (fgets( msgbuffer, MSG_BUFLEN-1, msgs ) == NULL) {
    // we weren't at eof, so must be an error, but we probably can't do anything
    return;
  }
  // so we got at least a blank line ... probably have a \n we don't want
  int l = strlen( msgbuffer ) - 1;
  if (maxsize < l) l = maxsize;
  memcpy( message, msgbuffer, l );
  *(message+l)='\0';
  // in practice we need to do escape processing to convert from "\n" to '\n'
  // in fact it is probably simplest to convert to <cr><lf>:
  char *check = message;
  while (check<(message+l)) {
    if (*check == '\\') {
      check++;
      if (*check == 'n') {
        check--; *check=' ';
        check++; *check='\n';
      }
    }
    check++;
  }
  if (feof( msgs )) return;
  // so we *must* have a newline unless the line was longer than MSG_BUFLEN-1  
  while (memchr( msgbuffer , '\n', MSG_BUFLEN-1 ) == NULL) {
    // throw away characters until we find a newline of eof
    if (fgets( msgbuffer, MSG_BUFLEN-1, msgs ) == NULL) {
      return;
    }
  }
  // must now be at the start of a line (or eof)
}

void xlate_messages(char *lang) {

// open file (in installed version) /usr/share/xfamily/<lang>
// if it doesn't open, we have a problem (because we should only
// be called for files which the main program has already decided
// exist). We should report the problem with the error message
// in the existing language, but be aware that if this is the first
// call, we won't have loaded any messages yet, so this message
// (and only this message) needs to be statically allocated in
// our default language (en-gb), into a const *char string which
// is long enough for our longest translation.

  char msgfile[38+strlen(lang)];
  strcpy(msgfile,"/usr/local/src/xfamily/v019/messages/");
  strcat(msgfile,lang);
  FILE *msgs;
  if ((msgs = fopen(msgfile,"r")) == NULL) {
    // can't have the language we chose, have we got a language to carry on ?
    if (strncmp(msg_nomsg, "xxxxxx", 6)!=0) {
      // here we can't load our new language, but can at least tell the user
      // in the language of the message set we have loaded
      // problem with this ? fl_message(msg_nomsg, lang);
      printf("fl_message(%s)",msg_nomsg);
      return;
    } else {
      // messages are uninitialised to a language - so program cannot have
      // got very far. See if we can load English messages:
      strcat(msgfile,"en");
      if ((msgs = fopen(msgfile,"r")) == NULL) {
        // we can't carry on, but can safely bomb out with an English message
        fl_message("Can't find messages for %s or for default (English)", lang);
        // whatever we do to utterly abandon the program
        return;
      }
      // we have got the english messages open, so can at least load them
    }
  }
  // here we can load the messages for our language of choice or english ...

#include "xlatestr.cxx"
// things like  fgetstringonly( msgs, msg_nomsg, MAX_nomsg );
// generated automatically by the stringfactory
  fclose( msgs );

}


  
