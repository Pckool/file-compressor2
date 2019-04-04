#include "asst2.h"

static masterFileList *paths = NULL;
static treeNode **mhArray = NULL;
static unsigned iCounter = 0;

int main(int argc, char * argv[]){
	if(argc < 3){
		fprintf(stderr, "Error. Not enough arguments.\n");
		return -1;
	}
	if(argc > 5){
		fprintf(stderr, "Error. Too many arguments.\n");
		return -1;
	}
	if((argv[1][0] == '-' && argv[1][1] == 'b' && argv[2][0] == '-' && argv[2][1] == 'R') || (argv[1][0] == '-' && argv[1][1] == 'R' && argv[2][0] == '-' && argv[2][1] == 'b')){
		treeNode * head = NULL;
		head = fileIterator(argv[3], head);
		char *outputf = concat(argv[3], "/codebook");
		finalOutput(head, outputf);
		tDestroy(head);
	}
	if(argv[1][0] == '-' && argv[1][1] == 'd'){
		//decompress stuff goes here
	}
	if(argv[1][0] == '-' && argv[1][1] == 'c'){
		char* dir = argv[2];
		compressFiles(dir);
	}
	return 0;
}

fileList * createLinkNode(char * fileName){
	fileList * temp = (fileList*)malloc(sizeof(fileList));
	temp->counter = 1;
	temp->fileName = strdup(fileName);
	temp->next = NULL;
	return temp;
}

fileList * addToFileList(fileList * fl, fileList * newLink){
	if(fl == NULL){
		fl = newLink;
		return fl;
	}
	else if(strcmp(fl->fileName, newLink->fileName) == 0){
		fl->counter++;
		free(newLink->fileName);
		free(newLink);
		return fl;
	}
	else{
		fl->next = addToFileList(fl->next, newLink);
		return fl;
	}
}

treeNode * createNode(char * newStr){

	treeNode * temp = (treeNode*)malloc(sizeof(treeNode));
	temp->left = NULL;
	temp->right = NULL;
	temp->files = NULL;
	temp->str = strdup(newStr);
	return temp;
}

treeNode * addToTree(treeNode * head, treeNode *newNode, fileList * newLink){
	if(head == NULL){
		head = newNode;
		head->files = addToFileList(head->files, newLink);
		return head;
	}
	else if(strcasecmp(head->str, newNode->str) == 0){
		head->files = addToFileList(head->files, newLink);
		free(newNode->str);
		free(newNode);
		return head;
	}
	else if(strcasecmp(head->str, newNode->str) > 0){
		if(head->left == NULL){
			head->left = newNode;
			head->left->files = addToFileList(head->left->files, newLink);
		}
		else{
			head->left = addToTree(head->left, newNode, newLink);
		}
		return head;
	}
	else if(strcasecmp(head->str, newNode->str) < 0){
		if(head->right == NULL){
			head->right = newNode;
			head->right->files = addToFileList(head->right->files, newLink);
		}
		else{
			head->right = addToTree(head->right, newNode, newLink);
		}
		return head;
	}
	return head;
}

//Frees the Tree since its all dynamic
void tDestroy(treeNode * head){
	if(head == NULL){
		return;
	}
	if(head->left != NULL){
		tDestroy(head->left);
	}
	if(head->right != NULL){
		tDestroy(head->right);
	}
	lDestroy(head->files);
	free(head->str);
	free(head);
	return;
}

//Frees the List since its all dynamic
void lDestroy(fileList * fl){
	if(fl->next != NULL){
		lDestroy(fl->next);
	}
	free(fl->fileName);
	free(fl);
}

//Takes in a string and pulls out a smaller string. Turns a setence into its words basically
char * pullString(int start, int end, int size, char * originalString){
	int x, y;
	char * toReturn = (char*)calloc(size + 1, sizeof(char));
	for(x = 0, y = start; y < end; x++, y++){
		toReturn[x] = originalString[y];
	}
	return toReturn;
}

/**
	* Tokenizes the words that are passed to it.
	*/
