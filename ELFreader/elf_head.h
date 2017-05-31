//elf_head.h
#ifndef ELF_HEAD_H
#define ELF_HEAD_H
#define MAGIC "177ELF"
#define INVAL 0
#define INFO_POS 40
#define FOPEN_FAILED -1
#define FORMAT_ERROR -2
#define SUCCESS 0
#define MODTIP() do{puts("tEnter new value:");}while(0)
#define CUR_FTYPE_MAX 4
#define CUR_ARCH_MAX 40
typedef struct elf_head {
	char magic[4]; //Magic Number
	char addr_width; //Class 1 - 32bits(ELF32);2 - 64bits
	char byteorder; //Byte order 1 - little-endian;2 - big-endian
	char hversion; //Header version 1
	char pad[9]; //Padding bytes
	short filetype; //1 - relocatable;2 - executable;3 - shared object;4 - core-image
	short archtype; //Architecture 2 - SPARC;3 - x86;4 - 68K
	int fversion; //File version 1
	int entry;//Entry point if executable
	int phdrpos; //Program header position
	int shdrpos; //Section header position
	int flags; //Architecture relative
	short hdrsize; //ELF header size
	short phdrent;//Size of program headers
	short phdrcnt;//Number of program headers
	short shdrent;//Size of section headers
	short shdrcnt;//Number of section headers
	short strsec;//Section header string table index
}elf_header;
int read_elf_header(elf_header *ehdr, char *filename); //读取文件头信息
int print_elfhdr_info(elf_header *ehdr); //打印相关信息
int modify_elfhdr(elf_header *ehdr, char *filename); //修改文件头
#endif /*ELF_HEAD_H*/