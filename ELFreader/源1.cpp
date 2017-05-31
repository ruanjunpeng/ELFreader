#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include<memory.h>
#include "elf_head.h"

#define bzero(a, b) memset(a, 0, b)//bzero在linux下用


static char *file_type[] = { //文件类型字段的描述字符串数组
	"Unknown",
	"Relocatable",
	"Executable",
	"Shared object",
	"Core imgae"
};
static char *arch_type[] = { //硬件平台的描述字符串数组
	"Unknown",
	"Reserved",
	"SPARC",
	"x86",
	"68K",
	" ",  //5
	" ",
	" ",
	" ",
	" ",
	" ",  //10
	" ",
	" ",
	" ",
	" ",
	" ", //15
	" ",
	" ",
	" ",
	" ",
	" ",  //20
	" ",
	" ",
	" ",
	" ",
	" ", //25
	" ",
	" ",
	" ",
	" ",
	" ", //30
	" ",
	" ",
	" ",
	" ",
	" ",  //35
	" ",
	" ",
	" ",
	" ",
	"ARM",   //40
};
//Check if the file is an elf format file
static int is_elf(elf_header *ehdr)
{
	if (strncmp(ehdr->magic, MAGIC, strlen(MAGIC)) == 0)
		return 1;
	else
		return 0;
}
int read_elf_header(elf_header *ehdr, char *filename)
{
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL)
		return FOPEN_FAILED;
    bzero(ehdr, sizeof(elf_header));
	fread(ehdr, sizeof(elf_header), 1, fp);
	if (strncmp(ehdr->magic, MAGIC, strlen(MAGIC)) != 0)
	{
		fclose(fp);
		return FORMAT_ERROR;
	}
	fclose(fp);
	return SUCCESS;
}
static void put_tip(char *tip)
{
	printf(" %s", tip);
	printf("33[%dC", INFO_POS - strlen(tip)); //每次输出第二列信息，即读出的数据的时候，先向右移动光固定个单位，保证对齐。详见ANSI控制码表 
}
int print_elfhdr_info(elf_header *ehdr)
{
	if (strncmp(ehdr->magic, MAGIC, strlen(MAGIC)) != 0)
	{
		puts("Not an elf format file.");
		return FORMAT_ERROR;
	}
	puts("ELF header:");
	put_tip("Magic:");
	printf("%2x %2x %2x %2x (%c%c%c%c)n"
		, ehdr->magic[0], ehdr->magic[1], ehdr->magic[2], ehdr->magic[3]
		, ehdr->magic[0], ehdr->magic[1], ehdr->magic[2], ehdr->magic[3]);
	put_tip("Class:");
	if (ehdr->addr_width == 1)
		printf("ELF32n");
	if (ehdr->addr_width == 2)
		printf("ELF64n");
	put_tip("Byte order:");
	if (ehdr->byteorder == 1)
		printf("Little-endiann");
	else
		printf("Big-endiann");
	put_tip("Version:");
	printf("0x%02xn", ehdr->hversion);
	put_tip("File type:");
	if (ehdr->filetype > CUR_FTYPE_MAX)
		ehdr->filetype = 0;
	puts(file_type[ehdr->filetype]);
	put_tip("Machine:");
	if (ehdr->archtype > CUR_ARCH_MAX)
	{
		ehdr->archtype = 0;
	}
	puts(arch_type[ehdr->archtype]);
	put_tip("File version");
	printf("0x%xn", ehdr->fversion);
	put_tip("Entry point address:");
	printf("0x%xn%", ehdr->entry);
	put_tip("Start of program headers:");
	printf("%d (bytes into file)n", ehdr->phdrpos);
	put_tip("Start of section headers:");
	printf("%d (bytes into file)n", ehdr->shdrpos);
	put_tip("flags:");
	printf("%d(0x%x)n", ehdr->flags, ehdr->flags);
	put_tip("Size of this header:");
	printf("%d bytesn", ehdr->hdrsize);
	put_tip("Size of program headers:");
	printf("%d bytesn", ehdr->phdrent);
	put_tip("Number of program headers:");
	printf("%dn", ehdr->phdrcnt);
	put_tip("Size of section headers:");
	printf("%d bytesn", ehdr->shdrent);
	put_tip("Number of section headers:");
	printf("%dn", ehdr->shdrcnt);
	put_tip("Section header string table index:");
	printf("%d(0x%x)n", ehdr->strsec, ehdr->strsec);
	return SUCCESS;
}
static void mod_help()
{
	puts("Select the part you want to modify:");
	puts("1 -- Magic number.");
	puts("2 -- Class(1:ELF32 2:ELF64).");
	puts("3 -- Byteorder(1:Little-endian 2:Big-endian).");
	puts("4 -- File type(1:relocatable 2:executable 3:shared object 4:core-image)");
	puts("s -- save");
	puts("q -- quit without saving");
	puts("x -- save and quit");
}
static int atoh(char *buf)//将16进制形式的字符串转换成数值
{
	int tmp = 0;
	while (*buf != '\0')
	{
		if (*buf >= '0' && *buf <= '9')
			tmp = tmp * 16 + *buf - '0';
		else
		{
			if ((*buf >= 'a' && *buf <= 'f'))
				tmp = tmp * 16 + *buf - 'a' + 10;
			else
			{
				if ((*buf >= 'A' && *buf <= 'F'))
					tmp = tmp * 16 + *buf - 'A' + 10;
				else
					return INVAL;
			}
		}
		buf++;
	}
	return tmp;
}
//以下是修改ELF文件头的函数， 只为了测试，如果随意修改会使文件无法使用 
static void mod_magic(elf_header *ehdr)//修改MAGIC号
{
	int i;
	int magic;
	int cur_val;
	char buf[20];
	char *tmp;
	printf("tOld magic number: %x %x %x %xn", ehdr->magic[0], ehdr->magic[1], ehdr->magic[2], ehdr->magic[3]);
	MODTIP();
	puts("te.g: 0a0b0c0d or 0A0B0C0D");
	puts("tEnter new value");
	bzero(buf, 20);
	putchar('t');
	fgets(buf, 19, stdin);
	tmp = buf;
	while (*tmp != '\0')
	{
		if ((*tmp >= '0' && *tmp <= '9') || (*tmp >= 'a' && *tmp <= 'f') || (*tmp >= 'A' && *tmp <= 'Z'))
		{
			*(tmp + 8) = 0;
			magic = atoh(tmp);
			break;
		}
		tmp++;
	}
	//如果输入的是0x11223344，那么存在文件上就是0x44332211(小端模式)，而访问magic字段的时候是以char型来访问，所以访问顺序正好相反，这里要交换字节序存入文件。 
	tmp = (char *)(&magic) + 3;
	for (i = 0; i < 4; i++)
		ehdr->magic[i] = *(tmp - i);

}

