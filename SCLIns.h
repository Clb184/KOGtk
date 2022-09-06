#pragma once

#define NOP                 0x30
#define SET					0x31

#define LOAD				0x40
#define RECT				0x41
#define ANIME				0x42
#define LOAD2				0x43
#define LOADEX				0x44
#define STOP				0x45

#define CALL				0x50
#define ATK					0x51
#define ESET				0x52
#define RET					0x53
#define ANM					0x54
#define FATK				0x55
#define ATKNP				0x56

#define MOV					0x57
#define ACC					0x58
#define ROL					0x59
#define WAITATOBJ			0x5a

#define PSE					0x5b
#define KILL				0x5c

#define MDMG				0x5d

#define CHILD				0x5e
#define CHGTASK				0x5f
#define PARENT				0x60

#define PMOV				0x61
#define PACC				0x62
#define PROL				0x63
#define PNOP				0x64

#define ATK2				0x65

#define EFC					0x66

#define TAMA				0x70
#define LASER				0x71
#define DEGE				0x72
#define DEGS				0x73
#define LLCHARGE			0x74
#define LLOPEN				0x75
#define LLCLOSE				0x76
#define HLASER				0x77
#define LSPHERE				0x78
#define RLASER				0x79
#define CROSS				0x7a
#define FLOWER				0x7b
#define GFIRE				0x7c
#define IONRING				0x7d

#define TALKMSG				0x90
#define TALKKEY				0x91
#define TALKNEWL			0x92
#define TALKWAIT			0x93

#define TASK				0xa0
#define TEXMODE				0xa1

#define PUSHR				0xc0
#define POPR				0xc1
#define MOVC				0xc2

#define PUSHC				0xc4
#define TJMP				0xc5
#define FJMP				0xc6
#define JMP					0xc7
#define OJMP				0xca
#define AJMP				0xcb
#define EXIT				0xcc
#define LPOP				0xcd
#define LJMP				0xce

#define ADD					0xd0
#define SUB					0xd1
#define MUL					0xd2
#define DIV					0xd3
#define MOD					0xd4
#define NEG					0xd5

#define SINL				0xd6
#define COSL				0xd7
#define RND					0xd8
#define ATAN				0xd9

#define EQUAL				0xda
#define NOTEQ				0xdb
#define GREAT				0xdc
#define LESS				0xdd
#define GREATEQ				0xde
#define LESSEQ				0xdf

#define MAX					0xe0
#define MIN					0xe1