treeNode * tokenize(char * fileContents, treeNode * head, char * currentFile){
	if(fileContents == NULL){
		return head;
	}
	char * inputString = fileContents;
	char * tempString;
	int startingPos = -1;
	int endingPos = 0;
	int sizeOfString = 0;
	int  len = 0;
	int  i = 0;
	len = strlen(inputString);
	treeNode * tempNode;
	fileList * tempLink;
	for(i = 0; i <= len; i++){
		//if(isalpha(inputString[i]) == 0 && isdigit(inputString[i]) == 0){
		if(isspace(inputString[i]) != 0){
			if(sizeOfString == 0){
				continue;
			}
			else{
				endingPos = i;
				tempString = pullString(startingPos, endingPos, sizeOfString, inputString);
				tempNode = createNode(tempString);
				tempLink = createLinkNode(currentFile);
				head = addToTree(head, tempNode, tempLink);
				free(tempString);
				startingPos = -1;
				sizeOfString = 0;
			}
		}
		else{
			if(startingPos == -1){
				startingPos = i;
				sizeOfString++;
			}
			else{
				sizeOfString++;
			}
		}
	}
	return head;
}

//Gets ready to output the tree/book
void finalOutput(treeNode * head, char * outputFileName){
	errno = 0;
	int errsv;
	int status = 0;
	int amtToWrite;
	char line[256];
	int fd = open(outputFileName, O_RDONLY);
	close(fd);
	fd = open(outputFileName, O_WRONLY | O_APPEND | O_CREAT, 00700);
	errsv = errno;
	if(errsv == 13){
		fprintf(stderr, "\nYou don't have access to file \"%s\"\n", outputFileName);
		fprintf(stderr, "No output file can be written.\n");
		return;
	}
	if(fd == -1){
		fprintf(stderr, "\nError opening file \"%s\" to write our output.\n", outputFileName);
		return;
	}
	if(head == NULL){
		fprintf(stderr, "\nNo output, empty tree.\n");
		fprintf(stderr, "There were no files in the directory, the files were ");
		fprintf(stderr, "empty or access to the files wasn't grated.\n");
		return;
	}
	// printTree(head, fd); //Change this line to writeBook one you are ready to test the programs ability to make codebooks
	writeBook(head, fd);
	close(fd);
}

/**
	* This is where we write the codebook.
	* @param head The head of the tree created
	* @param fd stands for file descriptor. This is what is used to write the codebook file
	* @return NULL
*/
void writeBook(treeNode * head, int fd){
	fileList * ptr = head->files;
	if(head == NULL){
		fprintf(stderr, "Empty Tree\n");
		fprintf(stderr, "There were no files in the directory, the files were ");
		fprintf(stderr, "empty or access to the files wasn't grated.\n");
		return;
	}
	unsigned numWords = getLeafCount(head);

	toArray(head, numWords);
	HuffmanCodes(numWords, fd);
}

/**
	* Pulls all the data out of a given file designated by path
	*/
char * extract(char * path){
	errno = 0;
	int fd = open(path, O_RDONLY);
	int errsv;
	if(errsv == 13){
		fprintf(stderr, "\nNot allowed to open file \"%s\".\n", path);

		return NULL;
	}
	int fileLength = lseek(fd, 0, SEEK_END);
	int status = 0;
	int amtToRead = fileLength;
	lseek(fd, 0, SEEK_SET);
	if(fd == -1){
		fprintf(stderr, "\nError opening file \"%s\".\n", path);
		return NULL;
	}
	if(fileLength == 0){
		fprintf(stderr, "Error, file \"%s\" is empty.\nMoving to next file in directory.\n", path);
		return NULL;
	}
	char * fileContents = (char*)malloc((sizeof(char) * fileLength) + 1);
	while(amtToRead > 0){
		status = read(fd, fileContents, amtToRead);
		amtToRead -= status;
	}
	fileContents[fileLength] = '\0';
	close(fd);
	return fileContents;
}

/**
	* Prints out the unsorted tree
	*/
void printTree(treeNode * head, int fd){
	fileList * ptr = head->files;
	if(head == NULL){
		fprintf(stderr, "Empty Tree\n");
		fprintf(stderr, "There were no files in the directory, the files were ");
		fprintf(stderr, "empty or access to the files wasn't grated.\n");
		return;
	}
	sorter(ptr);

	if(head->left != NULL){
		printTree(head->left, fd);
	}

	printf("%s\n",head->str);
	ptr = head->files;

	while(ptr != NULL){
		printf("%s %d\n", ptr->fileName, ptr->counter);
		ptr = ptr->next;
	}

	if(head->right != NULL)
	{
		printTree(head->right, fd);
	}
}

/**
	* This function will eventually sort the tree in descending frequency.
	*/