static void mod_class(elf_header *ehdr) //修改地址宽度，32位或是64位 
{
	char opt[2];
	printf("tOld class: %d nt1:ELF32 nt2:ELF64n", ehdr->addr_width);
	MODTIP();
	while (1)
	{
		opt[0] = getchar();
		opt[1] = getchar();
		if (opt[0] > '2' || opt[0] < '1')
			puts("tWrong choice.Value should be 1 or 2");
		else
			break;
	}
	ehdr->addr_width = opt[0] - '0';
}
static void mod_byteorder(elf_header *ehdr) //修改字节序
{
	char opt[2];
	printf("tOld byte order: %d nt1:Little-endian nt2:Big-endiann", ehdr->byteorder);
	MODTIP();
	while (1)
	{
		opt[0] = getchar();
		opt[1] = getchar();
		if (opt[0] > '2' || opt[0] < '1')
			puts("tWrong choice.Value should be 1 or 2");
		else
			break;
	}
	ehdr->byteorder = opt[0] - '0';
}
static void mod_filetype(elf_header *ehdr)//修改文件类型
{
	char opt[2];
	printf("tOld file type: %d nt1:Relocatable nt2:Executable nt3:Shared object nt4:Core imagen)", ehdr->filetype);
	MODTIP();
	while (1)
	{
		opt[0] = getchar();
		opt[1] = getchar();
		if (opt[0] > '4' || opt[0] < '1')
			puts("tWrong choice.Value should be 1 or 2");
		else
			break;
	}
	ehdr->filetype = opt[0] - '0';
}
int modify_elfhdr(elf_header *ehdr, char *filename)
{
	char opt[2];
	FILE *fp;
	//      elf_header ehdr_backup;
	fp = fopen(filename, "r+");
	if (fp == NULL)
		return FOPEN_FAILED;

	fread(ehdr, sizeof(elf_header), 1, fp);
	if (!is_elf(ehdr))
	{
		puts("Not an elf format file.");
		return FORMAT_ERROR;
	}
	//      memcpy(&ehdr_backup,ehdr,sizeof(elf_header));
	while (1)
	{
		mod_help();
		opt[0] = getchar();
		opt[1] = getchar();
		switch (opt[0])
		{
		case '1':
			mod_magic(ehdr);
			break;
		case '2':
			mod_class(ehdr);
			break;
		case '3':
			mod_byteorder(ehdr);
			break;
		case '4':
			mod_filetype(ehdr);
			break;
		case 's':
			fseek(fp, 0, SEEK_SET);
			fwrite(ehdr, sizeof(elf_header), 1, fp);
			break;
		case 'q':
			goto out;
			break;
		case 'x':
			fseek(fp, 0, SEEK_SET);
			fwrite(ehdr, sizeof(elf_header), 1, fp);
			goto out;
			break;
		default:
			puts("Wrong choice.");
		}
	}
out:
	fclose(fp);
	return SUCCESS;
}