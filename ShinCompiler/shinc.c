#if defined _WIN32 || defined __linux__
#else
#error Unsupported platform. 
#endif
#include "shinc.system.h"
#include "shinc.basic.h"
#include "shinc.parser.h"

int wmain(int argc, sh_char* argv[]) {
	argc = 2;
	argv[1] = L"H:\\Projects\\ShinLanguage\\demo.shin";
	sh_setcp();
	if (argc > 3) printf("Warning: Currently only support two argument.s \n");
	if (argc == 1 || STRSAME(argv[1], L"")) {
		printf("Usage: \n");
		printf("   shinc [file [output]]\n");
		printf("      Compile [file], with [output] as the output file. \n");
		return 0;
	}
	sh_uint len = STRLEN(argv[1]) + 4;
	if (argc >= 3) len = MAX(len, STRLEN(argv[2]));
	sh_char* output = NEWSTR(len);
	if (!output) {
		printf("Failed to allocate a new memory space. ");
		goto END;
	}
	if (argc >= 3 && !STRSAME(argv[2], L"")) STRCPY(output, argv[2]);
	else {
		STRCPY(output, argv[1]);
		if (len > 0) {
			sh_uint dot = len - 1;
			for (; output[dot] != L'.' && dot > 0; dot--);
			if (output[dot] != L'.') dot++;
			output[dot] = L'.';
			output[dot + 1] = L's';
			output[dot + 2] = L'h';
			output[dot + 3] = L'c';
			output[dot + 4] = 0;
		}
	}
	printf("Compiling [%S] to [%S]... \n", argv[1], output);
	FILE* fpr = _wfopen(argv[1], L"r");
	if (!fpr) {
		printf("Failed to open [%S]: %s", argv[1], strerror(errno));
		goto FREE_OUTPUT;
	}
	fseek(fpr, 0, SEEK_END);
	fpos_t _file_len;
	fgetpos(fpr, &_file_len);
	sh_sint file_len = _file_len;
	fseek(fpr, 0, SEEK_SET);
	sh_schr* _file_content = NEWLIST(sh_schr, file_len);
	if (!_file_content) {
		printf("Failed to allocate a new memory space. ");
		goto FREE_READ_FILE;
	}
	fread(_file_content, sizeof(sh_schr), file_len, fpr);
	sh_char* file_content = sh_str_convert(_file_content);
	FREE(_file_content);
	if (!file_content) {
		printf("Failed to allocate a new memory space. ");
		goto FREE_READ_FILE;
	}
	printf("%S", file_content);
	sh_uint byte_len;
	sh_byte* byte_content = sh_code_compile(file_content, &byte_len);
	if (!file_content) {
		printf("Failed to compile. ");
		goto FREE_CONTENT;
	}
	FILE* fpw = _wfopen(output, L"w");
	fwrite(byte_content, sizeof(sh_byte), byte_len, fpw);
FREE_WRITE_FILE:
	fclose(fpw);
FREE_CODE_BYTES:
	FREE(byte_content);
FREE_CONTENT:
	FREE(file_content);
FREE_READ_FILE:
	fclose(fpr);
FREE_OUTPUT:
	FREE(output);
END:
	return 0;
}