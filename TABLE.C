/* STD.H */

#ifdef __DOS__
#include "DOS_OW.H"
#endif

#ifdef STDC
#include "STDC.H"
#endif

#include <ctype.h>

void except_in(char *msg, char const *f) {
	char f_buf[8+2+1];

	print("EXCEPTION:$");
	my_strcpy(f_buf, f);
	my_strcat(f_buf, ":$");
	print(f_buf);
	print(msg);
	print("\n$");
	my_abort();
}

#define except(message) do { except_in(message, __func__); } while (0)

#define ExcEOF() except("premature EOF$");
#define ExcWr() except("write error, disk full?$");

int my_strnicmp(char* dest, char* src, size_t count) {
	char a, b;
	while (count--) {
		a = tolower(*dest++);
		b = tolower(*src++);
		if (a > b) { return 1; }
		if (a < b) { return -1; }
		if (a == 0) return 0;
	}

	return 0;
}

static void print_num(int value)
{
	char buf[12];
	my_strcpy(buf, my_utoa(value));
	my_strcat(buf, "$");
	print(buf);
}

/* DBA.H */


#include "DBA.H"

/* COL.C */
/*
void CdMake(char *name, char type, int len, int dec_cnt, struct TbCOLDEF *col_out) {
	my_strcpy(col_out->name, name);
	col_out->type = type;
	col_out->data
	col_out->len = len;
	col_out->dec_cnt = dec_cnt;
	col_out->reserved1 = 0;

	col_out->mdx_flag = 0;
	col_out->reserved2 = 0;
	col_out->next_ai = 0;
	col_out->reserved3 = 0;
}*/

/* TABLE.C */

char idump = 0;

struct TbH tbl;

void TbOpt(int type, int value) {
	switch (type) {
		case TO_IDUMP: idump = value; break;
	}
}

int TbDef(char *name, int ncol, struct TbCOLDEF *cols) {
	int handle;

	/* open file for writing */
	handle = creat(name);

	if (handle < 0) {
		print("TABLE CREAT ERR\n$");
		return handle;
	}
	else {
		// print("TABLE CREAT OK\n$");
	}

	/* build header & write it */

	/* write columns */

	return handle;
}

/* TaBle Info Dump */
void TbIDump(int handle) {
	struct TbHDR hdr;
	// int pos;
	// char end_mk;
	int cum_r_sz;

	read(handle, &hdr, sizeof(hdr));

#define I_CHARS(field_, num_) do { char buf[num_ + 1]; my_memcpy(buf, hdr.field_, num_); buf[num_] = '$'; print("  " #field_ "=\t'$"); print(buf); print("'\n$"); } while (0)
#define I_DEC(field_) do { print("  " #field_ "=\t$"); print_num(hdr.field_); print("\n$"); } while (0)
#define I_YYMMDD(field_) do { print("  " #field_ "=\t$"); print_num(hdr.field_[0]); print("-$"); \
		print_num(hdr.field_[1]); print("-$"); \
		print_num(hdr.field_[2]); print("\n$"); } while (0)

	print("TABLE\n$");
	I_DEC(flags);
	I_YYMMDD(datemark);
	I_DEC(num_rec);
	I_DEC(hdr_siz);
	I_DEC(rec_siz);
	I_DEC(reserved1);
	I_DEC(tx_pend);
	I_DEC(crypt);
	// I_DEC(reserved2[0]); I_DEC(reserved2[1]); I_DEC(reserved2[2]);
	// I_DEC(mdx_flag);
	// I_DEC(drv_id);
	// I_DEC(reserved3);

