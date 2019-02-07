/*\
 * vim: tw=0: cindent: sw=3: ts=3: sts=3: noet: list
 *
 * RISC-V Instruction Set Simulator, developed for the SoCLib Project
 * Copyright (C) 2018  ##LAB_REMOVED_FOR_BLIND_REVIEW##
 *
 * Contributing authors:
 *     ##AUTHOR_REMOVED_FOR_BLIND_REVIEW##
 *     Bootstrapped the developement
 *
 * Interprets only the RV32IM instruction subset in machine mode, which
 * is good enough for the goal I'm pursuing now, but not for booting a
 * full fledge operating system.
 *
 * As opposed to the MIPS I formerly did, I choose to build the
 * instruction fields inside the ‘switch’ itself, given the complexity
 * of doing it more or less twice.
 * The code is thus a mess, and vim folders help reading it :)
 * This brutal yet efficient approach is generaly used, e.g. in Qemu, and
 * is justified in detail in the paper: "Branch Prediction and the
 * Performance of Interpreters-Don't Trust Folklore", published at
 * CGO 2015 by my esteemed colleagues Erven Rohou, Bharath Narasimha Swamy,
 * and André Seznec.
\*/

/*\
 * Dump instructions as they are executed, following more or less the format
 * that objdump produces with options ‘--disassembler-options=no-aliases,numeric’
\*/
#define RV32I_DISAS 0

#include <stdarg.h>
#include <string>
#include <math.h>
#include <fenv.h>
#pragma STDC FENV_ACCESS ON
#include "rv32im.h"
#include "soclib_endian.h"
#include "arithmetics.h"

using namespace std;

#if RV32I_DISAS == 1
/* A bit brutal but life ain't simple */
typedef struct csr {
	uint16_t    code;
	uint16_t    access;
	const char *name;
} csr;

static csr csr_array[] = {
   {0x000, 0b0011, "ustatus"},
   {0x004, 0b0011, "uie"},
   {0x005, 0b0011, "utvec"},
   {0x040, 0b0011, "uscratch"},
   {0x041, 0b0011, "uepc"},
   {0x042, 0b0011, "ucause"},
   {0x043, 0b0011, "utval"},
   {0x044, 0b0011, "uip"},
   {0x001, 0b0011, "fflags"},
   {0x002, 0b0011, "frm"},
   {0x003, 0b0011, "fcsr"},
   {0xc00, 0b0010, "cycle"},
   {0xc01, 0b0010, "time"},
   {0xc02, 0b0010, "instret"},
   {0xc03, 0b0010, "hpmcounter3"},
   {0xc04, 0b0010, "hpmcounter4"},
   {0xc05, 0b0010, "hpmcounter5"},
   {0xc06, 0b0010, "hpmcounter6"},
   {0xc07, 0b0010, "hpmcounter7"},
   {0xc08, 0b0010, "hpmcounter8"},
   {0xc09, 0b0010, "hpmcounter9"},
   {0xc0a, 0b0010, "hpmcounter10"},
   {0xc0b, 0b0010, "hpmcounter11"},
   {0xc0c, 0b0010, "hpmcounter12"},
   {0xc0d, 0b0010, "hpmcounter13"},
   {0xc0e, 0b0010, "hpmcounter14"},
   {0xc0f, 0b0010, "hpmcounter15"},
   {0xc10, 0b0010, "hpmcounter16"},
   {0xc11, 0b0010, "hpmcounter17"},
   {0xc12, 0b0010, "hpmcounter18"},
   {0xc13, 0b0010, "hpmcounter19"},
   {0xc14, 0b0010, "hpmcounter20"},
   {0xc15, 0b0010, "hpmcounter21"},
   {0xc16, 0b0010, "hpmcounter22"},
   {0xc17, 0b0010, "hpmcounter23"},
   {0xc18, 0b0010, "hpmcounter24"},
   {0xc19, 0b0010, "hpmcounter25"},
   {0xc1a, 0b0010, "hpmcounter26"},
   {0xc1b, 0b0010, "hpmcounter27"},
   {0xc1c, 0b0010, "hpmcounter28"},
   {0xc1d, 0b0010, "hpmcounter29"},
   {0xc1e, 0b0010, "hpmcounter30"},
   {0xc1f, 0b0010, "hpmcounter31"},
   {0xc80, 0b0010, "cycleh"},
   {0xc81, 0b0010, "timeh"},
   {0xc82, 0b0010, "instreth"},
   {0xc83, 0b0010, "hpmcounter3h"},
   {0xc84, 0b0010, "hpmcounter4h"},
   {0xc85, 0b0010, "hpmcounter5h"},
   {0xc86, 0b0010, "hpmcounter6h"},
   {0xc87, 0b0010, "hpmcounter7h"},
   {0xc88, 0b0010, "hpmcounter8h"},
   {0xc89, 0b0010, "hpmcounter9h"},
   {0xc8a, 0b0010, "hpmcounter10h"},
   {0xc8b, 0b0010, "hpmcounter11h"},
   {0xc8c, 0b0010, "hpmcounter12h"},
   {0xc8d, 0b0010, "hpmcounter13h"},
   {0xc8e, 0b0010, "hpmcounter14h"},
   {0xc8f, 0b0010, "hpmcounter15h"},
   {0xc90, 0b0010, "hpmcounter16h"},
   {0xc91, 0b0010, "hpmcounter17h"},
   {0xc92, 0b0010, "hpmcounter18h"},
   {0xc93, 0b0010, "hpmcounter19h"},
   {0xc94, 0b0010, "hpmcounter20h"},
   {0xc95, 0b0010, "hpmcounter21h"},
   {0xc96, 0b0010, "hpmcounter22h"},
   {0xc97, 0b0010, "hpmcounter23h"},
   {0xc98, 0b0010, "hpmcounter24h"},
   {0xc99, 0b0010, "hpmcounter25h"},
   {0xc9a, 0b0010, "hpmcounter26h"},
   {0xc9b, 0b0010, "hpmcounter27h"},
   {0xc9c, 0b0010, "hpmcounter28h"},
   {0xc9d, 0b0010, "hpmcounter29h"},
   {0xc9e, 0b0010, "hpmcounter30h"},
   {0xc9f, 0b0010, "hpmcounter31h"},
   {0x100, 0b0111, "sstatus"},
   {0x102, 0b0111, "sedeleg"},
   {0x103, 0b0111, "sideleg"},
   {0x104, 0b0111, "sie"},
   {0x105, 0b0111, "stvec"},
   {0x106, 0b0111, "scounteren"},
   {0x140, 0b0111, "sscratch"},
   {0x141, 0b0111, "sepc"},
   {0x142, 0b0111, "scause"},
   {0x143, 0b0111, "stval"},
   {0x144, 0b0111, "sip"},
   {0x180, 0b0111, "satp"},
   {0xf11, 0b1010, "mvendorid"},
   {0xf12, 0b1010, "marchid"},
   {0xf13, 0b1010, "mimpid"},
   {0xf14, 0b1010, "mhartid"},
   {0x300, 0b1011, "mstatus"},
   {0x301, 0b1011, "misa"},
   {0x302, 0b1011, "medeleg"},
   {0x303, 0b1011, "mideleg"},
   {0x304, 0b1011, "mie"},
   {0x305, 0b1011, "mtvec"},
   {0x306, 0b1011, "mcounteren"},
   {0x340, 0b1011, "mscratch"},
   {0x341, 0b1011, "mepc"},
   {0x342, 0b1011, "mcause"},
   {0x343, 0b1011, "mtval"},
   {0x344, 0b1011, "mip"},
   {0x3a0, 0b1011, "pmpcfg0"},
   {0x3a1, 0b1011, "pmpcfg1"},
   {0x3a2, 0b1011, "pmpcfg2"},
   {0x3a3, 0b1011, "pmpcfg3"},
   {0x3b0, 0b1011, "pmpaddr0"},
   {0x3b1, 0b1011, "pmpaddr1"},
   {0x3b2, 0b1011, "pmpaddr2"},
   {0x3b3, 0b1011, "pmpaddr3"},
   {0x3b4, 0b1011, "pmpaddr4"},
   {0x3b5, 0b1011, "pmpaddr5"},
   {0x3b6, 0b1011, "pmpaddr6"},
   {0x3b7, 0b1011, "pmpaddr7"},
   {0x3b8, 0b1011, "pmpaddr8"},
   {0x3b9, 0b1011, "pmpaddr9"},
   {0x3ba, 0b1011, "pmpaddr10"},
   {0x3bb, 0b1011, "pmpaddr11"},
   {0x3bc, 0b1011, "pmpaddr12"},
   {0x3bd, 0b1011, "pmpaddr13"},
   {0x3be, 0b1011, "pmpaddr14"},
   {0x3bf, 0b1011, "pmpaddr15"},
   {0xb00, 0b1011, "mcycle"},
   {0xb02, 0b1011, "minstret"},
   {0xb03, 0b1011, "mhpmcounter3"},
   {0xb04, 0b1011, "mhpmcounter4"},
   {0xb05, 0b1011, "mhpmcounter5"},
   {0xb06, 0b1011, "mhpmcounter6"},
   {0xb07, 0b1011, "mhpmcounter7"},
   {0xb08, 0b1011, "mhpmcounter8"},
   {0xb09, 0b1011, "mhpmcounter9"},
   {0xb0a, 0b1011, "mhpmcounter10"},
   {0xb0b, 0b1011, "mhpmcounter11"},
   {0xb0c, 0b1011, "mhpmcounter12"},
   {0xb0d, 0b1011, "mhpmcounter13"},
   {0xb0e, 0b1011, "mhpmcounter14"},
   {0xb0f, 0b1011, "mhpmcounter15"},
   {0xb10, 0b1011, "mhpmcounter16"},
   {0xb11, 0b1011, "mhpmcounter17"},
   {0xb12, 0b1011, "mhpmcounter18"},
   {0xb13, 0b1011, "mhpmcounter19"},
   {0xb14, 0b1011, "mhpmcounter20"},
   {0xb15, 0b1011, "mhpmcounter21"},
   {0xb16, 0b1011, "mhpmcounter22"},
   {0xb17, 0b1011, "mhpmcounter23"},
   {0xb18, 0b1011, "mhpmcounter24"},
   {0xb19, 0b1011, "mhpmcounter25"},
   {0xb1a, 0b1011, "mhpmcounter26"},
   {0xb1b, 0b1011, "mhpmcounter27"},
   {0xb1c, 0b1011, "mhpmcounter28"},
   {0xb1d, 0b1011, "mhpmcounter29"},
   {0xb1e, 0b1011, "mhpmcounter30"},
   {0xb1f, 0b1011, "mhpmcounter31"},
   {0xb80, 0b1011, "mcycleh"},
   {0xb82, 0b1011, "minstreth"},
   {0xb83, 0b1011, "mhpmcounter3h"},
   {0xb84, 0b1011, "mhpmcounter4h"},
   {0xb85, 0b1011, "mhpmcounter5h"},
   {0xb86, 0b1011, "mhpmcounter6h"},
   {0xb87, 0b1011, "mhpmcounter7h"},
   {0xb88, 0b1011, "mhpmcounter8h"},
   {0xb89, 0b1011, "mhpmcounter9h"},
   {0xb8a, 0b1011, "mhpmcounter10h"},
   {0xb8b, 0b1011, "mhpmcounter11h"},
   {0xb8c, 0b1011, "mhpmcounter12h"},
   {0xb8d, 0b1011, "mhpmcounter13h"},
   {0xb8e, 0b1011, "mhpmcounter14h"},
   {0xb8f, 0b1011, "mhpmcounter15h"},
   {0xb90, 0b1011, "mhpmcounter16h"},
   {0xb91, 0b1011, "mhpmcounter17h"},
   {0xb92, 0b1011, "mhpmcounter18h"},
   {0xb93, 0b1011, "mhpmcounter19h"},
   {0xb94, 0b1011, "mhpmcounter20h"},
   {0xb95, 0b1011, "mhpmcounter21h"},
   {0xb96, 0b1011, "mhpmcounter22h"},
   {0xb97, 0b1011, "mhpmcounter23h"},
   {0xb98, 0b1011, "mhpmcounter24h"},
   {0xb99, 0b1011, "mhpmcounter25h"},
   {0xb9a, 0b1011, "mhpmcounter26h"},
   {0xb9b, 0b1011, "mhpmcounter27h"},
   {0xb9c, 0b1011, "mhpmcounter28h"},
   {0xb9d, 0b1011, "mhpmcounter29h"},
   {0xb9e, 0b1011, "mhpmcounter30h"},
   {0xb9f, 0b1011, "mhpmcounter31h"},
   {0x323, 0b1011, "mhpmevent3"},
   {0x324, 0b1011, "mhpmevent4"},
   {0x325, 0b1011, "mhpmevent5"},
   {0x326, 0b1011, "mhpmevent6"},
   {0x327, 0b1011, "mhpmevent7"},
   {0x328, 0b1011, "mhpmevent8"},
   {0x329, 0b1011, "mhpmevent9"},
   {0x32a, 0b1011, "mhpmevent10"},
   {0x32b, 0b1011, "mhpmevent11"},
   {0x32c, 0b1011, "mhpmevent12"},
   {0x32d, 0b1011, "mhpmevent13"},
   {0x32e, 0b1011, "mhpmevent14"},
   {0x32f, 0b1011, "mhpmevent15"},
   {0x330, 0b1011, "mhpmevent16"},
   {0x331, 0b1011, "mhpmevent17"},
   {0x332, 0b1011, "mhpmevent18"},
   {0x333, 0b1011, "mhpmevent19"},
   {0x334, 0b1011, "mhpmevent20"},
   {0x335, 0b1011, "mhpmevent21"},
   {0x336, 0b1011, "mhpmevent22"},
   {0x337, 0b1011, "mhpmevent23"},
   {0x338, 0b1011, "mhpmevent24"},
   {0x339, 0b1011, "mhpmevent25"},
   {0x33a, 0b1011, "mhpmevent26"},
   {0x33b, 0b1011, "mhpmevent27"},
   {0x33c, 0b1011, "mhpmevent28"},
   {0x33d, 0b1011, "mhpmevent29"},
   {0x33e, 0b1011, "mhpmevent30"},
   {0x33f, 0b1011, "mhpmevent31"},
   {0x7a0, 0b1011, "tselect"},
   {0x7a1, 0b1011, "tdata1"},
   {0x7a2, 0b1011, "tdata2"},
   {0x7a3, 0b1011, "tdata3"},
   {0x7b0, 0b1111, "dcsr"},
   {0x7b1, 0b1111, "dpc"},
   {0x7b2, 0b1111, "dscratch"},
};

