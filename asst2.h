#ifndef ASST2_H
#define ASST2_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

typedef struct treeNode{
	struct treeNode * left;
 	struct treeNode * right;
 	struct fileList * files;
 	char * str;
}treeNode;

 typedef struct fileList{
 	int counter;
 	char * fileName;
 	struct fileList * next;
 }fileList;


void lDestroy(fileList * fl);
void tDestroy(treeNode * head);
void finalOutput(treeNode * head, char * outputFileName);
void printTree(treeNode * head, int fd);
void writeBook(treeNode * head, int fd);
void llSort(fileList * fl);
void swap(fileList * link1, fileList * link2);
int getLen(int x);
char * extract(char * path);
char * mkPath(char * currentPath, char * nextDir);
treeNode * fileIterator(char * name, treeNode * head);
treeNode * tokenize(char * fileContents, treeNode * head, char * currentFile);

#endif
