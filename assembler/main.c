#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "instructions.h"
#include "number.h"
#include "string.h"
#include "vector.h"
#include "error.h"

#define MAX_COL 100
#define MAX_ELEM 8

enum elem_type {
	ELEM_OP = 1,
	ELEM_DIRECTIVE = 2,
	ELEM_LABEL = 3,
};

enum line_type {
	LINE_NONE = 0,
	LINE_OP = 1,
	LINE_DIRECTIVE = 2,
};

typedef struct {
	string_t label;
	uint32_t addr;
} symbol_t;

void
symbol_free(symbol_t *s)
{
	string_free(&s->label);
}

cmp_t
symbol_cmp(symbol_t *a, symbol_t *b)
{
	return string_cmp(&a->label, &b->label);
}

cmp_t
string_symbol_cmp(string_t *s, symbol_t *symbol)
{
	return string_cmp(s, &symbol->label);
}

typedef struct {
	char *src_path;
	FILE *src_file;
	char *out_path;
	FILE *out_file;
	int linum;
	int colnum;
	char str[MAX_COL]; // Buffer to hold a temporary string
	size_t addr;
	vector_t sym_table;
} context_t;

error_t
context_init(context_t *ctx)
{
	error_t err;
	ctx->src_path = NULL;
	ctx->src_file = NULL;
	ctx->out_path = NULL;
	ctx->out_file = NULL;

	TRY(vector_init(&ctx->sym_table, sizeof(symbol_t), 0,
			       (void (*)(void *)) symbol_free));

	ctx->linum = 1;
	ctx->colnum = 0;
	ctx->addr = 0;

	return OK;
}

error_t
context_open(context_t *ctx, char *src_path, char *out_path)
{
	ctx->src_path = src_path;
	ctx->src_file = fopen(ctx->src_path, "r");
	if (!ctx->src_file) {
		fprintf(stderr, "ERR: Can't read open %s, errno = %d\n", ctx->src_path, errno);
		return ERR_IO;
	}
	ctx->out_path = out_path;
	ctx->out_file = fopen(ctx->out_path, "wb");
	if (!ctx->out_file) {
		fprintf(stderr, "ERR: Can't write open %s, errno = %d\n", ctx->out_path, errno);
		return ERR_IO;
	}
	return OK;
}

int
context_reset_src_file(context_t *ctx)
{
	ctx->linum = 0;
	ctx->addr = 0;
	return fseek(ctx->src_file, 0, SEEK_SET);
}

void
context_free(context_t *ctx)
{
	vector_free(&ctx->sym_table);
	if (ctx->src_file != NULL) {
		fclose(ctx->src_file);
		ctx->src_file = NULL;
	}
	if (ctx->out_file != NULL) {
		fclose(ctx->out_file);
		ctx->out_file = NULL;
	}
}

void
error(context_t *ctx, error_t err, char *msg)
{
	fprintf(stderr, "ERR");
	if (ctx != NULL) {
		fprintf(stderr, " at %s:%d:%d", ctx->src_path, ctx->linum, ctx->colnum);
	}
	if (msg != NULL) {
		fprintf(stderr, " %s", msg);

	}
	fprintf(stderr, " (%s)\n", error_string[err]);
}

bool
is_whitespace(char c)
{
	if (c == ' ' || c == '\t' || c == '\n') {
		return true;
	}
	return false;
}

bool
is_alpha(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
		return true;
	}
	return false;
}

#define MAX_STR_LEN 256
#define MAX_TOKENS 32

typedef enum {
	LIST,
	QUOTE_LIST,
	SYMBOL,
	QUOTE_SYMBOL,
	STRING,
	INTEGER,
	NIL,
} sexpr_type_t;

typedef struct {
	union {
		vector_t list; // list of sexpr_t
		string_t symbol;
		string_t string;
		int32_t integer;
	};
	sexpr_type_t type;
} sexpr_t;

void
sexpr_init(sexpr_t *sexpr)
{
	sexpr->type = NIL;
}

void
sexpr_free(sexpr_t *sexpr)
{
	switch (sexpr->type) {
	case SYMBOL:
	case QUOTE_SYMBOL:
		string_free(&sexpr->symbol);
		break;
	case STRING:
		string_free(&sexpr->string);
		break;
	case INTEGER:
		break;
	case NIL:
		break;
	case LIST:
	case QUOTE_LIST:
		vector_free(&sexpr->list);
		break;
	}
	sexpr->type = NIL;
}

