#ifndef _opcodes_h_
#define _opcodes_h_

#define VAL8 ((int8_t) ip[0])
#define VAL16 ((int16_t) (ip[0] | (ip[1] << 8)))
#define VAL32 ((int32_t) (ip[0] | (ip[1] << 8) | (ip[2] << 16) | (ip[3] << 24)))
#define UVAL16 ((uint16_t) VAL16)
#define DUP *++sp = tos
#define DROP tos = *sp--
#define NIP --sp

#if defined(__APPLE__)
# define OS_ID 1
#else
# define OS_ID 0
#endif

#define INSTRUCTIONS(V) \
  V(0,   NOP,     "nop",      ) \
  V(1,   DEC,     "_dec",     DUP; tos.i = VAL32; ip += 4) \
  V(2,   HEX,     "_hex",     DUP; tos.u = (uint32_t) VAL32; ip += 4) \
  V(3,   STRING,  "_str",     DUP; tos.ip = ip + 2; \
                              DUP; tos.u = UVAL16 - 1; ip += 3 + tos.u) \
  V(4,   CALL,    "_call",    (++rp)->ip = ip + 2; ip += 2 + VAL16;) \
  V(5,   USER,    "_user",    DUP; tos.a = &up[UVAL16]; ip += 2) \
  V(6,   TICK,    "'",        DUP; tos.ip = ip + 2 + VAL16; ip += 2) \
  V(7,   IF,      "if",       if (sp->u) \
                              ((++rp)->ip = ip, ip = tos.ip); DROP; DROP) \
  V(8,   IFELSE,  "ifelse",   (++rp)->ip = ip; \
                              if (sp[-1].u) ip = sp->ip; \
                              else ip = tos.ip; DROP; DROP; DROP) \
  V(9,   FOR,     "for",      *++rp = tos; DROP; *++rp = tos; DROP) \
  V(10,  NEXT,    "next",     if (rp->u--) \
                              ((++rp)->ip = ip - 1, ip = rp[-2].ip); \
                              else rp -= 2) \
  V(11,  REPEAT,  "repeat",   (++rp)->u = 0; *++rp = tos; DROP) \
  V(12,  FOREVER, "forever",  (++rp)->ip = ip - 1; ip = rp[-1].ip) \
  V(13,  BRIF,    "brif",     if (tos.u) (rp -= 3, ip = (rp--)->ip); DROP) \
  V(14,  EXIT,    ";",        ip = (rp--)->ip) \
  V(15,  EXECUTE, "execute",  (++rp)->ip = ip; ip = tos.ip; DROP) \
  V(16,  LOAD,    "@",        tos = *tos.a) \
	V(17,  STORE,   "!",        *tos.a = *sp; --sp; DROP) \
  V(18,  CLOAD,   "c@",       tos.u = *tos.b) \
  V(19,  CSTORE,  "c!",       *tos.b = sp->u; --sp; DROP) \
  V(20,  SLOAD,   "s@",       tos.u = *tos.s) \
  V(21,  SSTORE,  "s!",       *tos.s = sp->u; --sp; DROP) \
  V(22,  LLOAD,   "l@",       tos.u = *tos.l) \
  V(23,  LSTORE,  "l!",       *tos.l = sp->u; --sp; DROP) \
  V(24,  PUSH,    ">r",       *++rp = tos; DROP) \
  V(25,  POP,     "r>",       DUP; tos = *rp--) \
  V(26,  TROS,    "r@",       DUP; tos = *rp) \
  V(27,  DROP,    "drop",     DROP) \
  V(28,  DUP,     "dup",      DUP) \
  V(29,  SWAP,    "swap",     t = tos; tos = *sp; *sp = t) \
  V(30,  OVER,    "over",     DUP; tos = sp[-1]) \
  V(31,  ZLESS,   "0<",       tos.u = tos.i < 0 ? -1 : 0) \
  V(32,  AND,     "and",      tos.u &= sp->u; NIP) \
  V(33,  OR,      "or",       tos.u |= sp->u; NIP) \
  V(34,  XOR,     "xor",      tos.u ^= sp->u; NIP) \
  V(35,  UMADD,   "um+",      t.u = tos.u + sp->u; \
                              tos.u = t.u < tos.u || \
                              t.u < sp->u; sp->u = t.u) \
  V(36,  ADD,     "+",        tos.u += sp->u; NIP) \
  V(37,  SUB,     "-",        tos.u = sp->u - tos.u; NIP) \
  V(38,  MUL,     "*",        tos.u *= sp->u; NIP) \
  V(39,  ADVANCE, "+!",       tos.a->u += sp->u; --sp; DROP) \
  V(40,  REGRESS, "-!",       tos.a->u -= sp->u; --sp; DROP) \
  V(41,  SCALE,   "*!",       tos.a->u *= sp->u; --sp; DROP) \
  V(42,  RDROP,   "rdrop",    --rp) \
  V(43,  LSHIFT,  "lshift",   tos.u = sp->u << tos.i; --sp) \
  V(44,  RSHIFT,  "rshift",   tos.u = sp->u >> tos.i; --sp) \
  V(45,  ARSHIFT, "arshift",  tos.i = sp->i >> tos.i; --sp) \
  V(46,  TWODIV,  "2/",       tos.i = tos.i >> 1) \
  V(47,  TWOMUL,  "2*",       tos.i = tos.i << 1) \
  V(64,  CALL0,   "call0",    tos = tos.f0()) \
  V(65,  CALL1,   "call1",    tos = tos.f1(sp[0]); sp -= 1) \
  V(66,  CALL2,   "call2",    tos = tos.f2(sp[-1],sp[0]); sp -= 2) \
  V(67,  CALL3,   "call3",    tos = tos.f3(sp[-2],sp[-1],sp[0]); sp -= 3) \
  V(68,  CALL4,   "call4",    tos = tos.f4(sp[-3],sp[-2],sp[-1], \
                              sp[0]); sp -= 4) \
  V(69,  CALL5,   "call5",    tos = tos.f5(sp[-4],sp[-3],sp[-2],sp[-1], \
                              sp[0]); sp -= 5) \
  V(70,  CALL6,   "call6",    tos = tos.f6(sp[-5],sp[-4],sp[-3],sp[-2],sp[-1], \
                              sp[0]); sp -= 6) \
  V(71,  CALL7,   "call7",    tos = tos.f7(sp[-6],sp[-5],sp[-4],sp[-3],sp[-2], \
                              sp[-1],sp[0]); sp -= 7) \
  V(72,  CALL8,   "call8",    tos = tos.f8(sp[-7],sp[-6],sp[-5],sp[-4],sp[-3], \
                              sp[-2],sp[-1],sp[0]); sp -= 8) \
  V(96,  SPLOAD,   "sp@",     DUP; tos.a = sp) \
  V(97,  SPSTORE,  "sp!",     sp = tos.a; DROP) \
  V(98,  RPLOAD,   "rp@",     DUP; tos.a = rp) \
  V(99,  RPSTORE,  "rp!",     rp = tos.a; DROP) \
  V(100, UPLOAD,   "up@",     DUP; tos.a = up) \
  V(101, UPSTORE,  "up!",     up = tos.a; DROP) \
  V(102, CELL,     "cell",    DUP; tos.u = sizeof(cell_t)) \
  V(103, CELLS,    "cells",   tos.u *= sizeof(cell_t)) \
  V(104, CELLADD,  "cell+",   tos.u += sizeof(cell_t)) \
  V(130, DLSYM,    "dlsym",   tos.v = dlsym(tos.v, sp->v); NIP) \
  V(131, OS,       "os",      DUP; tos.u = OS_ID) \
  V(192, I,        "i",       DUP; tos = *rp) \
  V(193, J,        "j",       DUP; tos = rp[-2]) \
  V(200, CHECK,    "check",   fprintf(stderr, "ck: %d\n", (int)tos.i); DROP) \
  V(201, DASH,     "--",      ) \
  V(252, COMMENT,  "_comment",ip += UVAL16 + 2) \
  V(253, DEFUSER,  "_defuser",ip += UVAL16 + 2) \
  V(254, DEFINE,   "_define", ip += UVAL16 + 2) \

enum {
#define V(num, cname, _, _junk) OP_##cname = num,
  INSTRUCTIONS(V)
#undef V
};

#endif
