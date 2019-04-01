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
	struct treeNode *left, *right;
 	struct fileList *files;
 	char *str;
}treeNode;

typedef struct fileList{
	int counter;
	char * fileName;
	struct fileList * next;
}fileList;

typedef struct masterFileList{
	char * fileName;
	char *path;
	struct masterFileList * next;
}masterFileList;

typedef struct MinHeap {
	unsigned size;
  unsigned capacity;
  struct treeNode** array;
}MinHeap;

void lDestroy(fileList * fl);
void tDestroy(treeNode * head);
void finalOutput(treeNode * head, char * outputFileName);
void printTree(treeNode * head, int fd);
void writeBook(treeNode * head, int fd);
void llSort(fileList * fl);
void swap(fileList * link1, fileList * link2);
void sorter(fileList * fl);
int getLen(int x);
char * extract(char * path);
char * mkPath(char * currentPath, char * nextDir);
treeNode * fileIterator(char * dirName, treeNode * head);
treeNode * tokenize(char * fileContents, treeNode * head, char * currentFile);
masterFileList *addPath(const char *path, const char *name, masterFileList *paths);
void printPaths();
unsigned countPaths();
treeNode **toArray(treeNode *head, unsigned size);
treeNode *getLeaf(treeNode *head);

treeNode* newNode(char *data, unsigned freq);
char *HuffmanCodes(unsigned size);
MinHeap* createMinHeap(unsigned capacity);
void swapMinHeapNode(treeNode** a, treeNode** b);
void minHeapify(struct MinHeap* minHeap,unsigned size, int idx);
int isSizeOne(MinHeap* minHeap);
treeNode* extractMin(MinHeap* minHeap);
void insertMinHeap(MinHeap* minHeap, treeNode* minHeapNode);
void buildMinHeap(MinHeap* minHeap);
void printArr(int arr[], int n);
int isLeaf(treeNode* root);
MinHeap* createAndBuildMinHeap(treeNode **arr, int size);
treeNode* buildHuffmanTree(treeNode **arr, int size);
char *printCodes(treeNode* root, int arr[], int top, char *rslt);


char* concat(const char *s1, const char *s2);
char* tabConcat(const char *s1, const char *s2);
char* parseInt(const int num);
unsigned int getLeafCount(treeNode* node);
#endif