static const char *csr_name(int n)
{
   for (size_t i = 0; i < sizeof(csr_array)/sizeof(csr_array[0]); i++) {
      if (csr_array[i].code == n)
         return csr_array[i].name;
   }
   return "xxxx";
}

static const char *rounding_name(int n)
{
	switch (n) {
		case 0b000: return "rne";
		case 0b001: return "rtz";
		case 0b010: return "rdn";
		case 0b011: return "rup";
		case 0b100: return "rmm";
		case 0b101: return "ror";
		case 0b110: return "ror";
		case 0b111: return "dyn";
	}
	return "xxx"; // Well, never reached actually
}

/*\
 * Using a file so as to avoid polluting the uart output
\*/
static void asmout(FILE *f, uint32_t pc, uint32_t insn, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt); 
	fprintf(f, "%08x	%08x	", pc, insn);
	vfprintf(f, fmt, args);
	fprintf(f, "\n");
	va_end(args);
}

// Macros to output disassembled instructions
#define asm_str             const char *s = 0
#define asm_ins(x)          s = (x)
#define asm_out(format,...) asmout(dump, r_pc, m_ir, format, __VA_ARGS__)
#else
#define asm_str
#define asm_ins(x)
#define asm_out(format,...)
#endif

typedef union {
	int32_t  s;
	uint32_t u;
	float    f;
} suf_t;

/*\
 * Rounding modes for float
\*/

enum {
	rne, rtz, rdn, rup, rmm, rxx, ryy, dyn
};

static int rounding[] = {
	[rne] = FE_TONEAREST,
	[rtz] = FE_TOWARDZERO,
	[rdn] = FE_DOWNWARD,
	[rup] = FE_UPWARD,
	[rmm] = FE_TONEAREST, // Best I could find
	[rxx] = -1,
	[ryy] = -1,
	[dyn] = -2
};


/*\
 * Memory accesses to fit the current SoCLib Iss strategy
 * The type, addr, dest and wdata fields are inherited from Iss
\*/

