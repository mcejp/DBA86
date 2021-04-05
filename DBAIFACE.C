#include "TABLE.C"

int echo = 1;
int eof = 0;

int Ask(char *buf, char *q) {
	size_t have;
	int ch;

	if (echo) {
		print(q);
	}

	for (have = 0; ; ) {
		ch = getch();
		// print("-$");
		// print_num(ch);
		// print("-$");
		if (ch == 0x1A) {
			eof = 1;
			break;
		}
		else if (ch == '\r') {
			break;
		}
		buf[have++] = ch;
	}

	buf[have] = 0;
	return have;
}

int main(void)
{
	char cmd[40];

	print("DBA/86 v0.0\n$");

	while (!eof) {
		Ask(cmd, "DBA>$");

		if (!my_strnicmp(cmd, "OPEN", 40)) {
			Ask(cmd, "TABLE>$");
			my_strcat(cmd, ".DBF");
			TbOpen(cmd);
		}
		else if (!my_strnicmp(cmd, "INFO", 40)) {
			print("records:$");
			print_num(tbl.hdr.num_rec);
			print("\n$");
		}
		else if (!my_strnicmp(cmd, "FIND", 40)) {
			Ask(cmd, "VALUE>$");
			SetVal(cmd);
			Ask(cmd, "COLUMN>$");
			ScanCol(cmd);

			if (IsSel()) {
				print("result:\n$");
				DumpRec();
			}
			else {
				print("no matching row\n$");
			}
		}
		else if (!my_strnicmp(cmd, "SCHEMA", 40)) {
			Ask(cmd, "TABLE>$");
			my_strcat(cmd, ".DBF");
			TbOpt(TO_IDUMP, 1);
			TbOpen(cmd);
			TbOpt(TO_IDUMP, 0);
		}
		else if (!my_strnicmp(cmd, "BATCH", 40)) {
			echo = 0;
		}
		else if (!my_strnicmp(cmd, "END", 40)) {
			eof = 1;
		}
		else if (*cmd) {
			print("did not understand\n$");
		}
	}

	return 0;
}
