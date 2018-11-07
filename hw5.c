#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	while(1)
	{
		printf("0$ ");
		int i,count=0,bar_count=0;
		//int pfd[2];
		char* command[100];  
		char input[100];  //輸入的指令
		fgets(input,100,stdin);
		command[0]=strtok(input, " ");
		//printf("%s",command[0]);
		while(command[count]!='\0')
		{
			count=count+1;
			command[count]=strtok(NULL, " ");
			//printf("%s",command[count]);
		}
		command[count-1]=strtok(command[count-1],"\n");
		command[count]=NULL;	
		/*
		for(i=0;i<count;i++)
		{
			if(strcmp(command[i],"|")==0)
				bar_count=bar_count+1;
		}*/
		//printf("%s",command[0]);	
		//if(bar_count==0)
		{
			pid_t child; //產生出來的process的名字
			child = fork();
			if(child==0)
			{
				if(strcmp(command[0],"exit")==0)
					exit(0);		
				if(strcmp(command[0],"cd")==0)
					chdir(command[1]);
				else
					execvp(command[0],command);
			}
			else
			{
				wait(NULL);
				if(strcmp(command[0],"exit")==0)
					exit(0);		
			}
		}
		/*
		else
		{
			int j,temp=0;
			char* cmd[bar_count+1][100];
			for(i=0;i<bar_count+1;i++)
			{
				for(j=temp;j<=count;j++)
				{
					if(strcmp(command[j],"|")==0)
					{
						temp=temp+1;
						break;
					}
					else
					{
						temp=temp+1;
						strcat(cmd[i],command[j]);
						strcat(cmd[i]," ");
					}
				}	
			}
			for(i=0;i<bar_count+1;i++){
				printf("%s",cmd[i]);printf("\n");}
			for(i=0;i<bar_count+1;i++)
			{
				pipe(pfd);	
				pid_t child; //產生出來的process的名字
				child = fork();
				if(child==0)
				{
					close(pfd[1]);
					dup2(pfd[1],1);
					close(pfd[0]);
					//execvp();
				}
				else
				{
					close(pfd[0]);
					dup2(pfd[1],1);
					close(pfd[1]);
					
				}
			}
		}*/
	}
	return 0;
}
