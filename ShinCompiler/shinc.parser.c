#include "shinc.parser.h"

ENUM(sh_error) {
	SH_UNMATCHED_ROUND
};
ENUM(sh_warning) {
	SH_INVALID_ESCAPE
};
void	sh_code_error(sh_char*line, sh_uint num, sh_uint pos, sh_error msg) {
	printf("Line %lld \n%S\n", num, line);
	if (pos != 0) {
		for (sh_uint i = 0; i < pos-1; i++) {
			switch (line[i]) {
			case L'\t':
			case L'\b': printf("%c", line[i]); break;
			default: printf(" "); break;
			}
		}
	}
	printf("^");
	printf("\nError: ");
	switch (msg) {
	case SH_UNMATCHED_ROUND: {
		printf("Unmatched round");
	} break;
	}
	printf(". \n");
}
void	sh_code_warning(sh_char*line, sh_uint num, sh_uint pos, sh_warning msg) {
	printf("Line %lld \n%S\n", num, line);
	if (pos != 0) {
		for (sh_uint i = 0; i < pos-1; i++) {
			switch (line[i]) {
			case L'\t':
			case L'\b': printf("%c", line[i]); break;
			default: printf(" "); break;
			}
		}
	}
	printf("^");
	printf("\nWarning: ");
	switch (msg) {
	case SH_INVALID_ESCAPE: {
		printf("Invalid escape");
	} break;
	}
	printf(". \n");
}

