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
	size_t addr;
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

typedef struct {
	char *src_path;
	FILE *src_file;
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

	if ((err = vector_init(&ctx->sym_table, sizeof(symbol_t), 0,
			       (void (*)(void *)) symbol_free)) != OK) {
		return err;
	}

	ctx->linum = 1;
	ctx->colnum = 0;
	ctx->addr = 0;

	return OK;
}

error_t
context_open(context_t *ctx, char *src_path)
{
	ctx->src_path = src_path;
	ctx->src_file = fopen(ctx->src_path, "r");
	if (!ctx->src_file) {
		fprintf(stderr, "ERR: Can't open %s, errno = %d\n", ctx->src_path, errno);
		return ERR_UNK;
	}
	return OK;
}

int
context_reset_file(context_t *ctx)
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
	SYMBOL,
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
sexpr_set(sexpr_t *sexpr, string_t *str, lexer_state_t state)
{
	error_t err;
	assert(str->length != 0);

	// printf("DBG: token: (%p) '", sexpr);
	// string_write(str, stdout);
	// printf("'\n");

	switch (state) {
	case IN_SYMBOL:
		sexpr->type = SYMBOL;
		string_set(&sexpr->symbol, str);
		break;
	case IN_STRING:
		sexpr->type = STRING;
		string_set(&sexpr->string, str);
		break;
	case IN_INTEGER:
		sexpr->type = INTEGER;
		err = str2num(str, (uint32_t *) &sexpr->integer);
		if (err != OK) { return err; }
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
				err = vector_push(list, sexpr);
				if (err != OK) { return err; }
				sexpr = (sexpr_t *) vector_get(list, list->length - 1);
			}
			err = sexpr_set(sexpr, &str, old_state);
			if (err != OK) { return err; }
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
				err = vector_push(list, sexpr);
				if (err != OK) { return err; }
				sexpr = (sexpr_t *) vector_get(list, list->length - 1);
			}
			sexpr->type = LIST;
			err = vector_init(&sexpr->list, sizeof(sexpr_t), 2,
					  (void (*)(void *)) sexpr_free);
			if (err != OK) { return err; }
			err = parse(ctx, sexpr, level+1);
			if (err != OK) { return err; }
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
			return ERR_UNK;
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

error_t
dir_symbol(context_t *ctx, string_t *name)
{
	error_t err;
 	symbol_t symbol;

	err = string_set(&symbol.label, name);
	if (err != OK) { return err; }
	symbol.addr = ctx->addr;
	return vector_push(&ctx->sym_table, &symbol);
}

error_t
eval_directive(context_t *ctx, string_t *dir, vector_t *args)
{
	error_t err;

 	if (string_cmp_c(dir, ".s") == EQUAL) {
		err = validate_args(args, 1, (sexpr_type_t[]) { SYMBOL });
		if (err != OK) { return err; }
		return dir_symbol(ctx, &((sexpr_t *) vector_get(args, 0))->symbol);
 	} else {
		printf("DBG Unknown directive: ");
		string_write(dir, stdout);
		printf("\n");
	}
	return OK;
}

error_t
eval_inst(context_t *ctx, string_t *inst, vector_t *args)
{
	printf("DBG Unknown inst: ");
	string_write(inst, stdout);
	printf("\n");

	ctx->addr += 4;

	return OK;
}

error_t
eval_fn(context_t *ctx, string_t *fn, vector_t *args)
{
	printf("DBG Unknown fn: ");
	string_write(fn, stdout);
	printf("\n");
	return OK;
}

