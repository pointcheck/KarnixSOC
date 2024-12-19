#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int line_counter = 0;

	while(!feof(stdin)) {
		size_t line_size = 0;
		char *line = NULL;
		if(getline(&line, &line_size, stdin) < 0)
			break;
		unsigned int l = strtol(line, NULL, 16);	
		//fprintf(stdout, "XXX: 0x%08X\n", l);
		write(1, &l, 4);
		free(line);
		line_counter++;
	}

	fprintf(stderr, "hexx2bin: total lines processed = %d\n", line_counter);
}
