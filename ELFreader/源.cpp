
//main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
//#include <sys/ioctl.h>
#include "elf_head.h"
static void help()
{
	puts("******************************************");
	puts("*     ELF header reader and modifier     *");
	puts("*     0 - exit                           *");
	puts("*     1 - print header info              *");
	puts("*     2 - modify elf header              *");
	puts("******************************************");
}
int main(int argc, char **argv)
{
	char cmd;
	elf_header ehdr;
	if (argc != 2)
	{
 		printf("Usage: %s [filename]n", strchr(argv[0], '/') + 1);
		return 0;
	}
	switch (read_elf_header(&ehdr, argv[1]))//argv[1]是从命令行输入的目标文件的地址
	{
	case FOPEN_FAILED:
		printf("Can not open %s.n", argv[1]);
		goto out;
	case FORMAT_ERROR:
		printf("%s is not an elf format file.n", argv[1]);
		goto out;
	}
	while (cmd != '0')
	{
		help();
		cmd = getchar();
		switch (cmd)
		{
		case '1':
			getchar();//由于终端不是非标准模式，所以输入字符后都要使用回车，回车符本身会当作输入的字符，这里使用getchar()目的就是使下次获得输入的字符时不为回车符
			print_elfhdr_info(&ehdr);
			break;
		case '2':
			getchar();
			modify_elfhdr(&ehdr, argv[1]);
			break;
		default:
			break;
		}
	}
out:
	return 0;
}