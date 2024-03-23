#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define N 5 //number of threads

typedef struct {
    char* word;
    int tId;
    char* filePath;
    int* count;
} ThreadArgs;

// reads size of file in bytes
long getFileSize(FILE *file){
    	fseek(file, 0, SEEK_END);	// reach the EOF
    	long fileSize = ftell(file); 	// read file size
	return fileSize; 
}

// reads a segment of the file starting at startIndex and of charCount long
void readFileSegment(FILE* file, int startIndex, int charCount, char* buffer) {
    if (file == NULL || buffer == NULL) return;

    fseek(file, startIndex, SEEK_SET);
    size_t readCount = fread(buffer, sizeof(char), charCount, file);
    buffer[readCount] = '\0'; // Null-terminate the string
}

// check if 2 words match
int wordsMatch(char* word1, char* word2){
	int i = 0;
	while(word1[i] != '\0' || word2[i] != '\0'){
		if(word1[i] != word2[i]) return 0; 
		i++;
	}
	return 1;
} 

// count how many times the word appears in a segment of the text, starting at startIndex and ending at startIndex + segmentSize
int countMatches(char* word, int startIndex, int segmentSize, FILE* file, long fileSize){
	int wordLen = strlen(word);
	int match = 0;
	int stopIndex = startIndex + segmentSize;
	int cont = 0;
	char buffer[wordLen]; 
	while(startIndex < stopIndex){
		if (startIndex >= fileSize) return cont;	
		readFileSegment(file, startIndex, wordLen, buffer);
		//printf("%s\n", buffer);
		match = wordsMatch(buffer, word);		
		if (match == 1) cont++;
		startIndex++;
	}	
	return cont;
}


void* threadFunc(void* arg) {
    ThreadArgs* args = (ThreadArgs*) arg;
    
    FILE* file = fopen(args->filePath, "r"); // Open file in thread
    if (file == NULL) {
        printf("Could not open file.\n");
        return NULL;
    }

	long fileSize = getFileSize(file); // file size in bytes

    int segmentSize = ceil(fileSize/N) - 1;
    int startIndex = args->tId * segmentSize;

    *args->count = countMatches(args->word, startIndex, segmentSize, file, fileSize);

	fclose(file);	//close file

    return NULL;
}

int main() {
    	FILE *file;			// pointer to file
    	long fileSize;			// file size in bytes
		clock_t start, end;		// handle timer
		double cpu_time_used;		// time required for the process

		char filePath[] = "el_quijote.txt"; // Define file path
		char word[] = "Sancho";			// word that is going to be searched

		int count0 = 0, count1 = 0, count2 = 0, count3 = 0, count4 = 0;				// word counters for every thread
		ThreadArgs args0 = {word, 0, filePath, &count0};
		ThreadArgs args1 = {word, 1, filePath, &count1};
		ThreadArgs args2 = {word, 2, filePath, &count2};
		ThreadArgs args3 = {word, 3, filePath, &count3};
		ThreadArgs args4 = {word, 4, filePath, &count4};

		//start timer	
		start = clock();
		
		pthread_t t0, t1, t2, t3, t4; 	// Thread identifiers

		//create threads
		pthread_create(&t0, NULL, threadFunc, &args0);
		pthread_create(&t1, NULL, threadFunc, &args1);
		pthread_create(&t2, NULL, threadFunc, &args2);
		pthread_create(&t3, NULL, threadFunc, &args3);
		pthread_create(&t4, NULL, threadFunc, &args4);

		//wait for threads to end
        pthread_join(t0, NULL);
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        pthread_join(t3, NULL);
        pthread_join(t4, NULL);
	
		//stop timer
		end = clock();

		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;	//compute time
		
		int count = count0 + count1 + count2 + count3 + count4;
		printf("The word \"%s\" appeared: %d times\n", word, count);
		printf("Time taken to search all matches of the word with %d threads: %fs\n", N, cpu_time_used);

    	return 0;
}