void
dbg_print_sexpr(sexpr_t *sexpr)
{
	int i;
	sexpr_t *sexpr0;
	switch (sexpr->type) {
	case QUOTE_SYMBOL:
		printf("'");
	case SYMBOL:
		printf("sym:");
		string_write(&sexpr->symbol, stdout);
		printf(" ");
		break;
	case STRING:
		printf("str:'");
		string_write(&sexpr->string, stdout);
		printf("' ");
		break;
	case INTEGER:
		printf("int:%d ", sexpr->integer);
		break;
	case NIL:
		printf("nil ");
		break;
	case QUOTE_LIST:
		printf("'");
	case LIST:
		printf("(");
		for (i = 0; i < sexpr->list.length; i++) {
			sexpr0 = (sexpr_t *) vector_get(&sexpr->list, i);
			assert(sexpr0 != NULL);
			// printf("%d:", i);
			dbg_print_sexpr(sexpr0);
		}
		printf(") ");
		break;
	}
}

typedef enum {
	IN_WS, // White Space
	IN_SYMBOL,
	IN_STRING,
	IN_INTEGER,
	IN_COMMENT,
} lexer_state_t;

error_t
sexpr_set(sexpr_t *sexpr, string_t *str, lexer_state_t state, bool quote)
{
	error_t err;
	assert(str->length != 0);

	// printf("DBG: token: (%p) '", sexpr);
	// string_write(str, stdout);
	// printf("'\n");

	switch (state) {
	case IN_SYMBOL:
		if (quote) {
			sexpr->type = QUOTE_SYMBOL;
		} else {
			sexpr->type = SYMBOL;
		}
		string_set(&sexpr->symbol, str);
		break;
	case IN_STRING:
		sexpr->type = STRING;
		string_set(&sexpr->string, str);
		break;
	case IN_INTEGER:
		sexpr->type = INTEGER;
		TRY(str2num(str, (uint32_t *) &sexpr->integer));
		break;
	default:
		break;
	}
	return OK;
}

error_t
parse(context_t *ctx, sexpr_t *sexpr, int level)
{
	error_t err;
	bool str_scape = false;
	bool quote = false;
	lexer_state_t old_state = IN_WS, state = IN_WS;
	char c;
	// sexpr_t *_sexpr = sexpr;
	vector_t *list = &sexpr->list;
	sexpr_t new_sexpr;
	static char _data[MAX_COL];
	string_t str = (string_t) { data: _data, length: 0 };

	// printf("DBG parse %d %p\n", level, sexpr);
	if (level != 0) {
		sexpr = &new_sexpr;
	}

	while ((c = getc(ctx->src_file)) != EOF) {
		// printf("DBG %c\n", c);
		ctx->colnum++;
		if (c == '\n') {
			ctx->linum++;
			ctx->colnum = 0;
		}

		if (c == '\n') {
			if (state == IN_COMMENT) {
				state = IN_WS;
			}
		} else if (state == IN_COMMENT) {
			str.length = 0;
		} else if (state == IN_STRING) {
			if (str_scape) {
				str_scape = false;
			} else if (c == '"') {
				// Remove trailing '"'
				str.data++;
				str.length--;
				state = IN_WS;
			} else if (c == '\\') {
				str_scape = true;
			}
		} else if (c == '\'') {
			quote = true;
		} else if (c == ';') {
			state = IN_COMMENT;
		} else if (c == '(' || c == ')'){
			state = IN_WS;
		} else if (is_whitespace(c)) {
			state = IN_WS;
		} else { // token character
			if (state == IN_WS) {
				if (c == '"') {
					state = IN_STRING;
				} else if (c >= '0' && c <= '9') {
					state = IN_INTEGER;
				} else {
					state = IN_SYMBOL;
				}
				if (state != IN_WS) {
					str.length = 0;
				}
			}
		}
		// printf("DBG c:'%c' (%d)\n", c, state);
		if (state == IN_COMMENT) {
			goto parse_do;
		}

		// If we changed state comming from a non-whitespace/comment
		// state, we have completed a token.
		if (old_state != IN_WS && old_state != IN_COMMENT && state != old_state) {
			if (level != 0) {
				// First reserve a new slot in the list, assign
				// it to sexpr temporarly.
				TRY(vector_push(list, sexpr));
				sexpr = (sexpr_t *) vector_get(list, list->length - 1);
			}
			TRY(sexpr_set(sexpr, &str, old_state, quote));
			quote = false;
			if (level == 0) {
				return OK;
			} else {
				// Set sexpr back to the stack allocated
				// new_sexpr for the following iterations.
				sexpr = &new_sexpr;
				// printf("DBG sexpr (%p)\n", _sexpr);
				// dbg_print_sexpr(_sexpr);
				// printf(" %d \n", _sexpr->list.length);
			}
		}

		// If we are at the beginning of the list, prepare an empty
		// list and recurse.
		if (c == '(') {
			if (level != 0) {
				TRY(vector_push(list, sexpr));
				sexpr = (sexpr_t *) vector_get(list, list->length - 1);
			}
			if (quote) {
				sexpr->type = QUOTE_LIST;
			} else {
				sexpr->type = LIST;
			}
			quote = false;
			TRY(vector_init(&sexpr->list, sizeof(sexpr_t), 2,
					  (void (*)(void *)) sexpr_free));
			TRY(parse(ctx, sexpr, level+1));
			if (level == 0) {
				return OK;
			} else {
				sexpr = &new_sexpr;
			}
		// If we are at the end of the list, return from recursion.
		} else if (c == ')') {
			return OK;
		}

parse_do:
		old_state = state;
		str.data[str.length] = (char) c;
		str.length++;
		// If str is full, grow its capacity
		if (str.length == MAX_COL) {
			return ERR_MAX_STR;
		}
	}
	return ERR_EOF;
}

