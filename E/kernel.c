#include <stdio.h>

void printString(char*);
void readString(char*);
void readSector(char*, int);
void writeSector(char*, int);
void readFile(char*, char*);
void writeFile(char*, char*, int);
void executeProgram(char*);
void terminate();
int mod(int, int);
int div(int,int);
void deleteFile(char*);
void handleInterrupt21(int , int , int , int );
void handleTimerInterrupt(int, int);
void killProcess(char*);
main();

//do not want to run contents of array as machine code 
main2()
{
	main();
}

//two parts of the process table
int active[8];
int processStackPointer[8];
int currentProcess;

main()
{ 

	//initializes all processes to 0
	active[0] = 0;
	active[1] = 0;
	active[2] = 0;
	active[3] = 0;
	active[4] = 0;
	active[5] = 0;
	active[6] = 0;
	active[7] = 0;

	//initializes all processes to point to a specific segment 
	processStackPointer[0] = 0xff00;
	processStackPointer[1] = 0xff00;
	processStackPointer[2] = 0xff00;
	processStackPointer[3] = 0xff00;
	processStackPointer[4] = 0xff00;
	processStackPointer[5] = 0xff00;
	processStackPointer[6] = 0xff00;
	processStackPointer[7] = 0xff00;

	//intializes current process
	currentProcess = -1;

    	makeInterrupt21();
    	interrupt(0x21, 4, "shelll", 0, 0);
	makeTimerInterrupt();
    	while(1);
}

void killProcess(char* processIdChar)
{	int processIdNum;

	if(processIdChar[0] == '0')
	{
		processIdNum = 0;
	}
	else if(processIdChar[0] == '1')
	{
		processIdNum = 1;
	}
	else if(processIdChar[0] == '2')
	{
		processIdNum = 2;
	}
	else if(processIdChar[0] == '3')
	{
		processIdNum = 3;
	}
	else if(processIdChar[0] == '4')
	{
		processIdNum = 4;
	}
	else if(processIdChar[0] == '5')
	{
		processIdNum = 5;
	}
	else if(processIdChar[0] == '6')
	{
		processIdNum = 6;
	}
	else 
	{
		processIdNum = 7;
	}
	//calls the correct segment
	setKernelDataSegment();
	//sets the process to inactive 
	active[processIdNum] = 0;	
	restoreDataSegment();


}

void handleTimerInterrupt(int segment, int sp)
{
	//calls the correct segment
	setKernelDataSegment();
	//saves the stack pointer(sp) to the current process table entry
	if(currentProcess != -1)
	{
		processStackPointer[currentProcess] = sp;
	}

	//looks through the process table starting at the next entry after current 		process	
	while(1)
	{
		currentProcess++;
		//loops back to the start of the array
		if(currentProcess == 8)  
		{
			currentProcess = 0;
		}
		//a process is active and needs to be put in the schedule
		if(active[currentProcess] == 1)
		{
			//puts the correct segment associated with the current process
			segment = (currentProcess+2)*0x1000;
			//loads the stack to the processes stack pointer
			sp = processStackPointer[currentProcess];
			break;
		}
		
	}
	restoreDataSegment();
	
	returnFromTimer(segment, sp);
}

void printString(char* chars)
{
    	int i = 0;
    	//loops until end of line is reached
    	while(chars[i] != 0x00)
    	{
    	    //al is the char stored in chars
    	    char al = chars[i];
    	    //ah needs to be 0xe for this interrupt
    	    char ah = 0xe;
    	    //ax is one of the parameters for an interrupt
    	    int ax = ah *256 + al;
    	    //prints out a character
        	interrupt(0x10, ax,  0, 0, 0);
        	i++;
	}
}

