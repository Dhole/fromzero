#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

#include "instructions.h"
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

void
get_elems(vector_t *elems, char *line)
{
	int begin = -1, in_str = false, str_scape = false, i;
	char c;
	string_t elem;

	string_init(&elem);
	for (i = 0; i < MAX_COL; i++) {
		c = line[i];
		if (in_str) {
			if (c == '\0') {
				break;
			} else if (str_scape) {
				str_scape = false;
			} else if (c == '"') {
				in_str = false;
			} else if (c == '\\') {
				str_scape = true;
			}
		} else if (c == ' ' || c == '\t' || c == '\0' || c == ';' || c == ']') {
			if (begin != -1) {
				string_set_ref_s(&elem, &line[begin], i - begin);
				assert(elem.length != 0);
				vector_push(elems, &elem);
				begin = -1;
			}
			if (c == '\0' || c == ';' || elems->length == MAX_ELEM) {
				break;
			}
		} else { // token character
			if (begin == -1) {
				begin = i;
				if (c == '"') {
					in_str = true;
					continue;
				}
				if (c == '[') { // skip '['
					begin++;
					i++;
					continue;
				}
			}
		}
	}
}

enum elem_type
get_elem_type(string_t *elem)
{
	if (elem->data[0] == '.') {
		return ELEM_DIRECTIVE;
	} else if (elem->data[elem->length - 1] == ':') {
		return ELEM_LABEL;
	} else {
		return ELEM_OP;
	}
}

typedef struct {
	string_t label;
	enum line_type type;
	vector_t elems;
} line_t;

void
parse_line(line_t *parsed, char *line)
{
	enum elem_type elem0_type;
	string_t *elem0;

	get_elems(&parsed->elems, line);
	parsed->label.length = 0;
	if (parsed->elems.length == 0) {
		parsed->type = LINE_NONE;
		return;
	}

	elem0 = vector_get(&parsed->elems, 0);
	elem0_type = get_elem_type(elem0);
	if (elem0_type == ELEM_LABEL) {
		elem0->length--; // Remove ':' suffix
		parsed->label = *elem0;
		vector_pop_front(&parsed->elems, NULL);

		if (parsed->elems.length == 0) {
			parsed->type = LINE_NONE;
			return;
		}
		elem0 = vector_get(&parsed->elems, 0);
		elem0_type = get_elem_type(elem0);
	}

	if (elem0_type == ELEM_OP) {
		parsed->type = LINE_OP;
		string_lower(elem0);
		return;
	} else if (elem0_type == ELEM_DIRECTIVE) {
		parsed->type = LINE_DIRECTIVE;
		elem0->data++; // Remove '.' prefix
		return;
	}
}

void
dbg_print_sym_table(vector_t *sym_table)
{
	symbol_t *sym_ref;
	printf("\n---\n\n");
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

void
dbg_print_parsed_line(vector_t *line_elems, int linum)
{
	enum elem_type elem_type;
	string_t *elem;
	printf("%02d ", linum+1);
	if (line_elems->length > 0) {
		elem_type = get_elem_type(vector_get(line_elems, 0));
		switch (elem_type) {
			case ELEM_OP: printf("OP "); break;
			case ELEM_LABEL: printf("LABEL "); break;
			case ELEM_DIRECTIVE: printf("DIRECTIVE "); break;
		}
	}
	for (int i = 0; i < line_elems->length; i++) {
		printf("%d:", i);
		elem = (string_t *) vector_get(line_elems, i);
		string_write(elem, stdout);
		printf(" ");
	}
	printf("\n");
}

typedef struct {
	char *src_path;
	FILE *src_file;
	char line[MAX_COL+1];
	int linum;
	size_t addr;
	vector_t sym_table;
} context_t;

int
context_init(context_t *ctx, char *src_path)
{
	ctx->src_path = src_path;
	ctx->src_file = fopen(ctx->src_path, "r");
	if (!ctx->src_file) {
		fprintf(stderr, "ERR: Can't open test.asm, err = %d\n", errno);
		return -1;
	}

	if (vector_init(&ctx->sym_table, sizeof(symbol_t), 0, (void (*)(void *)) symbol_free) != OK) {
		fprintf(stderr, "ERR: Out of memory\n");
		return -1;
	}

	ctx->linum = 0;
	ctx->addr = 0;

	return 0;
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
	}
}

