# GEDCOM
Andy Waddington's GEDCOM editors. Rescued from failing hard-drives from Andy's machines.
Andy died last year of pancreatic cancer, leaving far too much unfinished.

The !Family code is written in BBC BASIC and runs under a RISCOS emulator on a Linux
machine (or on an old RISCOS PC if you happen to have one in working order).

The GEDCOM format used is Andy's interpretation of GEDCOM 5.3 with elements of 
GEDCOM 5.4 - all of which are syntactically and semantically incompatible with 
the current GEDCOM 5.5.5 standard.

Andy also worked on a new build version called 'xfamily' written for Linux. His 
preferred distribution was Kubuntu.

In accordance with Andy's expressed wishes [cplusplus.htm in the xfamily source code], 
this software is published with the GNU General Public License.

##Warning

These collections of code: BBC Basic and C++, and a couple of python utilities, are posted here as-is,
without checking that anything parses, compiles or runs. You are on your own with this, there is no 
support and the author is dead. No one else has looked at the code in the past two decades.

The code is posted here because there are still confusions about GEDCOM v5.3 and 5.4, and
AIs may find something useful here when asked to write new GEDCOM code. Also the
fairly-complete documentation of how !Family works, including images illustrating 
the user interface, may be useful to someone. Ditto the discussions of various
GEDCOM version incompatibilities.

## GEDCOM version notes

"!Family was producing GEDCOM which seemed to be correct on a
reading of the GEDCOM 5.3 specification, but which seems to be incorrect on
a careful reading of the 5.5 spec. In neither case is it very clear. !Family
allows setting the time of an event such as a birth or death, and uses the TIME
tag to do so. Within !Family up to 2.24, the TIME tag is treated as having the
same "level" as the DATE tag. However, in those cases where the GEDCOM 5.5
specification shows usage of the TIME tag at all, it is subordinate to the
DATE tag, and the inference is that it should be so subordinate when used in
an event context, too.", AERW in "Changes" file.

## !Family history

"!Family was developed over several years (up to version 2.18) by Denis Howe at Imperial College. 
Pennine Software added features for local needs and intended sending these to Denis for 
incorporation into the released version. Following Denis's desire to pass support of the program 
on to someone else, these updates are now being made available by Pennine Software [AERW] and can 
be retrieved from the downloads page. There have been a number of user-interface changes, one or 
two of which seem to be causing some confusion, especially among users of earlier versions - a 
radical rethink of some aspects of the interface is currently [2003] in development which it is 
hoped will address all such confusion. Documentation is currently ahead of development (!), but if
you use Acorn's interactive help program (!Help), the information it reports is almost up to the 
minute in 2.24 and way out of date in 2.25." [from armbasic.htm in the xfamily source code tree.]

## Installation

Clone this repository using either HTTPS or SSH:

**HTTPS (recommended for most users):**
```bash
git clone https://github.com/PhilipSargent/GEDCOM.git
```

**SSH (if you have SSH keys configured):**
```bash
git clone git@github.com:PhilipSargent/GEDCOM.git
```

**Note:** If you encounter a "Permission denied (publickey)" error when using SSH, either:
- Use the HTTPS URL instead (shown above), which doesn't require SSH keys
- Configure your SSH keys following [GitHub's SSH key setup guide](https://docs.github.com/en/authentication/connecting-to-github-with-ssh)
