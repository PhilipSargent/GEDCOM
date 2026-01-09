#!/usr/bin/env python3
"""
BBC BASIC V Detokeniser
(c) 2007 Matt Godbolt.
Updated for Python 3.13 standards by Gemini
Philip Sargent, 29 December 2025.

Converts binary BBC BASIC V format files e.g. Time2to3,ffb 
to plain text e.g. Time2to3.bbc
"""

import argparse
import re
import struct
import sys
from pathlib import Path
from typing import List, Tuple, BinaryIO, TextIO

# fmt: off
# The list of BBC BASIC V tokens starting at 0x7f
TOKENS = [
    'OTHERWISE', 'AND', 'DIV', 'EOR', 'MOD', 'OR', 'ERROR', 'LINE', 'OFF',
    'STEP', 'SPC', 'TAB(', 'ELSE', 'THEN', '<line>', 'OPENIN', 'PTR',
    'PAGE', 'TIME', 'LOMEM', 'HIMEM', 'ABS', 'ACS', 'ADVAL', 'ASC',
    'ASN', 'ATN', 'BGET', 'COS', 'COUNT', 'DEG', 'ERL', 'ERR',
    'EVAL', 'EXP', 'EXT', 'FALSE', 'FN', 'GET', 'INKEY', 'INSTR(',
    'INT', 'LEN', 'LN', 'LOG', 'NOT', 'OPENUP', 'OPENOUT', 'PI',
    'POINT(', 'POS', 'RAD', 'RND', 'SGN', 'SIN', 'SQR', 'TAN',
    'TO', 'TRUE', 'USR', 'VAL', 'VPOS', 'CHR$', 'GET$', 'INKEY$',
    'LEFT$(', 'MID$(', 'RIGHT$(', 'STR$', 'STRING$(', 'EOF',
    '<ESCFN>', '<ESCCOM>', '<ESCSTMT>',
    'WHEN', 'OF', 'ENDCASE', 'ELSE', 'ENDIF', 'ENDWHILE', 'PTR',
    'PAGE', 'TIME', 'LOMEM', 'HIMEM', 'SOUND', 'BPUT', 'CALL', 'CHAIN',
    'CLEAR', 'CLOSE', 'CLG', 'CLS', 'DATA', 'DEF', 'DIM', 'DRAW',
    'END', 'ENDPROC', 'ENVELOPE', 'FOR', 'GOSUB', 'GOTO', 'GCOL', 'IF',
    'INPUT', 'LET', 'LOCAL', 'MODE', 'MOVE', 'NEXT', 'ON', 'VDU',
    'PLOT', 'PRINT', 'PROC', 'READ', 'REM', 'REPEAT', 'REPORT', 'RESTORE',
    'RETURN', 'RUN', 'STOP', 'COLOUR', 'TRACE', 'UNTIL', 'WIDTH', 'OSCLI'
]

CFN_TOKENS = ['SUM', 'BEAT']
COM_TOKENS = [
    'APPEND', 'AUTO', 'CRUNCH', 'DELET', 'EDIT', 'HELP', 'LIST', 'LOAD',
    'LVAR', 'NEW', 'OLD', 'RENUMBER', 'SAVE', 'TEXTLOAD', 'TEXTSAVE', 'TWIN',
    'TWINO', 'INSTALL'
]
STMT_TOKENS = [
    'CASE', 'CIRCLE', 'FILL', 'ORIGIN', 'PSET', 'RECT', 'SWAP', 'WHILE',
    'WAIT', 'MOUSE', 'QUIT', 'SYS', 'INSTALL', 'LIBRARY', 'TINT', 'ELLIPSE',
    'BEATS', 'TEMPO', 'VOICES', 'VOICE', 'STEREO', 'OVERLAY'
]
# fmt: on

def detokenise(line_bytes: bytes) -> str:
    """Replace all tokens in the byte array with their ASCII equivalent."""

    def replace_func(match: re.Match) -> str:
        ext = match.group(1)
        token_bytes = match.group(2)
        token_ord = token_bytes[0]

        if ext:
            if ext == b'\xc6':
                return CFN_TOKENS[token_ord - 0x8E]
            if ext == b'\xc7':
                return COM_TOKENS[token_ord - 0x8E]
            if ext == b'\xc8':
                return STMT_TOKENS[token_ord - 0x8E]
            raise RuntimeError(f"Bad extension token: {ext.hex()}")
        
        # Normal token, plus any extra characters (decoded as latin-1)
        token_str = TOKENS[token_ord - 127]
        suffix = token_bytes[1:].decode('latin-1', errors='replace')
        return token_str + suffix

    # Note: Using bytes-based regex for binary data processing
    # \xf4 is the REM token
    pattern = rb"([\xc6-\xc8])?(\xf4.*|[\x7f-\xff])"
    # We decode the whole result to string after substitution
    result = re.sub(pattern, lambda m: replace_func(m).encode('latin-1'), line_bytes)
    return result.decode('latin-1', errors='replace')

def read_lines(data: bytes) -> List[Tuple[int, bytes]]:
    """Returns a list of [line number, tokenised line] from binary data."""
    lines = []
    offset = 0
    while offset < len(data):
        if data[offset] != ord('\r'):
            # Some files might have padding or unexpected headers
            if data[offset] == 0:
                offset += 1
                continue
            raise RuntimeError(f"Bad program format at offset {offset}: expected CR")
        
        if offset + 1 >= len(data):
            break
            
        if data[offset + 1] == 0xFF:
            break
            
        if offset + 4 > len(data):
            raise RuntimeError("Unexpected end of data while reading line header")
            
        line_number, length = struct.unpack(">hB", data[offset+1:offset+4])
        
        # Length includes the 4-byte header
        line_data = data[offset+4 : offset+length]
        lines.append((line_number, line_data))
        offset += length
        
    return lines

def decode_file(input_path: Path, output_path: Path) -> None:
    """Reads binary input and writes decoded text to output."""
    try:
        data = input_path.read_bytes()
        lines = read_lines(data)
        
        with output_path.open("w", encoding="utf-8") as f:
            for line_number, line_bytes in lines:
                text = detokenise(line_bytes)
                # BBC BASIC lines are traditionally just printed; 
                # we don't include the line number in the output string 
                # as detokenise handles the internal line format.
                f.write(f"{text}\n")
                
        print(f"Successfully decoded {len(lines)} lines to {output_path}")
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert binary BBC BASIC V files to text."
    )
    parser.add_argument("input", type=Path, help="Input binary file")
    parser.add_argument("output", type=Path, help="Output text file")
    
    args = parser.parse_args()
    
    if not args.input.exists():
        print(f"Error: Input file {args.input} does not exist.", file=sys.stderr)
        sys.exit(1)
        
    decode_file(args.input, args.output)

if __name__ == "__main__":
    main()