void sorter(fileList * fl){
	fileList * currentLink = fl;
	fileList * iterPtr;
	fileList * max;
	while(currentLink != NULL){
		max = currentLink;
		iterPtr = currentLink->next;
		while(iterPtr != NULL){
			if(max->counter < iterPtr->counter){
				max = iterPtr;
			}
			else if(max->counter == iterPtr->counter){
				if(max->counter < iterPtr->counter){
					max = iterPtr;
				}
			}

			iterPtr = iterPtr->next;
		}

		swap(currentLink, max);
		currentLink = currentLink->next;
	}
}

/**
	* Iterates through a directory opening up the files
	*/
treeNode * fileIterator(char * dirName, treeNode * head){
	DIR * dir;
	struct dirent * entry;
	char * fileContents;
	errno = 0;
	int errsv;
	if((dir = opendir(dirName)) == NULL){
		errsv = errno;
		if(errsv == 2){
			fprintf(stderr, "\nNo such file, directory, or improperly formed path: \"%s\".\n", dirName);
			return head;
		}
		if(errsv == 20){
			fileContents = extract(dirName);
			if(fileContents == NULL){
				return head;
			}
			head = tokenize(fileContents, head, dirName);
			free(fileContents);
			return head;
		}
		return NULL;
	}
	while((entry = readdir(dir))){
		// if it is a directory
		if(entry->d_type == DT_DIR){
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			    continue;
			}
			else{
				char *temp = concat(entry->d_name, "/");
				char * path = concat(dirName, temp);
				free(temp);
				head = fileIterator(path, head);
				free(path);
			}
		}
		// if it is a normal file
		else if(entry->d_type == DT_REG){
			paths = addPath(dirName, entry->d_name, paths);
			printPaths();
			char * path = mkPath(dirName, entry->d_name);
			fileContents = extract(path);
			head = tokenize(fileContents, head, entry->d_name);
			free(fileContents);
			free(path);
		}
		// if it is anything else
		else{
			char * path = mkPath(dirName, entry->d_name);
			fprintf(stderr, "I don't know how to handle this entry. %s\n", path);
			free(path);
		}
	}
	closedir(dir);
	return head;
}

/**
	* makes a pointer to the path that you passed to it
	*/
char * mkPath(char * currentPath, char * nextDir){
	int len = strlen(currentPath)+strlen(nextDir)+2;
	char * path = malloc(len);
	if(strcmp(currentPath, "./") == 0){
		snprintf(path, (len), "%s%s", currentPath, nextDir);
	}
	else{
		snprintf(path, (len), "%s/%s", currentPath, nextDir);
	}
	return path;
}

//Returns the number of digits in an int. Was needed at one point but not anymore. Havent deleted yet incase it is needed again
int getLen(int x){
	int toReturn = 0;
	while(x > 0){
		toReturn++;
		x /= 10;
	}
	return toReturn;
}

/**
	* Swaps the contents of two nodes except there leaves.
	*/
void swap(fileList * link1, fileList * link2){
	char * tempFileName = link1->fileName;
	int tempCounter = link1->counter;
	link1->fileName = link2->fileName;
	link1->counter = link2->counter;
	link2->fileName = tempFileName;
	link2->counter = tempCounter;
}

/**
 	* Find all of the files in the directory, pair up their paths, then put that in a chain. An easy way to keep track of the files found.
	*/
masterFileList *addPath(const char *path, const char *name, masterFileList *pathsHere){
	// the initial path add
	if(pathsHere == NULL){
		pathsHere = malloc(sizeof(masterFileList));
		pathsHere->fileName = (char*)malloc((strlen(name)+1) * sizeof(char));
		pathsHere->path = (char*)malloc((strlen(path)+1) * sizeof(char));
		pathsHere->next = NULL;
		strcpy(pathsHere->path, path);
		strcpy(pathsHere->fileName, name);
		printf("created paths and added %s\n", pathsHere->fileName);
		 return pathsHere;
	}
	else{
		if(pathsHere->next != NULL){
			addPath(path, name, pathsHere->next);
			 return pathsHere;
		}
		else if(pathsHere->next == NULL){
			masterFileList *tempPath = malloc(sizeof(masterFileList));
			tempPath->fileName = (char*)malloc((strlen(name)+1) * sizeof(char));
			tempPath->path = (char*)malloc((strlen(path)+1) * sizeof(char));
			tempPath->next = NULL;
			strcpy(tempPath->path, path);
			strcpy(tempPath->fileName, name);

			pathsHere->next = tempPath;
			printf("%s added to paths\n", pathsHere->next->fileName);

			 return pathsHere;
		}

	}

}

