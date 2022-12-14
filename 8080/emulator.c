#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct ConditionCodes{
	uint8_t	z:1;
	uint8_t	s:1;
	uint8_t	p:1;
	uint8_t	cy:1;
	uint8_t	ac:1;
	uint8_t	pad:3;
} ConditionCodes;

typedef struct State8080{
	uint8_t	a;
	uint8_t	b;
	uint8_t	c;
	uint8_t	d;
	uint8_t	e;
	uint8_t	f;
	uint8_t	h;
	uint8_t	l;
	uint16_t	sp;
	uint16_t	pc;
	uint8_t	*memory;
	struct	ConditionCodes	cc;
	uint8_t	int_enable;
} State8080;

int Disassemble8080Op(unsigned char *codebuffer, int pc);
void UnimplementedInstruction(State8080* state);
void UndefinedInstruction(State8080* state);
int Emulate8080Op(State8080* state);

int main(int argc, char**argv){
	FILE *f= fopen(argv[1], "rb");
	if (f==NULL){
		printf("error: Couldn't open%s\n", argv[1]);
		exit(1);
	}

	fseek(f, 0L, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);

	unsigned char *buffer=malloc(fsize);

	fread(buffer, fsize, 1, f);
	fclose(f);

	int pc = 0;

	while(pc < fsize){
		//printf("%d\n", pc);
		pc += Disassemble8080Op(buffer,pc);
	}
	return 0;
}

