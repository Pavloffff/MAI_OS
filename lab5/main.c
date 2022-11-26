#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tree.h"

int main(int argc, char const *argv[])
{	
	printf("\nWrite:\nINSERT to insert elem in tree\n");
	printf("DELETE to delete elem from tree\n");
	printf("PRINT to show tree\n");
	printf("HELP to see commands again\n");
	printf("BYE to exit\n\n");
	node *t = NULL;
	char str[100];
	int elem;
	char *choose;
	bool flag = true;
	while (flag) {
		choose = gets_(str);
		if (!feof(stdin)) {
			if (!strcmp(choose, "INSERT")) {
				printf("Write number to insert it to tree:\n");
				scanf("%d", &elem);
				if (!feof(stdin)) {
					t = insert(t, elem);
				}
			} else if (!strcmp(choose, "DELETE")) {
				printf("Write number to delete it from tree:\n");
				scanf("%d", &elem);
				if (!feof(stdin)) {
					t = delete(t, elem);
				}
			} else if (!strcmp(choose, "PRINT")) {
				printf("\n");
				print(t, 0);
			} else if (!strcmp(choose, "BYE")) {
				flag = false;	
			} else if (!strcmp(choose, "HELP")) {
				printf("\nWrite:\ninsert to insert elem in tree\n");
				printf("DELETE to delete elem from tree\n");
				printf("PRINT to show tree\n");
				printf("HELP to see commands again\n");
				printf("BYE to exit\n\n");
			} else {
				continue;
			}
		} else {
			break;
		}
	}
	return 0;
}