void printPaths(){
	masterFileList *temp = paths;
	int go = 1;
	printf("paths:");
	if(paths != NULL){
		while(go){
			printf(" %s", temp->fileName);
			if(temp->next != NULL){
				temp = temp->next;
				printf(",");
			}
			else{
				printf("\n");
				go = 0;
			}
		}
	}
}

unsigned countPaths(){
	masterFileList *temp = paths;
	int go = 1;
	unsigned count = 0;
	while(go){
		++count;
		if(temp->next != NULL){
			temp = temp->next;
		}
		else{
			go = 0;
		}
	}
	return count;
}

treeNode **toArray(treeNode *head, unsigned size){
	mhArray = (treeNode**)malloc(size * sizeof(treeNode*));
	//free(head);
	getLeaf(head);

}

treeNode *getLeaf(treeNode *head){
	treeNode *temp;
	if(head != NULL && head->left != NULL){
		temp = getLeaf(head->left);
		head->left = NULL;
		getLeaf(head);
		// ++iCounter;

		return temp;
	}
	else if(head != NULL && head->left == NULL){

	}
	if(head != NULL && head->right != NULL){
		temp = getLeaf(head->right);
		head->right = NULL;
		getLeaf(head);
		// ++iCounter;
		return temp;
	}
	else if(head != NULL && head->right == NULL){

	}
	if(head != NULL && head->left == NULL && head->right == NULL){
		mhArray[iCounter] = head;
		++iCounter;
		return head;
	}
}


/************************************************/
// HUFFMAN CODE
/************************************************/


#define MAX_TREE_HT 1000

/**
 	* A utility function allocate a new min heap node with given string and frequency of the string
	*/
treeNode* newNode(char *data, unsigned freq){
	treeNode * temp = (treeNode*)malloc(sizeof(treeNode));
	temp->left = NULL;
	temp->right = NULL;
	temp->files = NULL;
	temp->str = strdup(data);
	fileList *fls = (fileList*)malloc(sizeof(fileList));
	fls->fileName = NULL;
	fls->counter = freq;
	fls->next = NULL;
	temp->files = fls;
  return temp;
}

/**
 	* A utility function to create a min heap of given capacity
	*/
MinHeap* createMinHeap(unsigned capacity){

    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));

    // current size is 0
    minHeap->size = 0;

    minHeap->capacity = iCounter;
    // minHeap->array = (treeNode**)malloc(3 * sizeof(treeNode*));
		minHeap->array = NULL;
    return minHeap;
}

/**
 	* A utility function to swap two min heap nodes
	*/
void swapMinHeapNode(treeNode** a, treeNode** b){

    treeNode* t = *a;
    *a = *b;
    *b = t;
}
/**
 	* The standard minHeapify function.
	*/
void minHeapify(MinHeap* minHeap,unsigned size, int idx){

		int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    if (left < size && minHeap->array[left]->files->counter < minHeap->array[smallest]->files->counter)
      smallest = left;
    if (right < size && minHeap->array[right]->files->counter < minHeap->array[smallest]->files->counter)
      smallest = right;
    if (smallest != idx) {
      swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
      minHeapify(minHeap, size, smallest);
    }
}

/**
 	* A utility function to check if size of heap is 1 or not
	*/
int isSizeOne(MinHeap* minHeap){
    return (minHeap->size == 1);
}

/**
 	* A standard function to extract minimum value node from heap
	*/
treeNode* extractMin(MinHeap* minHeap){

    treeNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, minHeap->size, 0);

    return temp;
}

/**
 	* A utility function to insert a new node to Min Heap
	*/
void insertMinHeap(MinHeap* minHeap, treeNode* minHeapNode){

    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->files->counter < minHeap->array[(i - 1) / 2]->files->counter) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    minHeap->array[i] = minHeapNode;
}

/**
	* build min heap
	*/
void buildMinHeap(MinHeap* minHeap){

    int n = minHeap->size - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i){
      minHeapify(minHeap, minHeap->size, i);
		}
}

/**
 	* Utility function to check if this node is leaf
	*/
int isLeaf(treeNode* root){

    return !(root->left) && !(root->right);
}

