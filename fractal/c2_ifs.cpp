#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void usage(const char * n)
{
	printf("usage:\n");
	printf("%s [-f filename] [-t type] [-l order] \n", n);
	printf("-f -- read lsystem from file (stdin - default)\n");
	printf("-t -- points save type: \n");
	printf("\t\t png -- PNG\n");
	printf("\t\t default -- txt (for vizualizer)\n");
	printf("-l -- default order \n");
	exit(0);
}

int main(int argc, char * argv[])
{
	FILE * f  = 0;
	int level = 0;
	int type  = 0; //txt

	for (uint i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-f") && i < argc - 1) {
			f = fopen(argv[i + 1], "r");
		} else if (!strcmp(argv[i], "-l") && i < argc - 1) {
			level = atoi(argv[i + 1]);
		} else if (!strcmp(argv[i], "-t") && i < argc - 1) {
			if (!strcmp(argv[i + 1], "png")) {
				type = 2;
			}			
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0]);
		}
	}

	return 0;
}
