#! /usr/bin/env python

"""String Factory

Usage: expects a file "MessageSrc" and a directory "messages" in the
       current directory (which is OK, as we are prototyping for a
       Linux build tool, which would indeed be run with a suitable
       current directory).

MessageSrc specfies what languages it contains, and we build a file
       in messages for each language (eg. messages.en-gb ). we also
       build a file strings.cxx which contains C source to allocate
       static space for each message, with enough space to hold the
       longest translation of each message.

Caveat: this will need modifying to make the strings longer than that
       if they will need to be modified dynamically by the target code.
       eg. a menu item "Edit " may need a person's name adding to the
       end and we must make the static string big enough to have that
       name dynamically copied in.

Long-term: this is a prototype for a portable program (in particular,
       one that can be run under Linux and Windows) which might well
       end up being in perl .... However, this project involves me
       learning C++, Make, FLTK, and various other things already.
       Learning perl at this stage is not an option.    Later ...

Hmmm, this has grown somewhat, hasn't it ? We now write:

languages/h   which #defines a macro to say how many languages there are
languages/cxx which generates the language submenu structure
messages.<xx> where the <xx>s are the various ISO-639 language codes
strings.h     declares all the static strings
strings.cxx   defines all the static strings
xlatestr.cxx  the code which calls the function to copy the messages
"""

# prerequisites
true = -1
false = 0
maxlen = 0
import sys
import os
import string

def GetXlations():
  # should get called once per message token to read the list of translations
  SrcLine = I.readline()
  for j in range(0,n+1):
    Length[j] = 0
    Line[j] = 0
  # next
  maxlen = 0 # a maximum length over all translations
  LineInt = 0 # defensive initialisation
  while SrcLine!="\n":
    C = string.find(SrcLine,"::")
    if C!=-1:
      # start of new message group
      LineInt = 0
      nlang = 0
      D = C+2
      tok = SrcLine[:C]
      # tok Should be a language code list (of one or more, comma separated)
      while tok!="":
        S = string.find(tok,",")
        if S!=-1:
          # this message is used for more than one language
          lang[nlang] = tok[:S]
          nlang += 1
          tok = tok[S+1:]
        else:
          lang[nlang] = tok
          tok = ""
        # endif
      # endwhile
    else:
      # Didn't find a '::', so this should be a continuation line
      LineInt += 1
      D = 0
    # endif
    for k in range(0,nlang+1):
    # for each language for which this message is used
      for j in range(0,n+1):
      # look for the corresponding language index
        if Lang[j] == lang[k]:
          # and copy this line of message into its array
          Message[j].append(SrcLine[D:-1])
          Length[j] += len(Message[j][LineInt])
          Line[j]=LineInt+1
        # endif
      # next
    # next
    SrcLine = I.readline()
  # endwhile
  # now we've had a blank line, so all language groups for this token parsed
  # lets check we had at least one line of meassage for each language:

  for j in range(0,n+1):
    # check for each language
    if Line[j] == 0:
    # if there are no lines of message for this language
      for k in range(0,Line[0]):
        # copy the message line(s) for the default language (0)
        Message[j].append(Message[0][k])
      # next
      Line[j] = Line[0]
    # endif
    for k in range(0,Line[j]):
      MsgFile[j].write(Message[j][k])
    # next
    MsgFile[j].write("\n")
    if Length[j] > maxlen:
      maxlen = Length[j]
    # endif
  # next
  # now for Python, we need to reduce the messages back to empty lists:
  for j in range(0,n+1):
    Message[j]=[]
  return maxlen

def GetLanguages(Lang):
#LOCAL C%,N%,lang$

  N = -1

  Line = I.readline()
  maxlen = 0
  while N<MaxLang:
    N += 1
    Line = string.lstrip(Line)
    C = string.find(Line,",")
    if C!=-1:
      lang = Line[:C]
      Line = Line[C+1:]
    else:
      lang = Line
      Line = ""

    lang = string.rstrip(lang)
    Lang.append(lang)
    if len(lang) > maxlen:
      maxlen = len(lang)
    if Line=="":
      return N
  # endwhile
  return N