/**
	* Creates a min heap of capacity equal to size and inserts all character of data[] in min heap. Initially size of min heap is equal to capacity
	*/
MinHeap* createAndBuildMinHeap(treeNode **arr, int size){

    MinHeap* minHeap = createMinHeap(size);
		int i;
		minHeap->array = mhArray;

    minHeap->size = size;
		printf("building minheap...\n");
    buildMinHeap(minHeap);

    return minHeap;
}

// The main function that builds Huffman tree
treeNode* buildHuffmanTree(treeNode **arr, int size){
    treeNode *left, *right, *top;
		// Make the minheap (array)
    MinHeap* minHeap = createAndBuildMinHeap(mhArray, size);

    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap)) {
        // Find the two minimum freq items from min heap
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        // '$' is a special value for internal nodes, not used
        top = newNode("$", left->files->counter + right->files->counter);

        top->left = left;
        top->right = right;
      insertMinHeap(minHeap, top);
    }

    // tree is complete.
    return extractMin(minHeap);
}

/**
 	* A utility function to print an array of size n
	*/
char *printArr(char arr[], int n, int fd){
	char *send = "";
  int i;
  for (i = 0; i < n; ++i){
		send = charAppend(send, arr[i]);
	}
	return send;
}

/**
	* Prints huffman codes from the root of Huffman Tree. It uses arr[] to store codes
	*/
int printCodes(treeNode* root, char arr[], int top, char *rslt, int fd){

    // Assign 0 to left edge and recur
    if (root->left) {

        arr[top] = '0';
        printCodes(root->left, arr, top + 1, rslt, fd);
    }

    // Assign 1 to right edge and recur
    if (root->right) {

        arr[top] = '1';
        printCodes(root->right, arr, top + 1, rslt, fd);
    }

    /**
		 	* If this is a leaf node, then it contains one of the
			* input characters, print the character and its code from arr[]
			*/
		if (isLeaf(root)) {
			//printf("%s\n", printArr(arr, top));
			char *code = printArr(arr, top, fd);
			rslt = concat(rslt, tabConcat(code, "\t") );


			rslt = concat(rslt, root->str);

			rslt = concat(rslt, "\n\0");
			// printf("%s\n", rslt);
			if(write(fd, rslt, strlen(rslt) ) != strlen(rslt)){
				char * err = "There was an error writing to";
				printf("%s\n", concat(err, root->str));
				return 1;
			}
			free(rslt);
			return 0;
    }
}

/**
 	* The main function that builds a Huffman Tree and print codes by traversing the built Huffman Tree
	* @param head the head of the tree
	* @param fileNames an array of all of the file names in the dir
	* @return NULL
	*/
void HuffmanCodes(unsigned size, int fd){
    // Construct Huffman Tree
    treeNode* root = buildHuffmanTree(mhArray, size);

    // Print Huffman codes using the Huffman tree built above
    char arr[MAX_TREE_HT];
		int top = 0;
		char *rslt = "";

    if(printCodes(root, arr, top, rslt, fd) == 1){

		};
}


/************************************************/
// Compress functions
/************************************************/

void compressFiles(char *dirName){
	wordsList *words = NULL;
		bitDict *dict = findCodebook(words, dirName);
	scrubFiles(dirName, words, dict);
}

wordsList *scrubFiles(char *dirName, wordsList *words, bitDict *dict){

	DIR * dir;
	struct dirent * entry;
	char * fileContents;
	errno = 0;
	int errsv;
	if((dir = opendir(dirName)) == NULL){
		errsv = errno;
		if(errsv == 2){
			fprintf(stderr, "\nNo such file, directory, or improperly formed path: \"%s\".\n", dirName);
			return words;
		}
		if(errsv == 20){
			fileContents = extract(dirName);
			if(fileContents == NULL){
				return words;
			}
			words = tokenize2(fileContents, words, dirName);
			free(fileContents);
			return words;
		}
		return NULL;
	}
	while((entry = readdir(dir))){
		// if it is a directory
		if(entry->d_type == DT_DIR){
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			    continue;
			}
			else{
				char *temp = concat(entry->d_name, "/");
				char * path = concat(dirName, temp);
				free(temp);
				words = scrubFiles(path, words, dict);
				free(path);
			}
		}
		// if it is a normal file
		else if(entry->d_type == DT_REG){
			paths = addPath(dirName, entry->d_name, paths);
			printPaths();
			char * path = mkPath(dirName, entry->d_name);

			fileContents = extract(path);
			words = tokenize2(fileContents, words, entry->d_name);

			// Compress the file found:
			compressFile(dirName, entry->d_name, words, dict);
			free(fileContents);
			free(path);
		}
		// if it is anything else
		else{
			char * path = mkPath(dirName, entry->d_name);
			fprintf(stderr, "I don't know how to handle this entry. %s\n", path);
			free(path);
		}
	}
	closedir(dir);
	return words;

	// if(files->next != NULL){
	// 	compressFiles(files->next);
	// }
}