error_t
validate_args(vector_t *args, int length, sexpr_type_t *types)
{
	int i;
	sexpr_t *sexpr;
	if (args->length != length) {
		return ERR_EVAL_ARGSLEN;
	}
	for (i = 0; i < length; i++) {
		sexpr = vector_get(args, i);
		if (sexpr->type != types[i]) {
			return ERR_EVAL_BADTYPE;
		}
	}
	return OK;
}

error_t eval(context_t *ctx, sexpr_t *sexpr, sexpr_t *res, int pass);

error_t
eval_args(context_t *ctx, vector_t *args, int pass)
{
	error_t err;
	int i;
	sexpr_t res;

	for (i = 0; i < args->length; i++) {
		TRY(eval(ctx, vector_get(args, i),
			&res, pass));
		* (sexpr_t *) vector_get(args, i) = res;
	}
	return OK;
}
error_t
dir_symbol(context_t *ctx, string_t *name, int pass)
{
	error_t err;
 	symbol_t symbol;

	if (pass != 1) {
		return OK;
	}
	TRY(string_set(&symbol.label, name));
	symbol.addr = ctx->addr;
	return vector_push(&ctx->sym_table, &symbol);
}

error_t
eval_directive(context_t *ctx, string_t *dir, vector_t *args, sexpr_t *res, int pass)
{
	printf("DBG Unknown directive: ");
	string_write(dir, stdout);
	printf("\n");
	return OK;
}

