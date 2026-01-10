"""
2026-01-03 Updated version
GEDCOM Version Converter (5.3 to 5.5.5)

- Implements two-pass logic for SOUR record substitution.
- Replaces subsidiary SOUR references with the full text of the original declaration.
- Replaces CNTC tag with NAME.
"""

import sys
import argparse
import re
import json
from pathlib import Path
from dataclasses import dataclass, field
from typing import List, Optional, Tuple

# --- Regex Patterns ---
GEDCOM_LINE = re.compile(r"^(\d+)\s+(@[^@]+@\s+)?([A-Z0-9_]+)(?:\s+(.*))?$")
JULIAN_DATE = re.compile(r"(@#DJULIAN@)\s+(ABT|EST|CAL|FROM|TO|INT|AFT|BEF|BET)?\s*(.*)$", re.IGNORECASE)
SLASH_YEAR = re.compile(r"(\b\d{4})/(\d{1,4})\b")
LEADING_ZERO_DATE = re.compile(r"(^| )0([1-9])( [A-Z]{3})")

@dataclass
class GedcomContext:
    """Maintains state across lines during the conversion process."""
    in_header: bool = False
    encoding_found: bool = False
    last_plac_index: int = -1
    last_note_level: Optional[int] = None
    current_schema: Optional[dict] = None
    # NEW: Stores mapping of {@ID@: ["line1", "line2", ...]} - full SOUR content
    source_definitions: dict = field(default_factory=dict)
    # Track if we're currently inside a SOUR record (0 SOUR @ID@)
    in_sour_record: bool = False
    current_sour_id: Optional[str] = None
    schemas: List[dict] = field(default_factory=list)
    head_record: List[str] = field(default_factory=list)  # Store HEAD block
    processed_lines: List[str] = field(default_factory=list)
    sour_records: List[List[str]] = field(default_factory=list)  # Store complete SOUR records
    current_record: List[str] = field(default_factory=list)  # Current record being built
    conversions_count: int = 0

def transform_date(value: str) -> str:
    """Normalizes dates: leading zeros, dual-years, and Julian modifiers."""
    # 1. Strip leading zeros
    value = LEADING_ZERO_DATE.sub(r"\1\2\3", value)
    
    # 2. Fix dual years (1698/9 -> 1698/99)
    def dual_year_sub(m):
        base = m.group(1)
        nxt = (int(base) + 1) % 100
        return f"{base}/{nxt:02d}"
    value = SLASH_YEAR.sub(dual_year_sub, value)
    
    # 3. Reorder Julian
    # Special handling for BET/AND ranges with Julian marker
    if value.startswith("@#DJULIAN@ BET "):
        # Remove initial marker
        rest = value[len("@#DJULIAN@ BET "):]
        # Split at ' AND '
        if " AND " in rest:
            first, second = rest.split(" AND ", 1)
            value = f"BET @#DJULIAN@ {first.strip()} AND @#DJULIAN@ {second.strip()}"
        else:
            value = f"BET @#DJULIAN@ {rest.strip()}"
    else:
        j_match = JULIAN_DATE.search(value)
        if j_match:
            marker, mod, rest = j_match.groups()
            if mod:
                value = f"{mod.upper()} {marker} {rest}".strip()
            else:
                value = f"{marker} {rest}".strip()
    return value

def transform_age(value: str) -> str:
    """Formats age strings (numeric -> 'y', adds spaces between units)."""
    if value.isdigit():
        return f"{value}y"
    return re.sub(r"(\d+[a-zA-Z])(?=\d)", r"\1 ", value)