/**
	* Tokenizes the words that are passed to it.
	*/
wordsList * tokenize2(char * fileContents, wordsList *words, char * currentFile){
	if(fileContents == NULL){
		return words;
	}
	char * inputString = fileContents;
	char * tempString;
	int startingPos = -1;
	int endingPos = 0;
	int sizeOfString = 0;
	int  len = 0;
	int  i = 0;
	len = strlen(inputString);
	wordsList * tempLink;
	for(i = 0; i <= len; i++){
		// if it is not a letter or a digit
		//if(isalpha(inputString[i]) == 0 && isdigit(inputString[i]) == 0){
		if(isspace(inputString[i]) != 0){
			if(sizeOfString == 0){
				continue;
			}
			else{
				endingPos = i;
				tempString = pullString(startingPos, endingPos, sizeOfString, inputString);
				tempLink = createWordLink(tempString);

				words = addToChain(words, tempLink);
				free(tempString);
				startingPos = -1;
				sizeOfString = 0;
			}
		}
		else{
			if(startingPos == -1){
				startingPos = i;
				sizeOfString++;
			}
			else{
				sizeOfString++;
			}
		}
	}
	// printChain(words);
	return words;
}

void compressFile(char *dirName, char *fileName, wordsList *words, bitDict *dict){
	char *comFile = concat(fileName, COMP_EXT);

	printf("New File Name: %s\n", comFile);
	char *temp = malloc(2*sizeof(char));
	strcpy(temp, "");

	// printChain(words);
	char *latestOutput = getCompressed(words, dict, dict, temp);
	printf("DATA: %s\n", latestOutput);
}

char *getCompressed(wordsList *words, bitDict *dict, bitDict *head, char *str){
	// if the current position in dict and the word we are looking at are the same
	if(words != NULL && dict != NULL && strcmp(words->word, dict->token) == 0){
		// add the bit representation from the dict into the string
		printf("Found a match for : %s\n", words->word);
		char *temp = concat(str, dict->bits);
		// free(str);
		printf("Data Currently: %s\n", temp);
		// if there is another word
		if(words->next != NULL)
			return getCompressed(words->next, head, head, temp);
		// if there are no more words left return the string
		else
			return temp;
	}
	// if they aren't
	else if (words != NULL && dict != NULL && strcmp(words->word, dict->token) != 0){
		if(dict->next != NULL)
			return getCompressed(words, dict->next, head, str);
	}
	else{
		printf("I'm here for some reason...\n" );
	}
}


/**
	* This will take in the codebook and tokenize it into a bit dictionary, a structure that will allow easy compressions and decompression
	*/
bitDict * tokenizeCodebook(char * fileContents, bitDict *dict){
	if(fileContents == NULL){
		return dict;
	}
	char * inputString = fileContents;
	char * tempString = "";
	char * tempBit = "";
	int startingPos = -1;
	int endingPos = 0;
	int sizeOfString = 0;
	int  len = 0;
	int  i = 0;
	len = strlen(inputString);
	bitDict * tempLink;
	int side = 0;
	// loop through the contents
	for(i = 0; i <= len; i++){
		// once we find \t then save that as a temp bit and switch side=1
		if(inputString[i] == '\t' && side == 0 && i != 0){
			++i;
			side = 1;
		}
		else if(inputString[i] == '\n' && side == 1 && i != 0){

			tempLink = createDictLink(tempString, tempBit);
			dict = addToDictChain(dict, tempLink);
			free(tempString);
			free(tempBit);
			tempString = "";
			tempBit = "";
			side = 0;
		}
		// if not a \t or a \n then save into one string with charAppend()
		else{
			// looking for bit rep
			if(side == 0){
				tempBit = charAppend(tempBit, inputString[i]);
			}
			// looking for token
			else if(side == 1){
				tempString = charAppend(tempString, inputString[i]);
			}

		}
	}
	// printDictChain(dict);
	return(dict);
}

