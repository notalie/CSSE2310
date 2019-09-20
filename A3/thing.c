#include <stdio.h>
#include <stdlib.h>

void zero_out_board(int* thing) {
	for (int i = 0; i < 8; i++) {
		thing[i] = 0;
	}
}

int main() {
	int* thing = malloc(sizeof(int) * 8);
	for (int i = 0; i < 8; i++) {
		thing[i] = i;
	}

	for (int i = 0; i < 8; i++) {
		printf("%d", thing[i]);
		if (i == 7) {
			printf("\n");
		}
	}
	zero_out_board(thing);
	for (int i = 0; i < 8; i++) {
		printf("%d", thing[i]);
		if (i == 7) {
			printf("\n");
		}
	}

}