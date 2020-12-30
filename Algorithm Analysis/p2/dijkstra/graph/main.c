#include <stdlib.h>
#include <stdio.h>
#include <graph.h>

enum {
	PROGNAME,
	FILENAME,
	NARGS
};

int main(int argc, char *argv[]) {

	if (argc != NARGS) {
		printf("usage: %s graph.info\n", argv[PROGNAME]);
		exit(0);
	}

	FILE *stream = fopen(argv[FILENAME], "r");
	if (!stream) {
		printf("File not found...\n");
		exit(0);
	}

	GRAPH *graph = graph_read_from_file(stream);
	graph_print(graph);
	graph_free(graph);

	return 0;
}