/**
	* Create a new link for a word chain
	*/
wordsList * createWordLink(char * newStr){

	wordsList * temp = (wordsList*)malloc(sizeof(wordsList));
	temp->next = NULL;
	temp->word = strdup(newStr);
	return temp;
}

/**
	*
	*/
wordsList *addToChain(wordsList *words, wordsList *newLink){
	if(words == NULL){
		return newLink;
	}
	if(words->next == NULL){
		words->next = newLink;
		return words;
	}
	else
		addToChain(words->next, newLink);
		return words;
}

/**
	*
	*/
void printChain(wordsList *words){
	if(words != NULL){
		printf("%s\n", words->word);
		if(words->next != NULL)
			printChain(words->next);
	}
	else
		printf("Null Words List...\n");
}

/**
	* Create a new link for the bit dict chain
	*/
bitDict * createDictLink(char * newStr, char *bits){

	bitDict * temp = (bitDict*)malloc(sizeof(bitDict));
	temp->next = NULL;
	temp->token = strdup(newStr);
	temp->bits = strdup(bits);
	return temp;
}

/**
	*
	*/
bitDict *addToDictChain(bitDict *dict, bitDict *newLink){
	if(dict == NULL)
		return newLink;
	if(dict->next == NULL){
		dict->next = newLink;
		return dict;
	}
	else
		addToDictChain(dict->next, newLink);
		return dict;
}

void printDictChain(bitDict *dict){
	if(dict != NULL){
		printf("%s\t%s\n", dict->bits, dict->token);
		if(dict->next != NULL)
			printDictChain(dict->next);
	}
}

/**
	*
	*/
bitDict *findCodebook(wordsList * words, char * codebookDir){
	errno = 0;
	int errsv;
	int status = 0;
	int amtToWrite;
	char line[256];
	printf("%s\n", concat(codebookDir, "codebook"));
	char *codebookPath = concat(codebookDir, "codebook");
	int fd = open(codebookPath, O_RDONLY);
	bitDict *dict = NULL;
	char *raw_data = extract(codebookPath);
	dict = tokenizeCodebook(raw_data, dict);
	// printDictChain(dict);
	close(fd);
	free(codebookPath);
	return dict;
}

/************************************************/
// Helping functions
/************************************************/

/**
	* a function to concatenate strings
	* @param s1 the first string
	* @param s2 The second string
	* @return result the concatenated string
	*/
char* concat(const char *s1, const char *s2){
	char *result = malloc(strlen(s1) + strlen(s2) +1); // +1 for the null-terminator
	strcpy(result, s1);
  strcat(result, s2);
  return result;
}

/**
	* concat with tab
	* @param s1 the first string
	* @param s2 The second string
	* @return result the concatenated string (MUST BE FREED)
	*/
char* tabConcat(const char *s1, const char *s2){
	char *temp = concat(s1, "\t");
  char *result = malloc(strlen(s1) + strlen(s2) + 2); // +1 for the null-terminator
  strcpy(result, temp);
  strcat(result, s2);
	free(temp);
  return result;
}

/**
	* Take in an int and convert it to a string
	* @param num the int to parse
	* @return str the resulting string. (MUST BE FREED)
	*/
char* parseInt(const int num){
	// printf("%d\n", getLen(num));
	char *str = malloc(20);
	sprintf(str, "%d", num);
	return str;

}

/*
	* Function to get the count of full Nodes in a binary tree
	* @param node the head node of the tree
	* @return count number of nodes in the tree
	*/
unsigned int getLeafCount(treeNode* head){
	int c =  1;
    if (head ==NULL)
        return 0;
    else
    {
        c += getLeafCount(head->left);
        c += getLeafCount(head->right);
        return c;
    }
}

/**
	* Adds char to the string that is being added to the codebook
	* The way I implemented this is actually has the potential to create memory leaks.
	* Thereason is because I can also pass a malloced string, and unless that is handled by the function that calles this, it will just abandon that original malloced string.
	* If this isn't fixed I ran out of time.
	*/
char *charAppend(char str[], char charr){
	char *newStr = malloc(strlen(str)+2);
	strcpy(newStr, str);
	newStr[strlen(str)] = charr;
	newStr[strlen(str)+1] = '\0';
	return newStr;
}

/**
	* get the file extension
	*/
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
