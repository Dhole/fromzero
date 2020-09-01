#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

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
		} else if (c == ' ' || c == '\t' || c == '\0' || c == ';') {
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
		return;
	} else if (elem0_type == ELEM_DIRECTIVE) {
		parsed->type = LINE_DIRECTIVE;
		elem0->data++; // Remove '.' prefix
		return;
	}
}

int
main(int argc, char **argv)
{
	FILE *source_file = NULL;
	int c;
	int linum = 0, colnum = 0;
	char line[MAX_COL+1];
	int err = 0;

	line_t parsed_line;
	if (vector_init(&parsed_line.elems, sizeof(string_t), 0, NULL) != OK) {
		fprintf(stderr, "ERR: Out of memory\n");
		err = -1;
		goto cleanup;
	}

	vector_t sym_table;
	if (vector_init(&sym_table, sizeof(symbol_t), 0, (void (*)(void *)) symbol_free) != OK) {
		fprintf(stderr, "ERR: Out of memory\n");
		err = -1;
		goto cleanup;
	}

	// enum type elem_type;
	source_file = fopen("test.asm", "r");
	if (!source_file) {
		fprintf(stderr, "ERR: Can't open test.asm, err = %d\n", errno);
		err = -1;
		goto cleanup;
	}

	symbol_t symbol;
	size_t addr = 0;
	string_t label;

	string_init(&label);
	while ((c = getc(source_file)) != EOF) {
		if (c == '\n') {
			line[colnum] = '\0';
			// printf("%02d: %s\n", linum+1, line);
			parse_line(&parsed_line, line);
			if (parsed_line.label.length > 0) {
				string_set(&symbol.label, &parsed_line.label);
				symbol.addr = addr;
				vector_push(&sym_table, &symbol);
			}
			if (parsed_line.type == LINE_OP) {
				addr += 2;
			}
			// DBG BEGIN
			// printf("%02d ", linum+1);
			// if (line_elems.length > 0) {
			// 	elem_type = get_elem_type(vector_get(&line_elems, 0));
			// 	switch (elem_type) {
			// 	case OP: printf("OP "); break;
			// 	case LABEL: printf("LABEL "); break;
			// 	case DIRECTIVE: printf("DIRECTIVE "); break;
			// 	}
			// }
			// for (int i = 0; i < line_elems.length; i++) {
			// 	printf("%d:", i);
			// 	elem = vector_get(&line_elems, i);
			// 	for (int j = 0; j < elem->length; j++) {
			// 		printf("%c", elem->data[j]);
			// 	}
			// 	printf(" ");
			// }
			// printf("\n");
			// DBG END
			colnum = 0;
			linum += 1;
			vector_clear(&parsed_line.elems);
		} else if (colnum < MAX_COL) {
			line[colnum] = (char) c;
			colnum += 1;
		}
	}
	if (colnum != 0) {
		fprintf(stderr, "ERR: Last line missing \n");
		return -1;
	}
	// DBG BEGIN
	symbol_t *sym_ref;
	printf("\n---\n\n");
	int i;
	for (i = 0; i < sym_table.length; i++) {
		sym_ref = vector_get(&sym_table, i);
		printf("%d: [%ld] ", i, sym_ref->addr);
		for (int j = 0; j < sym_ref->label.length; j++) {
			printf("%c", sym_ref->label.data[j]);
		}
		printf("\n");
	}
	// DBG END
	vector_sort(&sym_table, (cmp_t (*)(void *, void *)) symbol_cmp);
	// TODO: Find repetitions in the symbol table
	string_t *prev_label = NULL;
	for (i = 0; i < sym_table.length; i++) {
		sym_ref = vector_get(&sym_table, i);
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
cleanup:
	vector_free(&parsed_line.elems);
	vector_free(&sym_table);
	if (source_file != NULL) {
		fclose(source_file);
	}
	return err;
}