error_t
eval_inst(context_t *ctx, string_t *name, vector_t *args, sexpr_t *res, int pass)
{
	error_t err;
	int i;
	uint32_t out = 0;
	char word[4];
	size_t ret;
	uint32_t rd = 0, rs1 = 0, rs2 = 0;
	int32_t imm = 0;
	sexpr_t *arg;
	operand_t *op;
 	instruction_t *inst;
	res->type = NIL;

	ctx->addr += 4;
	if (pass != 2) {
		return OK;
	}
	TRY(eval_args(ctx, args, pass));

	// TODO: Replace by binary search
 	for (i = 0; i < instructions_len; i++) {
 		inst = (instruction_t *) &instructions[i];
 		if (string_cmp_c(name, inst->name) == EQUAL) {
 			// printf("%s %d %d\n", inst->name, ctx->linum, i);
 			break;
 		}
 	}
	if (i == instructions_len) {
		printf("DBG Unknown inst: ");
		string_write(name, stdout);
		printf("\n");
		return ERR_INST_UNK;
	}
	if (args->length != inst->ops_len) {
		// We use ops_len == -1 to denote that the argument is implicit
		// (imm in ecall and ebreak)
		if (!(args->length == 0 && inst->ops_len == -1)) {
			return ERR_INST_LEN;
		}
	}
	if (inst->ops_len == -1) {
		if (inst->ops[0].imm_opt == IMM_1) {
			imm = 0x1;
		}
		// If imm_opt == IMM_0, imm = 0x0
	}
	for (i = 0; i < args->length; i++) {
		arg = vector_get(args, i);
		op = &inst->ops[i];
		switch (op->type) {
		case REG:
			switch (op->reg_opt) {
			case RD: rd = arg->integer; break;
			case RS1: rs1 = arg->integer; break;
			case RS2: rs2 = arg->integer; break;
			}
			break;
		case IMM:
			imm = arg->integer;
			if (op->imm_opt == ABS && imm < 0) {
				return ERR_NEG;
			}
			if (op->imm_opt == IMM_5_11_00 || op->imm_opt == IMM_5_11_20) {
				if ((uint32_t) imm >= (1 << 5)) { return ERR_IMM_OUT_RANGE; }
				imm &= 0b11111;
				if (op->imm_opt == IMM_5_11_20) {
					imm |= 0x20 << 5;
				}
				// If imm_opt == IMM_5_11_00, imm[11:5] = 0x00
			}
			break;
		}
	}

	// printf("DBG imm: %d\n", imm);
	// For I-Type and S-Type, imm is signed
	if ((inst->fmt == I_TYPE || inst->fmt == S_TYPE) && imm < 0) {
		imm = (1 << 12) + imm;
		// printf("DBG imm b2: %d\n", imm);
	}
	if ((inst->fmt == B_TYPE) && imm < 0) {
		imm = (1 << 13) + imm;
		// printf("DBG imm b2: %d\n", imm);
	}
	if ((inst->fmt == J_TYPE) && imm < 0) {
		imm = (1 << 21) + imm;
		// printf("DBG imm b2: %d\n", imm);
	}

	// TODO: Validate size of imm
	out |= inst->opcode;
	switch (inst->fmt) {
	case R_TYPE:
		out |= rd << 7;
		out |= inst->funct3 << 12;
		out |= rs1 << 15;
		out |= rs2 << 20;
		out |= inst->funct7 << 25;
		break;
	case I_TYPE:
		if ((uint32_t) imm >= (1 << 12)) { return ERR_IMM_OUT_RANGE; }
		out |= rd << 7;
		out |= inst->funct3 << 12;
		out |= rs1 << 15;
		out |= imm << 20;
		break;
	case S_TYPE:
		if ((uint32_t) imm >= (1 << 12)) { return ERR_IMM_OUT_RANGE; }
		out |= (imm & 0b1111) << 7;
		out |= inst->funct3 << 12;
		out |= rs1 << 15;
		out |= rs2 << 20;
		out |= (imm & (0b1111111 << 5)) >> 5 << 25;
		break;
	case B_TYPE:
		if ((uint32_t) imm >= (1 << 13)) { return ERR_IMM_OUT_RANGE; }
		out |= (imm & (1 << 11)) >> 11 << 7;
		out |= (imm & (0b1111 << 1)) >> 1 << 8;
		out |= inst->funct3 << 12;
		out |= rs1 << 15;
		out |= rs2 << 20;
		out |= (imm & (0b111111 << 5)) >> 5 << 25;
		out |= (imm & (1 << 12)) >> 12 << 31;
		break;
	case U_TYPE:
		out |= rd << 7;
		out |= (imm & (0xfffff000)) >> 12 << 12;
		break;
	case J_TYPE:
		if ((uint32_t) imm >= (1 << 21)) { return ERR_IMM_OUT_RANGE; }
		out |= rd << 7;
		out |= (imm & (0b11111111 << 12)) >> 12 << 12;
		out |= (imm & (1 << 11)) >> 11 << 20;
		out |= (imm & (0b1111111111 << 1)) >> 1 << 21;
		out |= (imm & (1 << 20)) >> 20 << 31;
		break;
	default:
		break;
	}

	word[0] = (out & (0xff << (0 * 8))) >> (0 * 8);
	word[1] = (out & (0xff << (1 * 8))) >> (1 * 8);
	word[2] = (out & (0xff << (2 * 8))) >> (2 * 8);
	word[3] = (out & (0xff << (3 * 8))) >> (3 * 8);
	ret = fwrite(word, 4, 1, ctx->out_file);
	if (ret != 1) {
		printf("DBG ret=%ld\n", ret);
		return ERR_IO;
	}
	printf("inst: %02x %02x %02x %02x\n", word[0], word[1], word[2], word[3]);

	return OK;
}

