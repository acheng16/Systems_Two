#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */

/* sets the history file and history command to store an input ammount of commands. Takes in the File containing history and the count for how many commands to store */
void set_history(FILE *fo, int count, char histloc[]) {
	char line_number[MAXLINE]; //line number we read from original file
	char command[MAXLINE];     //command we read from original file
	FILE *temp_file;           //temporary file storing our final version of history
	int i;                     
	int line_count=0;          //the count of command lines currently in the history file
	fclose(fo);                
	fo = fopen(histloc,"r");         //reopens the history text file to make pointer point to the start of the file
	temp_file = fopen("temp.txt","w+");    
	//iterates two lines at a time, per two lines incremet one count of history command.
	while (fgets(line_number,MAXLINE,fo)!= NULL) {
		fgets(command,MAXLINE,fo);
		line_count++;
	}
	//repoint to start of file
	fclose(fo);
	fo = fopen(histloc,"r");
	
	//iterate through history file two lines at a time, saving the last (count) commands and their line_numbers to the temporary file
	for (i=0; i<(line_count);i++) {
		fgets(line_number,MAXLINE,fo);
		fgets(command,MAXLINE,fo);
		if (i>=(line_count-count)) {
			fprintf(temp_file,"%s",line_number);
			fprintf(temp_file,"%s",command);
		}
	}
	fclose(fo);
	fclose(temp_file);
 	rename("temp.txt",histloc);   //replace history file with temp file.
  	fo = fopen(histloc,"a");
	
}

/* searches through the file if the command entered exists */
void line_search(FILE *fo, char inBuffer[], char histloc[]){
  char line[MAXLINE];	//line retrieved from file
  FILE *temp_file;	//temp_file where the final version of history is stored
  int i;

  fclose(fo);
  fo = fopen(histloc,"r");
  temp_file = fopen("temp.txt","w+");
  
  //pulls line with line number
  fscanf(fo, "%d", &i);
  while(fgets(line,MAXLINE,fo)!=NULL){
    fgets(line,MAXLINE, fo);	//retrieves the line with the command
    if (strcmp(line,inBuffer) == 0) {  //if the line is equal to the new command entered (inBuffer) then don't add it to the temp history file
	i++;
    }else {
    	fprintf(temp_file,"%d\n",i);  //line_number
    	fprintf(temp_file,"%s",line); //command
	fscanf(fo, "%d", &i);
    }
  }
  fclose(fo);
  fclose(temp_file);
  rename("temp.txt",histloc);
  fo = fopen(histloc,"a");
}

/* prints the history using the history.txt file in our file system */
void print_history(FILE *fo, char *histloc) {
  char line[MAXLINE];
  fclose(fo);
  fo = fopen(histloc,"r");
  /* prints line_number and command until the next line is NULL*/
  while(fgets(line,MAXLINE,fo)!=NULL){
    strtok(line,"\n");   //new line is separator, and stores line into variable line
    printf("%s\t",line);
    fgets(line,MAXLINE, fo);
    printf("%s",line);
  }
  printf("\n");
  fclose(fo);
  fo = fopen(histloc,"a");
}

