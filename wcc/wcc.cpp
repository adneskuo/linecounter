#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
	char buf[65535];

	if (argc == 1) {
		fprintf(stderr, "usage: %s <C-sourcefile> ...\n", argv[0]);
		return 1;
	}

	size_t allLines = 0, allTotal = 0;
	for (int i = argbegin; i <= argc; i++) {
		if (fp == nullptr && i < argc)
			fopen_s(&fp, argv[i], "rt");
		if (fp) {
			size_t total = 0;
			size_t lines = 0;
			size_t comment = 0;	// コメント行トータル
			bool incomment = false;
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
						pc = strstr(p, "/*");
						if (char *pc2 = strstr(p, "//")) {
							if (pc == nullptr || pc2 < pc) {	// 先に//があるのでそれ以降は削除
								*pc2 = '\0';
								continue;
							}
						}
						if (pc) {
							incomment = true;
							pc[0] = pc[1] = ' ';
							p = pc + 2;
						}
						else {
							lines++;
							break;
						}
					}
					if (incomment) {
						if (char *pce = strstr(p, "*/")) {
							// コメントが終わった
							if (pc) {
								memset(pc, ' ', pce - pc + 2);	// 行内の部分コメントをスペースで埋める
								pc = nullptr;
							}
							else {
								memset(p, ' ', pce - p + 2);	// 行の先頭からスペースで埋める
							}
							incomment = false;
							p = pce + 2;
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
				fprintf(stderr, "%s : total(%lu) != lines(%lu) + comments(%lu)", total, lines, comment);
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
