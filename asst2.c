#include "asst2.h"

int main(int argc, char * argv[]){
	if(argc != 3){
		fprintf(stderr, "Error, incorrect number of arguments.\n");
		return -1;
	}
	treeNode * head = NULL;
	// argv[1] should be the output file
	// argv[2] should be the input file
	head = fileIterator(argv[2], head);
	finalOutput(head, argv[1]);
	tDestroy(head);
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

//Tokenizes the words that are passed to it.
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
		if(isalpha(inputString[i]) == 0 && isdigit(inputString[i]) == 0){
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

	sorter(ptr);

	if(head->left != NULL){
		writeBook(head->left, fd);
	}

	// printf("%s\n",head->str);
	ptr = head->files;

	while(ptr != NULL){
		// printf("%s %d\n", ptr->fileName, ptr->counter);

		char * line = tabConcat(ptr->fileName, head->str);
		char * fullLine = concat(line, "\n");
		// printf("line: %s\tsize: %i\n", line, strlen(line));

		if(write(fd, fullLine, strlen(line)+1 ) != strlen(line)+1){
			char * err = "There was an error writing to\n";
			printf("%s\n", concat(err, ptr->fileName));
		}
		free(line);
		free(fullLine);
		ptr = ptr->next;
	}

	if(head->right != NULL)
	{
		writeBook(head->right, fd);
	}
}

//Pulls all the data out of a given file designated by path
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

//Prints out the unsorted tree
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

//This function will eventually sort the tree in descending frequency.
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

//Iterates through a directory opening up the files
treeNode * fileIterator(char * name, treeNode * head){
	DIR * dir;
	struct dirent * entry;
	char * fileContents;
	errno = 0;
	int errsv;
	if((dir = opendir(name)) == NULL){
		errsv = errno;
		if(errsv == 2){
			fprintf(stderr, "\nNo such file, directory, or improperly formed path: \"%s\".\n", name);
			return head;
		}
		if(errsv == 20){
			fileContents = extract(name);
			if(fileContents == NULL){
				return head;
			}
			head = tokenize(fileContents, head, name);
			free(fileContents);
			return head;
		}
		return NULL;
	}
	while((entry = readdir(dir))){
		if(entry->d_type == DT_DIR){
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			    continue;
			}
			else{
				char * path = mkPath(name, entry->d_name);
				head = fileIterator(path, head);
				free(path);
			}
		}
		else if(entry->d_type == DT_REG){
			char * path = mkPath(name, entry->d_name);
			fileContents = extract(path);
			head = tokenize(fileContents, head, entry->d_name);
			free(fileContents);
			free(path);
		}
		else{
			char * path = mkPath(name, entry->d_name);
			fprintf(stderr, "I don't know how to handle this entry. %s\n", path);
			free(path);
		}
	}
	closedir(dir);
	return head;
}

//makes a pointer to the path that you passed to it
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

//Swaps the contents of two nodes except there leaves.
void swap(fileList * link1, fileList * link2){
	char * tempFileName = link1->fileName;
	int tempCounter = link1->counter;
	link1->fileName = link2->fileName;
	link1->counter = link2->counter;
	link2->fileName = tempFileName;
	link2->counter = tempCounter;
}

// Helping functions


/**
	* a function to concatenate strings
	* @param s1 the first string
	* @param s2 The second string
	* @return result the concatenated string
	*/
char* concat(const char *s1, const char *s2){
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
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
