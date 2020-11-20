#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
	char buf[65535];
	FILE *fp = stdin;
	int argbegin;
	bool totalOnly = false;
	enum {
		TypeC, TypePython,
	} type = TypeC;
	static const char *comm_lines[] = { "//\0", "#\0" };
	static const char *comm_blks[] = { "/*\0*/\0", "'''\0'''\0\"\"\"\0\"\"\"\0" };

	for (argbegin=1; argbegin < argc && argv[argbegin][0] == '-'; argbegin++) {
		switch (argv[argbegin][1]) {
		case 't':
			totalOnly = true;
			break;
		case '-':
			if (strcmp(argv[argbegin] + 2, "python") == 0) {
				type = TypePython;
				break;
			}
			// through break
		default:
			fprintf(stderr, "usage: wcc [-t] [--python] [<C-sourcefile> [...]]\n");
			fprintf(stderr, "-t output total count only\n");
			return 1;
		}
	}
	if (argbegin < argc) {
		fp = nullptr;
	}

	const char *comm_line = comm_lines[type];
	const char *comm_blk = comm_blks[type];

	size_t allLines = 0, allTotal = 0;
	for (int i = argbegin; i <= argc; i++) {
		if (fp == nullptr && i < argc)
			fopen_s(&fp, argv[i], "rt");
		if (fp) {
			size_t total = 0;
			size_t lines = 0;
			size_t comment = 0;	// コメント行トータル
			bool incomment = false;
			const char* comm_blk_end = nullptr;
			while (fgets(buf, sizeof(buf), fp)) {
				total++;

				auto trim = [](char *buf) -> char* {	// 前後のホワイトスペースを取り払って、先頭の非ホワイトスペースのポインタを返す
					char *p = &buf[strlen(buf)];
					while (p > buf && (*(p - 1) == '\r' || *(p - 1) == '\n' || *(p - 1) == ' ' || *(p - 1) == '\t'))
						p--;
					*p = '\0';
					p = buf;
					while (*p != '\0' && (*p == ' ' || *p == '\t'))
						p++;
					return p;
				};

				// ブロックコメントの先頭を探して、対応するコメントブロックキーワードを返す				
				auto strstr_blk = [](char* str, const char* comm, const char** comm_begin, const char** comm_end) -> char* {
					const char* comm_found = nullptr;
					char* str_found = nullptr;
					while (*comm) {
						char *pc = strstr(str, comm);
						if (pc != nullptr) {
							if (str_found == nullptr || str_found > pc) {
								comm_found = comm;
								str_found = pc;
							}
						}
						comm = comm + strlen(comm) + 1;
						comm = comm + strlen(comm) + 1;
					}
					if (str_found == nullptr)
						return nullptr;
					*comm_begin = comm_found;
					*comm_end = comm_found + strlen(comm_found) + 1;
					return str_found;
				};

				// 1行コメントを探す
				auto strstr_line = [](char* str, const char* comm) -> char* {
					const char* comm_found = nullptr;
					char* str_found = nullptr;
					while (*comm) {
						char *pc = strstr(str, comm);
						if (pc != nullptr) {
							if (str_found == nullptr || str_found > pc) {
								comm_found = comm;
								str_found = pc;
							}
						}
						comm = comm + strlen(comm) + 1;
					}
					if (str_found == nullptr)
						return nullptr;
					return str_found;
				};

				char *head = buf;
				char *p = buf;
				char *pc = nullptr;
				for (;;) {
					trim(head);
					if (*head == '\0') {
						// 空行はコメント行
						comment++;
						break;
					}

					if (!incomment) {
						const char* comm_blk_begin;
						pc = strstr_blk(p, comm_blk, &comm_blk_begin, &comm_blk_end);
						if (char *pc2 = strstr_line(p, comm_line)) {
							if (pc == nullptr || pc2 < pc) {	// 先に//があるのでそれ以降は削除
								*pc2 = '\0';
								continue;
							}
						}
						if (pc) {
							incomment = true;
							memset(pc, ' ', strlen(comm_blk_begin));
							p = pc + strlen(comm_blk_begin);
						}
						else {
							lines++;
							break;
						}
					}
					if (incomment) {
						if (char *pce = strstr(p, comm_blk_end)) {
							// コメントが終わった
							if (pc) {
								memset(pc, ' ', pce - pc + strlen(comm_blk_end));	// 行内の部分コメントをスペースで埋める
								pc = nullptr;
							}
							else {
								memset(p, ' ', pce - p + strlen(comm_blk_end));	// 行の先頭からスペースで埋める
							}
							incomment = false;
							p = pce + strlen(comm_blk_end);
							comm_blk_end = nullptr;
						}
						else {
							// コメントが終わらない
							if (pc) {
								if (pc > head) {
									lines++;
									break;
								}
								*pc = '\0';	// 同じ行から開始してるので開始位置から後ろをクリア
							}
							else {
								// 
								comment++;
								break;
							}
						}
					}
				}
			}
			if (fp != stdin)
				fclose(fp);

			if (lines + comment != total) {
				fprintf(stderr, "%s : total(%lu) != lines(%lu) + comments(%lu)", argv[i], total, lines, comment);
			}
			if (!totalOnly && i < argc) {
				printf("%7lu %7lu %s\n", total, lines, argv[i]);
			}
			allLines += lines;
			allTotal += total;
			if (fp == stdin)
				break;
			fp = nullptr;
		}
	}
	printf("%7lu %7lu%s\n", allTotal, allLines, totalOnly ? "" : " total");

	return 0;
}