error_t
eval_fn(context_t *ctx, string_t *fn, vector_t *args, sexpr_t *res, int pass)
{
	error_t err;
	res->type = NIL;
 	if (string_cmp_c(fn, "$") == EQUAL) {
		TRY(eval_args(ctx, args, pass));
		TRY(validate_args(args, 1, (sexpr_type_t[]) { SYMBOL }));
		return dir_symbol(ctx, &((sexpr_t *) vector_get(args, 0))->symbol, pass);
	} else if (string_cmp_c(fn, "@") == EQUAL) {
		TRY(eval_args(ctx, args, pass));
		TRY(validate_args(args, 1, (sexpr_type_t[]) { INTEGER }));
		res->type = INTEGER;
		res->integer = ((sexpr_t *) vector_get(args, 0))->integer - (((int32_t) ctx->addr) - 4);
		return OK;
 	} else {
		printf("DBG Unknown fn: ");
		string_write(fn, stdout);
		printf("\n");
	}
	return OK;
}

error_t
eval_symbol(context_t *ctx, string_t *s, sexpr_t *res, int pass)
{
	register_alias_t *reg;
	symbol_t *sym;

	// Search the symbol in the register table
	reg = (register_alias_t *) vector_bin_search((vector_t *) &registers,
		(cmp_t (*)(void *, void *)) string_register_alias_cmp, s);
	if (reg != NULL) {
		res->type = INTEGER;
		res->integer = reg->value;
		return OK;
	}

	if (pass != 1) {
		// Search the symbol in the symbol table (labels) // TODO: Maybe rename symbols to labels
		sym = (symbol_t *) vector_bin_search(&ctx->sym_table,
			(cmp_t (*)(void *, void *)) string_symbol_cmp, s);
		if (sym != NULL) {
			res->type = INTEGER;
			res->integer = sym->addr;
			return OK;
		}
	}
	res->type = NIL;
	return ERR_SYM_NOT_DEF;
}

error_t
eval(context_t *ctx, sexpr_t *sexpr, sexpr_t *res, int pass)
{
	error_t err;
	sexpr_t *head;
	vector_t args;
	char c;

	switch (sexpr->type) {
	case LIST:
		if (sexpr->list.length == 0) {
			sexpr_free(sexpr);
			res->type = NIL;
			return OK;
		}
		head = (sexpr_t *) vector_get(&sexpr->list, 0);
		if (head->type != SYMBOL) {
			err = ERR_EVAL_HEADNOSYM;
			goto eval_free;
		}
		args = sexpr->list;
		args.data += args.elem_size;
		args.length--;

		c = head->symbol.data[0];
		if (c == '.') {
			err = eval_directive(ctx, &head->symbol, &args, res, pass);
		} else if (is_alpha(c)) {
			err = eval_inst(ctx, &head->symbol, &args, res, pass);
		} else {
			err = eval_fn(ctx, &head->symbol, &args, res, pass);
		}
		goto eval_free;
	case SYMBOL:
		err = eval_symbol(ctx, &sexpr->symbol, res, pass);
		sexpr_free(sexpr);
		return err;
	default:
		switch (sexpr->type) {
		case QUOTE_LIST:
			sexpr->type = LIST;
			break;
		case QUOTE_SYMBOL:
			sexpr->type = SYMBOL;
			break;
		default:
			break;
		}
		*res = *sexpr;
		return OK;
	}

eval_free:
	sexpr_free(sexpr);
	return err;
}

void
dbg_print_sym_table(vector_t *sym_table)
{
	symbol_t *sym_ref;
	int i;
	for (i = 0; i < sym_table->length; i++) {
		sym_ref = vector_get(sym_table, i);
		printf("%d: [%d] ", i, sym_ref->addr);
		for (int j = 0; j < sym_ref->label.length; j++) {
			printf("%c", sym_ref->label.data[j]);
		}
		printf("\n");
	}
}

// error_t
// parse_reg(string_t *elem, uint32_t *n)
// {
// 	string_t s;
// 	char e0;
// 	if (elem->length < 2) {
// 		return ERR_UNK;
// 	}
// 	e0 = elem->data[0]; // e0 contains the element's first char
// 	s.data = elem->data + 1; // s contains the element without the first char
// 	s.length = elem->length-1;
// 	// string_write(&s, stdout);
// 	if (e0 == 'r') {
// 		return dec2num(&s, n);
// 	}
// 	return OK;
// }

