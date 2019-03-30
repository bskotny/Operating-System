enableInterrupts();
main()
{	
	while(1)
	{


		int a;
		//i = file name for type
		int i = 5;
		//j = counter used the file name character array
		int j = 0;

		//m = program name for execute
		int m = 8;
		int n = 0;
		
		//r = counter for program name for create
		int r = 7;
		//r = counter for program name for delete
		int e = 7;

		//f = starting index for first file name in copy function
		int f = 5;
		//keeps track of the size of the new file name in copy function
		int sizeOfNewFileName = 0;
		//counter index for file name in copy function
		int index = 0;
		//counter for file name in copy
		int s= 0;
		

		//g = counter to traverse through directory for dir function
		int g;
		//h = counter to traverse through file name for dir function
		int h;
		//keeps track of the length of the file name for dir function
		int sizeOfFileName = 0;
		//keeps track of the length of the original file name for dir 			function
		int sizeOfOriginalName = 0;
		

		//a counter to go through the line array in create
		int v;
		////a counter to go through the line array in create
		int w;
		//used as a boolean to determine if user entered blank line in 			create
		int blankLine = 1;
		//used as an incrementer for the buffer in create 
		int counter = 0;	


		char processNumber[1];		

		//a char array for the user input
		char line[80];

		//buffer for the filename the user has inputed
		char fileNameBuffer[6];

		//buffer used to hold the contents of the files in type
		char buff[13312];

		//buffer used to hold the contents of the files in copy and create
		char copyBuff[13312];

		//a char array used in create to get the user input after the 			first entered line
		char newLine[80];

		//a buffer that holds the directory
		char dBuff[512];

		//a char array with the filename, enter key, line feed, and end of 			line, this is used in dir
		char tempChar[9];

		//a char array used to hold the original file name in copy
		char copyFileNameOriginal[6];
		//a char array used to hold the new file name in copy
		char copyFileNew[6];
	
		//used to move the curser to the next line
		char temp[3];
		temp[0] = 0xd;
 		temp[1] = 0xa;
 		temp[2] = 0x0;

		
		//prints out shell
		interrupt(0x21, 0, "SHELL> ", 0 ,0);

		//reads in a line
		interrupt(0x21, 1, line, 0 , 0);
		
		//type shell command
		if(line[0] == 't' && line[1] == 'y' && line[2] == 'p' && 
		line[3] == 'e' && line[4] == ' ')
		{ 
			//gets the file name from line until 0x00 is found
			while(line[i] != 0x00)
			{
				fileNameBuffer[j] = line[i];
				i++;
				j++;
			}
			//puts the content of the file name into buff
	      		interrupt(0x21, 3, fileNameBuffer, buff, 0);
			//prints out buff
  			interrupt(0x21,0, buff, 0, 0);

			//the print string interrupt will always be called
			//this is why there is a blank when the file is not found
			//clears the buffer
			for(a = 0; a < 13312; a++)
			{
				buff[a] = ' ';
			}
		}
		//execute shell command
		else if(line[0] == 'e' && line[1] == 'x' && line[2] == 'e' && 			line[3] == 'c' && line[4] == 'u' && line[5] == 't' && 			line[6] == 'e' && line[7] == ' ')
		{ 
			//gets the file name from line until 0x00 is found
			while(line[m] != 0x00)
			{
				fileNameBuffer[n] = line[m];
				m++;
				n++;
			}
			interrupt(0x21,0,"here",0,0);
			//executes the program at segment 0x2000
	      		interrupt(0x21, 4, fileNameBuffer, 0, 0);
		}
                //delete shell command 
                else if(line[0] == 'd' && line[1] == 'e' && line[2] == 'l' && 			line[3] == 'e' && line[4] == 't' && line[5] == 'e' && 			line[6] == ' ')
                {

			while(line[e] != 0xd)
			{
				fileNameBuffer[j] = line[e];
				e++;
				j++;
			}
			interrupt(0x21, 7, fileNameBuffer, 0, 0);
                }
		//copy command 
                else if(line[0] == 'c' && line[1] == 'o' && line[2] == 'p' && 			line[3] == 'y' && line[4] == ' ')
                {


			//puts the name of the original file into an array
			while(line[f] != ' ')
			{
				copyFileNameOriginal[index] = line[f];
				f++;
				index++;
			}

			
			
			//resets the index
			index = 0;	
			f = f + 1;
			
			//puts the name of the destination file into an array
			while(line[f] != 0x0d)
			{
				copyFileNew[index] = line[f];
				f++;
				sizeOfNewFileName++;
				index++;
			}

			//puts a 0x00 into the new file name where there are blanks
			while(sizeOfNewFileName < 6)
			{
				copyFileNew[sizeOfNewFileName] = 0x00;
				sizeOfNewFileName++;
			}

			//puts the original file contents into copyBuff
			interrupt(0x21, 3, copyFileNameOriginal, copyBuff, 0);
			//put the contents of copyBuff into the new file
			interrupt(0x21, 8, copyFileNew, copyBuff, 26);
			interrupt(0x21, 0 , temp, 0, 0);

                }
		//dir command 
		else if(line[0] == 'd' && line[1] == 'i' && line[2] == 'r')
		{
			//load directory into buffer
			interrupt(0x21, 2, dBuff, 2, 0);
					

			//traverse through the directory
			for(g = 0; g < 512; g = g + 32)
			{
				
				//a file is in the directory
				if(dBuff[g] != 0x00)
				{
					//traverses through the file name and 						stores the characters in a temporary char 						array
					for(h = 0; h < 6; h++)
					{
						tempChar[h] = dBuff[g + h];
						//keeps track of the size of the 							file name
						if(dBuff[g+h] != 0x00)
						{
							sizeOfFileName++;
							
						}
						
					}
				if(sizeOfFileName < 6)
				{
					//adds enter, line feed, and end of line 						to the file name if it is less than six 					characters
					tempChar[sizeOfFileName] = 0xd;
					tempChar[sizeOfFileName + 1] = 0xa;
					tempChar[sizeOfFileName + 2] = 0x0;
				}

				if(sizeOfFileName == 6)
				{
					//adds a carrige return to tempChar
					tempChar[6] = 0xd;
					//adds a line feed to tempChar
					tempChar[7] = 0xa;
					//adds the end of the line to tempChar
					tempChar[8] = 0x0;
				}
				//prints out the file name
				interrupt(0x21,0 ,tempChar, 0, 	0);
				sizeOfFileName = 0;	
				}
			}
		}
		//create command
		else if(line[0] == 'c' && line[1] == 'r' && line[2] == 'e' && 			line[3] == 'a' && line[4] == 't' && line[5] == 'e' && 			line[6] == ' ')
		{
			//read in file name
			while(line[r] != 0x0d)
			{
				fileNameBuffer[s] = line[r];
				r++;
				s++;
			}
			//prints out a blank line
			interrupt(0x21, 0, temp, 0,0);
			//prints out enter a line
			interrupt(0x21, 0, "Enter a line: ", 0 ,0);
			//prints out a blank line
			interrupt(0x21, 0, temp, 0,0);

			//continue to allow user input until a blank line is 				entered
			while(blankLine == 1)
			{
				//allows user to enter a new line
				interrupt(0x21, 1 , newLine, 0, 0); 

				//a line with a letter or symbol has been entered
				if(newLine[0] == ' '){
				break;}


				
				//putting line into buffer
				for(v = 0; v < 80; v++)
				{
					//user has clicked enter so we put blanks 						after 0x00 so print string will work
					if(newLine[v] == 0x00 || newLine[v] == 0xa
 					|| newLine[v] == 0xd)
					{
						newLine[v] = 0x20;
					}
					//puts one character of the new line into 						copy buff
					copyBuff[v+counter] = newLine[v];
				}
				//increments the buffer holding the input lines
				counter = counter + 80;
				
				//clears the new line the user has entered
				for(w = 0; w < 80; w++)
				{
					newLine[w] = 0x00;
				}
			}

			//write the contents of the file to a buffer
			interrupt(0x21, 8, fileNameBuffer, copyBuff, 26);
		}
		//kill command
		else if(line[0] == 'k' && line[1] == 'i' && line[2] == 'l' && line[3] == 'l' && line[4] == ' ')
		{
			processNumber[0] = line[5];
			interrupt(0x21, 9, processNumber, 0 , 0);
		}
		else	
		{
		//invalid command
		interrupt(0x21, 0, "Bad command!", 0 , 0);
    		interrupt(0x21, 0, temp, 0,0);
		}
	interrupt(0x21, 0 , temp, 0, 0);
	}
}
