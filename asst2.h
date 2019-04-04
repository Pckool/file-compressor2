#ifndef ASST2_H
#define ASST2_H

#define COMP_EXT ".hcz"

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

typedef struct wordsList{
	char *word;
	struct wordsList *next;
}wordsList;

typedef struct bitDict{
	char *token;
	char *bits;
	struct bitDict *next;
}bitDict;

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
void HuffmanCodes(unsigned size, int fd);
MinHeap* createMinHeap(unsigned capacity);
void swapMinHeapNode(treeNode** a, treeNode** b);
void minHeapify(MinHeap* minHeap,unsigned size, int idx);
int isSizeOne(MinHeap* minHeap);
treeNode* extractMin(MinHeap* minHeap);
void insertMinHeap(MinHeap* minHeap, treeNode* minHeapNode);
void buildMinHeap(MinHeap* minHeap);
char *printArr(char arr[], int n, int fd);
int isLeaf(treeNode* root);
MinHeap* createAndBuildMinHeap(treeNode **arr, int size);
treeNode* buildHuffmanTree(treeNode **arr, int size);
int printCodes(treeNode* root, char arr[], int top, char *rslt, int fd);

void compressFiles(char *dirName);
wordsList *scrubFiles(char *dirName, wordsList *words, bitDict *dict);
wordsList * tokenize2(char * fileContents, wordsList *words, char * currentFile);
wordsList * createWordLink(char * newStr);
wordsList *addToChain(wordsList *words, wordsList *newLink);
void printChain(wordsList *words);
bitDict *findCodebook(wordsList * words, char * outputFileName);
bitDict *addToDictChain(bitDict *dict, bitDict *newLink);
bitDict * createDictLink(char * newStr, char *bits);
void printDictChain(bitDict *dict);
bitDict * tokenizeCodebook(char * fileContents, bitDict *dict);

void compressFile(char *dirName, char *fileName, wordsList *words, bitDict *dict);
char *getCompressed(wordsList *words, bitDict *dict, bitDict *head, char *str);

char* concat(const char *s1, const char *s2);
char* tabConcat(const char *s1, const char *s2);
char* parseInt(const int num);
unsigned int getLeafCount(treeNode* node);
char *charAppend(char str[], char charr);
const char *get_filename_ext(const char *filename);
#endif
