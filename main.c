#include "commons.h"

int main(int argc, char* argv[])
{
	int ch;
	opterr = 0;
	while ((ch = getopt(argc, argv, "a:bcde")) != -1)	
	{
		switch (ch)
		{
			case 'a':
				printf("option a: %s\n", optarg);
				break;
			case 'b':
				printf("option b\n");
				break;
			default:
				printf("%c\n", ch);
				break;	
		}
	}
	printf("optopt %c", optopt);
	return 0;
	
}