void	sh_code_split(sh_char* code, sh_char* words) {
	sh_uint len = STRLEN(code);
	sh_uint line_num = 0, line_pos = 0;
	sh_char* line = NEWLIST(sh_char, 1024);
	sh_uint esc = 0, esc_left = 0;
	sh_char esc_store = 0;
	sh_bool in_str = false;
	sh_bool in_comment1 = 0;
	sh_bool in_commentn = 0;
	sh_uint par_cnt = 0; // Parenthese
	sh_uint brk_cnt = 0; // Bracket
	sh_uint brc_cnt = 0; // Brace
	sh_uint y = 0, x = 0;
	sh_char cur_chr = 0;
	sh_char lst_chr = 0;
	sh_char pending_chr = 0;
	{
		line_pos = 0;
		sh_uint k = 0;
		for (; k < len && code[k] != L'\n'; k++)
			line[k] = code[k];
		line[k] = 0;
		line_num++;
	}
	for (sh_uint i = 0; i < len; i++) {
		line_pos++;
		lst_chr = cur_chr;
		cur_chr = code[i];
		pending_chr = cur_chr;
		if (cur_chr == L'\n') {
			line_pos = 0;
			sh_uint k = i+1;
			for (; k < len && code[k] != L'\n'; k++)
				line[k-i-1] = code[k];
			line[k-i-1] = 0;
			line_num++;
		}
		if (in_str) {
			switch (esc) {
			case 0: {
				if (cur_chr == L'\\') {
					esc = 1;
					goto CONTINUE;
				}
				words[y*512+x] = pending_chr;
				x++;
				if (cur_chr == L'"') {
					in_str = false;
					goto CONTINUE;
				}
			} break;
			case 1: {
				sh_uint flag = true;
				switch (cur_chr) {
				case L'a': { pending_chr = L'\a'; } break;
				case L'b': { pending_chr = L'\b'; } break;
				case L'f': { pending_chr = L'\f'; } break;
				case L'n': { pending_chr = L'\n'; } break;
				case L'r': { pending_chr = L'\r'; } break;
				case L't': { pending_chr = L'\t'; } break;
				case L'v': { pending_chr = L'\v'; } break;
				case L'\\': { pending_chr = L'\\'; } break;
				case L'\'': { pending_chr = L'\''; } break;
				case L'x': {
					esc = 2;
					esc_left = 2;
					flag = false;
				} break;
				case L'0':
				case L'1':
				case L'2':
				case L'3':
				case L'4':
				case L'5':
				case L'6':
				case L'7': {
					esc = 3;
					esc_left = 2;
					esc_store = cur_chr - L'0';
					flag = false;
				} break;
				default: {
					sh_code_warning(line, line_num, line_pos, SH_INVALID_ESCAPE);
					words[y*512+x] = L'\\';
					x++;
					words[y*512+x] = cur_chr;
					x++;
					esc = 0;
					flag = false;
				} break;
				}
				if (flag) {
					words[y*512+x] = pending_chr;
					x++;
					esc = 0;
				}
				goto CONTINUE;
			} break;
			case 2: {
				esc_store *= 16;
				esc_left--;
				if (cur_chr == L'0');
				else if (cur_chr >= L'1' && cur_chr <= L'9') esc_store += cur_chr - L'0';
				else if (cur_chr >= L'A' && cur_chr <= L'F') esc_store += cur_chr - L'A' + 10;
				else if (cur_chr >= L'a' && cur_chr <= L'f') esc_store += cur_chr - L'a' + 10;
				else esc_left = 0;
				if (esc_left == 0) {
					pending_chr = esc_store;
					esc = 0;
					words[y*512+x] = pending_chr;
					x++;
					esc_store = 0;
				}
				goto CONTINUE;
			} break;
			case 3: {
				esc_store *= 8;
				esc_left--;
				if (cur_chr == L'0');
				else if (cur_chr >= L'1' && cur_chr <= L'7') esc_store += cur_chr - L'0';
				else esc_left = 0;
				if (esc_left == 0) {
					pending_chr = esc_store;
					esc = 0;
					words[y*512+x] = pending_chr;
					x++;
					esc_store = 0;
				}
				goto CONTINUE;
			} break;
			}
		}
		else if (in_comment1) {
			if (cur_chr == L'\n') in_comment1 = false;
			goto CONTINUE;
		}
		else {
			if (cur_chr == L'"') in_str = true;
			if (lst_chr == L'/') {
				if (cur_chr == L'/') in_comment1 = true;
				else if (cur_chr == L'*') in_commentn = true;
				else {
					words[y*512+x] = L'/';
					x++;
					words[y*512+x] = pending_chr;
					x++;
				}
			}
			if (cur_chr == L'/') goto CONTINUE;
			if (cur_chr == L'(') par_cnt++;
			if (cur_chr == L')') {
				if (par_cnt == 0) {
					sh_code_error(line, line_num, line_pos, SH_UNMATCHED_ROUND); goto BREAK;
					goto BREAK;
				}
				else par_cnt--;
			}
			if (cur_chr == L'[') brk_cnt++;
			if (cur_chr == L']') {
				if (brk_cnt == 0) {
					sh_code_error(line, line_num, line_pos, SH_UNMATCHED_ROUND); goto BREAK;
					goto BREAK;
				}
				else brk_cnt--;
			}
			if (cur_chr == L'{') {
				brc_cnt++;
				words[y*512+x] = pending_chr;
				y++; x=0;
				goto CONTINUE;
			}
			if (cur_chr == L'}') {
				if (brc_cnt == 0) {
					sh_code_error(line, line_num, line_pos, SH_UNMATCHED_ROUND); goto BREAK;
					goto BREAK;
				}
				else {
					brc_cnt--;
					y++; x=1;
					words[y*512+0] = pending_chr;
					goto CONTINUE;
				}
			}
			if (brc_cnt != 0) {
				if (cur_chr == L' ' ||
					cur_chr == L'\t' ||
					cur_chr == L'\r' ||
					cur_chr == L'\n') {
					if (lst_chr != L' ' &&
						lst_chr != L'\t' &&
						lst_chr != L'\r' &&
						lst_chr != L'\n' &&
						x != 0) {
						words[y*512+x] = L' ';
						x++;
					}
					goto CONTINUE;
				}
			}
			else if (par_cnt == 0 && brk_cnt == 0) {
				if (cur_chr == L' ' ||
					cur_chr == L'\t' ||
					cur_chr == L'\r' ||
					cur_chr == L'\n') {
					if (lst_chr != L' ' &&
						lst_chr != L'\t' &&
						lst_chr != L'\r' &&
						lst_chr != L'\n' &&
						x != 0) {
						y++; x=0;
					}
					goto CONTINUE;
				}
				else if (cur_chr == L',' || cur_chr == L'.') {
					y++; x=0;
					words[y*512+x] = cur_chr;
					x++;
					goto CONTINUE;
				}
			}
			words[y*512+x] = pending_chr;
			x++;
		}
	CONTINUE:;
	}
	if (par_cnt || brk_cnt || brc_cnt) {
		sh_code_error(line, line_num, line_pos+1, SH_UNMATCHED_ROUND);
		goto BREAK;
	}
BREAK:
	FREE(line);
}

sh_byte*sh_code_compile(sh_char* code, sh_uint* plen) {
	sh_uint len = STRLEN(code);
	sh_byte* byte_code = NEWLIST(sh_byte, len); // Minium requirement. 
	*plen = 0;
	*plen++;
	sh_char* words = NEWLIST(sh_char, 32768*256); // 16 MB
	sh_code_split(code, words);
	FREE(words);
	return byte_code;
}