#undef I_CHARS
#undef I_DEC

	cum_r_sz = 1;	/* record validity marker (0x20/0x2A), always present */

	for (;;) {
		struct TbCOLDEF cd;

		if (read(handle, &cd, 1) != 1) { except("read coldef$"); }

		if (cd.name[0] == 0x0D) {
			break;
		}

		print("  FIELD$");

		if (read(handle, &cd.name[1], sizeof(cd) - 1) != sizeof(cd) - 1) { except("read coldef$"); }

		cum_r_sz += cd.len;

#define I_CHAR(field_) do { char buf[2]; buf[0] = cd.field_; buf[1] = '$'; print("    " #field_ "=\t'$"); print(buf); print("'\n$"); } while (0)
#define I_CHARS(field_, num_) do { char buf[num_ + 1]; my_memcpy(buf, cd.field_, num_); buf[num_] = '$'; print("    " #field_ "=\t'$"); print(buf); print("'\n$"); } while (0)
#define I_DEC(field_) do { print("    " #field_ "=\t$"); print_num(cd.field_); print("\n$"); } while (0)

		I_CHARS(name, sizeof(cd.name));
		I_CHAR(type);
		I_DEC(dataaddr);
		I_DEC(len);
		I_DEC(dec_cnt);
		// I_DEC(reserved1);
		// I_DEC(workarea);
		// I_DEC(reserved2[0]); I_DEC(reserved2[1]);
		// I_DEC(set_fiel);
		// I_CHARS(reserved3, sizeof(cd.reserved3));

#undef I_DEC
	}

	// read(handle, &end_mk, 1);

#define I_DEC(field_) do { print("  " #field_ "=\t$"); print_num(field_); print("\n$"); } while (0)

	// I_DEC(end_mk);
	I_DEC(cum_r_sz);

#undef I_DEC
}

int TbOpen(char *name) {
	int handle;
	struct TbCOLDEF *cd;
	int cum_r_sz;

	/* open file for writing */
	handle = open(name);

	if (handle < 0) {
		print("TABLE OPEN ERR\n$");
		return handle;
	}
	else {
		// print("TABLE OPEN OK\n$");
	}

	if (idump) {
		TbIDump(handle);
		lseek(handle, 0, 0);
	}

	if (read(handle, &tbl.hdr, sizeof(tbl.hdr)) != sizeof(tbl.hdr)) {
		/* ERROR */ return -1;
	}

	/* scan columns */
	tbl.ncols = 0;

	cum_r_sz = 1;	/* record validity marker (0x20/0x2A), always present */

	for (;;) {
		cd = &tbl.cols[tbl.ncols];

		if (read(handle, cd, 1) != 1) { except("read coldef$"); }

		if (cd->name[0] == 0x0D) {
			break;
		}

		if (tbl.ncols >= MAX_COLS) { except("MAX_COLS$"); }

		/* now read past first byte */
		if (read(handle, &cd->name[1], sizeof(*cd) - 1) != sizeof(*cd) - 1) { except("read coldef$"); }

		tbl.col_ofs[tbl.ncols] = cum_r_sz;

		cum_r_sz += cd->len;
		tbl.ncols++;
	}

	tbl.handle = handle;

	return handle;
}

void TbClose(void) {
	if (tbl.handle >= 0) {
		close(tbl.handle);
		tbl.handle = -1;
	}
}

void TbWrRec(int recno, char *rec_buf) {
	lseek(tbl.handle, tbl.hdr.hdr_siz + recno * tbl.hdr.rec_siz, 0);

	if (write(tbl.handle, rec_buf, tbl.hdr.rec_siz) != tbl.hdr.rec_siz) { ExcWr(); }
}

void TbWrHdr(void) {
	lseek(tbl.handle, 0, 0);

	if (write(tbl.handle, &tbl.hdr, sizeof(tbl.hdr)) != sizeof(tbl.hdr)) { ExcWr(); }
}

/* CUR.C */

char val[COL_LEN];
int val_len = 0;
int recno = -1;

char rec_buf[REC_LEN];

void Select(char* tabname) {
	/* no-op in this version */
	recno = -1;
}

void SelRec(int rec) {
	recno = rec;
}

int IsSel(void) {
	return (recno >= 0);
}