error_t
eval(context_t *ctx, sexpr_t *sexpr, sexpr_t *res, int pass)
{
	error_t err;
	sexpr_t head;
	sexpr_t elem;
	vector_t list;
	char c;
	int i;

	sexpr_init(&elem);

	switch (sexpr->type) {
	case LIST:
		if (sexpr->list.length == 0) {
			break;
		}
		err = vector_init(&list, sizeof(sexpr_t), 0,
				  (void (*)(void *)) sexpr_free);
		if (err != OK) { goto eval_free; }
		for (i = 0; i < sexpr->list.length; i++) {
			if (i == 0) {
				err = eval(ctx, vector_get(&sexpr->list, i),
					   &head, pass);
				if (err != OK) { goto eval_free; }
				if (head.type != SYMBOL) {
					err = ERR_EVAL_HEADNOSYM;
				}
			} else {
				err = vector_push(&list, &elem);
				if (err != OK) { goto eval_free; }
				err = eval(ctx, vector_get(&sexpr->list, i),
					   vector_get(&list, i - 1), pass);
			}
			if (err != OK) { goto eval_free; }
		}

		c = head.symbol.data[0];
		if (c == '.') {
			err = eval_directive(ctx, &head.symbol, &list);
		} else if (is_alpha(c)) {
			err = eval_inst(ctx, &head.symbol, &list);
		} else {
			err = eval_fn(ctx, &head.symbol, &list);
		}
		goto eval_free;
	default:
		break;
	}
	memcpy(res, sexpr, sizeof(sexpr_t));
	sexpr->type = NIL;
	return OK;

eval_free:
	sexpr_free(sexpr);
	sexpr_free(&head);
	vector_free(&list);
	sexpr->type = NIL; // TMP
	return err;
}

void
dbg_print_sym_table(vector_t *sym_table)
{
	symbol_t *sym_ref;
	int i;
	for (i = 0; i < sym_table->length; i++) {
		sym_ref = vector_get(sym_table, i);
		printf("%d: [%ld] ", i, sym_ref->addr);
		for (int j = 0; j < sym_ref->label.length; j++) {
			printf("%c", sym_ref->label.data[j]);
		}
		printf("\n");
	}
}

error_t
parse_reg(string_t *elem, uint32_t *n)
{
	string_t s;
	char e0;
	if (elem->length < 2) {
		return ERR_UNK;
	}
	e0 = elem->data[0]; // e0 contains the element's first char
	s.data = elem->data + 1; // s contains the element without the first char
	s.length = elem->length-1;
	string_write(&s, stdout);
	if (e0 == 'r') {
		return dec2num(&s, n);
	}
	return OK;
}

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
	char *src_path = NULL;
	int err = 0;
	context_t ctx;

	// Initialize objects

	if ((err = context_init(&ctx)) != OK) {
		goto main_free;
	}

	// Gather arguments

	if (argc < 2) {
		fprintf(stderr, "ERR: No input file\n");
		err = -1;
		goto main_free;
	}
	src_path = argv[1];
	if ((err = context_open(&ctx, src_path)) != OK) {
		goto main_free;
	}


	sexpr_t sexpr;
	sexpr_init(&sexpr);
	sexpr_t res;

	// Pass 1
	while (true) {
		err = parse(&ctx, &sexpr, 0);
		if (err == ERR_EOF) {
			err = 0;
			break;
		} else if (err != OK) {
			error(&ctx, err, "parse()");
			goto main_free;
		}
		sexpr_init(&res);
		dbg_print_sexpr(&sexpr);
		printf("\n");

		err = eval(&ctx, &sexpr, &res, 1);
		if (err != OK) {
			error(&ctx, err, "eval()");
			err = -1;
			goto main_free;
		}
		sexpr_free(&sexpr); // TMP
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
	if (context_reset_file(&ctx) != 0) {
		fprintf(stderr, "ERR: Seeking input file\n");
		err = -1;
		goto main_free;
	}
	// while (parse_line_file(&ctx, &parsed_line)) {
	// 	if (parsed_line.type == LINE_OP) {
	// 		if (assemble(&ctx, &parsed_line) != 0) {
	// 			fprintf(stderr, "ERR: Assemble line %d\n", ctx.linum-1);
	// 			err = -1;
	// 			goto main_free;
	// 		}
	// 		ctx.addr += 2;
	// 	}
	// }

main_free:
	context_free(&ctx);
	// vector_free(&parsed_line.elems);
	sexpr_free(&sexpr);
	sexpr_free(&res);
	return err;
}