void readString(char* chars)
{
    	//counter for curser
    	int i = 0;
    	//intializes al and tempAl with 0x00
    	char al = '0';
    	char tempAl = 0x0;

    	//while enter is not pressed
    	while(tempAl != 0xd)
        {
    	        //0x16 is the keyboard interrupt, this reads in the keyboard input
    	        //al is the ascii character of the keyboard input
    	        al = interrupt(0x16, 0x0*256+chars[i], 0, 0, 0);
	
    	        //backspace is clicked and the cusrser is not at the first position
    	        if((al == 0x8) && (i >= 1))
    		{
			//0x10 is the print interrupt
    	                //moves the curser to point to the last entered key
        	        interrupt(0x10, 0xe*256+al, 0 ,0 , 0);
        	        //puts the array counter to the previous spot
        	        i = i-1;
        	        //prints a blank space at the location of last key
        	        //cursor moves one spot from the delted key automatically
                   	interrupt(0x10, 0xe*256+' ', 0 ,0 ,0);
                    	//moves the curser to the spot at the deleted key
                    	interrupt(0x10, 0xe*256+al, 0, 0, 0);
        	}

            	//a letter or number is entered
            	else
        	{
                	//stores the ascii value of the keybaord input into our   				String array
                	chars[i] = al;
                	//puts al into a temporary variable
                	tempAl = al;
                	//prints the keyboard input to the screen
        	        interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
	                //increases counter for array
			i++;
            	}
	}
        //line feed
        chars[i] = 0xa;
        //end of string
        chars[i+1] = 0x0;
    
        //prints out line feed
        interrupt(0x10, 0xe*256+chars[i], 0, 0,0);
        //prints out end of string
        interrupt(0x10, 0xe*256+chars[i+1], 0 ,0 ,0);
}

//this function computes the integers used in interrupt to read a sector
void readSector(char* buffer, int sector)
{
        int relativeSector = mod(sector, 18) + 1;
        int tempHead = sector/18;
        int finalHead = mod(tempHead, 2);
        int track = sector/36;
        interrupt(0x13, 513 , buffer, track*256+relativeSector, finalHead*256+0);
}

//this function computes the integers used in interrupt to write a sector
void writeSector(char* buffer, int sector)
{
        int relativeSector = mod(sector, 18) + 1;
        int tempHead = sector/18;
        int finalHead = mod(tempHead, 2);
        int track = sector/36;
        interrupt(0x13, 769 , buffer, track*256+relativeSector, finalHead*256+0);
}