/* retrieves from history and runs in given the line_number (r_num) if (r_num) not in history return 0 which will give the error message in main */
int retrieve_from_history(char inBuffer[], FILE *fo, int r_num, char *args[],int *bkgrnd, int line_number, char histloc[]) {
  char line[MAXLINE];
  FILE *temp_file;   // temp file that has final history stored.
  int length;        // same var that was used in setup()
  int start;         // same var that was used in setup()
  int j;             // same var that was used in setup()
  int found=0;         // boolean that checks if r_num was found

  int count = 0;
  fclose(fo);
  int i = 0;
    start = -1;
    j = 0;

  fo = fopen(histloc,"r");
  temp_file = fopen("temp.txt","w+");

  //first line_number in history file
  fscanf(fo, "%d", &i);

  //Case one finds r_num and stops there or 
  while( ( fgets(line,MAXLINE,fo)!=NULL ) && (r_num!=(i)) ){
	fprintf(temp_file,"%d\n",i);
	fgets(line,MAXLINE, fo);
	fprintf(temp_file,"%s",line);
	fscanf(fo, "%d", &i);
  }

  //gets the line input (command) for that specific r number and place it in inBuffer
  if (r_num==i) {
	fgets(inBuffer,MAXLINE, fo);
	//get next line_number after r_num
  	fscanf(fo, "%d", &i);
	found = 1;
  }

  
  //add the rest of the history in the old history file to the temp history
  while ( fgets(line,MAXLINE,fo)!=NULL ){
	fprintf(temp_file,"%d\n",i);
	fgets(line,MAXLINE, fo);
	fprintf(temp_file,"%s",line);
	fscanf(fo, "%d", &i);
  }
  

  fclose(fo);
  fclose(temp_file);
  rename("temp.txt",histloc);
  fo = fopen(histloc,"a");

//only process inBuffer to args if r_num = i was found
if (found) {
  printf("%s", inBuffer);	//print retrieved command
  length = strlen(inBuffer);	

//starts same process as setup to break inBuffer to args
for (i = 0; i < length; i++) {
 
        switch (inBuffer[i]){
        case ' ':
        case '\t' :          /* Argument separators */
            if(start != -1){
                args[j] = &inBuffer[start];    /* Set up pointer */
                j++;
            }

            inBuffer[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
                args[j] = &inBuffer[start];     
                j++;
            }

            inBuffer[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *bkgrnd = 1;
            inBuffer[i] = '\0';
            break;
            
        default :             /* Some other character */
	  if (start == -1) {
	    start = i;
	  }
	}
 
    }    
    args[j] = NULL; /* Just in case the input line was > 80 */

}
    return found;
}


/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also 
modifies the args parameter so that it holds points to the null-terminated strings which 
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/

void setup(char inBuffer[], char *args[],int *bkgrnd, FILE *fo, char histloc[])
{
    int length,  /* #  characters in the command line */
        start,   /* Beginning of next command parameter           */
        i,       /* Index for inBuffer arrray          */
        j;       /* Where to place the next parameter into args[] */

    /* Read what the user enters */
    memset(inBuffer,0, 80);
    length = read(STDIN_FILENO, inBuffer, MAXLINE);
    line_search(fo, inBuffer, histloc);
    start = -1;
    j = 0;

    if (length == 0)
        exit(0);            /* Cntrl-d was entered, end of user command stream */

    if (length < 0){
        perror("error reading command");
	exit(-1);           /* Terminate with error code of -1 */
    }

    for (i = 0; i < length; i++) {
 
        switch (inBuffer[i]){
        case ' ':
        case '\t' :          /* Argument separators */
            if(start != -1){
                args[j] = &inBuffer[start];    /* Set up pointer */
                j++;
            }

            inBuffer[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
                args[j] = &inBuffer[start];     
                j++;
            }

            inBuffer[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *bkgrnd = 1;
            inBuffer[i] = '\0';
            break;
            
        default :             /* Some other character */
	  if (start == -1) {
	    start = i;
	  }
	}
 
    }    
    args[j] = NULL; /* Just in case the input line was > 80 */
} 

int getLineNumber(FILE* file_history, char histloc[], int *set_history_num){
	char temp[MAXLINE]; //line number we read from original file
	char line_number[MAXLINE]; //line number we read from original file
	char command[MAXLINE];     //command we read from original file
	int looped = 0;
	if (fgets(temp, MAXLINE, file_history!=NULL)) {
		set_history_num = atoi(temp);
	}
	while (fgets(temp,MAXLINE,file_history)!= NULL) {
		fgets(command,MAXLINE,file_history);
		strcpy(line_number, temp);
		looped = 1;
	}
	fclose(file_history);
	file_history = fopen(histloc,"a");
	
	if (looped == 0) {
		return 0;
	}else {
		return atoi(line_number);
	}

}

int main(void)
{
    char inBuffer[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    int bkgrnd;             /* Equals 1 if a command is followed by '&', else 0 */
    int exec;
    pid_t pid;		    /* To determine if child or parent */
    FILE *file_history;     /* FILE containing the history */
    int bool;		    /* Boolean, if set to 1, don't call execvp */
    int line_number;	    /* line_number that the command is on */
    int i;
    int digit;
    char histloc[80];	    /* history location : %HOME%/history.txt*/
    int *set_history_num=12; /* default history number is 12 */

    line_number=1;
    sprintf(histloc, "%s/history.txt",getenv("HOME"));
    file_history = fopen(histloc,"r");
    line_number += getLineNumber(file_history, histloc, &set_history_num);
    while (1){            /* Program terminates normally inside setup */
	bool = 0;
	bkgrnd = 0;

	printf("SysIIShell--> ");  /* Shell prompt */
        fflush(0);
        setup(inBuffer, args, &bkgrnd, file_history, histloc, &set_history_num);    /* Get next command */
	
//handle error when empty space entered.
if(args[0]!=NULL) {
	//case 1 r and no number entered, calls retrieve from history with the number
	if ( strcmp (args[0], "r")==0 && args[1] == NULL) {
		printf("Must include history number\n");
		bool = 1;
	//case 2 r entered and args[1] exists
	}else if (strcmp ( args[0], "r") == 0 && args[1] != NULL) {
		digit = atoi(args[1]);
		//if it is a valid digit after string to int conversion will attempt to retrieve
		if (digit<0){
		  printf("Please Invalid history number; Must be an integer greater then zero\n");
		  bool = 1;
		    }
		else if (digit!=0){
			if (!retrieve_from_history(inBuffer, file_history, digit, args, &bkgrnd, line_number, histloc)) {
					printf("History number %d doesn't exist\n", digit);
					bool = 1;
				}
			
		} else {
			printf("Invalid history number; Must be an integer greater than zero\n");
			bool = 1;
		}
	}	

	//case when only "rr" entered, calls retrieve_from_history with line_number-1
	if ( strcmp (args[0], "rr") == 0 && args[1] == NULL ) {
		if (line_number==1) {
			printf("No recent history!\n");
			bool = 1;
		}else {
			retrieve_from_history(inBuffer, file_history, line_number-1, args, &bkgrnd, line_number, histloc);
		}
	}
	//case when sethistory entered, calls set history
	if ( strcmp (args[0], "sethistory") == 0 && args[1] != NULL) {
		set_history_num = atoi(args[1]);
		if (digit!=0){
			set_history(file_history,set_history_num, histloc);
		}else {
			printf("Invalid history number; Must be an integer greater then zero");
		}
		bool  = 1;
	}

//adds command to file_history along with its line number
	if (bool != 1) {
		fprintf(file_history,"%i\n",line_number);
	     	for (i=0; args[i]!=NULL;i++){
			fprintf(file_history, "%s",args[i]);
			if (args[i+1]!=NULL) {
				fprintf(file_history, " ");
			}
		}
		//adds & if bkgrnd is 1
		if (bkgrnd == 1 ) {
			fprintf(file_history, " &");
		}
	
		fprintf(file_history, "\n");
		//increments line number since we just entered an entry to history
		line_number++;
	}
	//if its history we execute set_history to make sure it is either the default 12 or a value we have set directly.
			if ( strcmp ( args[0], "history") == 0 || strcmp ( args[0], "h") == 0 ) {
				set_history(file_history,set_history_num, histloc);
				print_history(file_history, histloc);
				bool = 1;
			}

	
	
}
	//forks process
	pid = fork();
	//waits for child
	if (bkgrnd == 0){
		if(pid != 0) {
		  waitpid(pid, bkgrnd, 0); 
		}

	}
	if (pid == 0){
		  if (bool != 1){
				exec = execvp(inBuffer,args);
				if (exec<0 && strcmp ( args[0], "history") != 0 && strcmp ( args[0], "h") != 0) {
			 	 printf("error exec new process: No such file or directory\n");
				}
		 	 }
	
			  _exit(0);
		}
	

	

	/* Fill in the code for these steps:  
	 (1) Fork a child process using fork(),
	 (2) The child process will invoke execvp(),
	 (3) If bkgrnd == 0, the parent will wait, 
		o/w returns to the setup() function. */
    }
}
