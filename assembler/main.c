#include <stdio.h>

#define MAX_COL 100
#define MAX_ELEM 8

#define TRUE 1
#define FALSE 0

struct Elem {
	int begin;
	int end;
};

int
get_elems(char *line, struct Elem *elems)
{
	int n = 0, begin = -1, in_str = FALSE, str_scape = FALSE, i;
	char c;
	for (i = 0; i < MAX_COL; i++) {
		c = line[i];
		if (in_str) {
			if (c == '\0') {
				break;
			} else if (str_scape) {
				str_scape = FALSE;
			} else if (c == '"') {
				in_str = FALSE;
			} else if (c == '\\') {
				str_scape = TRUE;
			}
		} else if (c == ' ' || c == '\0' || c == ';') {
			if (begin != -1) {
				elems[n++] = (struct Elem){begin, i};
				begin = -1;
			}
			if (c == '\0' || c == ';' || n == MAX_ELEM) {
				break;
			}
		} else {
			if (begin == -1) {
				begin = i;
				if (c == '"') {
					in_str = TRUE;
				}
			}
		}
	}
	return n;
}

int
main(int argc, char **argv)
{
	FILE *source_file;
	int c;
	int linum = 0, colnum = 0, num_elems;
	char line[MAX_COL+1];
	struct Elem elems[MAX_ELEM];
	source_file = fopen("test.asm", "r");
	if (!source_file) {
		return -1;
	}
	while ((c = getc(source_file)) != EOF) {
		if (c == '\n') {
			line[colnum] = '\0';
			// printf("%02d: %s\n", linum+1, line);
			num_elems = get_elems(line, elems);
			// DBG BEGIN
			printf("%02d ", linum+1);
			for (int i = 0; i < num_elems; i++) {
				printf("%d:", i);
				for (int j = elems[i].begin; j < elems[i].end; j++) {
					printf("%c", line[j]);
				}
				printf(" ");
			}
			printf("\n");
			// DBG END
			colnum = 0;
			linum += 1;
		} else if (colnum < MAX_COL) {
			line[colnum] = (char) c;
			colnum += 1;
		}
	}
	fclose(source_file);
}
