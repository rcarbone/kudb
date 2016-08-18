#include <stdio.h>
#include <unistd.h>

#define TIME 10

int main(int argc, char *argv[])
{
	int i;
	for (i = 0; i != TIME; ++i) {
		fprintf(stdout, "-- stdout: %d\n", i);
		fprintf(stderr, "-- stderr: %d\n", i);
		sleep(1);
	}
	fprintf(stdout, "-- stdout: END\n");
	fprintf(stderr, "-- stderr: END\n");
	return 0;
}
