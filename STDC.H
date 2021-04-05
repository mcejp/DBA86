#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void my_abort(void) {
	exit(-1);
}

void my_memcpy(char* dest, char* src, size_t count) {
	memcpy(dest, src, count);
}

int my_memcmp(char* dest, char* src, size_t count) {
	return memcmp(dest, src, count);
}

void my_strcpy(char* dest, char const* src) {
	strcpy(dest, src);
}

void my_strncpy(char* dest, char* src, size_t dest_size) {
	strncpy(dest, src, dest_size);
}

void my_strcat(char* dest, char* src) {
	strcat(dest, src);
}

size_t my_strlen(char* str) {
	return strlen(str);
}

int my_strncmp(char* dest, char* src, size_t count) {
	return strncmp(dest, src, count);
}

char* my_utoa(uint32_t value) {
	static char buf[11];
	sprintf(buf, "%u", value);
	return buf;
}

void print(char *string) {
	char* end = memchr(string, '$', SIZE_MAX);

	if (end) {
		fwrite(string, end - string, 1, stdout);
	}
}

FILE* f;

int creat(char *filename) {
	f = fopen(filename, "w+b");

	if (f) {
		return 1;
	}
	else {
		return 0;
	}
}

int open(char *filename) {
	f = fopen(filename, "r+b");

	if (f) {
		return 1;
	}
	else {
		return 0;
	}
}

int lseek(int handle, int pos, int whence) {
	return fseek(f, pos, whence);
}

int read(int handle, void *output, int count) {
	return fread(output, 1, count, f);
}

int write(int handle, void *data, int count) {
	return fwrite(data, 1, count, f);
}

int close(int handle) {
	fclose(f);
}

uint8_t getch(void) {
	int c = getchar();

	if (c < 0) {
		return 0x1A;
	}
	else if (c == '\n') {
		return '\r';
	}
	else {
		return c;
	}
}
