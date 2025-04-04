
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>

    #define MAX_CHARACTERS 1024
    #define MAX_WORDS 5
    #define MAX_WORDS_PER_LINE 100


    typedef struct Alias {
        char commandName[MAX_CHARACTERS];
        char command[MAX_CHARACTERS];
        struct Alias* next;
    } Alias;

    Alias* head = NULL;
    int flag=0;
    char * aliasCommandWithArgs [MAX_WORDS];


    // Function to add an alias
    void addAlias(const char* commandName, const char* command) {
        Alias* newAlias = (Alias*)malloc(sizeof(Alias));
        if (newAlias == NULL) {
            perror("Unable to allocate memory");
            return;
        }
        strncpy(newAlias->commandName, commandName, MAX_CHARACTERS);
        strncpy(newAlias->command, command, MAX_CHARACTERS);
        newAlias->next = head;
        head = newAlias;
    }

    // Function to delete an alias
    void deleteAlias(const char* commandName) {
        Alias* temp = head;
        Alias* prev = NULL;

        while (temp != NULL && strcmp(temp->commandName, commandName) != 0) {
            prev = temp;
            temp = temp->next;
        }

        if (temp == NULL) {
            printf("Alias not found.\n");
            return;
        }

        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }

        free(temp);
    }

    // Function to search for an alias
    Alias* searchAlias(const char* commandName) {
        Alias* temp = head;

        while (temp != NULL) {
            if (strcmp(temp->commandName, commandName) == 0) {
                return temp;
            }
            temp = temp->next;
        }

        return NULL;
    }

    // Function to print all aliases
    void printAliases() {
        Alias* temp = head;

        while (temp != NULL) {
            printf("%s=%s\n", temp->commandName, temp->command);
            temp = temp->next;
        }
    }

    // Function to split text into words
    void splitTextIntoWords(char* text, char** words, int* wordCount) {
        char* word = NULL;
        int count = 0;

        // Use strtok to split the text by space and newline characters
        word = strtok(text, " \n");
        while (word != NULL) {
            words[count] = strdup(word);
            if (words[count] == NULL) {
                perror("Unable to allocate memory");
                exit(EXIT_FAILURE);
            }
            count++;
            if (count > MAX_WORDS) {
                printf("Error: Number of words exceeds %d.\n", MAX_WORDS);
                // Free allocated memory before returning
                for (int i = 0; i < count; i++) {
                    free(words[i]);
                }
                *wordCount = 0;
                return;
            }
            word = strtok(NULL, " \n");
        }

        *wordCount = count;
    }

    // Function to check if the text contains any quotes
    int containsQuotes(char* text) {
        if (strchr(text, '"') != NULL || strchr(text, '\'') != NULL) {
            return 1;
        }
        return 0;
    }

    // Function to extract a word enclosed in quotes
    int extractQuotedWord(const char* text, char* newCommand,char* command) {
        const char* start = NULL;
        const char* end = NULL;
        const char* equal = NULL;
        const char * space =NULL;

        if ((start = strchr(text, '"')) != NULL) {
            end = strchr(start + 1, '"');

        } else if ((start = strchr(text, '\'')) != NULL) {
            end = strchr(start + 1, '\'');

        }if ((equal = strchr(text, '=')) != NULL) {
        size_t length = start-text-1;
            strncpy(newCommand, text, length);
                        newCommand[length] = '\0';
        }


        if (start != NULL && end != NULL && end > start) {
            size_t length = end - start - 1;
            strncpy(command, start + 1, length);
            command[length] = '\0';
            if (strchr(start + 1, ' ')!=NULL){
                int counter=0;
                splitTextIntoWords(command,aliasCommandWithArgs,&counter);
            }
            return 1;
        }

        return 0;
    }

    // Function to execute a command using fork and execvp
    void executeCommand( char** words) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {

            Alias* temp= searchAlias(words[0]);
            if(temp!=NULL){
            // Execute the command
                        words[0]=temp->command;
                        execvp(temp->command, words);
                        perror("execvp failed");
                        flag=1;
                        exit(EXIT_FAILURE);



            }else{
            // Execute the command
            execvp(words[0], words);
            perror("execvp failed");
            flag=1;
            exit(EXIT_FAILURE);

            }
        } else {
            // Parent process
            wait(NULL);
        }
    }

    void cd(char name[]){
        if(chdir(name)!=0)
            perror("error");
    }

      int readLinesAndStoreWords(const char *filename, char **words, int *lineCount, int *cmdCount, int* aliasCount,char* newCommand,char* command) {
                        FILE *file = fopen(filename, "r");
                        if (file == NULL) {
                            perror("Error opening file");
                            return -1;
                        }

                        *lineCount = 0;
                        words = malloc(MAX_WORDS * sizeof(char*)); // Allocate memory for words array
                        if (words == NULL) {
                            perror("Unable to allocate memory");
                            fclose(file);
                            return -1;
                        }
                        int wordCounter;

                        char buffer[MAX_CHARACTERS];

                        while (fgets(buffer, sizeof(buffer), file) != NULL) {
                            if (strlen(buffer) == 0) {

                                continue;
                            }

                            buffer[strcspn(buffer, "\n")] = 0;


                            splitTextIntoWords(buffer, words, &wordCounter);
                            if (wordCounter == 0) {

                                continue;
                            }

                            (*lineCount)++;

                            if (strcmp(words[0], "alias") == 0 && words[1] == NULL) {
                                printAliases();
                                cmdCount++;
                            } else if (strcmp(words[0], "alias") == 0 && words[1] != NULL) {
                                extractQuotedWord(words[1], newCommand, command);
                                addAlias(newCommand, command);
                                aliasCount++;
                                cmdCount++;
                            } else if (strcmp(words[0], "unalias") == 0) {
                                deleteAlias(words[1]);
                                cmdCount++;
                                aliasCount--;
                            } else if (strcmp(words[0], "exit_shell") == 0) {
                                break;
                            } else if(strcmp(words[0], "cd") == 0){
                                if(words[1]!=NULL)
                                    cd(words[1]);
                                else perror("enter the directory");


                            }else {
                                executeCommand(words);
                                if (flag != 1)
                                    cmdCount++;
                            }
                        }

                        free(words); // Free allocated memory for words array
                        fclose(file);
                        return 0;
                    }





    int main() {



    int cmdCount,aliasCount,scriptCount;
    cmdCount=0;
    aliasCount=0;
    scriptCount=0;
    char command[MAX_CHARACTERS];
    char newCommand[MAX_CHARACTERS];

        char* text = (char*)malloc(MAX_CHARACTERS * sizeof(char));
        if (text == NULL) {
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        }

        char** words = (char**)malloc(MAX_WORDS * sizeof(char*));
        if (words == NULL) {
            perror("Unable to allocate memory");
            free(text);
            return EXIT_FAILURE;
        }

        size_t size = MAX_CHARACTERS;
        int wordCount=0, i;
        char choice;
        int quoteTextCount = 0;
        char quotedWord[MAX_CHARACTERS];

        while (1) {

                    for (i = 0; i < wordCount; i++) {
                    words[i]=NULL;
                    }

            printf("#cmd:%d|#alias:%d|#script lines:%d>",cmdCount,aliasCount,scriptCount);
            getline(&text, &size, stdin);

            // Check if input text exceeds the maximum allowed characters
            if (strlen(text) > MAX_CHARACTERS) {
                printf("error your command is too long");
                continue;
            }
            // Remove trailing newline character from getline
            text[strcspn(text, "\n")] = 0;

            // Check for quotes in the text
            if (containsQuotes(text)) {
                quoteTextCount++;
            }
            // Split text into words
            splitTextIntoWords(text, words, &wordCount);

            if (wordCount == 0) {
                // Error message is printed inside splitTextIntoWords
                continue;
            }
            if(strcmp(words[0],"alias")==0&&words[1]==NULL){
            printAliases();
            cmdCount++;

            }else if(strcmp(words[0],"alias")==0&&words[1]!=NULL){
                extractQuotedWord(words[1],newCommand,command);
                addAlias(newCommand,command);
            aliasCount++;
            cmdCount++;
            }else if(strcmp(words[0],"unalias")==0){
            deleteAlias(words[1]);
            cmdCount++;
            aliasCount--;
            }else if(strcmp(words[0],"exit_shell")==0){
            break;
            }else if(strcmp(words[0],"source")==0){
                readLinesAndStoreWords(words[1],words,&scriptCount,&cmdCount,&aliasCount, newCommand, command);

            }else if(strcmp(words[0], "cd") == 0){
                if(words[1]!=NULL)
                    cd(words[1]);
                else perror("enter the directory");


            }else{
            executeCommand(words);
            if (flag!=1)
                cmdCount++;
            }


        }

        free(text);  // Free the input text buffer
        free(words); // Free the words array

        printf("%d", quoteTextCount);

        return 0;
    }
