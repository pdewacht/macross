/*
 * Copyright (c) 1987 Fujitsu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/*
	actions.c -- Actions associated with various machine instruction
		     classes (6502 version).

	Chip Morningstar -- Lucasfilm Ltd.

	16-November-1984
*/

#include "macrossTypes.h"
#include "macrossGlobals.h"

#define operand (evaluatedOperands[0])
#define address	(evaluatedOperands[0])->value
#define class	(evaluatedOperands[0])->addressMode
#define binary	opcode->opcode

/*
   These routines are vectored off of the opcode lookup table.  Each
   instruction is of a particular category that defines which address modes
   it accepts its operands in and what size (one byte or two) the operands
   are.  There is one "actionsXXXX" routine for each of these categories that
   grabs the operand, checks the address mode, and emits the binary opcode
   and operand.
 */

  
bool isByteAddress (valueType *value);
bool isDefined (valueType *value);
extern void emitByte (byte byteValue);
bool wordCheck (int value);
extern void putFixupsHere (fixupKindType kindOfFixupsToPut, int whichFixup);
extern void emitWord (wordType wordValue);
bool byteCheck (int value);
bool isByteOffset (int value);
extern void error (errorType theError, ...);

void
actionsDir1(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define	ZERO_PAGE_ADDRESS_BIT		0x00
#define NON_ZERO_PAGE_ADDRESS_BIT	0x08

	if(class==EXPRESSION_OPND && isByteAddress(operand) &&
			isDefined(operand)){
		emitByte(binary | ZERO_PAGE_ADDRESS_BIT);
		emitByte(address);
	} else if (wordCheck(address)) {
		emitByte(binary | NON_ZERO_PAGE_ADDRESS_BIT);
		putFixupsHere(WORD_FIXUP, 0);
		emitWord(address);
	}
}

  void
actionsDir2(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
	if (wordCheck(address)) {
		emitByte(binary);
		putFixupsHere(WORD_FIXUP, 0);
		emitWord(address);
	}
}

  void
actionsDirIndir(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define DIRECT_ADDRESS_BIT	0x00
#define INDIRECT_ADDRESS_BIT	0x20

	if (wordCheck(address)) {
		if (class == INDIRECT_OPND)
			emitByte(binary | INDIRECT_ADDRESS_BIT);
		else
			emitByte(binary | DIRECT_ADDRESS_BIT);
		putFixupsHere(WORD_FIXUP, 0);
		emitWord(address);
	}
}

  void
actionsDirX1(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define DIRECT_ADDRESS_ZERO_PAGE_BITS_X1	0x04
#define A_REGISTER_BITS_X1			0x08
#define DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_X1	0x0C
#define X_INDEXED_ZERO_PAGE_BITS_X1		0x14
#define X_INDEXED_NON_ZERO_PAGE_BITS_X1		0x1C

	if (class == EXPRESSION_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_X1);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary|DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_X1);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else if (class==X_INDEXED_OPND || class==X_SELECTED_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | X_INDEXED_ZERO_PAGE_BITS_X1);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary | X_INDEXED_NON_ZERO_PAGE_BITS_X1);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else {
		emitByte(binary | A_REGISTER_BITS_X1);
	}
}

  void
actionsDirX2(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define DIRECT_ADDRESS_ZERO_PAGE_BITS_X2	0x00
#define DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_X2	0x08
#define X_INDEXED_ZERO_PAGE_BITS_X2		0x10
#define X_INDEXED_NON_ZERO_PAGE_BITS_X2		0x18

	if (class == EXPRESSION_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_X2);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary|DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_X2);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | X_INDEXED_ZERO_PAGE_BITS_X2);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary | X_INDEXED_NON_ZERO_PAGE_BITS_X2);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	}
}

  void
actionsDirX3(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
	if (class == EXPRESSION_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_X2);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary|DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_X2);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else {
		if (byteCheck(address)) {
			emitByte(binary | X_INDEXED_ZERO_PAGE_BITS_X2);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	}
}

  void
actionsDirY(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define DIRECT_ADDRESS_ZERO_PAGE_BITS_Y		0x00
#define DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_Y	0x08
#define Y_INDEXED_ZERO_PAGE_BITS_Y		0x10

	if (class == EXPRESSION_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_Y);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary|DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_Y);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else {
		if (byteCheck(address)) {
			emitByte(binary | Y_INDEXED_ZERO_PAGE_BITS_Y);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	}
}

  void
actionsImmDir(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define IMMEDIATE_DATA_BITS_ID			0x00
#define DIRECT_ADDRESS_ZERO_PAGE_BITS_ID	0x04
#define DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_ID	0x0C

	if (class == IMMEDIATE_OPND) {
		if (byteCheck(address)) {
			emitByte(binary | IMMEDIATE_DATA_BITS_ID);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	} else {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_ID);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary|DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_ID);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	}
}

  void
actionsImmDirX(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define IMMEDIATE_DATA_BITS_IX			0x00
#define DIRECT_ADDRESS_ZERO_PAGE_BITS_IX	0x04
#define DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_IX	0x0C
#define X_INDEXED_ZERO_PAGE_BITS_IX		0x14
#define X_INDEXED_NON_ZERO_PAGE_BITS_IX		0x1C

	if (class == IMMEDIATE_OPND) {
		if (byteCheck(address)) {
			emitByte(binary | IMMEDIATE_DATA_BITS_IX);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	} else if (class == EXPRESSION_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_IX);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary|DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_IX);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | X_INDEXED_ZERO_PAGE_BITS_IX);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary | X_INDEXED_NON_ZERO_PAGE_BITS_IX);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	}
}

  void