MaxLang = 10
MaxLines= 0
MaxMsg=1000
#DIM Lang$(MaxLang%), Language$(MaxLang%), Message$(MaxLang%,MaxLines%), I%(MaxLang%), lang$(MaxLang%), Line%(MaxLang%), len%(MaxLang%)
# we probably need to do this by defining empty lists, and we probably can't have lists and scalars with the same name ?
Lang=[]
Language=[]
Message=[]
MsgFile=[]
lang=[]
Line=[]
Length=[]

Oc = 0
Oh = 0
Ol = 0
Ox = 0
UpdateUI = false
minbuffer = 0

if os.access("messages",os.F_OK)==0:
 os.mkdir("messages")
I = open("MessageSrc","r")

n=GetLanguages(Lang)
m=GetLanguages(Language)
if n!=m:
 print"Should be same number of language codes and language names"
 sys.exit()

for j in range (0,n+1):
  lang.append("")
  Line.append(0)
  Length.append(0)
  Message.append([])
  MsgFile.append(0)
  MsgFile[j] = open("messages/"+Lang[j],"w")


Oc = open("strings.cxx","w")
Oh = open("strings.h","w")
Ol = open("languages.cxx","w")

Oh.write("#ifndef famstrings_h\n")
Oh.write("#define famstrings_h\n\n")
Oh.write("#define langcount "+str(n)+"\n")
Oh.write("extern char *languages[];\n\n")
Oh.write("extern char *languagenames[];\n\n")
Oc.write("// definitions for global static strings which are used for any and all text\n")
Oc.write("// which is used within the code, and which may need to be translated into\n")
Oc.write("// another language. By convention all begin msg_\n\n")
Oc.write('#include "strings.h"\n\n')
Oc.write("char *languages[] = {\n")

for j in range(0,n+1):
  Ol.write("  menu_data[next_menu_index+"+str(j)+"] = (Fl_Menu_Item){languagenames["+str(j)+"], 0, (Fl_Callback*)language_cb, (void*)("+str(j)+"),  0, 0, 0, 14, 0 };\n")
  Oc.write('  "'+Lang[j]+'"')
  if j!=n:
    Oc.write(",\n")
  else:
    Oc.write("\n")
# next
Oc.write("};\n\n")
Oc.write("char *languagenames[] = {\n")
for j in range(0,n+1):
  Oc.write('  "'+Language[j]+'"')
  if j!=n:
    Oc.write(",\n")
  else:
    Oc.write("\n")
# next
Oc.write("};\n\n")
Ol.close()
Ol = open("languages.h","w")
Ol.write("#ifndef languages_h\n")
Ol.write("#define languages_h\n")
Ol.write("#define language_count "+str(n+1)+"\n")
Ol.write("#endif\n")
Ol.close()
Ox = open("xlatestr.cxx","w")

while true:
  # This is main loop looking for translations
  ptr = I.readline()
  while ptr=="\n":
    ptr = I.readline()
  # endwhile
  if ptr=="":
    # that's the end of the input file
    break
  # We've read what ought to be a message token
  ptr = ptr[:-1] # strip off the newline
  maxlen = GetXlations()
  if maxlen > minbuffer:
    minbuffer = maxlen
  Oc.write("char msg_"+ptr+"[MAX_"+ptr+"];\n")
  Oh.write("extern char msg_"+ptr+"[];\n")
  Oh.write("#define MAX_"+ptr+" "+str(maxlen+1)+"\n")
  Ox.write("fgetstringonly( msgs, msg_"+ptr+", MAX_"+ptr+" );\n")
# endwhile
for j in range(0,n+1):
  MsgFile[j].close()

Oc.close()
Oh.write("#define MSG_BUFLEN "+str(minbuffer+2)+"\n")
Oh.write("#endif\n")
Oh.close()
Ox.close()
I.close()
print"Remember that those gpl_box messages need to be 15 lines or less\n"
sys.exit()