void DumpRec(void) {
	char marker;
	int i;
	struct TbCOLDEF* cd;

	if (recno < 0) { except("no selection$"); }

	lseek(tbl.handle, tbl.hdr.hdr_siz + recno * tbl.hdr.rec_siz, 0);

	/* read marker */
	if (read(tbl.handle, &marker, 1) != 1) { ExcEOF(); }

	if (marker == '*') {
		print("record invalid\n$");
		return;
	}
	if (marker == ' ') {
		/* iterate columns */
		for (i = 0; i < tbl.ncols; i++) {
			cd = &tbl.cols[i];

			my_memcpy(val, cd->name, sizeof(cd->name));
			my_strcpy(val + sizeof(cd->name), "\t$");
			print(val);

			if (read(tbl.handle, val, cd->len) != cd->len) { ExcEOF(); }

			val[cd->len] = '$';
			print(val);
			print("\n$");
		}
	}
	else { print("record corrupt\n$"); }
}

void SetVal(char *value) {
	val_len = my_strlen(value);
	my_memcpy(val, value, val_len + 1);
}


int _FindCol(char* colname) {
	int i;

	for (i = 0; i < tbl.ncols; i++) {
		if (my_strncmp(tbl.cols[i].name, colname, sizeof(tbl.cols[i].name)) == 0) {
			return i;
		}
	}

	return -1;
}

int PutCol(char *colname) {
	int col_idx;
	struct TbCOLDEF* cd;
	int i;
	char marker;

	if (recno < 0) { except("no selection$"); }

	if ((col_idx = _FindCol(colname)) < 0) { except("no such column$"); }

	cd = &tbl.cols[col_idx];

	/* prevent truncation of value */
	if (val_len > cd->len) {
		except("value does not fit$");
	}

	/* copy val into the column and pad with \0 to column size */
	my_strncpy(rec_buf + tbl.col_ofs[col_idx], val, cd->len);

	return 0;
}

int ScanCol(char *colname) {
	int col_idx;
	struct TbCOLDEF* cd;
	int i;
	char marker;

	char val_exp[sizeof(val)];
	// size_t vale_len;

	if ((col_idx = _FindCol(colname)) < 0) { except("no such column$"); }

	my_strcpy(val_exp, val);
	// vale_len = val_len;

	cd = &tbl.cols[col_idx];

	if (recno < 0) {
		recno = 0;
	}

	for (; recno < tbl.hdr.num_rec; recno++) {
		lseek(tbl.handle, tbl.hdr.hdr_siz + recno * tbl.hdr.rec_siz, 0);

		/* read marker */
		if (!read(tbl.handle, &marker, 1)) { ExcEOF(); }
		if (marker != ' ') { continue; }

		/* read column & compare */
		lseek(tbl.handle, tbl.col_ofs[col_idx] - 1, 1);

		if (read(tbl.handle, val, cd->len) != cd->len) { ExcEOF(); }

		if (my_strncmp(val, val_exp, cd->len) == 0) {
			return 1;
		}
	}

	/* must not leave recno pointing beyond the end of the table, because that indicates that a new record is being inserted */
	recno = -1;
	return 0;
}

/*
 * InsRec: insert a record
 *
 * Exception raised on failure.
 */
void InsRec(void) {
	/* set recno to last + 1 */
	recno = tbl.hdr.num_rec;

	/* create memory buffer for the record */
	if (tbl.hdr.rec_siz > sizeof(rec_buf)) {
		except("record too large$");
	}

	rec_buf[0] = ' ';

	/* user then sets columns */
	/* user commits the record */
}

void CmmitRec(void) {
	if (recno > tbl.hdr.num_rec) {
		except("invalid record number$");
	}

	TbWrRec(recno, rec_buf);

	if (recno == tbl.hdr.num_rec) {
		tbl.hdr.num_rec++;
	}

	TbWrHdr();
}