actionsImmDirY(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define IMMEDIATE_DATA_BITS_IY			0x00
#define DIRECT_ADDRESS_ZERO_PAGE_BITS_IY	0x04
#define DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_IY	0x0C
#define Y_INDEXED_ZERO_PAGE_BITS_IY		0x14
#define Y_INDEXED_NON_ZERO_PAGE_BITS_IY		0x1C

	if (class == IMMEDIATE_OPND) {
		if (byteCheck(address)) {
			emitByte(binary | IMMEDIATE_DATA_BITS_IY);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	} else if (class == EXPRESSION_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_IY);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary|DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_IY);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | Y_INDEXED_ZERO_PAGE_BITS_IY);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary | Y_INDEXED_NON_ZERO_PAGE_BITS_IY);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	}
}

  void
actionsImmIndex(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
#define PRE_INDEXED_BITS_A			0x00
#define DIRECT_ADDRESS_ZERO_PAGE_BITS_A		0x04
#define IMMEDIATE_DATA_BITS_A			0x08
#define DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_A	0x0C
#define POST_INDEXED_BITS_A			0x10
#define X_INDEXED_ZERO_PAGE_BITS_A		0x14
#define Y_INDEXED_NON_ZERO_PAGE_BITS_A		0x18
#define X_INDEXED_NON_ZERO_PAGE_BITS_A		0x1C

	if (class == EXPRESSION_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_A);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary |DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_A);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else if (class==X_INDEXED_OPND || class==X_SELECTED_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | X_INDEXED_ZERO_PAGE_BITS_A);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary | X_INDEXED_NON_ZERO_PAGE_BITS_A);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else if (class==Y_INDEXED_OPND || class==Y_SELECTED_OPND) {
		if (wordCheck(address)) {
			emitByte(binary | Y_INDEXED_NON_ZERO_PAGE_BITS_A);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else if (class == IMMEDIATE_OPND) {
		if (byteCheck(address)) {
			emitByte(binary | IMMEDIATE_DATA_BITS_A);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	} else if (class == POST_INDEXED_Y_OPND) {
		if (byteCheck(address)) {
			emitByte(binary | POST_INDEXED_BITS_A);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	} else {
		if (byteCheck(address)) {
			emitByte(binary | PRE_INDEXED_BITS_A);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	}
}

  void
actionsIndex(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
	if (class == EXPRESSION_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | DIRECT_ADDRESS_ZERO_PAGE_BITS_A);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary |DIRECT_ADDRESS_NON_ZERO_PAGE_BITS_A);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else if (class==X_INDEXED_OPND || class==X_SELECTED_OPND) {
		if (isByteAddress(operand) && isDefined(operand)) {
			emitByte(binary | X_INDEXED_ZERO_PAGE_BITS_A);
			emitByte(address);
		} else if (wordCheck(address)) {
			emitByte(binary | X_INDEXED_NON_ZERO_PAGE_BITS_A);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else if (class==Y_INDEXED_OPND || class==Y_SELECTED_OPND) {
		if (wordCheck(address)) {
			emitByte(binary | Y_INDEXED_NON_ZERO_PAGE_BITS_A);
			putFixupsHere(WORD_FIXUP, 0);
			emitWord(address);
		}
	} else if (class == POST_INDEXED_Y_OPND) {
		if (byteCheck(address)) {
			emitByte(binary | POST_INDEXED_BITS_A);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	} else {
		if (byteCheck(address)) {
			emitByte(binary | PRE_INDEXED_BITS_A);
			putFixupsHere(BYTE_FIXUP, 0);
			emitByte(address);
		}
	}
}

  void
actionsNone(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
	emitByte(binary);
}

  void
actionsRelative(opcodeTableEntryType *opcode, int numberOfOperands, valueType **evaluatedOperands)
{
	int	offset;

	if (operand->kindOfValue == UNDEFINED_VALUE || (currentCodeMode ==
			RELOCATABLE_BUFFER && targetOffset == 0))
		offset = 0;
	else
		offset = address - (currentLocationCounter.value - targetOffset) - 1;
	if (offset < 0)
		offset--;
	if (isByteOffset(offset)) {
		emitByte(binary);
		putFixupsHere(BYTE_RELATIVE_FIXUP, 0);
		emitByte(offset);
	} else {
		error(RELATIVE_OFFSET_TOO_LARGE_ERROR);
	}
}

/* 
   Miscellaneous helper predicates.
 */

  bool
isByte(int value)
{
	return (-129<value && value<256);
}

  bool
isByteOffset(int value)
{
	return (-129<value && value<128);
}

  bool
isWordOffset(int value)
{
	return (-32769<value && value<32768);
}

  bool
isByteAddress(valueType *value)
{
	return(value->kindOfValue==ABSOLUTE_VALUE && isByte(value->value));
}

  bool
isWord(int value)
{
	return (-32769<value && value<65536);
}

  bool
byteCheck(int value)
{
	if (isByte(value)) {
		return(TRUE);
	} else {
		error(BYTE_VALUE_TOO_LARGE_ERROR, value);
		return(FALSE);
	}
}

  bool
wordCheck(int value)
{
	if (isWord(value)) {
		return(TRUE);
	} else {
		printf("word: %d\n", value);
		error(WORD_VALUE_TOO_LARGE_ERROR, value);
		return(FALSE);
	}
}

  bool
isDefined(valueType *value)
{
	return(value!=NULL && value->kindOfValue!=UNDEFINED_VALUE);
}
