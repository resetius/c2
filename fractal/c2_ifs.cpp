/*$Id$*/

/* Copyright (c) 2008 Alexey Ozeritsky (Алексей Озерицкий)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Alexey Ozeritsky.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