void readFile(char* fileName, char* buff)
{
	//a counter used to keep track of where i is
    	int index;
        //i is the start of entry in the directory
        int i;
        //used as an index or the six characters in the file name if filename = 6  		characters
     	int j;
        //counter for the thirty two bytes for each directory entry
        int k;
        //counter for the file name
    	int u;
    	//counter for file name size when file name is less than six characters
    	int z;
    	//boolean to determine if the entry found is the correct one
    	int isWrongFile;
    	//keeps track of the file name size
    	int fileNameSize;
    	//counter for the index of the file name 
    	int fileNameSizeIndex;
    	//boolean to determine if the file we are looking for is found 
    	int isFound;
    	//a buff to hold the directory
        char dirBuff[512];
    	//puts directory into dirBuff
        readSector(dirBuff, 2);

        //traverses through the directory
        for(i = 0; i < 512; i = i + 32)
        {
        	index = i;
        	fileNameSize = 0;
        	fileNameSizeIndex = 0;
        	isWrongFile = 0;
        	isFound = 0;

        	for(u = 0; u < 6; u++)
        	{
            		if(fileName[u] != 0x00 && fileName[u] != 0x0d && 
            		fileName[u] != 0x0a)
            		{
                		fileNameSize++;
            		}        
        	}

        	if(fileNameSize == 6)
        	{
            	//traverses through the file name to see if the file is            			there
            		for(j = 0; j < 6; j++)
            		{
                	//the filename does not match up with the filname in the 				directory
                		if(fileName[j] != dirBuff[i + j])
                		{
                		//goes back to the first for loop to check the    					next file in the directory
                		break;
                		}
            		}

            		if(j == 6)
            		{
                		//file is found
                		k = i + 6;
                		//puts all the sector numbers into a buffer array
                		while((dirBuff[k] != 0x00) && (k < index + 32))
                		{
                    			//puts the sector into a buffer array
                    			readSector(buff, dirBuff[k]);
                    			//add 512 to buffer address
                    			buff = buff + 512;
                    			k = k + 1;
                		}
            		//file is found 
            		isFound = 1;
            		break;
            		}
        	//end of if below
       		}

        	else
        	{
            		//checks to make sure the file name we are looking for is 				correct
            		for(z = 0; z < fileNameSize; z++)
            		{
                		if(fileName[z] != dirBuff[i + z])
                		{
                			isWrongFile = 1;
                			break;
                		}
            		}

            		//move onto next directory entry
            		if(isWrongFile == 1)
            		{
                		continue;
            		}

            		//checks to make sure the rest of the file name is 0x00
          		for(fileNameSizeIndex = fileNameSize; fileNameSizeIndex < 				6; fileNameSizeIndex++)
            		{
                		//file name does not contain 0x00 and is wrong file
                		if(dirBuff[i + fileNameSizeIndex] != 0x00)
                		{
                    			isWrongFile = 1;
                    			break;
                		}
            		}

            		//move onto next directoory entry
            		if(isWrongFile == 1)
            		{
                		continue;
            		}

            		//file is found
            		k = i + 6;
            		//puts all the sector numbers into a buffer array
            		while(k < index + 32)
            		{
                		//puts the sector into a buffer array
                		readSector(buff, dirBuff[k]);
                		//add 512 to buffer address
                		buff = buff + 512;
                		k = k + 1;
            		}

        	//file is found
        	isFound = 1;
        	break;
        	//end of else
        	}
        	//since file is found we do not need to contnue looking
        	if(isFound == 1)
        	{
        	    break;
        	}
        	//end of for below
	}
        return;
}

void executeProgram(char* name)
{    
	//index for a free entry in the process array
	int freeEntryIndex = 0;
	//segment used in intializeProgram to start at a free segment
	int segment;
	//buff for file contents
	char buff[13312];
	int m;
	int i = 0x000;

	//reads the file into buff
	readFile(name, buff);

	//calls the correct segment
	setKernelDataSegment();

	//searches for a free entry
	for(freeEntryIndex = 0; freeEntryIndex < 8; freeEntryIndex++)
	{
		//a free entry is found
		if(active[freeEntryIndex] == 0)
		{
			break;
		}
	}

	restoreDataSegment();
	
	//sets the segment to the correct hex number
	segment = (freeEntryIndex+2) * 0x1000;

	//loop goes until the end of buffer
	//transfers the file from the buffer into the bottom of memory (0000) of 	 	 memory at the segment in the parameter
	for(m = 0; m < 13312; m++)
	{
	putInMemory(segment, i, buff[i]);
	i = i + 0x1;
	}
	//this function sets up a stack frame and registers for a new program, but does not 		actually start running it
	initializeProgram(segment);

	//calls the correct segment
	setKernelDataSegment();
	//sets the process to active
	active[freeEntryIndex] = 1;
	restoreDataSegment();
}


void terminate()
{
	//calls the correct segment
	setKernelDataSegment();
	//sets the process to inactive 
	active[currentProcess] = 0;	
	restoreDataSegment();
	
	while(1);
}