#define load(rega, type, sign, addr)               \
	do {                                            \
		r_mem_req      = true;                       \
		r_mem_type     = type;                       \
		r_mem_unsigned = sign;                       \
		r_mem_addr     = addr;                       \
		r_mem_dest     = rega;                       \
	} while (0)

#define store(type, addr, data)                    \
	do {                                            \
		r_mem_req   = true;                          \
		r_mem_type  = type;                          \
		r_mem_addr  = addr;                          \
		r_mem_wdata = data;                          \
	} while (0)

/*\
 * Macros to extract fields depending on instruction X-type
 * so as to avoid duplicating the code too much
\*/

#define decode_csr_type(rd, csr, val)              \
do {                                               \
	rd  = (m_ir >>  7) & 0x1f;                      \
	csr = (m_ir >> 20) & 0xfffff;                   \
	val = (m_ir >> 15) & 0x1f;                      \
} while (0)

#define decode_u_type(rd, imm)                     \
do {                                               \
	rd  = (m_ir >>  7) & 0x1f;                      \
	imm = m_ir & 0xfffff000;                        \
} while (0)

#define decode_j_type(rd, imm)                     \
do {                                               \
	rd  = (m_ir >>  7) & 0x1f;                      \
	imm = ((int32_t)(m_ir & 0x80000000) >> 11)      \
			| (m_ir & 0x000ff000)                     \
			| ((m_ir & 0x00100000) >> 9)              \
			| ((m_ir & 0x7fe00000) >> 20);            \
} while (0)

#define decode_i_type(rd, rs1, imm)                \
do {                                               \
	rd  = (m_ir >>  7) & 0x1f;                      \
	rs1 = (m_ir >> 15) & 0x1f;                      \
	imm = (int32_t)(m_ir & 0xfff00000) >> 20;       \
} while (0)

#define decode_b_type(rs1, rs2, imm)               \
do {                                               \
	rs1 = (m_ir >> 15) & 0x1f;                      \
	rs2 = (m_ir >> 20) & 0x1f;                      \
	imm = ((int32_t)(m_ir & 0x80000000) >> 19)      \
			| ((m_ir & 0x00000080) << 4)              \
			| ((m_ir & 0x00000f00) >> 7)              \
			| ((m_ir & 0x7e000000) >> 20);            \
} while (0)

#define decode_s_type(rs1, rs2, imm)               \
do {                                               \
	rs1 = (m_ir >> 15) & 0x1f;                      \
	rs2 = (m_ir >> 20) & 0x1f;                      \
	imm = ((int32_t)(m_ir & 0xfe000000) >> 20)      \
			| ((m_ir & 0x00000f80) >> 7);             \
} while (0)

#define decode_r_type(rd, rs1, rs2)                \
do {                                               \
	rd  = (m_ir >>  7) & 0x1f;                      \
	rs1 = (m_ir >> 15) & 0x1f;                      \
	rs2 = (m_ir >> 20) & 0x1f;                      \
} while (0)

#define decode_r4_type(rd, rs1, rs2, rs3, fmt, rm) \
do {                                               \
	rd  = (m_ir >>  7) & 0x1f;                      \
	rs1 = (m_ir >> 15) & 0x1f;                      \
	rs2 = (m_ir >> 20) & 0x1f;                      \
	rs3 = (m_ir >> 27) & 0x1f;                      \
	fmt = (m_ir >> 25) & 0x03;                      \
	rm  = (m_ir >> 12) & 0x07;                      \
} while (0)


namespace soclib { namespace common {

	namespace {

		static inline std::string mkname(uint32_t no)
		{
				char tmp[32];
				snprintf(tmp, 32, "rv32im_iss%d", (int)no);
				return std::string(tmp);
		}
	}

	Rv32imIss::Rv32imIss(uint32_t ident)
		: Iss(mkname(ident), ident)
	{

		char *s = getenv("ISSLOG");
		dump = stderr;

		/* Fallback to stderr for instruction logging in case opening a file
		 * for writing is not possible */
		if (s != NULL) {
			dump = fopen(s, "w+");
			if (dump == NULL)
				dump = stderr;
		}

		r_csr[csr_mhartid]   = ident;
		r_pc                 = RESET_VECTOR;
		r_csr[csr_mvendorid] = 0x00bada55;
		r_csr[csr_misa]      = 0x40001100; /* rv32im */
		r_csr[csr_mimpid]    = 0x02144906; /* soclibvz */
	}

	void Rv32imIss::reset(void)
	{
		r_pc                 = RESET_VECTOR;
		r_dbe                = false;
		m_ibe                = false;
		m_dbe                = false;
		r_mem_req            = false;
		r_gpr[0]             = 0;
		r_csr[csr_mstatus]   = 0x00001800; /* boot in machine mode */
		r_csr[csr_mcause]    = 0;
		r_csr[csr_mcycle]    = 0;
		r_csr[csr_mcycleh]   = 0;
		r_csr[csr_minstret]  = 0;
		r_csr[csr_minstreth] = 0;
		m_ir                 = 0x00000013; /* addi x0, x0, 0 */
	};

	int Rv32imIss::cpuCauseToSignal(uint32_t cause) const
	{
		switch (cause) {
			case ILLEGAL_INSTRUCTION_TRAP:
				return 4; // Illegal instruction
			case LOAD_ACCESS_FAULT_TRAP:
			case STORE_AMO_ACCESS_FAULT_TRAP:
			case INSTRUCTION_ACCESS_FAULT_TRAP:
				return 5; // Trap (nothing better)
			default:
				return 5; // GDB SIGTRAP
		}
	}

	void Rv32imIss::setDataResponse(bool error, uint32_t data)
	{
		m_dbe = error;
		r_mem_req = false;

		if (error) return;

		switch (r_mem_type) {
			case WRITE_BYTE :
			case WRITE_HALF :
			case WRITE_WORD :
			case LINE_INVAL :
				break;
			case READ_BYTE:
				*r_mem_dest = r_mem_unsigned
									? data & 0xff
									: (int32_t)((data & 0xff) << 24) >> 24;
				break;
			case READ_HALF:
				*r_mem_dest = r_mem_unsigned
									? data & 0xffff
									: (int32_t)((data & 0xffff) << 16) >> 16;
				break;
			case READ_WORD:
				*r_mem_dest = data;
				break;
			case READ_LINKED :
				fprintf(stderr, "Unhandled READ_LINKED request\n");
				exit(EXIT_FAILURE);
				break;
			case STORE_COND:
				fprintf(stderr, "Unhandled STORE_COND request\n");
				exit(EXIT_FAILURE);
				break;
			default:
				fprintf(stderr, "Unhandled memory access request\n");
				exit(EXIT_FAILURE);
				break;
		}
	}

