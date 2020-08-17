#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

#include "string.h"
#include "vector.h"
#include "error.h"

#define MAX_COL 100
#define MAX_ELEM 8

enum type {
	op = 0,
	label = 1,
	directive = 2,
};

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
				string_set_ref(&elem, &line[begin], i - begin);
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

enum type
get_elem_type(string_t *elem)
{
	if (elem->data[0] == '.') {
		return directive;
	} else if (elem->data[elem->length - 1] == ':') {
		return label;
	} else {
		return op;
	}
}

int
main(int argc, char **argv)
{
	FILE *source_file;
	int c;
	int linum = 0, colnum = 0;
	char line[MAX_COL+1];

	vector_t elems;
	// elems = vector_new(sizeof(string_t), 0, (void (*)(void *)) string_free);
	if (vector_init(&elems, sizeof(string_t), 0, NULL) != OK) {
		fprintf(stderr, "ERR: Out of memory\n");
		return -1;
	}

	enum type elem_type;
	source_file = fopen("test.asm", "r");
	if (!source_file) {
		fprintf(stderr, "ERR: Can't open test.asm, err = %d\n", errno);
		return -1;
	}
	string_t *elem;
	while ((c = getc(source_file)) != EOF) {
		if (c == '\n') {
			line[colnum] = '\0';
			// printf("%02d: %s\n", linum+1, line);
			get_elems(&elems, line);
			// DBG BEGIN
			printf("%02d ", linum+1);
			if (elems.length > 0) {
				elem_type = get_elem_type(vector_get(&elems, 0));
				switch (elem_type) {
				case op: printf("OP "); break;
				case label: printf("LABEL "); break;
				case directive: printf("DIRECTIVE "); break;
				}
			}
			for (int i = 0; i < elems.length; i++) {
				printf("%d:", i);
				elem = vector_get(&elems, i);
				for (int j = 0; j < elem->length; j++) {
					printf("%c", elem->data[j]);
				}
				printf(" ");
			}
			printf("\n");
			// DBG END
			colnum = 0;
			linum += 1;
			vector_clear(&elems);
		} else if (colnum < MAX_COL) {
			line[colnum] = (char) c;
			colnum += 1;
		}
	}
	if (colnum != 0) {
		fprintf(stderr, "ERR: Last line missing \n");
		return -1;
	}
	vector_free(&elems);
	fclose(source_file);
	return 0;
}