int Disassemble8080Op(unsigned char *codebuffer, int pc){
	unsigned char *code = &codebuffer[pc];
	int opbytes = 1;
	printf("%04x ", pc);
	switch(*code){
		case 0x00: printf("NOP"); break;
		case 0x01: printf("LXI	B,#$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x02: printf("STAX	B"); break;
		case 0x03: printf("INX	B"); break;
		case 0x04: printf("INR	B"); break;
		case 0x05: printf("DCR	B"); break;
		case 0x06: printf("MVI	B,#$%02x", code[1]); opbytes=2; break;
		case 0x07: printf("RLC"); break;
		case 0x08: printf("UNDEFINED OPCODE 0x08"); break;
		case 0x09: printf("DAD	B"); break;
	   	case 0x0a: printf("LDAX	B"); break;
		case 0x0b: printf("DCX	B"); break;
		case 0x0c: printf("INR	C"); break;
		case 0x0d: printf("DCR	C"); break;
		case 0x0e: printf("MVI	C,#$%02x", code[1]); opbytes=2; break;
		case 0x0f: printf("RRC"); break;
		case 0x10: printf("UNDEFINED OPCODE 0x10"); break;
		case 0x11: printf("LXI	D,#$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x12: printf("STAX	D"); break;
		case 0x13: printf("INX	D"); break;
		case 0x14: printf("INR	D"); break;
		case 0x15: printf("DCR	D"); break;
		case 0x16: printf("MVI	D,#0x%02x", code[1]); opbytes=2; break;
		case 0x17: printf("RAL"); break;
		case 0x18: printf("UNDEFINED OPCODE 0x18"); break;
		case 0x19: printf("DAD	D"); break;
		case 0x1a: printf("LDAX	D"); break;
		case 0x1b: printf("DCX	D"); break;
		case 0x1c: printf("INR	E"); break; 
		case 0x1d: printf("DCR	E"); break;
		case 0x1e: printf("MVI	E,#0x%02x", code[1]); opbytes=2; break;
		case 0x1f: printf("RAR"); break;
		case 0x20: printf("RIM"); break;
		case 0x21: printf("LXI	H,#$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x22: printf("SHLD	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x23: printf("INX	H"); break;
		case 0x24: printf("INR	H"); break;
		case 0x25: printf("DCR	H"); break;
		case 0x26: printf("MVI	H,#0x%02x", code[1]); opbytes=2; break;
		case 0x27: printf("DAA"); break;
		case 0x28: printf("UNDEFINED OPCODE 0x28"); break;
		case 0x29: printf("DAD	H"); break;
		case 0x2a: printf("LHLD	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x2b: printf("DCX	H"); break;
		case 0x2c: printf("INR	L"); break;
		case 0x2d: printf("DCR	L"); break;
		case 0x2e: printf("MVI	L,#0x%02x", code[1]); opbytes=2; break;
		case 0x2f: printf("CMA"); break;
		case 0x30: printf("SIM"); break;
		case 0x31: printf("LXI	SP,#$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x32: printf("STA	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x33: printf("INX	SP"); break;
		case 0x34: printf("INR	M"); break;
		case 0x35: printf("DCR	M"); break;
		case 0x36: printf("MVI	M,#0x%02x", code[1]); opbytes=2; break;
		case 0x37: printf("STC"); break;
		case 0x38: printf("UNDEFINED OPCODE 0x38"); break;
		case 0x39: printf("DAD	SP"); break;
		case 0x3a: printf("LDA	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0x3b: printf("DCX	SP"); break;
		case 0x3c: printf("INR	A"); break;
		case 0x3d: printf("DCR	A"); break;
		case 0x3e: printf("MVI	A,#0x%02x", code[1]); opbytes=2; break;
		case 0x3f: printf("CMC"); break;
		case 0x40: printf("MOV	B,B"); break;
		case 0x41: printf("MOV	B,C"); break;
		case 0x42: printf("MOV	B,D"); break;
		case 0x43: printf("MOV	B,E"); break;
		case 0x44: printf("MOV	B,H"); break;
		case 0x45: printf("MOV	B,L"); break;
		case 0x46: printf("MOV	B,M"); break;
		case 0x47: printf("MOV	B,A"); break;
		case 0x48: printf("MOV	C,B"); break;
		case 0x49: printf("MOV	C,C"); break;
		case 0x4a: printf("MOV	C,D"); break;
		case 0x4b: printf("MOV	C,E"); break;
		case 0x4c: printf("MOV	C,L"); break;
		case 0x4d: printf("MOV	C,M"); break;
		case 0x4e: printf("MOV	C,A"); break;
		case 0x4f: printf("MOV	D,B"); break;
		case 0x50: printf("MOV	D,C"); break;
		case 0x51: printf("MOV	D,D"); break;
		case 0x52: printf("MOV	D,D"); break;
		case 0x53: printf("MOV	D,E"); break;
		case 0x54: printf("MOV	D,H"); break;
		case 0x55: printf("MOV	D,L"); break;
		case 0x56: printf("MOV	D,M"); break;
		case 0x57: printf("MOV	D,A"); break;
		case 0x58: printf("MOV	E,B"); break;
		case 0x59: printf("MOV	E,C"); break;
		case 0x5a: printf("MOV	E,D"); break;
		case 0x5b: printf("MOV	E,E"); break;
		case 0x5c: printf("MOV	E,H"); break;
		case 0x5d: printf("MOV	E,L"); break;
		case 0x5e: printf("MOV	E,M"); break;
		case 0x5f: printf("MOV	E,A"); break;
		case 0x60: printf("MOV	H,B"); break;
		case 0x61: printf("MOV	H,C"); break;
		case 0x62: printf("MOV	H,D"); break;
		case 0x63: printf("MOV	H,E"); break;
		case 0x64: printf("MOV	H,H"); break;
		case 0x65: printf("MOV	H,L"); break;
		case 0x66: printf("MOV	H,M"); break;
		case 0x67: printf("MOV	H,A"); break;
		case 0x68: printf("MOV	L,B"); break;
		case 0x69: printf("MOV	L,C"); break;
		case 0x6a: printf("MOV	L,D"); break;
		case 0x6b: printf("MOV	L,E"); break;
		case 0x6c: printf("MOV	L,H"); break;
		case 0x6d: printf("MOV	L,L"); break;
		case 0x6e: printf("MOV	L,M"); break;
		case 0x6f: printf("MOV	L,A"); break;
		case 0x70: printf("MOV	M,B"); break;
		case 0x71: printf("MOV	M,C"); break;
		case 0x72: printf("MOV	M,D"); break;
		case 0x73: printf("MOV	M,E"); break;
		case 0x74: printf("MOV	M,H"); break;
		case 0x75: printf("MOV	M,L"); break;
		case 0x76: printf("HLT"); break;
		case 0x77: printf("MOV	M,A"); break;
		case 0x78: printf("MOV	A,B"); break;
		case 0x79: printf("MOV	A,C"); break;
		case 0x7a: printf("MOV	A,D"); break;
		case 0x7b: printf("MOV	A,E"); break;
		case 0x7c: printf("MOV	A,H"); break;
		case 0x7d: printf("MOV	A,L"); break;
		case 0x7e: printf("MOV	A,M"); break;
		case 0x7f: printf("MOV	A,A"); break;
		case 0x80: printf("ADD	B"); break;
		case 0x81: printf("ADD	C"); break;
		case 0x82: printf("ADD	D"); break;
		case 0x83: printf("ADD	E"); break;
		case 0x84: printf("ADD	H"); break;
		case 0x85: printf("ADD	L"); break;
		case 0x86: printf("ADD	M"); break;
		case 0x87: printf("ADD	A"); break;
		case 0x88: printf("ADC	B"); break;
		case 0x89: printf("ADC	C"); break;
		case 0x8a: printf("ADC	D"); break;
		case 0x8b: printf("ADC	E"); break;
		case 0x8c: printf("ADC	H"); break;
		case 0x8d: printf("ADC	L"); break;
		case 0x8e: printf("ADC	M"); break;
		case 0x8f: printf("ADC	A"); break;
		case 0x90: printf("SUB	B"); break;
		case 0x91: printf("SUB	C"); break;
		case 0x92: printf("SUB	D"); break;
		case 0x93: printf("SUB	E"); break;
		case 0x94: printf("SUB	H"); break;
		case 0x95: printf("SUB	L"); break;
		case 0x96: printf("SUB	M"); break;
		case 0x97: printf("SUB	A"); break;
		case 0x98: printf("SBB	B"); break;
		case 0x99: printf("SBB	C"); break;
		case 0x9a: printf("SBB	D"); break;
		case 0x9b: printf("SBB	E"); break;
		case 0x9c: printf("SBB	H"); break;
		case 0x9d: printf("SBB	L"); break;
		case 0x9e: printf("SBB	M"); break;
		case 0x9f: printf("SBB	A"); break;
		case 0xa0: printf("ANA	B"); break;
		case 0xa1: printf("ANA	C"); break;
		case 0xa2: printf("ANA	D"); break;
		case 0xa3: printf("ANA	E"); break;
		case 0xa4: printf("ANA	H"); break;
		case 0xa5: printf("ANA	L"); break;
		case 0xa6: printf("ANA	M"); break;
		case 0xa7: printf("ANA	A"); break;
		case 0xa8: printf("XRA	B"); break;
		case 0xa9: printf("XRA	C"); break;
		case 0xaa: printf("XRA	D"); break;
		case 0xab: printf("XRA	E"); break;
		case 0xac: printf("XRA	H"); break;
		case 0xad: printf("XRA	L"); break;
		case 0xae: printf("XRA	M"); break;
		case 0xaf: printf("XRA	A"); break;
		case 0xb0: printf("ORA	B"); break;
		case 0xb1: printf("ORA	C"); break;
		case 0xb2: printf("ORA	D"); break;
		case 0xb3: printf("ORA	E"); break;
		case 0xb4: printf("ORA	H"); break;
		case 0xb5: printf("ORA	L"); break;
		case 0xb6: printf("ORA	M"); break;
		case 0xb7: printf("ORA	A"); break;
		case 0xb8: printf("CMP	B"); break;
		case 0xb9: printf("CMP	C"); break;
		case 0xba: printf("CMP	D"); break;
		case 0xbb: printf("CMP	E"); break;
		case 0xbc: printf("CMP	H"); break;
		case 0xbd: printf("CMP	L"); break;
		case 0xbe: printf("CMP	M"); break;
		case 0xbf: printf("CMP	A"); break;
		case 0xc0: printf("RNZ"); break;
		case 0xc1: printf("POP	B"); break;
		case 0xc2: printf("JNZ	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xc3: printf("JMP	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xc4: printf("CNZ	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xc5: printf("PUSH	B"); break;
		case 0xc6: printf("ADI	#0x%02x", code[1]); opbytes=2; break;
		case 0xc7: printf("RST	0"); break;
		case 0xc8: printf("RZ"); break;
		case 0xc9: printf("RET"); break;
		case 0xca: printf("JZ		$%02x%02x", code[2], code[1]); opbytes=3; break;
	   	case 0xcb: printf("UNDEFINED OPCODE 0xcb"); break;
		case 0xcc: printf("CZ		$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xcd: printf("CALL	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xce: printf("ACI	#0x%02x", code[1]); opbytes=2; break;
		case 0xcf: printf("RST	1"); break;
		case 0xd0: printf("RNC"); break;
		case 0xd1: printf("POP	D"); break;
		case 0xd2: printf("JNC	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xd3: printf("OUT	#0x%02x", code[1]); opbytes=2; break;
		case 0xd4: printf("CNC	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xd5: printf("PUSH	D"); break;
		case 0xd6: printf("SUI	#0x%02x", code[1]); opbytes=2; break;
		case 0xd7: printf("RST	2"); break;
		case 0xd8: printf("RC"); break;
		case 0xd9: printf("UNDEFINED OPCODE 0xd9"); break;
		case 0xda: printf("JC		$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xdb: printf("IN		#0x%02x", code[1]); opbytes=2; break;
		case 0xdc: printf("CC		$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xdd: printf("UNDEFINED OPCODE 0xdd"); break;
		case 0xde: printf("SBI	#0x%02x", code[1]); opbytes=2; break;
		case 0xdf: printf("RST	3"); break;
		case 0xe0: printf("RPO"); break;
		case 0xe1: printf("POP	H"); break;
		case 0xe2: printf("JPO	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xe3: printf("XTHL"); break;
		case 0xe4: printf("CPO	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xe5: printf("PUSH	H"); break;
		case 0xe6: printf("ANI	#0x%02x", code[1]); opbytes=2; break;
		case 0xe7: printf("RST 4"); break;
		case 0xe8: printf("RPE"); break;
		case 0xe9: printf("PCHL"); break;
		case 0xea: printf("JPE	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xeb: printf("XCHG"); break;
		case 0xec: printf("CPE	$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xed: printf("UNDEFINED OPCODE 0xed"); break;
		case 0xee: printf("XRI	#0x%02x", code[1]); opbytes=2; break;
		case 0xef: printf("RST 5"); break;
		case 0xf0: printf("RP"); break;
		case 0xf1: printf("POP	PSW"); break;
		case 0xf2: printf("JP		$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xf3: printf("DI"); break;
		case 0xf4: printf("CM		$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xf5: printf("PUSH	PSW"); break;
		case 0xf6: printf("ORI	#0x%02x", code[1]); opbytes=2; break;
		case 0xf7: printf("RST	6"); break;
		case 0xf8: printf("RM"); break;
		case 0xf9: printf("SPHL"); break;
		case 0xfa: printf("CM		$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xfb: printf("EI"); break;
		case 0xfc: printf("CM		$%02x%02x", code[2], code[1]); opbytes=3; break;
		case 0xfd: printf("UNDEFINED OPCODE 0xfd"); break;
		case 0xfe: printf("CPI	#0x%02x", code[1]); opbytes=2; break;
		case 0xff: printf("RST	7"); break;
	}
	printf("\n");
	return opbytes;
}

void UnimplementedInstruction(State8080* state){
	//pc will have advanced one, so undo that
	printf("error: Unimplemented instruction\n");
	exit(1);
}

void UndefinedInstruction(State8080* state){
	printf("error: undefined instruction\n");
	exit(1);
}

int Emulate8080Op(State8080* state){
	unsigned char *opcode = &state->memory[state->pc];

	switch(*opcode){
		case 0x00: break; //NOP
		case 0x01:{  //LXI B,word
			   state->c = opcode[1];
			   state->b = opcode[2];
			  }
			  break;
		case 0x02: UnimplementedInstruction(state); break;	
		case 0x03:{ //INX BC
			    	uint16_t answer = (((uint16_t)state->b << 4) | (uint16_t)state->c) + 0x01;
				state->c = answer & 0xff;
				state->b = answer >> 4;
			  } 	
			  break;
		case 0x04:{ //INR B
			   	uint16_t answer = (uint16_t) state->b + 0x01;
			   	state->cc.z = ((answer & 0xff) == 0);
			   	state->cc.s = ((answer & 0x80) != 0);
			   	state->cc.p = Parity(answer & 0xff);
			   	state->b = answer & 0xff;
			  } 	
			  break;
		case 0x05:{ //DCR B
			  	uint16_t answer = (uint16_t)state->b - 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->b = answer & 0xff;
			  }
		  	  break;	  
		case 0x06: UnimplementedInstruction(state); break; 	
		case 0x07: UnimplementedInstruction(state); break;
		case 0x08: UndefinedInstruction(state); break; 	
		case 0x09: UnimplementedInstruction(state); break; 	
		case 0x0a: UnimplementedInstruction(state); break; 	
		case 0x0b:{ //DCX BC
			    	uint16_t answer = (((uint16_t)state->b << 4) | (uint16_t)state->c) + 0x01;
				state->c = answer & 0xff;
				state->b = answer >> 4;
			  } 
			  break; 	
		case 0x0c:{ //INR C
			   	uint16_t answer = (uint16_t)state->b + 0x01;
      				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->c = answer & 0xff;		
			  }
			  break;
		case 0x0d:{ //INR C
			    	uint16_t answer = (uint16_t)state->c - 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->c = answer & 0xff;		  
			  } 	
			  break;
		case 0x0e: UnimplementedInstruction(state); break; 	
		case 0x0f:{ //RRC
			    	uint16_t MSB = ((uint16_t)state->a >> 7);
			  	state->cc.cy = MSB;
			       		
			  } 	
			  break;
		case 0x10: UndefinedInstruction(state); break; 	
		case 0x11:{ //LXI D, word
				state->e = opcode[1];
				state->d = opcode[2];			  
			  }	
			  break;
		case 0x12: UnimplementedInstruction(state); break; 	
		case 0x13:{ //INX DE 
			    	uint16_t answer = (((uint16_t)state->d << 4) | (uint16_t)state->e) + 0x01;
				state->d = answer & 0xff;
				state->e = answer >> 4;
			  } 	
			  break;
		case 0x14:{ //INR D
			   	uint16_t answer = (uint16_t) state->d + 0x01;
			   	state->cc.z = ((answer & 0xff) == 0);
			   	state->cc.s = ((answer & 0x80) != 0);
			   	state->cc.p = Parity(answer & 0xff);
			   	state->d = answer & 0xff;
			  } 	
			  break;
		case 0x15:{ //DCR D
			    	uint16_t answer = (uint16_t)state->d - 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->d = answer & 0xff;		  
			  }
		  	  break;	  
		case 0x16: UnimplementedInstruction(state); break; 	
		case 0x17: UnimplementedInstruction(state); break; 	
		case 0x18: UndefinedInstruction(state); break; 	
		case 0x19: UnimplementedInstruction(state); break; 	
		case 0x1a: UnimplementedInstruction(state); break; 	
		case 0x1b:{ //DCX DE 
			    	uint16_t answer = (((uint16_t)state->d << 4) | (uint16_t)state->e) - 0x01;
				state->e = answer & 0xff;
				state->d = answer >> 4;
			  } 
			  break; 	
		case 0x1c:{ //INR E
			  	uint16_t answer = (uint16_t)state->e + 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->e = answer & 0xff;
			  }
			  break;
		case 0x1d:{ //DCR E
			    	uint16_t answer = (uint16_t)state->e - 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->e = answer & 0xff;
			  } 	
			  break;
		case 0x1e: UnimplementedInstruction(state); break; 	
		case 0x1f: UnimplementedInstruction(state); break; 	
		case 0x20: UndefinedInstruction(state); break; 	
		case 0x21:{ //LXI H, word
			  	state->l = opcode[1];
				state->h = opcode[2];
			  } 	
			  break;
		case 0x22: UnimplementedInstruction(state); break; 	
		case 0x23:{ //INX Hl 
			    	uint16_t answer = (((uint16_t)state->h << 4) | (uint16_t)state->l) + 0x01;
				state->l = answer & 0xff;
				state->h = answer >> 4;
			  } 	
			  break;
		case 0x24:{ //INR H
			   	uint16_t answer = (uint16_t) state->h + 0x01;
			   	state->cc.z = ((answer & 0xff) == 0);
			   	state->cc.s = ((answer & 0x80) != 0);
			   	sate->cc.p = Parity(answer & 0xff);
			   	state->h = answer & 0xff;
			  }	
			  break;
		case 0x25:{ //DCR H
			    	uint16_t answer = (uint16_t)state->h - 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->h = answer & 0xff;
			  } 	
			  break;
		case 0x26: UnimplementedInstruction(state); break; 	
		case 0x27: UnimplementedInstruction(state); break; 	
		case 0x28: UndefinedInstruction(state); break; 	
		case 0x29: UnimplementedInstruction(state); break; 	
		case 0x2a: UnimplementedInstruction(state); break; 	
		case 0x2b:{ //DCX Hl 
			    	uint16_t answer = (((uint16_t)state->h << 4) | (uint16_t)state->l) - 0x01;
				state->l = answer & 0xff;
				state->h = answer >> 4;
			  } 	
			  break;
		case 0x2c:{ //INR L
			  	uint16_t answer = (uint16_t)state->l + 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->l = answer & 0xff;
			  } 	
			  break;
		case 0x2d:{ //DCR L
			    	uint16_t answer = (uint16_t)state->l - 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->l = asnwer & 0xff;
			  }  	
			  break;
		case 0x2e: UnimplementedInstruction(state); break; 	
		case 0x2f: UnimplementedInstruction(state); break; 	
		case 0x30: UndefinedInstruction(state); break; 	
		case 0x31:{ //LXI SP,word.   SP is 16 bit, unsure if endian issue is present
				state-> sp = (opcode[1] << 4) | opcode[2];
			  }
			  break;
		case 0x32: UnimplementedInstruction(state); break; 	
		case 0x33: UnimplementedInstruction(state); break; 	
		case 0x34: UnimplementedInstruction(state); break; 	
		case 0x35: UnimplementedInstruction(state); break; 	
		case 0x36: UnimplementedInstruction(state); break; 	
		case 0x37: UnimplementedInstruction(state); break; 	
		case 0x38: UndefinededInstruction(state); break; 	
		case 0x39: UnimplementedInstruction(state); break; 	
		case 0x3a: UnimplementedInstruction(state); break; 	
		case 0x3b: UnimplementedInstruction(state); break; 	
		case 0x3c:{ //INR A
			    uint16_t answer = (uint16_t)state->a + 0x01;
			    state->cc.z = ((answer & 0xff) == 0);
			    state->cc.s = ((answer & 0x80) != 0);
			    state->cc.p = Parity(answer & 0xff);
			    state->a = answer & 0xff;
			  }
			  break;
		case 0x3d:{ //DCR A
			  	uint16_t answer = (uint16_t)state->a - 0x01;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
		          break;	  
        	case 0x3e: UnimplementedInstruction(state); break; 
        	case 0x3f: UnimplementedInstruction(state); break;
		case 0x40: state->b = state->b; break; //MOV B,B
		case 0x41: state->b = state->c; break; //MOV B,C
		case 0x42: state->b = state->d; break; //MOV B,D	
		case 0x43: state->b = state->e; break; //MOV B,E 	
		case 0x44: state->b = state->h; break; //MOV B,H
		case 0x45: state->b = state->l; break; //MOV B,L 	
		case 0x46:{ //MOV B,M
			  	uint16_t offset = (state->h<<8) | (state->l);
				state->b = state->memory[offset];
			  }
			  break;
		case 0x47: state->b = state->a; break; //MOV B,A
		case 0x48: state->c = state->b; break; //MOV C,B	
		case 0x49: state->c = state->c; break; //MOV C,C	
		case 0x4a: state->c = state->d; break; //MOV C,D	
		case 0x4b: state->c = state->e; break; //MOV C,E
		case 0x4c: state->c = state->h; break; //MOV C,H	
		case 0x4d: state->c = state->l; break; //MOV C,L	
		case 0x4e: UnimplementedInstruction(state); break; //MOV C,M
		case 0x4f: state->c = state->a; break; //MOV C,A	
		case 0x50: state->d = state->b; break; //MOV D,B	
		case 0x51: state->d = state->c; break; //MOV D,C
		case 0x52: state->d = state->d; break; //MOV D,D
		case 0x53: state->d = state->e; break; //MOV D,E
		case 0x54: state->d = state->h; break; //MOV D,H
		case 0x55: state->d = state->l; break; //MOV D,L
		case 0x56: UnimplementedInstruction(state); break; //MOV D,M 
		case 0x57: state->d = state->a; break; //MOV D,A
		case 0x58: state->e = state->b; break; //MOV E,B	
		case 0x59: state->e = state->c; break; //MOV E,C	
		case 0x5a: state->e = state->d; break; //MOV E,D	
		case 0x5b: state->e = state->e; break; //MOV E,E	
		case 0x5c: state->e = state->h; break; //MOV E,H	
		case 0x5d: state->e = state->l; break; //MOV E,L	
		case 0x5e: UnimplementedInstruction(state); break; //MOV E,M
		case 0x5f: state->e = state->a; break; //MOV E,A	
		case 0x60: state->h = state->b; break; //MOV H,B	
		case 0x61: state->h = state->c; break; //MOV H,C	
		case 0x62: state->h = state->d; break; //MOV H,D	
		case 0x63: state->h = state->e; break; //MOV H,E	
		case 0x64: state->h = state->h; break; //MOV H,H	
		case 0x65: state->h = state->l; break; //MOV H,L	
		case 0x66: UnimplementedInstruction(state); break; //MOV H,M
		case 0x67: state->h = state->a; break; //MOV H,A	
		case 0x68: state->l = state->b; break; //MOV L,B	
		case 0x69: state->l = state->c; break; //MOV L,C	
		case 0x6a: state->l = state->d; break; //MOV L,D	
		case 0x6b: state->l = state->e; break; //MOV L,E	
		case 0x6c: state->l = state->h; break; //MOV L,H	
		case 0x6d: state->l = state->l; break; //MOV L,L	
		case 0x6e: UnimplementedInstruction(state); break; //MOV L,M
		case 0x6f: state->l = state->a; break; //MOV L,A	
		case 0x70: UnimplementedInstruction(state); break; //MOV M,B
		case 0x71: UnimplementedInstruction(state); break; //MOV M,C
		case 0x72: UnimplementedInstruction(state); break; //MOV M,D
		case 0x73: UnimplementedInstruction(state); break; //MOV M,E
		case 0x74: UnimplementedInstruction(state); break; //MOV M,H
		case 0x75: UnimplementedInstruction(state); break; //MOV M,L
		case 0x76: UnimplementedInstruction(state); break; //HLT
		case 0x77: UnimplementedInstruction(state); break; //MOV M,A
		case 0x78: state->a = state->b; break; //MOV A,B 	
		case 0x79: state->a = state->c; break; //MOV A,C 	
		case 0x7a: state->a = state->d; break; //MOV A,D 	
		case 0x7b: state->a = state->e; break; //MOV A,E 	
		case 0x7c: state->a = state->h; break; //MOV A,H
		case 0x7d: state->a = state->l; break; //MOV A,L   
        	case 0x7e: UnimplementedInstruction(state); break; //MOV A,M 
        	case 0x7f: state->a = state->a; break; //MOV A,A
		case 0x80:{ //ADD B
			   	uint16_t answer = (uint16_t) state->a + (uint16_t) state->b;
			   	state->cc.z = ((answer & 0xff) == 0);
			   	state->cc.s = ((answer & 0x80) != 0);
			   	state->cc.cy = (answer > 0xff);
			   	sate->cc.p = Parity(answer & 0xff);
			   	state->a = answer & 0xff;
			  }
			  break;
		case 0x81:{ //ADD C
			  	uint16_t answer = (uint16_t) state->a + (uint16_t)state->c;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;
			  }
			  break;
		case 0x82:{ //ADD D
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->d;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;		   
			  }
			  break;
		case 0x83:{ //ADD E
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->e;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;
			  }	
			  break;
		case 0x84:{ //ADD H
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->h;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;
			  } 	
			  break;
		case 0x85:{ //ADD L
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->l;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  	
			  break;
		case 0x86:{ //ADD M
			  	uint16_t offset = (state->h<<8) | (state->1);
			  	uint16_t answer = (uint16_t) state->a + state->memory[offset];
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer&0xff);
			  	state->a = answer & 0xff;
			  } 
		  	  break;	  
		case 0x87:{ //ADD A
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->a;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  } 	
			  break;
		case 0x88:{ //ADC B
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->b + (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;		  
			  } 	
			  break;
		case 0x89:{ //ADC C
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->c + (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;		  
			  } 
		  	  break;	  
		case 0x8a:{ //ADC D
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->d + (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;		  
			  } 	
			  break;
		case 0x8b:{ //ADC E
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->e + (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;		  
			  } 	
			  break;
		case 0x8c:{ //ADC H
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->h + (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;		  
			  } 	
			  break;
		case 0x8d:{ //ADC L
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->l + (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;		  
			  } 	
			  break;
		case 0x8e: UnimplementedInstruction(state); break; 	
		case 0x8f:{ //ADC A
				uint16_t answer = (uint16_t) state->a + (uint16_t)state->a + (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff;		  
			  } 	
			  break;
		case 0x90:{ //SUB B
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->b;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  } 
			  break;
		case 0x91:{ //SUB C
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->c;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }
 			  break;
		case 0x92:{ //SUB D
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->d;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }
			  break;
		case 0x93:{ //SUB E
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->e;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  	
			  break;
		case 0x94:{ //SUB H
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->h;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  } 
		  	  break;	  
		case 0x95:{ //SUB L
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->l;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  	
			  break;
		case 0x96:{ //SUB M
			  	uint16_t offset = (state->h<<8) | (state->l);
				uint16_t answer = (uint16_t) state->a - state->memory[offset];
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = (answer > 0xff);
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
			  break;
		case 0x97:{ //SUB A
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->a;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  } 	
			  break;
		case 0x98:{ //SBB B
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->b - (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  	
			  break;
		case 0x99:{ //SBB C
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->c - (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  	
		case 0x9a:{ //SBB D
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->d - (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  	
			  break;
		case 0x9b:{ //SBB E
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->e - (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  	
			  break;
		case 0x9c:{ //SBB H
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->h - (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  
		  	  break;	  
		case 0x9d:{ //SBB L
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->l - (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }  	
			  break;
		case 0x9e:{ //SBB M
			  	uint16_t offset = (state->h<<8) | (state->1);
				uint16_t answer = (uint16_t) state-> a - state->memory[offset] - (uint8_t)state->cc.cy;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = (answer > 0xff);
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
			  break;
		case 0x9f:{ //SBB A
				uint16_t answer = (uint16_t) state->a - (uint16_t)state->a - (uint8_t)state->cc.cy;
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			       	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer & 0xff);
			  	state->a = answer & 0xff; 
			  }	
			  break;
		case 0xa0:{ //ANA B
			  	uint16_t answer = (uint16_t)state->a & (uint16_t)state->b;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xa1:{ //ANA C
			  	uint16_t answer = (uint16_t)state->a & (uint16_t)state->c;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
			  break;
		case 0xa2:{ //ANA D
			  	uint16_t answer = (uint16_t)state->a & (uint16_t)state->d;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }
			  break;
		case 0xa3:{ //ANA E
			  	uint16_t answer = (uint16_t)state->a & (uint16_t)state->e;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }
			  break;
		case 0xa4:{ //ANA H
			  	uint16_t answer = (uint16_t)state->a & (uint16_t)state->h;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }
			  break;
		case 0xa5:{ //ANA L
			  	uint16_t answer = (uint16_t)state->a & (uint16_t)state->l;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }
			  break;
		case 0xa6:{
			  	uint16_t offset = (state->h<<8) | (state->l);
				uint16_t answer = (uint16_t)state->a & state-memory[offset];
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
			  break;
		case 0xa7:{ //ANA A
			  	uint16_t answer = (uint16_t)state->a & (uint16_t)state->a;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
			  break;
		case 0xa8:{ //XRA B
			  	uint16_t answer = (uint16_t)state->a ^ (uint16_t)state->b;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
			  break;
		case 0xa9:{ //XRA C
			  	uint16_t answer = (uint16_t)state->a ^ (uint16_t)state->c;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xaa:{ //XRA D
			  	uint16_t answer = (uint16_t)state->a ^ (uint16_t)state->d;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  
		  	  break;	  
		case 0xab:{ //XRA E
			  	uint16_t answer = (uint16_t)state->a ^ (uint16_t)state->e;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xac:{ //XRA H
			  	uint16_t answer = (uint16_t)state->a ^ (uint16_t)state->h;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xad:{ //XRA L
			  	uint16_t answer = (uint16_t)state->a ^ (uint16_t)state->l;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xae:{ //XRA M
			  	uint16_t offset = (state->h<<8) | (state->l);
				uint16_t answer = (uint16_t)state-> ^ state->memory[offset];
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
			  break;
		case 0xaf:{ //XRA A
			  	uint16_t answer = (uint16_t)state->a ^ (uint16_t)state->a;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  } 	
			  break;
		case 0xb0:{ //ORA B
			  	uint16_t answer = (uint16_t)state->a | (uint16_t)state->b;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }
			  break;
		case 0xb1:{ //ORA C
			  	uint16_t answer = (uint16_t)state->a | (uint16_t)state->c;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xb2:{ //ORA D
			  	uint16_t answer = (uint16_t)state->a | (uint16_t)state->d;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xb3:{ //ORA E
			  	uint16_t answer = (uint16_t)state->a | (uint16_t)state->e;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xb4:{ //ORA H
			  	uint16_t answer = (uint16_t)state->a | (uint16_t)state->h;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
			  break;
		case 0xb5:{ //ORA L
			  	uint16_t answer = (uint16_t)state->a | (uint16_t)state->l;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }
		  	  break;	  
		case 0xb6:{ //ORA M
			    	uint16_t offset = (state->h<<8) | (state->l);
			  	uint16_t answer = (uint16_t)state->a | state->memory[offset];
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }
			  break;
		case 0xb7:{ //ORA A
			  	uint16_t answer = (uint16_t)state->a | (uint16_t)state->a;
				state->cc.z = ((answer & 0xff) == 0);
				state->cc.s = ((answer & 0x80) != 0);
				state->cc.cy = false;
				state->cc.ac = false;
				state->cc.p = Parity(answer & 0xff);
				state->a = answer & 0xff;
			  }  	
		case 0xb8:{ //CMP B
			    	uint16_t answer = (uint16_t)state->a - (uint16_t)state->b;
				state->cc.z = (answer == 0);
				state->cc.s = ((answer & 0x80) != 0);
		      		state->cc.cy = (answer > 0);
				state->cc.p = Parity(answer & 0xff);	
			  }
			  break;
		case 0xb9:{ //CMP C
			    	uint16_t answer = (uint16_t)state->a - (uint16_t)state->c;
				state->cc.z = (answer == 0);
				state->cc.s = ((answer & 0x80) != 0);
		      		state->cc.cy = (answer > 0);
				state->cc.p = Parity(answer & 0xff);	
			  }
			  break;
		case 0xba:{ //CMP D
			    	uint16_t answer = (uint16_t)state->a - (uint16_t)state->d;
				state->cc.z = (answer == 0);
				state->cc.s = ((answer & 0x80) != 0);
		      		state->cc.cy = (answer > 0);
				state->cc.p = Parity(answer & 0xff);	
			  }
		  	  break;	  
		case 0xbb:{ //CMP E
			    	uint16_t answer = (uint16_t)state->a - (uint16_t)state->e;
				state->cc.z = (answer == 0);
				state->cc.s = ((answer & 0x80) != 0);
		      		state->cc.cy = (answer > 0);
				state->cc.p = Parity(answer & 0xff);	
			  }
			  break;
		case 0xbc:{ //CMP H
			    	uint16_t answer = (uint16_t)state->a - (uint16_t)state->h;
				state->cc.z = (answer == 0);
				state->cc.s = ((answer & 0x80) != 0);
		      		state->cc.cy = (answer > 0);
				state->cc.p = Parity(answer & 0xff);	
			  } 
			  break;
		case 0xbd: { //CMP L
			    	uint16_t answer = (uint16_t)state->a - (uint16_t)state->l;
				state->cc.z = (answer == 0);
				state->cc.s = ((answer & 0x80) != 0);
		      		state->cc.cy = (answer > 0);
				state->cc.p = Parity(answer & 0xff);	
			  }
			  break;	   
        	case 0xbe: UnimplementedInstruction(state); break; 
        	case 0xbf:{ //CMP A, should be able to collapse this
			    	uint16_t answer = (uint16_t)state->a - (uint16_t)state->a;
				state->cc.z = (answer == 0);
				state->cc.s = ((answer & 0x80) != 0);
		      		state->cc.cy = (answer > 0);
				state->cc.p = Parity(answer & 0xff);	
			  }
			  break; 
		case 0xc0: UnimplementedInstruction(state); break;
		case 0xc1:{ //POP B
			  	state->c = state->memory[state->sp];
				state->b = state->memory[state->sp+1];
				state->sp += 2;
			  }
			  break;
		case 0xc2: UnimplementedInstruction(state); break;	
		case 0xc3: UnimplementedInstruction(state); break; 	
		case 0xc4: UnimplementedInstruction(state); break; 	
		case 0xc5:{ //PUSH B
			  	state->memory[state->sp-1] = state->b;
				state->memory[state->sp-2] = state->c;
				state->sp = state->sp-2;
			  } 	
			  break;
		case 0xc6:{ //ADI d8
			  	uint16_t answer = (uint16_t) state->a + (uint16_t) opcode[1];
			  	state->cc.z = ((answer & 0xff) == 0);
			  	state->cc.s = ((answer & 0x80) != 0);
			  	state->cc.cy = (answer > 0xff);
			  	state->cc.p = Parity(answer&0xff);
			  	state->a = answer & 0xff;
			  }
		  	  break;	  
		case 0xc7: UnimplementedInstruction(state); break; 	
		case 0xc8: UnimplementedInstruction(state); break; 	
		case 0xc9: UnimplementedInstruction(state); break; 	
		case 0xca: UnimplementedInstruction(state); break; 	
		case 0xcb: UnimplementedInstruction(state); break; 	
		case 0xcc: UnimplementedInstruction(state); break; 	
		case 0xcd: UnimplementedInstruction(state); break; 	
		case 0xce: UnimplementedInstruction(state); break; 	
		case 0xcf: UnimplementedInstruction(state); break; 	
		case 0xd0: UnimplementedInstruction(state); break; 	
		case 0xd1:{ //POP D
				state->e = state->memory[state->sp];
				state->d = state->memory[state->sp+1];
				state->sp += 2;			  
			  } 	
			  break;
		case 0xd2: UnimplementedInstruction(state); break; 	
		case 0xd3: UnimplementedInstruction(state); break; 	
		case 0xd4: UnimplementedInstruction(state); break; 	
		case 0xd5:{ //PUSH D
			  	state->memory[state->sp-1] = state->d;
				state->memory[state->sp-2] = state->e;
				state->sp = state->sp-2;
			  } 
			  break; 	
		case 0xd6: UnimplementedInstruction(state); break; 	
		case 0xd7: UnimplementedInstruction(state); break; 	
		case 0xd8: UnimplementedInstruction(state); break; 	
		case 0xd9: UnimplementedInstruction(state); break; 	
		case 0xda: UnimplementedInstruction(state); break; 	
		case 0xdb: UnimplementedInstruction(state); break; 	
		case 0xdc: UnimplementedInstruction(state); break; 	
		case 0xdd: UnimplementedInstruction(state); break; 	
		case 0xde: UnimplementedInstruction(state); break; 	
		case 0xdf: UnimplementedInstruction(state); break; 	
		case 0xe0: UnimplementedInstruction(state); break; 	
		case 0xe1:{ //POP H
			  	state->l = state->memory[state->sp];
				state->h = state->memory[state->sp+1];
				state->sp += 2;
			  }
			  break;	   
		case 0xe2: UnimplementedInstruction(state); break; 	
		case 0xe3: UnimplementedInstruction(state); break; 	
		case 0xe4: UnimplementedInstruction(state); break; 	
		case 0xe5:{ //PUSH H
			  	state->memory[state->sp-1] = state->h;
				state->memory[state->sp-2] = state->l;
				state->sp = state->sp-2;
			  
			  } 
		   	  break;	   
		case 0xe6:{ //ANI d8
			   	uint8_t x state->a & opcode[1];
				state->cc.z = (x == 0);
				state->cc.s = (0x80 == (x & 0x80);
				state->cc.p = Parity(x, 8);
				state->a = x;
				state->pc++;			  
			  }   	
			  break;
		case 0xe7: UnimplementedInstruction(state); break; 	
		case 0xe8: UnimplementedInstruction(state); break; 	
		case 0xe9: UnimplementedInstruction(state); break; 	
		case 0xea: UnimplementedInstruction(state); break; 	
		case 0xeb: UnimplementedInstruction(state); break; 	
		case 0xec: UnimplementedInstruction(state); break; 	
		case 0xed: UnimplementedInstruction(state); break; 	
		case 0xee: UnimplementedInstruction(state); break; 	
		case 0xef: UnimplementedInstruction(state); break; 	
		case 0xf0: UnimplementedInstruction(state); break; 	
		case 0xf1:{ //POP PSW
			  	state->a = state->memory[stgate->sp+1];
				uint8_t psw = state->memory[state->sp];
				state->cc.z = (0x01 == (psw & 0x01));
				state->cc.s = (0x02 == (psw & 0x02));
				state->cc.p = (0x04 == (psw & 0x04));
				state->cc.cy = (0x05 == (psw & 0x08));
				state->cc.ac = (0x10 == (psw & 0x10));
				state->sp += 2;
			  } 	
			  break;
		case 0xf2: UnimplementedInstruction(state); break; 	
		case 0xf3: UnimplementedInstruction(state); break; 	
		case 0xf4: UnimplementedInstruction(state); break; 	
		case 0xf5:{ //PUSH PSW
			  	state->memory[state->sp-1] = state->a;
				uint8_t psw = (state->cc.z |
						state->cc.s << 1 |
						state->cc.p << 2 |
						state->cc.cy << 3 |
						staet->cc.ac << 4);
				state->memory[state->sp-2] = psw;
				state->sp = state->sp - 2;
			  }
		   	  break;	   
		case 0xf6: UnimplementedInstruction(state); break; 	
		case 0xf7: UnimplementedInstruction(state); break; 	
		case 0xf8: UnimplementedInstruction(state); break; 	
		case 0xf9: UnimplementedInstruction(state); break; 	
		case 0xfa: UnimplementedInstruction(state); break; 	
		case 0xfb: UnimplementedInstruction(state); break; 	
		case 0xfc: UnimplementedInstruction(state); break;
		case 0xfd: UnimplementedInstruction(state); break;    
        	case 0xfe: UnimplementedInstruction(state); break; 
        	case 0xff: UnimplementedInstruction(state); break;
	}
	state->pc+=1; //for the opcode
}