//a function that deletes a file 
void deleteFile(char* name)
{
    	int u;
    	//a counter used to keep track of where i is
    	int index;
    	//i is the start of entry in the directory
    	int i;
    	//used as an index or the six characters in the file name 
       	int j;
    	//counter for the thirty two bytes for each directory entry
        int k;
    	//counter for file name size when file name is less than six characters
    	int z;
    	//boolean to determine if the entry found is the correct one
    	int isWrongFile;
    	//keeps track of the file name size
    	int fileNameSize;
    	//counter for the index of the file name 
    	int fileNameSizeIndex;
    	//boolean to determine if the file we are looking for is found 
    	int isFound;
    	//a buffer for the map and directory 
        char mapBuff[512];
        char dirBuff[512];
    
        //loads the map and directory into a character buffer 
        readSector(mapBuff, 1);
        readSector(dirBuff, 2);
        //traverses through the directory
        for(i = 0; i < 512; i = i + 32)
        {    
        	index = i;
        	fileNameSize = 0;
        	fileNameSizeIndex = 0;
        	isWrongFile = 0;
        	isFound = 0;

        	//traverses through the file name
        	for(u = 0; u < 6; u++)
        	{
            		if(name[u] != 0x00 && name[u] != 0x0d && name[u] != 0x0a)
            		{
                		fileNameSize++;
            		}        
        	}

        	//file name size is 6
        	if(fileNameSize == 6)
            	{    
            	//traverses through the file name to see if the file is        			there
            		for(j = 0; j < 6; j++)
            		{ 
                	//the filename does not match up with the filname          				in the directory
                		if(name[j] != dirBuff[i + j])
                		{
                		//goes back to the first for loop to check the   					next file in the directory
                			break;
                		}
            		}
            		if(j == 6)
            		{
                   		//k starts at the sector 
                		k = i + 6;
                		//file is found
                		//sets the first character of the file name to 0
                		dirBuff[i] = 0x00;
    	
                    		//this loop starts at the first sector after the 					file name
                    		//this loop ends at the last sector after the file 					name
                		//goes through each of the sectors used by the 					file and sets the correspoding sector in Map to 				zero
                		while(dirBuff[k] != 0x00)
                		{
                    			mapBuff[dirBuff[k]+1] = 0x00;
                    			k = k + 1;
                		}
            		isFound = 1;
            		break;
            		}
        	}

        	else
        	{
            	//checks to make sure the file name we are looking for is        			correct
			for(z = 0; z < fileNameSize; z++)
        	    	{
        		        if(name[z] != dirBuff[i + z])
                		{
                			isWrongFile = 1;
                			break;
                		}
            		}
            		//move onto next directory entry
            		if(isWrongFile == 1)
            		{
            			continue;
            		}
            		//checks to make sure the rest of the file name is 0x00
            		for(fileNameSizeIndex = fileNameSize; fileNameSizeIndex <           		6; fileNameSizeIndex++)
            		{
                		//file name does not contain 0x00 and is wrong file
                		if(dirBuff[i + fileNameSizeIndex] != 0x00)
                		{
                		    isWrongFile = 1;
                		    break;
                		}
            		}
            		//move onto next directoory entry
            		if(isWrongFile == 1)
            		{
            			continue;
            		}

            		//k starts at the sector 
            		//file is found
            		k = i + 6;
            		//file is found
            		//sets the first character of the file name to 0
            		dirBuff[i] = 0x00;
    
                	//this loop starts at the first sector after the          				file name
                	//this loop ends at the last sector after the file        				name
        		//goes through each of the sectors used by the          			file and sets the correspoding sector in Map to zero
			while(dirBuff[k] != 0x00)
        		{
         		       mapBuff[dirBuff[k]+1] = 0x00;
         		       k = k + 1;
         		}
        		//file is found
        		isFound = 1;
        		break;
        	}
        	//since file is found we do not need to contnue looking
        	if(isFound == 1)
        	{
        		break;
        	}

        }
    	//writes the character arrays back to map and directory
    	writeSector(mapBuff,1);
    	writeSector(dirBuff,2);
    
}