def handle_plac_logic(level: int, tag: str, value: str, xref_id: str, ctx: GedcomContext) -> Tuple[bool, str]:
    """
    Handles PLAC, SITE, CEME, and PLOT tag logic.
    Returns (handled, modified_tag) where:
      - handled: True if the tag was fully handled and should not be processed further
      - modified_tag: The potentially modified tag name
    """
    if tag == "PLAC":
        target_list = ctx.current_record if (ctx.in_header or ctx.in_sour_record) else ctx.processed_lines
        ctx.last_plac_index = len(target_list)
        return (False, tag)  # Continue processing with original tag
    elif tag == "SITE" and ctx.last_plac_index != -1:
        target_list = ctx.current_record if (ctx.in_header or ctx.in_sour_record) else ctx.processed_lines
        if ctx.last_plac_index < len(target_list):
            prev_line = target_list[ctx.last_plac_index]
            sep = " "
            target_list[ctx.last_plac_index] = f"{prev_line}{sep} {value}"
            ctx.conversions_count += 1
            return (True, tag)  # Tag was handled, stop processing
    elif tag == "CEME":
        target_list = ctx.current_record if (ctx.in_header or ctx.in_sour_record) else ctx.processed_lines
        if xref_id:
             target_list.append(f"{level} NOTE {value}")
        else:
             target_list.append(f"{level} NOTE {value}")
        ctx.conversions_count += 1
        return (True, tag)  # Tag was handled, stop processing
    elif tag == "PLOT":
        target_list = ctx.current_record if (ctx.in_header or ctx.in_sour_record) else ctx.processed_lines
        if xref_id:
             target_list.append(f"{level } CONT {value}")
        else:
             target_list.append(f"{level } CONT {value}")
        ctx.conversions_count += 1
        return (True, tag)  # Tag was handled, stop processing
   
    return (False, tag)  # Tag not handled, continue processing with original tag

def handle_note_logic(level: int, tag: str, ctx: GedcomContext) -> Tuple[int, str]:
    """
    Implements NOTE sequence logic:
    - If NOTE follows NOTE (same level or child level), second is CONT.
    - Aligns level of CONT/CONC to parent NOTE + 1.
    """
    if tag == "NOTE":
        is_consecutive = False
        if ctx.last_note_level is not None:
            # Check horizontal (same level) or hierarchical (level+1)
            if level == ctx.last_note_level or level == ctx.last_note_level + 1:
                is_consecutive = True
        
        if is_consecutive:
            ctx.conversions_count += 1
            return (ctx.last_note_level + 1, "CONT")
        else:
            ctx.last_note_level = level
            return (level, tag)

    if tag in ["CONT", "CONC"] and ctx.last_note_level is not None:
        return (ctx.last_note_level + 1, tag)

    # Any other tag breaks the note sequence
    ctx.last_note_level = None
    return (level, tag)

