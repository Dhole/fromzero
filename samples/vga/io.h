#ifndef IO_H
#define IO_H

#define INPUT_BUF_LEN 32

void input_push(int c);
int input_pop();
int input_peek();
void putc(char c);
void delc();
int getchar();
int peek();

#endif /* IO_H */
