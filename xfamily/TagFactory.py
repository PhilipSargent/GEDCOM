#! /usr/bin/env python
import sys
#I% = OPENIN"KnownTags"
I = open("KnownTags","r")
#J% = OPENOUT"tags/cxx"
J = open("tags.cxx","w")
#K% = OPENOUT"taginc/cxx"
K = open("taginc.cxx","w")
#L% = OPENOUT"tags/h"
L = open("tags.h","w")

#WHILE NOT EOF#I%
while 1:
#  tag$=GET$#I%
 tag=I.readline()
 if tag=="":
  break
 # strip the newline
 tag=tag[0:-1]
#  BPUT#J%,"GEDCOM_tag *"+tag$+"_tag = NULL;"
 J.write("GEDCOM_tag *"+tag+"_tag = NULL;\n")
#  BPUT#K%,tag$+"_tag = new GEDCOM_tag("""+tag$+""");"
 K.write(tag+'_tag = new GEDCOM_tag("'+tag+'");\n')
#  BPUT#L%,"extern GEDCOM_tag *"+tag$+"_tag;"
 L.write("extern GEDCOM_tag *"+tag+"_tag;\n")
 continue
#ENDWHILE

#CLOSE#I%
I.close()
#CLOSE#J%
J.close()
#CLOSE#K%
K.close()
#CLOSE#L%
L.close()
#END
sys.exit()
