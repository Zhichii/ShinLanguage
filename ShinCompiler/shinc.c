#include "basic.h"

int wmain(int argc, sh_char* argv[]) {
	if (argc > 3) printf("Warning: Currently only support two argument.s \n");
	if (argc == 1 || STRSAME(argv[1], L"")) {
		printf("Usage: \n");
		printf("   shinc [file [output]]\n");
		printf("      Compile [file], with [output] as the output file. \n");
		return 0;
	}
	else {
		sh_uint len = STRLEN(argv[1])+4;
		printf("%lld\n", len);
		if (argc >= 3) len = MAX(len, STRLEN(argv[2]));
		sh_char* output = NEWSTR(len);
		if (argc >= 3 && !STRSAME(argv[2], L"")) STRCPY(output, argv[2]);
		else {
			STRCPY(output, argv[1]);
			if (len > 0) {
				sh_uint dot = len-1;
				for (; output[dot]!=L'.' && dot > 0; dot--);
				if (output[dot] != L'.') dot++;
				output[dot] = L'.';
				output[dot+1] = L's';
				output[dot+2] = L'h';
				output[dot+3] = L'c';
				output[dot+4] = 0;
			}
		}
		printf("Compiling [%S] to [%S]... \n", argv[1], output);
		FILE* fp = _wfopen(argv[1], L"r");
		if (!fp) {
			printf("Failed to open [%S]: %s", argv[1], strerror(errno));
			return 1;
		}

		fclose(fp);
		FREE(output);
		return 0;
	}
}