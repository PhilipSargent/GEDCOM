# GEDCOM
The purpose of this repo is to publish Andy Waddington's GEDCOM editors for genealogical data. 
These were rescued from failing hard-drives from Andy's machines.
Andy (AERW) died last year (2025) of pancreatic cancer, leaving far too much unfinished.

There are two variants of the GEDCOM editor: !Family for RISCOS and xFamily for Linux.

The !Family code is written in BBC BASIC and runs under a RISCOS emulator on a Linux
machine (or on an old RISCOS PC if you happen to have one in working order). The xFamily code is writeen in C++
and runs natively on Linux. Andy's preferred distribution was Kubuntu.

The GEDCOM format used is Andy's interpretation of GEDCOM 5.3 with elements of 
GEDCOM 5.4 - all of which are syntactically and semantically incompatible with 
the current GEDCOM 5.5.5 standard.

In accordance with Andy's expressed wishes [cplusplus.htm in the xfamily source code], 
this software is published with the GNU General Public License.

## Warning

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
a careful reading of the 5.5 spec. In neither case is it very clear.", AERW in "Changes" file.

## !Family history

"!Family was developed over several years (up to version 2.18) by Denis Howe at Imperial College. 
Pennine Software [i.e. Andy] added features for local needs and intended sending these to Denis for 
incorporation into the released version. Following Denis's desire to pass support of the program 
on to someone else, these updates are now being made available by Pennine Software [AERW] and can 
be retrieved from the downloads page. There have been a number of user-interface changes, one or 
two of which seem to be causing some confusion, especially among users of earlier versions - a 
radical rethink of some aspects of the interface is currently [2003] in development which it is 
hoped will address all such confusion. Documentation is currently ahead of development (!), but if
you use Acorn's interactive help program (!Help), the information it reports is almost up to the 
minute in 2.24 and way out of date in 2.25." [from armbasic.htm in the xfamily source code tree.]

## Conversion from GEDCOM 5.3 to modern GEDCOM 5.5.5

In helping Andy's relatives get access to his genealogical data, I converted his personal .ged file (of
nearly 6,000 people, dating back to before 1066) from his variant of GEDCOM 5.3 into modern v5.5.5.
I used the 2019 version of GEDCOM: <a href="https://www.tamurajones.net/GEDCOM555JustARevision.xhtml">GEDCOM555 discussion</a>.
As she says: "the GEDCOM 5.5.1 specification includes support for a few new useful record types, such as those for email and web addresses, it is no more than a revision of GEDCOM 5.5, which was released on 11 Dec 1995, almost a quarter century ago.
Nothing much has changed in a quarter century [...] 
The GEDCOM 5.5.5 focus isn't on features, but on resolving issues with GEDCOM itself. [...]
It's a smaller, leaner, meaner and simpler GEDCOM." 

More details in the <a href="https://www.gedcom.org/press/PRGEDCOM555.pdf">5.5.5 release document</a>.
The wonderfully acerbic annotated 5.5.5 specification edited by Tamura Jones is (unfortunately) published as <a href="https://www.gedcom.org/specs/GEDCOM555.zip">a zipped PDF</a> and not as simple UTF-8 plain text which would be suitable for generating parsers and validators.

My converter is hacked up just to deal with a single .ged file, so it is in no sense a general purpose converter. It fails with over 130 error messages on Andy's
85,000 line family tree ged file. I used two validators: <a href="https://ged-inline.org/">GED inline</a> which is an online service and a locally-downloaded copy of 
teh open source <a href="https://gramps-project.org/blog/2025/11/gramps-6-0-6-released/">gramps</a> software (v6.0.6).

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