bool
parse_line_file(context_t *ctx, line_t *parsed_line)
{
	int c, colnum = 0;
	vector_clear(&parsed_line->elems);
	while ((c = getc(ctx->src_file)) != EOF) {
		if (c == '\n') {
			ctx->line[colnum] = '\0';
			// printf("%02d: %s\n", linum+1, line);
			parse_line(parsed_line, ctx->line);
			ctx->linum += 1;
			return true;
		} else if (colnum < MAX_COL) {
			ctx->line[colnum] = (char) c;
			colnum += 1;
		}
	}
	return false;
}

int
assemble(context_t *ctx, line_t *parsed_line)
{
	string_t *op;
	instruction_t *inst;
	int i;

	op = vector_get(&parsed_line->elems, 0);
	for (i = 0; i < instructions_len; i++) {
		inst = (instruction_t *) &instructions[i];
		if (string_cmp_c(op, inst->inst) == EQUAL) {
			printf("%s %d %d\n", inst->inst, ctx->linum-1, i);
			break;
		}
	}
	if (i == instructions_len) {
		return -1; // Mnemonic not found
	}
	if ((parsed_line->elems.length - 1) != inst->ops_len) {
		printf("Expected %d, found %d\n", inst->ops_len, parsed_line->elems.length - 1);
		return -1; // Invalid number of operands
	}
	return 0;
}

int
main(int argc, char **argv)
{
	char *src_path = NULL;
	// FILE *src_file = NULL;
	// int c;
	// int linum = 0, colnum = 0;
	// char line[MAX_COL+1];
	int err = 0;
	context_t ctx;

	if (argc < 2) {
		fprintf(stderr, "ERR: No input file\n");
		err = -1;
		goto cleanup;
	}
	src_path = argv[1];

	if ((err = context_init(&ctx, src_path)) != 0) {
		goto cleanup;
	}

	line_t parsed_line;
	if (vector_init(&parsed_line.elems, sizeof(string_t), 0, NULL) != OK) {
		fprintf(stderr, "ERR: Out of memory\n");
		err = -1;
		goto cleanup;
	}

	// Pass 1.  Parse lines
	symbol_t symbol;
	while (parse_line_file(&ctx, &parsed_line)) {
		if (parsed_line.label.length > 0) {
			string_set(&symbol.label, &parsed_line.label);
			symbol.addr = ctx.addr;
			vector_push(&ctx.sym_table, &symbol);
		}
		if (parsed_line.type == LINE_OP) {
			ctx.addr += 2;
		}
		dbg_print_parsed_line(&parsed_line.elems, ctx.linum-1);
	}
	dbg_print_sym_table(&ctx.sym_table);
	vector_sort(&ctx.sym_table, (cmp_t (*)(void *, void *)) symbol_cmp);

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
				goto cleanup;
			}
		}
		prev_label = &sym_ref->label;
	}

	// Pass 2
	if (context_reset_file(&ctx) != 0) {
		fprintf(stderr, "ERR: Seeking input file\n");
		err = -1;
		goto cleanup;
	}
	while (parse_line_file(&ctx, &parsed_line)) {
		if (parsed_line.type == LINE_OP) {
			if (assemble(&ctx, &parsed_line) != 0) {
				fprintf(stderr, "ERR: Assemble line %d\n", ctx.linum-1);
				err = -1;
				goto cleanup;
			}
			ctx.addr += 2;
		}
	}
cleanup:
	context_free(&ctx);
	vector_free(&parsed_line.elems);
	return err;
}