	void Rv32imIss::step(void)
	{
		asm_str;
		/*\
		 * Initialization not really necessary, but so catches up some errors
		\*/
		bool branch = 0xdeadbeef;
		int  next_pc = 0xdeadbeef;

		/*\
		 * Local variable used to build the value send on the interconnect.
		 * It looks like endianness and byte enable are misteriously
		 * interpreted, so each byte or half world is replicated to
		 * avoid headaches
		\*/

		uint32_t  data;
		uint32_t  addr;
		DataAccessType  access;
		bool      unsign;

		/*\
		 * Helper variables representing the instruction field, to ease
		 * writing (and most reading in a while ago) the instruction
		 * execution code.
		\*/
		int       rd;
		int       rs1;
		int       rs2;
		int       imm;
		int       csr;
		/*\
		 * Floating point
		\*/
		int       rs3;
		int       fmt;
		int       rm;
		suf_t     suf;
		suf_t     fus;
		/*\
		 * Temporary
		\*/
		uint32_t  reg;

		/*\
		 * Setting exceptions
		 * FIXME: Check the priority order in the specs
		\*/
		bool exception = false;

		if (m_ibe) {
			r_csr[csr_mcause] = INSTRUCTION_ACCESS_FAULT_TRAP;
			exception         = true;
		}

		if (m_dbe) {
			r_csr[csr_mcause] = LOAD_ACCESS_FAULT_TRAP;
			r_csr[csr_mtval]  = r_mem_addr;
			exception         = true;
		}

		if (r_dbe) {
			r_csr[csr_mcause] = STORE_AMO_ACCESS_FAULT_TRAP;
			r_csr[csr_mtval]  = r_mem_addr;
			r_dbe             = false;
			exception         = true;
		}

		/*\
		 * Check for exceptions and interruptions
		 * Assume everything runs in machine mode for now
		\*/
		if (exception) {
			r_csr[csr_mepc] = r_pc;
			r_pc = r_csr[csr_mtvec];
			r_csr[csr_mstatus] |= 0xf0c;
		} else if ((r_csr[csr_mip] & r_csr[csr_mie]) && (r_csr[csr_mstatus] & 0x8)) {
			uint32_t irqs = r_csr[csr_mip] & r_csr[csr_mie];
			/* Priority as defined section 3.1.14 */
			if (irqs & 0x800) 
				r_csr[csr_mcause] = MACHINE_EXTERNAL_INTERRUPT_TRAP;
			else if (irqs & 0x008) 
				r_csr[csr_mcause] = MACHINE_SOFTWARE_INTERRUPT_TRAP;
			else if (irqs & 0x080) 
				r_csr[csr_mcause] = MACHINE_TIMER_INTERRUPT_TRAP;
			else {
				fprintf(stderr, "Unhandled interrupt trap = 0x%03x\n", irqs);
			}
			r_csr[csr_mepc] = r_pc;
			if ((r_csr[csr_mtvec] & 0b11) == 0)
				r_pc = r_csr[csr_mtvec];
			else if ((r_csr[csr_mtvec] & 0b11) == 1)
				r_pc = r_csr[csr_mtvec] + (r_csr[csr_mcause] << 2);
			else {
				fprintf(stderr, "Unknown mtvec mode 0b%c%c\n",
							'0' + (r_csr[csr_mtvec] & 0b10),
							'0' + (r_csr[csr_mtvec] & 0b01));
			}
			r_csr[csr_mstatus] |= 0x00001800;  // Previous mode was machine mode
			r_csr[csr_mstatus] |= ((r_csr[csr_mstatus] & 0x8) << 4); // Copy in stack
			r_csr[csr_mstatus] &= ~0x8; // Mask current
		} else {
			/*\
			 * Decode and execute instructions
			\*/
			/*\
			 * risc-v encodings:
			 * shamelessly extracted from the doc using pdftotext -f 150 -l 150 -layout riscv-spec.pdf
															RV32I Base Instruction Set
			 *    31         27   26 25 24         20   19         15   14    12   11      7     6             0
			 *            funct7             rs2             rs1        funct3        rd             opcode        R-type
			 *                 imm[11:0]                     rs1        funct3        rd             opcode        I-type
			 *          imm[11:5]           rs2              rs1        funct3      imm[4:0]         opcode        S-type
			 *         imm[12|10:5]         rs2              rs1        funct3     imm[4:1|11]       opcode        B-type
			 *                          imm[31:12]                                    rd             opcode        U-type
			 *                      imm[20|10:1|11|19:12]                             rd             opcode        J-type
			 *
			 *                                RV32I Base Instruction Set
			 *                            imm[31:12]                        rd                       0110111       LUI
			 *                            imm[31:12]                        rd                       0010111       AUIPC
			 *                       imm[20|10:1|11|19:12]                  rd                       1101111       JAL
			 *                  imm[11:0]                  rs1   000        rd                       1100111       JALR
			 *         imm[12|10:5]           rs2          rs1   000    imm[4:1|11]                  1100011       BEQ
			 *         imm[12|10:5]           rs2          rs1   001    imm[4:1|11]                  1100011       BNE
			 *         imm[12|10:5]           rs2          rs1   100    imm[4:1|11]                  1100011       BLT
			 *         imm[12|10:5]           rs2          rs1   101    imm[4:1|11]                  1100011       BGE
			 *         imm[12|10:5]           rs2          rs1   110    imm[4:1|11]                  1100011       BLTU
			 *         imm[12|10:5]           rs2          rs1   111    imm[4:1|11]                  1100011       BGEU
			 *                  imm[11:0]                  rs1   000        rd                       0000011       LB
			 *                  imm[11:0]                  rs1   001        rd                       0000011       LH
			 *                  imm[11:0]                  rs1   010        rd                       0000011       LW
			 *                  imm[11:0]                  rs1   100        rd                       0000011       LBU
			 *                  imm[11:0]                  rs1   101        rd                       0000011       LHU
			 *           imm[11:5]            rs2          rs1   000     imm[4:0]                    0100011       SB
			 *           imm[11:5]            rs2          rs1   001     imm[4:0]                    0100011       SH
			 *           imm[11:5]            rs2          rs1   010     imm[4:0]                    0100011       SW
			 *                  imm[11:0]                  rs1   000        rd                       0010011       ADDI
			 *                  imm[11:0]                  rs1   010        rd                       0010011       SLTI
			 *                  imm[11:0]                  rs1   011        rd                       0010011       SLTIU
			 *                  imm[11:0]                  rs1   100        rd                       0010011       XORI
			 *                  imm[11:0]                  rs1   110        rd                       0010011       ORI
			 *                  imm[11:0]                  rs1   111        rd                       0010011       ANDI
			 *            0000000           shamt          rs1   001        rd                       0010011       SLLI
			 *            0000000           shamt          rs1   101        rd                       0010011       SRLI
			 *            0100000           shamt          rs1   101        rd                       0010011       SRAI
			 *            0000000             rs2          rs1   000        rd                       0110011       ADD
			 *            0100000             rs2          rs1   000        rd                       0110011       SUB
			 *            0000000             rs2          rs1   001        rd                       0110011       SLL
			 *            0000000             rs2          rs1   010        rd                       0110011       SLT
			 *            0000000             rs2          rs1   011        rd                       0110011       SLTU
			 *            0000000             rs2          rs1   100        rd                       0110011       XOR
			 *            0000000             rs2          rs1   101        rd                       0110011       SRL
			 *            0100000             rs2          rs1   101        rd                       0110011       SRA
			 *            0000000             rs2          rs1   110        rd                       0110011       OR
			 *            0000000             rs2          rs1   111        rd                       0110011       AND
			 *           fm          pred       succ     00000   000      00000                      0001111       FENCE
			 *          0000        0000        0000     00000   001      00000                      0001111       FENCE.I
			 *                000000000000               00000   000      00000                      1110011       ECALL
			 *                000000000001               00000   000      00000                      1110011       EBREAK
			 *                     csr                     rs1   001        rd                       1110011       CSRRW
			 *                     csr                     rs1   010        rd                       1110011       CSRRS
			 *                     csr                     rs1   011        rd                       1110011       CSRRC
			 *                     csr                   zimm    101        rd                       1110011       CSRRWI
			 *                     csr                   zimm    110        rd                       1110011       CSRRSI
			 *                     csr                   zimm    111        rd                       1110011       CSRRCI
			 *
			 *                                RV32M Standard Extension
			 *      0000001                   rs2      rs1      000                     rd           0110011       MUL
			 *      0000001                   rs2      rs1      001                     rd           0110011       MULH
			 *      0000001                   rs2      rs1      010                     rd           0110011       MULHSU
			 *      0000001                   rs2      rs1      011                     rd           0110011       MULHU
			 *      0000001                   rs2      rs1      100                     rd           0110011       DIV
			 *      0000001                   rs2      rs1      101                     rd           0110011       DIVU
			 *      0000001                   rs2      rs1      110                     rd           0110011       REM
			 *      0000001                   rs2      rs1      111                     rd           0110011       REMU
			 *
			 * I am afraid I have to add the ‘compressed’ instruction set also, but I'll wait a bit for that.
			 * Note that 32/64 bit insns opcodes end in 11, the 00/01/10 seem to be reserved to the compressed instruction
			 * set, and are named C0/C1/C2 in the document.
			\*/

			switch (m_ir & 0x7f) {
				case 0b0110111: // U-type LUI rd,imm
					decode_u_type(rd, imm);
					asm_out("lui	x%d,0x%x", rd, (uint32_t)imm >> 12);
					r_gpr[rd] = imm;
					next_pc = r_pc + 4;
					break;
				case 0b0010111: // U-type AUIPC rd, imm
					decode_u_type(rd, imm);
					asm_out("auipc	x%d,0x%x", rd, (uint32_t)imm >> 12);
					r_gpr[rd] = r_pc + imm;
					next_pc = r_pc + 4;
					break;
				case 0b1101111: // J-type JAL rd, imm
					decode_j_type(rd, imm);
					asm_out("jal	x%d,0x%x", rd, r_pc + imm);
					r_gpr[rd] = r_pc + 4;
					next_pc   = r_pc + imm;
					break;
				case 0b1100111: // I-type JALR
					decode_i_type(rd, rs1, imm);
					if (imm == 0) {
						asm_out("jalr	x%d,0(x%d)", rd, rs1);
					} else {
						asm_out("jalr	x%d,x%d,0x%x", rd, rs1, r_pc + imm);
					}
					next_pc   = (r_gpr[rs1] + imm) & ~1; // Lower bit must be set to 0
					r_gpr[rd] = r_pc + 4;
					break;
				case 0b1100011: // B-type
					decode_b_type(rs1, rs2, imm);
					switch ((m_ir >> 12) & 0x7) {
						case 0b000:  // BEQ
							asm_ins("beq");
							branch = r_gpr[rs1] == r_gpr[rs2];
							break;
						case 0b001:  // BNE
							asm_ins("bne");
							branch = r_gpr[rs1] != r_gpr[rs2];
							break;
						case 0b100:  // BLT
							asm_ins("blt");
							branch = (int32_t)r_gpr[rs1] < (int32_t)r_gpr[rs2];
							break;
						case 0b101:  // BGE
							asm_ins("bge");
							branch = (int32_t)r_gpr[rs1] >= (int32_t)r_gpr[rs2];
							break;
						case 0b110:  // BLTU
							asm_ins("bltu");
							branch = r_gpr[rs1] < r_gpr[rs2];
							break;
						case 0b111:  // BGEU
							asm_ins("bgeu");
							branch = r_gpr[rs1] >= r_gpr[rs2];
							break;
						default:
							fprintf(stderr, "Unknown branch instruction: func3 = 0b%c%c%c\n",
										'0' + ((m_ir >> 12) & 0x4),
										'0' + ((m_ir >> 12) & 0x2),
										'0' + ((m_ir >> 12) & 0x1));
					}
					asm_out("%s	x%d,x%d,%x", s, rs1, rs2, r_pc + imm);
					next_pc = r_pc + (!branch ? 4 : imm);
					break;
				case 0b0000011: // I-type
					decode_i_type(rd, rs1, imm);
					switch ((m_ir >> 12) & 0x7) {
						case 0b000:  // LB
							asm_ins("lb");
							access = READ_BYTE;
							unsign = false;
							break;
						case 0b001:  // LH
							asm_ins("lh");
							access = READ_HALF;
							unsign = false;
							break;
						case 0b010:  // LW
							asm_ins("lw");
							access = READ_WORD;
							unsign = true; // to make gcc shy
							break;
						case 0b100:  // LBU
							asm_ins("lbu");
							access = READ_BYTE;
							unsign = true;
							break;
						case 0b101:  // LHU
							asm_ins("lhu");
							access = READ_HALF;
							unsign = true;
							break;
						default:
							// to make gcc shy
							access = READ_WORD;
							unsign = true;
							fprintf(stderr, "Unknown load instruction: func3 = 0b%c%c%c\n",
										'0' + ((m_ir >> 12) & 0x4),
										'0' + ((m_ir >> 12) & 0x2),
										'0' + ((m_ir >> 12) & 0x1));
					}
					asm_out("%s	x%d,%d(x%d)", s, rd, imm, rs1);
					addr = r_gpr[rs1] + imm;
					load(&r_gpr[rd], access, unsign, addr);
					next_pc = r_pc + 4;
					break;
				case 0b0100011: // S-type
					decode_s_type(rs1, rs2, imm);
					addr = r_gpr[rs1] + imm;
					switch ((m_ir >> 12) & 0x7) {
						case 0b000:  // SB
							asm_ins("sb");
							access = WRITE_BYTE;
							data   = r_gpr[rs2] & 0xff;
							data   = (data << 24) | (data << 16) | (data << 8) | data;
							break;
						case 0b001:  // SH
							asm_ins("sh");
							access = WRITE_HALF;
							data   = r_gpr[rs2] & 0xffff;
							data   = (data << 16) | data;
							break;
						case 0b010:  // SW
							asm_ins("sw");
							access = WRITE_WORD;
							data   = r_gpr[rs2];
							break;
						default:
							fprintf(stderr, "Unknown load instruction: func3 = 0b%c%c%c\n",
										'0' + ((m_ir >> 12) & 0x4),
										'0' + ((m_ir >> 12) & 0x2),
										'0' + ((m_ir >> 12) & 0x1));
							exit(1);
					}
					asm_out("%s	x%d,%d(x%d)", s, rs2, imm, rs1);
					store(access, addr, data);
					next_pc = r_pc + 4;
					break;
				case 0b0010011: // I-type
					decode_i_type(rd, rs1, imm);
					switch ((m_ir >> 12) & 0x7) {
						case 0b000:  // ADDI
							asm_ins("addi");
							r_gpr[rd] = r_gpr[rs1] + imm;
							break;
						case 0b001:  // SLLI
							asm_ins("slli");
							r_gpr[rd] = r_gpr[rs1] << imm;
							break;
						case 0b010:  // SLTI
							asm_ins("slti");
							r_gpr[rd] = (int32_t)r_gpr[rs1] < imm;
							break;
						case 0b011:  // SLTIU
							asm_ins("sltiu");
							r_gpr[rd] = r_gpr[rs1] < (uint32_t)imm;
							break;
						case 0b100:  // XORI
							asm_ins("xori");
							r_gpr[rd] = r_gpr[rs1] ^ imm;
							break;
						case 0b101:
							if ((m_ir & 0xfe000000) == 0x00000000) { // SRLI
								asm_ins("srli");
								r_gpr[rd] = r_gpr[rs1] >> imm;
							} else if ((m_ir & 0xfe000000) == 0x40000000) { // SRAI, must mask imm
								asm_ins("srai");
								r_gpr[rd] = (int32_t)r_gpr[rs1] >> (imm & 0x1f);
							} else
								fprintf(stderr, "Unknown immediate shift right instruction: func7 = 0b%c%c%c%c%c%c%c\n",
											'0' + ((m_ir >> 25) & 0x40),
											'0' + ((m_ir >> 25) & 0x20),
											'0' + ((m_ir >> 25) & 0x10),
											'0' + ((m_ir >> 25) & 0x08),
											'0' + ((m_ir >> 25) & 0x04),
											'0' + ((m_ir >> 25) & 0x02),
											'0' + ((m_ir >> 25) & 0x01));
							break;
						case 0b110:  // ORI
							asm_ins("ori");
							r_gpr[rd] = r_gpr[rs1] | imm;
							break;
						case 0b111:  // ANDI
							asm_ins("andi");
							r_gpr[rd] = r_gpr[rs1] & imm;
							break;
						default:
							fprintf(stderr, "Unknown immediate instruction: func3 = 0b%c%c%c\n",
										'0' + ((m_ir >> 12) & 0x4),
										'0' + ((m_ir >> 12) & 0x2),
										'0' + ((m_ir >> 12) & 0x1));
					}
					asm_out("%s	x%d,x%d,0x%x", s, rd, rs1, imm);
					next_pc = r_pc + 4;
					break;
				case 0b0110011: // R-type
					decode_r_type(rd, rs1, rs2);
					if ((m_ir & 0xbe000000) == 0x00000000) { // ALU operations
						switch ((m_ir >> 12) & 0x7) {
							case 0b000:
								if ((m_ir & 0xfe000000) == 0x00000000) { // ADD
									asm_ins("add");
									r_gpr[rd] = r_gpr[rs1] + r_gpr[rs2];
								} else if ((m_ir & 0xfe000000) == 0x40000000) { // SUB
									asm_ins("sub");
									r_gpr[rd] = r_gpr[rs1] - r_gpr[rs2];
								} else
									fprintf(stderr, "Unknown add/sub instruction: func7 = 0b%c%c%c%c%c%c%c\n",
												'0' + ((m_ir >> 25) & 0x40),
												'0' + ((m_ir >> 25) & 0x20),
												'0' + ((m_ir >> 25) & 0x10),
												'0' + ((m_ir >> 25) & 0x08),
												'0' + ((m_ir >> 25) & 0x04),
												'0' + ((m_ir >> 25) & 0x02),
												'0' + ((m_ir >> 25) & 0x01));
								break;
							case 0b001:  // SLL
								asm_ins("sll");
								r_gpr[rd] = r_gpr[rs1] << (r_gpr[rs2] & 0x1f);
								break;
							case 0b010:  // SLT
								asm_ins("slt");
								r_gpr[rd] = (int32_t)r_gpr[rs1] < (int32_t)r_gpr[rs2];
								break;
							case 0b011:  // SLTU
								asm_ins("sltu");
								r_gpr[rd] = r_gpr[rs1] < r_gpr[rs2];
								break;
							case 0b100:  // XOR
								asm_ins("xor");
								r_gpr[rd] = r_gpr[rs1] ^ r_gpr[rs2];
								break;
							case 0b101:
								if ((m_ir & 0xfe000000) == 0x00000000) { // SRL
									asm_ins("srl");
									r_gpr[rd] = r_gpr[rs1] >> (r_gpr[rs2] & 0x1f);
								} else if ((m_ir & 0xfe000000) == 0x40000000) { // SRA
									asm_ins("sra");
									r_gpr[rd] = (int32_t)r_gpr[rs1] >> (r_gpr[rs2] & 0x1f);
								} else
									fprintf(stderr, "Unknown register shift right instruction: func7 = 0b%c%c%c%c%c%c%c\n",
												'0' + ((m_ir >> 25) & 0x40),
												'0' + ((m_ir >> 25) & 0x20),
												'0' + ((m_ir >> 25) & 0x10),
												'0' + ((m_ir >> 25) & 0x08),
												'0' + ((m_ir >> 25) & 0x04),
												'0' + ((m_ir >> 25) & 0x02),
												'0' + ((m_ir >> 25) & 0x01));
								break;
							case 0b110:  // OR
								asm_ins("or");
								r_gpr[rd] = r_gpr[rs1] | r_gpr[rs2];
								break;
							case 0b111:  // AND
								asm_ins("and");
								r_gpr[rd] = r_gpr[rs1] & r_gpr[rs2];
								break;
							default:
								fprintf(stderr, "Unknown register instruction: func3 = 0b%c%c%c\n",
										'0' + ((m_ir >> 12) & 0x4),
										'0' + ((m_ir >> 12) & 0x2),
										'0' + ((m_ir >> 12) & 0x1));
						}
					} else if ((m_ir & 0xfe000000) == 0x02000000) { // MULT/DIV operations
						switch ((m_ir >> 12) & 0x7) {
							case 0b000:  // MUL
								asm_ins("mul");
								r_gpr[rd] = r_gpr[rs1] * r_gpr[rs2];
								break;
							case 0b001:  // MULH
								asm_ins("mulh");
								r_gpr[rd] = ((int64_t)((int32_t)r_gpr[rs1]) * ((int32_t)r_gpr[rs2])) >> 32;
								break;
							case 0b010:  // MULHSU
								asm_ins("mulhsu");
								r_gpr[rd] = ((int64_t)((uint32_t)r_gpr[rs1]) * ((int32_t)r_gpr[rs2])) >> 32;
								break;
							case 0b011:  // MULHU
								asm_ins("mulhu");
								r_gpr[rd] = ((uint64_t)r_gpr[rs1] * r_gpr[rs2]) >> 32;
								break;
							case 0b100:  // DIV
								asm_ins("div");
								r_gpr[rd] = (int32_t)r_gpr[rs1] / (int32_t)r_gpr[rs2];
								break;
							case 0b101:  // DIVU
								asm_ins("divu");
								r_gpr[rd] = r_gpr[rs1] / r_gpr[rs2];
								break;
							case 0b110:  // REM
								asm_ins("rem");
								r_gpr[rd] = (int32_t)r_gpr[rs1] % (int32_t)r_gpr[rs2];
								break;
							case 0b111:  // REMU
								asm_ins("remu");
								r_gpr[rd] = r_gpr[rs1] % r_gpr[rs2];
								break;
						}
					} else {
						fprintf(stderr, "Unknown register alu instruction: func7 = 0b%c%c%c%c%c%c%c\n",
									'0' + ((m_ir >> 25) & 0x40),
									'0' + ((m_ir >> 25) & 0x20),
									'0' + ((m_ir >> 25) & 0x10),
									'0' + ((m_ir >> 25) & 0x08),
									'0' + ((m_ir >> 25) & 0x04),
									'0' + ((m_ir >> 25) & 0x02),
									'0' + ((m_ir >> 25) & 0x01));
					}
					asm_out("%s	x%d,x%d,x%d", s, rd, rs1, rs2);
					next_pc = r_pc + 4;
					break;
				case 0b0001111: // ?-type, unimplemented yet
					if ((m_ir & 0x000fffff) == 0x0000000f) { // FENCE
							asm_out("%s", "fence");
					} else if (m_ir == 0x0000100f) { // FENCE.I
							asm_out("%s", "fence.i");
					} else
						fprintf(stderr, "Unknown fence instruction\n");
					next_pc = r_pc + 4;
					break;
				case 0b1110011: // I-type, or close, unimplemented yet
					if (m_ir == 0x00000073) { // ECALL, TODO
							asm_out("%s", "ecall");
					} else if (m_ir == 0x00100073) { // EBREAK, TODO
							asm_out("%s", "ebreak");
					} else {
						switch ((m_ir >> 12) & 0x7) {
							case 0b000:  // PRIV
								if (m_ir == 0x10500073) {
										asm_out("%s", "wfi");
								} else if (m_ir == 0x30200073) {
										asm_out("%s", "mret");
										// MPP is set t machine mode
										r_csr[csr_mstatus] |= 0x00001800;
										// MPIE is set to 1 and MIE is set to MPIE
										r_csr[csr_mstatus] |= 0x80 | ((r_csr[csr_mstatus] & 0x80) >> 4);
										// next instruction follows the one that was interrupted
										next_pc = r_csr[csr_mepc];
										// as this is the only instruction of that kind that doesn't use
										// r_pc, let us jump directly to the end
										goto skip_next_pc;
								} else
									fprintf(stderr, "Unknown priviledged instruction: 0x%08x\n", m_ir);
								break;
							case 0b001:  // CSRRW
								decode_csr_type(rd, csr, rs1);
								asm_ins("csrrw");
								asm_out("%s	x%d,%s,x%d", s, rd, csr_name(csr), rs1);
								m_update_csr  = true;
								m_csr_changed = csr;
								reg           = r_csr[csr];
								r_csr[csr]    = r_gpr[rs1];
								if (rd != 0) {
									r_gpr[rd] = reg;
								}
								break;
							case 0b010:  // CSRRS
								decode_csr_type(rd, csr, rs1);
								asm_ins("csrrs");
								asm_out("%s	x%d,%s,x%d", s, rd, csr_name(csr), rs1);
								reg = r_csr[csr];
								if (rs1 != 0) {
									m_update_csr  = true;
									m_csr_changed = csr;
									r_csr[csr]   |= r_gpr[rs1];
								}
								if (rd != 0) {
									r_gpr[rd]     = reg;
								}
								break;
							case 0b011:  // CSRRC
								decode_csr_type(rd, csr, rs1);
								asm_ins("csrrc");
								asm_out("%s	x%d,%s,x%d", s, rd, csr_name(csr), rs1);
								reg = r_csr[csr];
								if (rs1 != 0) {
									m_update_csr  = true;
									m_csr_changed = csr;
									r_csr[csr]   &= ~r_gpr[rs1];
								}
								if (rd != 0) {
									r_gpr[rd] = reg;
								}
								break;
							case 0b101:  // CSRRWI
								decode_csr_type(rd, csr, imm);
								asm_ins("csrrwi");
								asm_out("%s	x%d,%s,%d", s, rd, csr_name(csr), imm);
								m_update_csr  = true;
								m_csr_changed = csr;
								reg = r_csr[csr];
								r_csr[csr]    = imm;
								if (rd != 0) {
									r_gpr[rd] = reg;
								}
								break;
							case 0b110:  // CSRRSI
								decode_csr_type(rd, csr, imm);
								asm_ins("csrrsi");
								asm_out("%s	x%d,%s,%d", s, rd, csr_name(csr), imm);
								reg = r_csr[csr];
								if (imm != 0) {
									m_update_csr  = true;
									m_csr_changed = csr;
									r_csr[csr]   |= r_gpr[imm];
								}
								break;
							case 0b111:  // CSRRCI
								decode_csr_type(rd, csr, imm);
								asm_ins("csrrci");
								asm_out("%s	x%d,%s,%d", s, rd, csr_name(csr), imm);
								reg = r_csr[csr];
								if (imm != 0) {
									m_update_csr  = true;
									m_csr_changed = csr;
									r_csr[csr]   &= ~imm;
								}
								if (rd != 0) {
									r_gpr[rd] = reg;
								}
								break;
							default:
								fprintf(stderr, "Unknown csr instruction: func3 = 0b%c%c%c\n",
											'0' + ((m_ir >> 12) & 0x4),
											'0' + ((m_ir >> 12) & 0x2),
											'0' + ((m_ir >> 12) & 0x1));
						}
						next_pc = r_pc + 4;
					}
					break;
				/*\
				 *
				 * End of Integer + Mult extensions
				 *
				 * Start of single precision floating point.
				 * Note that we do it the "quick and dirty poor old's man" way, by using the host floating points instead
				 * of floating point emulation as QEMU and other do to actually following the exact
				 * riscv specs.
				 * Note that we do not either set the floating point exception flags, ...
				 * 31           27      26   25   24          20   19         15   14    12   11     7   6             0
				 *             funct7                   rs2             rs1        funct3       rd           opcode        R-type
				 *       rs3      funct2                rs2             rs1        funct3       rd           opcode        R4-type
				 *              imm[11:0]                               rs1        funct3       rd           opcode        I-type
				 *        imm[11:5]                     rs2             rs1        funct3     imm[4:0]       opcode        S-type
				 *                               RV32F Standard Extension
				 *         imm[11:0]                        rs1     010                   rd           0000111       FLW
				 *   imm[11:5]                    rs2       rs1     010                 imm[4:0]       0100111       FSW
				 * rs3         00                 rs2       rs1     rm                    rd           1000011       FMADD.S
				 * rs3         00                 rs2       rs1     rm                    rd           1000111       FMSUB.S
				 * rs3         00                 rs2       rs1     rm                    rd           1001011       FNMSUB.S
				 * rs3         00                 rs2       rs1     rm                    rd           1001111       FNMADD.S
				 *    0000000                     rs2       rs1     rm                    rd           1010011       FADD.S
				 *    0000100                     rs2       rs1     rm                    rd           1010011       FSUB.S
				 *    0001000                     rs2       rs1     rm                    rd           1010011       FMUL.S
				 *    0001100                     rs2       rs1     rm                    rd           1010011       FDIV.S
				 *    0101100                    00000      rs1     rm                    rd           1010011       FSQRT.S
				 *    0010000                     rs2       rs1     000                   rd           1010011       FSGNJ.S
				 *    0010000                     rs2       rs1     001                   rd           1010011       FSGNJN.S
				 *    0010000                     rs2       rs1     010                   rd           1010011       FSGNJX.S
				 *    0010100                     rs2       rs1     000                   rd           1010011       FMIN.S
				 *    0010100                     rs2       rs1     001                   rd           1010011       FMAX.S
				 *    1100000                    00000      rs1     rm                    rd           1010011       FCVT.W.S
				 *    1100000                    00001      rs1     rm                    rd           1010011       FCVT.WU.S
				 *    1110000                    00000      rs1     000                   rd           1010011       FMV.X.W
				 *    1010000                     rs2       rs1     010                   rd           1010011       FEQ.S
				 *    1010000                     rs2       rs1     001                   rd           1010011       FLT.S
				 *    1010000                     rs2       rs1     000                   rd           1010011       FLE.S
				 *    1110000                    00000      rs1     001                   rd           1010011       FCLASS.S
				 *    1101000                    00000      rs1     rm                    rd           1010011       FCVT.S.W
				 *    1101000                    00001      rs1     rm                    rd           1010011       FCVT.S.WU
				 *    1111000                    00000      rs1     000                   rd           1010011       FMV.W.X
				\*/
 				case 0b0000111:
					decode_i_type(rd, rs1, imm);
					asm_ins("flw");
					asm_out("%s	f%d,%d(x%d)", s, rd, imm, rs1);
					unsign = true; // unused, but gcc doesn't know it
					addr   = r_gpr[rs1] + imm;
					load((uint32_t *)&r_fpr[rd], READ_WORD, unsign, addr);
					next_pc = r_pc + 4;
					break;
				case 0b0100111:
					decode_s_type(rs1, rs2, imm);
					asm_ins("fsw");
					asm_out("%s	f%d,%d(f%d)", s, rs2, imm, rs1);
					addr    = r_gpr[rs1] + imm;
					suf.f   = r_fpr[rs2];
					store(WRITE_WORD, addr, suf.u);
					next_pc = r_pc + 4;
					break;

#define SETLEGALROUNDING                     \
do {                                         \
	if (rm == rxx || rm == ryy) {             \
		/* TODO: goto Raise FP exception */    \
	} else if (rm == dyn) {                   \
		rm = r_csr[csr_frm];                   \
		if (rm == 0b111) {                     \
			/* TODO: goto Raise FP exception */ \
		}                                      \
	}                                         \
	fesetround(rounding[rm]);                 \
} while (0)

				case 0b1000011:
					decode_r4_type(rd, rs1, rs2, rs3, fmt, rm);
					asm_ins("fmadd.s");
					asm_out("%s	f%d,f%d,f%d,f%d)", s, rd, rs1, rs2, rs3);
					SETLEGALROUNDING;
//#pragma STDC FP_CONTRACT ON
					r_fpr[rd] = r_fpr[rs1] * r_fpr[rs2] + r_fpr[rs3];
//#pragma STDC FP_CONTRACT OFF
					next_pc = r_pc + 4;
					break;
				case 0b1000111:
					decode_r4_type(rd, rs1, rs2, rs3, fmt, rm);
					asm_ins("fmsub.s");
					asm_out("%s	f%d,f%d,f%d,f%d)", s, rd, rs1, rs2, rs3);
					SETLEGALROUNDING;
//#pragma STDC FP_CONTRACT ON
					r_fpr[rd] = r_fpr[rs1] * r_fpr[rs2] - r_fpr[rs3];
//#pragma STDC FP_CONTRACT OFF
					next_pc = r_pc + 4;
					break;
				case 0b1001011:
					decode_r4_type(rd, rs1, rs2, rs3, fmt, rm);
					asm_ins("fnmsub.s");
					asm_out("%s	f%d,f%d,f%d,f%d)", s, rd, rs1, rs2, rs3);
					SETLEGALROUNDING;
//#pragma STDC FP_CONTRACT ON
					r_fpr[rd] = -r_fpr[rs1] * r_fpr[rs2] - r_fpr[rs3];
//#pragma STDC FP_CONTRACT OFF
					next_pc = r_pc + 4;
					break;
				case 0b1001111:
					asm_ins("fnmadd.s");
					decode_r4_type(rd, rs1, rs2, rs3, fmt, rm);
					asm_ins("fmadd.s");
					asm_out("%s	f%d,f%d,f%d,f%d)", s, rd, rs1, rs2, rs3);
					SETLEGALROUNDING;
//#pragma STDC FP_CONTRACT ON
					r_fpr[rd] = -r_fpr[rs1] * r_fpr[rs2] + r_fpr[rs3];
//#pragma STDC FP_CONTRACT OFF
					next_pc = r_pc + 4;
					break;
				case 0b1010011: // R-type, OP-FP
					decode_r_type(rd, rs1, rs2);
					rm = (m_ir >> 12) & 3;
					SETLEGALROUNDING;
					switch ((m_ir >> 25) & 0x7f) {
						case 0b0000000:
							asm_ins("fadd.s");
							asm_out("%s	f%d,f%d,f%d", s, rd, rs1, rs2);
							r_fpr[rd] = r_fpr[rs1] + r_fpr[rs2];
							break;
						case 0b0000100:
							asm_ins("fsub.s");
							asm_out("%s	f%d,f%d,f%d", s, rd, rs1, rs2);
							r_fpr[rd] = r_fpr[rs1] - r_fpr[rs2];
							break;
						case 0b0001000:
							asm_ins("fmul.s");
							asm_out("%s	f%d,f%d,f%d", s, rd, rs1, rs2);
							r_fpr[rd] = r_fpr[rs1] * r_fpr[rs2];
							break;
						case 0b0001100:
							asm_ins("fdiv.s");
							asm_out("%s	f%d,f%d,f%d", s, rd, rs1, rs2);
							r_fpr[rd] = r_fpr[rs1] / r_fpr[rs2];
							break;
						case 0b0101100:
							asm_ins("fsqrt.s");
							r_fpr[rd] = sqrt(r_fpr[rs1]);
							asm_out("%s	f%d,f%d", s, rd, rs1);
							break;
						case 0b0010000:
							switch ((m_ir >> 12) & 0x7) { // func3
								case 0b00:
									if (rs1 == rs2) {
										asm_ins("fmv.s");
										asm_out("%s	f%d,f%d", s, rd, rs1);
									} else {
										asm_ins("fsgnj.s");
										asm_out("%s	f%d,f%d,f%d", s, rd, rs1, rs2);
									}
									suf.f = r_fpr[rs1];
									fus.f = r_fpr[rs2];
									suf.u = (fus.u & 0x80000000) | (suf.u & 0x7fffffff);
									r_fpr[rd] = suf.f;
									break;
								case 0b01:
									if (rs1 == rs2) {
										asm_ins("fneg.s");
										asm_out("%s	f%d,f%d", s, rd, rs1);
									} else {
										asm_ins("fsgnjn.s");
										asm_out("%s	f%d,f%d,f%d", s, rd, rs1, rs2);
									}
									suf.f = r_fpr[rs1];
									fus.f = r_fpr[rs2];
									suf.u = (0x80000000 ^ (fus.u & 0x80000000)) | (suf.u & 0x7fffffff);
									r_fpr[rd] = suf.f;
									break;
								case 0b10:
									if (rs1 == rs2) {
										asm_ins("fabs.s");
										asm_out("%s	f%d,f%d", s, rd, rs1);
									} else {
										asm_ins("fsgnjx.s");
										asm_out("%s	f%d,f%d,f%d", s, rd, rs1, rs2);
									}
									suf.f = r_fpr[rs1];
									fus.f = r_fpr[rs2];
									suf.u = ((suf.u & 0x80000000) ^ (fus.u & 0x80000000)) | (suf.u & 0x7fffffff);
									r_fpr[rd] = suf.f;
									break;
								default:
									fprintf(stderr, "Illegal single precision float instruction: func3 = 0b%c%c%c\n",
												'0' + ((m_ir >> 12) & 0x4),
												'0' + ((m_ir >> 12) & 0x2),
												'0' + ((m_ir >> 12) & 0x1));
							}
							break;
						case 0b0010100:
							switch ((m_ir >> 12) & 0x7) {
								case 0b000:
									asm_ins("fmin.s");
									r_fpr[rd] = r_fpr[rs1] <= r_fpr[rs2] ? r_fpr[rs1] : r_fpr[rs2];
									break;
								case 0b001:
									asm_ins("fmax.s");
									r_fpr[rd] = r_fpr[rs1] >= r_fpr[rs2] ? r_fpr[rs1] : r_fpr[rs2];
									break;
								default:
									fprintf(stderr, "Illegal single precision float instruction: func3 = 0b%c%c%c\n",
												'0' + ((m_ir >> 12) & 0x4),
												'0' + ((m_ir >> 12) & 0x2),
												'0' + ((m_ir >> 12) & 0x1));
							}
							asm_out("%s	f%d,f%d,f%d", s, rd, rs1, rs2);
							break;
						case 0b1100000:
							switch (rs2) {
								case 0b00000:
									asm_ins("fcvt.w.s");
									r_gpr[rd] = (int32_t)r_fpr[rs1];
									break;
								case 0b00001:
									asm_ins("fcvt.wu.s");
									r_gpr[rd] = (uint32_t)r_fpr[rs1];
									break;
								default:
									fprintf(stderr, "Illegal floating point instruction ‘0x%08x’ !\n", m_ir);
							}
							asm_out("%s	x%d,f%d,%s", s, rd, rs1, rounding_name(rm));
							break;
						case 0b1010000:
							switch ((m_ir >> 12) & 0x7) {
								case 0b010:
									asm_ins("feq.s");
									r_gpr[rd] = r_fpr[rs1] == r_fpr[rs2] ? 1 : 0;
									break;
								case 0b001:
									asm_ins("flt.s");
									r_gpr[rd] = r_fpr[rs1] < r_fpr[rs2] ? 1 : 0;
									break;
								case 0b000:
									asm_ins("fle.s");
									r_gpr[rd] = r_fpr[rs1] <= r_fpr[rs2] ? 1 : 0;
									break;
								default:
									fprintf(stderr, "Illegal single precision float instruction: func3 = 0b%c%c%c\n",
												'0' + ((m_ir >> 12) & 0x4),
												'0' + ((m_ir >> 12) & 0x2),
												'0' + ((m_ir >> 12) & 0x1));
							}
							asm_out("%s	x%d,f%d,f%d", s, rd, rs1, rs2);
							break;
						case 0b1110000:
							switch (rm) {
								case 0b000:
									asm_ins("fmv.x.w");
									suf.f = r_fpr[rs1];
									r_gpr[rd] = suf.u;
									break;
								case 0b001:
									asm_ins("fclass.s");
									switch (fpclassify(r_fpr[rs1])) {
										case FP_NAN:
											suf.f = r_fpr[rs1];
											r_gpr[rd] = 1 << (((suf.u & 0x00c00000) == 0x00800000) ? 8 : 9);
											break;
										case FP_INFINITE:
											r_gpr[rd] = 1 << (signbit(r_fpr[rs1]) ? 0 : 7);
											break;
										case FP_ZERO:
											r_gpr[rd] = 1 << (signbit(r_fpr[rs1]) ? 3 : 4);
											break;
										case FP_SUBNORMAL:
											r_gpr[rd] = 1 << (signbit(r_fpr[rs1]) ? 2 : 5);
											break;
										case FP_NORMAL:
											r_gpr[rd] = 1 << (signbit(r_fpr[rs1]) ? 1 : 6);
											break;
										default:
											suf.f = r_fpr[rs1];
											fprintf(stderr, "Argh! Cannot classify single precision floating point “0x%08x”\n", suf.u);
									}
									break;
								default:
									fprintf(stderr, "Illegal floating point instruction ‘0x%08x’ !\n", m_ir);
							}
							asm_out("%s	x%d,f%d", s, rd, rs1);
							break;
						case 0b1101000:
							switch (rs2) {
								case 0b00000:
									asm_ins("fcvt.s.w");
									r_fpr[rd] = (float)((int32_t)r_gpr[rs1]);
									break;
								case 0b00001:
									asm_ins("fcvt.s.wu");
									r_fpr[rd] = (float)r_gpr[rs1];
									break;
								default:
									fprintf(stderr, "Illegal floating point instruction ‘0x%08x’ !\n", m_ir);
							}
							asm_out("%s	f%d,x%d", s, rd, rs1);
							break;
						case 0b1111000:
							asm_ins("fmv.w.x");
							asm_out("%s	f%d,x%d", s, rd, rs1);
							suf.u = r_gpr[rs1];
							r_fpr[rd] = suf.f;
							break;
						default:
							fprintf(stderr, "Unknown single precision floating point instruction: func7 = 0b%c%c%c%c%c%c%c\n",
										'0' + ((m_ir >> 25) & 0x40),
										'0' + ((m_ir >> 25) & 0x20),
										'0' + ((m_ir >> 25) & 0x10),
										'0' + ((m_ir >> 25) & 0x08),
										'0' + ((m_ir >> 25) & 0x04),
										'0' + ((m_ir >> 25) & 0x02),
										'0' + ((m_ir >> 25) & 0x01));
					}
					next_pc = r_pc + 4;
					break;
				default:
					fprintf(stderr, "Unknown rv32im instruction: 0x%08x\n", m_ir);
			}
skip_next_pc:
			/*\
			 * Ensures that we get out of here with a zeroed r0
			\*/
			r_gpr[0] = 0;
			/*\
			 * Update pc
			\*/
			r_pc  = next_pc;
		}
	} // end Rv32imIss::step

	uint32_t Rv32imIss::getDebugRegisterValue(unsigned int reg) const
	{
		switch (reg) {
			case 0:
				return 0;
			case 1 ... 31:
				return r_gpr[reg];
			case 32:
				return r_pc;
			case 33:
				return r_csr[csr_mstatus];
			case 34:
				return r_csr[csr_mtval];
			case 35:
				return r_csr[csr_mcause];
			case 36:
				return r_csr[csr_fflags];
			default:
				return 0;
		}
	}

	void Rv32imIss::setDebugRegisterValue(unsigned int reg, uint32_t value)
	{
		switch (reg) {
			case 1 ... 31:
				r_gpr[reg] = value;
				break;
			case 32:
				r_pc = value;
				break;
			case 33:
				r_csr[csr_mcause] = value;
				break;
			case 34:
				r_csr[csr_mtval] = value;
				break;
			case 35:
				r_csr[csr_mcause] = value;
				break;
			default:
				break;
		}
	}

} // end common
} // end soclib
