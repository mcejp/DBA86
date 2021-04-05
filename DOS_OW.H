#include <dos.h>

#define MY_O_RDWR 2

typedef unsigned char          uint8_t;
typedef char                   int8_t;
typedef unsigned short         uint16_t;
typedef short                  int16_t;
typedef unsigned long int      uint32_t;
typedef long int               int32_t;
typedef unsigned long long int uint64_t;
typedef long long int          int64_t;
typedef unsigned short         size_t;

/* referenced by something. not critical though, if deemed undesirable */
static void print_num(int value);

void my_abort(void);
#ifdef __WATCOMC__
#pragma aux my_abort aborts;
#pragma aux my_abort = \
		"mov ah, 4Ch" 	\
		"int 0x21" 		\
		modify 	[ah];
#endif

int my_memcmp(char* dest, char* src, size_t count) {
	while (count--) {
		if (*dest > *src) { return 1; }
		if (*dest < *src) { return -1; }
		src++;
		dest++;
	}

	return 0;
}

void my_memcpy(char* dest, char* src, size_t count) {
	while (count--) {
		*dest = *src;
		src++;
		dest++;
	}
}

void my_strcpy(char* dest, char const* src) {
	while (*src) {
		*dest = *src;
		src++;
		dest++;
	}
	*dest = 0;
}

/* copy string up to length & pad with zero */
void my_strncpy(char* dest, char* src, size_t dest_size) {
	while (dest_size > 0 && *src) {
		*dest = *src;
		src++;
		dest++;
		dest_size--;
	}
	for (;dest_size > 0; dest_size--) {
		*dest = 0;
		dest++;
	}
}

void my_strcat(char* dest, char* src) {
	while (*dest) {
		dest++;
	}
	my_strcpy(dest, src);
}

size_t my_strlen(char* str) {
	size_t len = 0;

	while (*str) {
		len++;
		str++;
	}

	return len;
}

int my_strncmp(char* dest, char* src, size_t count) {
	while (count--) {
		if (*dest > *src) { return 1; }
		if (*dest < *src) { return -1; }
		if (*dest == 0) return 0;
		src++;
		dest++;
	}

	return 0;
}

char* my_utoa(uint32_t value) {
	static char buf[11];
	char* p;

	p = &buf[sizeof(buf) - 1];
	*p = 0;

	if (value) {
		while (value) {
			p--;
			*p = '0' + (value % 10);
			value /= 10;
		}
	}
	else {
		p--;
		*p = '0';
	}
	return p;
}

uint8_t getch(void);
#ifdef __WATCOMC__
#pragma aux getch = \
		"mov ah, 01h" 	\
		"int 0x21" 		\
		value 	[al]	\
		modify 	[ah];
#endif

void print(char *string);
#ifdef __WATCOMC__
#pragma aux print = \
		"mov ah, 09h" 	\
		"int 0x21" 		\
		parm 	[dx]	\
		modify 	[ax];
#endif

int creat(char *filename) {
	int handle;

	if (_dos_creat(filename, 0, &handle) != 0) {
		return -1;
	}

	return handle;
}

int open(char *filename) {
	int handle;

	if (_dos_open(filename, MY_O_RDWR, &handle) != 0) {
		return -1;
	}

	return handle;
}

int read(int handle, void *output, int count) {
	unsigned int nread;

	if (_dos_read(handle, output, count, &nread) != 0) {
		return -1;
	}

	return nread;
}

int write(int handle, void *data, int count) {
	unsigned int written;

	if (_dos_write(handle, data, count, &written) != 0) {
		return -1;
	}

	return written;
}

uint32_t lseek(int handle, uint32_t pos, char whence);
#ifdef __WATCOMC__
/* source: https://github.com/open-watcom/open-watcom-v2/blob/052c7ead9ae2dac6f63aa812e6c7f593575bad9f/bld/watcom/h/tinyio.h#L2067 */
#pragma aux lseek = \
        "mov  ah,42h"   \
        "int  21h"      \
        "rcl  dx,1"     \
        "ror  dx,1"     \
    __parm __caller     [__bx] [__dx __cx] [__al] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]
#endif

int close(int handle) {
	if (_dos_close(handle) != 0) {
		return -1;
	}

	return 0;
}