def process_line(line: str, ctx: GedcomContext):
    """Parses a single line and applies transformations based on tag and context."""

    match = GEDCOM_LINE.match(line.strip())
    if not match:
        if ctx.in_sour_record:
            ctx.current_record.append(line.strip())
        else:
            ctx.processed_lines.append(line.strip())
        return

    level, tag, value = int(match.group(1)), match.group(3), (match.group(4) or "").strip()
    xref_id = (match.group(2) or "").strip()  # Extract the @ID@ if present

    # Track when we enter/exit a SOUR record (0 SOUR @ID@)
    if level == 0 and tag == "SOUR" and xref_id:
        # Save previous record if we were building one
        if ctx.current_record:
            if ctx.in_header:
                ctx.head_record = ctx.current_record
            elif ctx.in_sour_record:
                ctx.sour_records.append(ctx.current_record)
            else:
                ctx.processed_lines.extend(ctx.current_record)
            ctx.current_record = []
        ctx.in_sour_record = True
        ctx.current_sour_id = xref_id
        # Add the 0 @ID@ SOUR line to the record
        ctx.current_record.append(line.strip())
        return
    elif level == 0:
        # Save previous record
        if ctx.current_record:
            if ctx.in_header:
                ctx.head_record = ctx.current_record
            elif ctx.in_sour_record:
                ctx.sour_records.append(ctx.current_record)
            else:
                ctx.processed_lines.extend(ctx.current_record)
            ctx.current_record = []
        # Reset flags when moving to next 0-level record
        ctx.in_header = False
        ctx.in_sour_record = False
        ctx.current_sour_id = None
    
    # If we're inside a SOUR record and encounter a SOUR reference, substitute it
    if ctx.in_sour_record and level > 0 and tag == "SOUR" and value.startswith("@") and value in ctx.source_definitions:
        # Replace the SOUR reference with the full content, recursively expanding any nested SOUR references
        sour_content = ctx.source_definitions[value]
        for content_line in sour_content:
            # Recursively check if this line itself contains a SOUR reference
            m_nested = GEDCOM_LINE.match(content_line.strip())
            if m_nested:
                nested_lvl = int(m_nested.group(1))
                nested_tag = m_nested.group(3)
                nested_val = (m_nested.group(4) or "").strip()
                nested_xref = (m_nested.group(2) or "").strip()
                
                # Apply tag conversions
                if nested_tag == "CLAS":
                    nested_tag = "TYPE"
                
                if nested_tag == "SOUR" and nested_val.startswith("@") and nested_val in ctx.source_definitions:
                    # Recursively expand this SOUR reference too
                    for nested_content_line in ctx.source_definitions[nested_val]:
                        # Parse and convert tags in the nested content as well
                        m_deep = GEDCOM_LINE.match(nested_content_line.strip())
                        if m_deep:
                            deep_lvl = int(m_deep.group(1))
                            deep_tag = m_deep.group(3)
                            deep_val = (m_deep.group(4) or "").strip()
                            deep_xref = (m_deep.group(2) or "").strip()
                            
                            # Apply tag conversions
                            if deep_tag == "CLAS":
                                deep_tag = "TYPE"
                            
                            # Reconstruct the line
                            if deep_xref:
                                deep_line = f"{deep_lvl} {deep_xref} {deep_tag} {deep_val}".strip()
                            else:
                                deep_line = f"{deep_lvl} {deep_tag} {deep_val}".strip()
                            ctx.current_record.append(deep_line)
                        else:
                            ctx.current_record.append(nested_content_line)
                else:
                    # Reconstruct the line with converted tag
                    if nested_xref:
                        converted_line = f"{nested_lvl} {nested_xref} {nested_tag} {nested_val}".strip()
                    else:
                        converted_line = f"{nested_lvl} {nested_tag} {nested_val}".strip()
                    ctx.current_record.append(converted_line)
            else:
                ctx.current_record.append(content_line)
        ctx.conversions_count += 1
        return

    # Insert '1 CHAR UTF-8' immediately after '0 HEAD'
    if level == 0 and tag == "HEAD":
        ctx.current_record.append(line.strip())
        ctx.current_record.append("1 CHAR UTF-8")
        ctx.in_header = True
        return

    # --- Schema Tracking ---
    if tag == "SCHEMA":
        ctx.current_schema = {"tag": value, "details": {}}
        ctx.schemas.append(ctx.current_schema)
    elif ctx.current_schema and level > 1:
        ctx.current_schema["details"][tag] = value
    else:
        ctx.current_schema = None

    # --- Generic Tag Replacements ---
    if tag == "CLAS":
        tag, ctx.conversions_count = "TYPE", ctx.conversions_count + 1
    elif tag == "FIDE":
        tag = "NOTE" if "ORIGINAL" in value.upper() or "PHOTOCOPY" in value.upper() else "_FIDE"
        ctx.conversions_count += 1
    # New replacement for CNTC -> NAME
    elif tag == "CNTC":
        tag, ctx.conversions_count = "NAME", ctx.conversions_count + 1
    # --- NOTE/CONT/CONC Logic ---
    level, tag = handle_note_logic(level, tag, ctx)

    # --- Special Transformations ---
    if tag == "LVG":
        ctx.processed_lines.append(f"{level} EVEN")
        ctx.processed_lines.append(f"{level + 1} TYPE alive")
        ctx.conversions_count += 1
        return

    if tag == "DATE" and value:
        old_val = value
        value = transform_date(value)
        if value != old_val: ctx.conversions_count += 1

    if tag == "AGE" and value:
        old_val = value
        value = transform_age(value)
        if value != old_val: ctx.conversions_count += 1

    # --- PLAC/SITE/CEME/PLOT Logic ---
    handled, tag = handle_plac_logic(level, tag, value, xref_id, ctx)
    if handled:
        return  # Tag was fully handled

    # --- Header Management ---
    if level == 0 and tag == "HEAD":
        ctx.in_header = True
    
    if ctx.in_header:
        if level == 2 and tag == "VERS":
            value = "5.5.5"
        

    # --- Output Assembly ---
    # Determine target list based on current context
    if ctx.in_header or ctx.in_sour_record:
        target_list = ctx.current_record
    else:
        target_list = ctx.processed_lines
    
    # Special handling for CAUS tag: split long values into CONT lines
    if tag == "CAUS" and len(value) > 80:
        # Find a split point at or before 70 chars, at a word boundary
        split_point = value.rfind(' ', 0, 70)
        if split_point == -1:
            split_point = 70  # fallback: hard split
        first_part = value[:split_point].rstrip()
        rest = value[split_point:].lstrip()
        # Include xref_id if present
        if xref_id:
            target_list.append(f"{level} {xref_id} {tag} {first_part}")
        else:
            target_list.append(f"{level} {tag} {first_part}")
        # CONT lines, indent one level deeper
        cont_level = level + 1
        while len(rest) > 0:
            if len(rest) > 80:
                next_split = rest.rfind(' ', 0, 70)
                if next_split == -1:
                    next_split = 70
                cont_text = rest[:next_split].rstrip()
                rest = rest[next_split:].lstrip()
            else:
                cont_text = rest
                rest = ''
            target_list.append(f"{cont_level} CONT {cont_text}")
    else:
        # Include xref_id if present (e.g., "@I697@" in "0 @I697@ INDI")
        if xref_id:
            out_line = f"{level} {xref_id} {tag} {value}".strip()
        else:
            out_line = f"{level} {tag} {value}".strip()
        target_list.append(out_line)