// int
// assemble(context_t *ctx, line_t *parsed_line)
// {
// 	error_t err;
// 	string_t *elem;
// 	vector_t *elems = &parsed_line->elems;
// 	operand_t *op;
// 
// 	instruction_t *inst;
// 	int i;
// 
// 	elem = vector_get(elems, 0);
// 	for (i = 0; i < instructions_len; i++) {
// 		inst = (instruction_t *) &instructions[i];
// 		if (string_cmp_c(elem, inst->inst) == EQUAL) {
// 			printf("%s %d %d\n", inst->inst, ctx->linum-1, i);
// 			break;
// 		}
// 	}
// 	if (i == instructions_len) {
// 		return -1; // Mnemonic not found
// 	}
// 	if ((elems->length - 1) != inst->ops_len) {
// 		printf("Expected %d, found %d\n", inst->ops_len, elems->length - 1);
// 		return -1; // Invalid number of operands
// 	}
// 
// 	uint32_t n;
// 	for (i = 0; i < elems->length - 1; i++) {
// 		elem = vector_get(elems, i + 1);
// 		op = &inst->ops[i];
// 		switch (op->type) {
// 		case REG:
// 			if ((err = parse_reg(elem, &n)) != OK) {
// 				printf(" ERR parse_reg: %s\n", error_string[err]);
// 				return -1;
// 			}
// 			printf("r%d ", n);
// 			break;
// 		case IMM:
// 			// parse_imm();
// 			break;
// 		}
// 	}
// 	printf("\n");
// 	return 0;
// }

int
main(int argc, char **argv)
{
	char *src_path = NULL, *out_path = NULL;
	int err = 0;
	context_t ctx;
	sexpr_t res;
	sexpr_t sexpr;

	// Initialize objects

	if ((err = context_init(&ctx)) != OK) {
		goto main_free;
	}
	sexpr_init(&res);
	sexpr_init(&sexpr);

	// Gather arguments

	if (argc < 3) {
		fprintf(stderr, "ERR: No input file\n");
		err = -1;
		goto main_free;
	}
	src_path = argv[1];
	out_path = argv[2];
	err = context_open(&ctx, src_path, out_path);
	if (err != OK) {
		goto main_free;
	}



	// Pass 1
	while (true) {
		err = parse(&ctx, &sexpr, 0);
		if (err == ERR_EOF) {
			err = 0;
			break;
		} else if (err != OK) {
			error(&ctx, err, "pass 1 parse()");
			goto main_free;
		}
		sexpr_init(&res);
		dbg_print_sexpr(&sexpr);
		printf("\n");

		err = eval(&ctx, &sexpr, &res, 1);
		if (err != OK) {
			error(&ctx, err, "pass 1 eval()");
			err = -1;
			goto main_free;
		}
		sexpr_free(&res);

	}
	vector_sort(&ctx.sym_table, (cmp_t (*)(void *, void *)) symbol_cmp);
	printf("\n---\n\n");
	dbg_print_sym_table(&ctx.sym_table);

	// Find repetitions in the symbol table
	string_t *prev_label = NULL;
	int i;
	symbol_t *sym_ref;
	for (i = 0; i < ctx.sym_table.length; i++) {
		sym_ref = vector_get(&ctx.sym_table, i);
		if (prev_label != NULL) {
			if (string_cmp(prev_label, &sym_ref->label) == EQUAL) {
				fprintf(stderr, "ERR: Repeated label: ");
				string_write(prev_label, stderr);
				fprintf(stderr, "\n");
				err = -1;
				goto main_free;
			}
		}
		prev_label = &sym_ref->label;
	}

	// Pass 2
	if (context_reset_src_file(&ctx) != 0) {
		fprintf(stderr, "ERR: Seeking input file\n");
		err = -1;
		goto main_free;
	}

	while (true) {
		err = parse(&ctx, &sexpr, 0);
		if (err == ERR_EOF) {
			err = 0;
			break;
		} else if (err != OK) {
			error(&ctx, err, "pass 2 parse()");
			goto main_free;
		}
		sexpr_init(&res);
		dbg_print_sexpr(&sexpr);
		printf("\n");

		err = eval(&ctx, &sexpr, &res, 2);
		if (err != OK) {
			error(&ctx, err, "pass 2 eval()");
			err = -1;
			goto main_free;
		}
		sexpr_free(&res);

	}

main_free:
	context_free(&ctx);
	// vector_free(&parsed_line.elems);
	// sexpr_free(&sexpr);
	sexpr_free(&res);
	return err;
}