//a function to write a file 
void writeFile(char* name, char* buffer, int numberOfSectors)
{
        //a buffer for the map and directory 
        char mapBuff[512];
        char dirBuff[512];
    
    	//i is used as a counter to go through the directory
        int i;

    	//j is used as a counter to go through the file name
        int j;

    	//k is used a counter for the free bytes in the directory
        int k;
    
    	//z goes through the map
    	int z;

        //m is the number of sectors in th file
        int m;

    	//assume there are no free slots in the directory
    	int isFreeSlot = 0;
        //loads the map and directory into a character buffer 
        readSector(mapBuff, 1);
        readSector(dirBuff, 2);

        for(i = 0; i < 512; i = i + 32)
        {

    		//there is a free slot in the directory
        	if(dirBuff[i] == 0x00)
        	{
        		isFreeSlot = 1;
            		//copies the file name into the directory buffer
            		for(j = 0; j < 6; j++)
            		{
               		//put an if to fill the remaining bytes with 0 if         				name < 6
                		if(name[j] == 0x00)
                		{
                    			dirBuff[i + j] = 0x00;
                		}
                		else
                		{
                    			dirBuff[i + j] = name[j];
                		}	
            		}
                    
                	//For each sector in the file
        		for(m = 0; m < numberOfSectors; m++)
        		{   
        		//goes through the map to find an empty sector
            			for(z = 5; z < 512; z++)
            			{    
	
                			//an empty sector is found
                			if(mapBuff[z] == 0x00)
                			{
	
                	    			//set the map sector used 
                	                        mapBuff[z] = 0xFF;
                	    			//adds the sector number to the file's 							directory entry
                	    			dirBuff[i + j + m] = z;
	
                	    		//writes 512 bytes from the buffer          	               holding the file to the sector
                	    		writeSector(buffer, z);
                	    		buffer = buffer + 512;
                	    		break;
                			}
            			}
        		}
        		if(numberOfSectors != 26)
        		{
        			//file the remaining bytes in the directory to 0x00
            			for(k = i + j + m + 1; k < i + 32; k++)
            			{
                		dirBuff[k] = 0x00;
            			}
        		}
        		break;
            	}		

	}
    
	    //writes the character arrays back to map and directory
	    writeSector(mapBuff,1);
	    writeSector(dirBuff,2);
    
	    //there free slots left in the directory
    	if(isFreeSlot == 0)
    	{
    		return;
    	}
}
    
//a function for mod
int mod(int a, int b)
{
	while(a >= b)
        {
            a = a - b;
        }
        return a;
}

//a function for division
int div(int a, int b)
{
        int q = 0;
        while((q*b) <= a)
        {
            q = q + 1;
        }
        return q - 1;
}

//depend on what ax value is given through the interrupt call, a function
//with that interrupt will be called
void handleInterrupt21(int ax, int bx, int cx, int dx)
{
        if(ax == 0)
        {
        	//bx  = address of character array holding the string
                printString(bx);
        }
        else if(ax == 1)
        {
        	//bx  = address of character array holding the string
                readString(bx);
        }
        else if(ax == 2)
        {
        	//bx  = address where the datashould be stored to
        	//cx = sector that is being copied to bx
                readSector(bx, cx);
        }
        else if(ax == 3)
        {
                readFile(bx, cx);
        }
        else if(ax == 4)
        { 
		//bx = address of a character array holding the file name 
                executeProgram(bx);
        }	
        else if(ax == 5)
        {
                terminate();
        }
        else if(ax == 6)
       	{
            	//bx = address of a character array holding the sector to write 
            	//cx = sector number
                writeSector(bx, cx);
        }
        else if(ax == 7)
        {
        	//bx = address of a character array holding the file name 
        	deleteFile(bx);
        }    
        else if(ax == 8)
        {
		//bx = address of a character array holding the file name
        	//cx = anddress of a character array holding the file to be written
                //dx = number of sectors
                writeFile(bx, cx, dx);
        }
	else if(ax == 9)
	{
		//bx = process id
		killProcess(bx);
	}
        else
        {
		printString("Error");
        }
}