def convert_gedcom(input_path: Path, output_path: Path):
    """Two-pass conversion process."""
    if not input_path.exists():
        print(f"Error: {input_path} not found.")
        return

    ctx = GedcomContext()

    try:
        # PASS 1: Build Source Library (capture full SOUR record content)
        with input_path.open('r', encoding='utf-8-sig', errors='replace') as f_pass1:
            active_id = None
            current_sour_content = []
            in_head = False
            for line in f_pass1:
                stripped = line.strip()
                m = GEDCOM_LINE.match(stripped)
                if not m:
                    if active_id and stripped:
                        current_sour_content.append(stripped)
                    continue
                lvl, tag, val = int(m.group(1)), m.group(3), (m.group(4) or "").strip()
                xref = (m.group(2) or "").strip()  # Extract @ID@ if present
                
                # Track HEAD block
                if lvl == 0 and tag == "HEAD":
                    in_head = True
                elif lvl == 0:
                    in_head = False
                
                # Skip SOUR records inside HEAD
                if in_head:
                    continue
                
                if lvl == 0 and tag == "SOUR" and xref:
                    # Save previous SOUR record if exists
                    if active_id and current_sour_content:
                        ctx.source_definitions[active_id] = current_sour_content
                    # Start new SOUR record
                    active_id = xref
                    current_sour_content = []
                elif lvl == 0:
                    # Save SOUR record before moving to next 0-level record
                    if active_id and current_sour_content:
                        ctx.source_definitions[active_id] = current_sour_content
                    active_id = None
                    current_sour_content = []
                elif active_id and lvl > 0:
                    # Add lines belonging to the SOUR record
                    current_sour_content.append(stripped)
            # Don't forget the last SOUR record
            if active_id and current_sour_content:
                ctx.source_definitions[active_id] = current_sour_content

        # PASS 2: Convert
        with input_path.open('r', encoding='utf-8-sig', errors='replace') as fin:
            for line in fin:
                process_line(line, ctx)
        
        # Save any remaining record
        if ctx.current_record:
            if ctx.in_header:
                ctx.head_record = ctx.current_record
            elif ctx.in_sour_record:
                ctx.sour_records.append(ctx.current_record)
            else:
                ctx.processed_lines.extend(ctx.current_record)

        with output_path.open('w', encoding='utf-8') as fout:
            # Write UTF-8 BOM
            fout.write('\ufeff')
            # Write HEAD record first
            for line in ctx.head_record:
                fout.write(line + "\n")
            # Write all other records (except TRLR)
            trlr_line = None
            for pline in ctx.processed_lines:
                if pline.strip() == "0 TRLR":
                    trlr_line = pline
                else:
                    fout.write(pline + "\n")
            # Write all SOUR records before TRLR
            for sour_record in ctx.sour_records:
                for line in sour_record:
                    fout.write(line + "\n")
            # Write TRLR at the end
            if trlr_line:
                fout.write(trlr_line + "\n")

        print(f"Conversion Complete: {output_path}")
        print(f"Adjustments made: {ctx.conversions_count}")
        if ctx.schemas:
            print("\nSchemas found:")
            print(json.dumps(ctx.schemas, indent=2))

    except Exception as e:
        print(f"Critical error during processing: {e}")
        import traceback
        traceback.print_exc()

def main():
    parser = argparse.ArgumentParser(description="Convert GEDCOM 5.3 to 5.5.5")
    parser.add_argument("input", help="Source GEDCOM file")
    parser.add_argument("-o", "--output", help="Destination file (optional)")
    args = parser.parse_args()
    inp = Path(args.input)
    out = Path(args.output) if args.output else inp.with_name(f"{inp.stem}_v555{inp.suffix}")
    convert_gedcom(inp, out)

if __name__ == "__main__":
    main()