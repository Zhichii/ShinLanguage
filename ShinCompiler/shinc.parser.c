#include "shinc.parser.h"

sh_byte* sh_code_parse(sh_char* sentence) {

}

sh_byte* sh_code_compile(sh_char* file, sh_uint* plen) {
	sh_uint len = STRLEN(file);
	sh_byte* byte_code = NEWLIST(sh_byte, len); // Minium requirement. 
	*plen = 0;
	*plen++;
	return byte_code;
}