/* https://www.oocities.org/geoff_wass/dBASE/GaryWhite/dBASE/FAQ/qformt.htm */

struct TbHDR {
	char flags;
	char datemark[3];
	uint32_t num_rec;
	uint16_t hdr_siz;
	uint16_t rec_siz;
	uint16_t reserved1;
	char tx_pend;			/* reserved before dBASE IV 2.0 */
	char crypt;     		/* reserved before dBASE IV 2.0 */
	uint32_t reserved2[3];	/* reserved for multi-user env */

	/* reserved before dBASE IV 2.0 */
	char mdx_flag;
	char drv_id;
	uint16_t reserved3;

	/* end for dBASE III PLUS */
	/*char drv_name[32];
	uint32_t reserved4;*/
};

struct TbCOLDEF {
	char name[11];
	char type;
	uint32_t dataaddr;
	uint8_t len;
	char dec_cnt;
	uint16_t reserved1; /* Reserved for dBASE III PLUS on a LAN. */
	char workarea;
	char reserved2[2]; /* Reserved for dBASE III PLUS on a LAN. */
	char set_fiel;
	char reserved3[8];
};

#ifdef __GNUC__
_Static_assert(sizeof(struct TbHDR) == 32, "Unexpected size of struct TbHDR");
_Static_assert(sizeof(struct TbCOLDEF) == 32, "Unexpected size of struct TbCOLDEF");
#endif

/* maximum column length (no source, our limit) */
#define COL_LEN 255

/* maximum record length (arbitrary limit because allocated statically for now) */
#define REC_LEN 255

/* maximum column count */
#define MAX_COLS 8

/* table handle */
struct TbH {
	struct TbHDR hdr;
	struct TbCOLDEF cols[MAX_COLS];
	int handle;
	int ncols;
	int col_ofs[MAX_COLS];
};

#define TO_IDUMP 1		/* Info Dump on table open */
