#include "Mailbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>

static int s_fd, c_fd[20];
static int i,temp;
static char list[20][50];
static mail_t mail;
static int check=2,count=0;

void *p_leave(void *argu)
{
	while(1){
		char leave[10];
		mail_t close_mail;
		scanf("%s",leave);
		if(strcmp(leave,"LEAVE")==0)
		{
			strcpy(close_mail.sstr,"server");
			close_mail.type=3;
			close_mail.from=s_fd;
			strcpy(close_mail.lstr,"Server is closed. Please enter \"LEAVE\" to quit.");
			for(i=0;i<count;i++)
			{
 				if(mailbox_check_full((mailbox_t)c_fd[i])==0)
               	         		mailbox_send((mailbox_t)c_fd[i],&close_mail);
			}
			if(mailbox_check_full((mailbox_t)s_fd)==0)
                        	mailbox_send((mailbox_t)s_fd,&close_mail);
			mailbox_unlink(0);
			break;
		}
	}
	return NULL;
}

void *p_server(void *argu)
{
	while(1) {
//		check=mailbox_check_empty((mailbox_t)s_fd);
//		printf("%d\n",check);
		while(mailbox_check_empty((mailbox_t)s_fd)==0){
			mailbox_recv((mailbox_t)s_fd, &mail);
			for(i=0;i<count;i++)
			{
				if(strcmp(mail.sstr,list[i])==0)
					temp=i;
			}
			//printf("%d",mail.type);
			if(mail.type==1)
			{
			//	printf("%d\n",mail.from);
				c_fd[count] = (int)mailbox_open(mail.from);
				printf("%s JOIN\n",mail.sstr);
				strcpy(list[count],mail.sstr);
				char message[100];
				strcpy(message,mail.sstr);
				strcat(message," JOIN");
				strcpy(mail.lstr,message);
				strcpy(mail.sstr,"server");
				for(i=0;i<count;i++)
                                {
                                        if(mailbox_check_full((mailbox_t)c_fd[i])==0)
                                	{
                                        	mailbox_send((mailbox_t)c_fd[i],&mail);
                                        }
                                }
				count=count+1;
			}
			else if(mail.type==2)
			{	
				printf("%s: %s\n", mail.sstr,mail.lstr);
				for(i=0;i<count;i++)
				{
					if(strcmp(mail.sstr,list[i])!=0){	
						if(mailbox_check_full((mailbox_t)c_fd[i])==0)
						{
							mailbox_send((mailbox_t)c_fd[i],&mail);
							//printf("%d\n",c_fd[i]);
						}	
					}
				}
			}
			else if(mail.type==3)
			{
				printf("%s LEAVE\n",mail.sstr);
				if(strcmp(mail.sstr,"server")==0)
				{
					return NULL;
				}
				int t;
				for(i=0;i<count;i++)
				{			
					if(strcmp(mail.sstr,list[i])==0)
						t=i;
				}
				for(i=t;i<count-1;i++)
				{
					c_fd[i]=c_fd[i+1];
					strcpy(list[i],list[i+1]);
				}	
				c_fd[count]=0;
				strcpy(list[count],"");
				count=count-1;
				char message[100];
                                strcpy(message,mail.sstr);
                                strcat(message," LEAVE");
                                strcpy(mail.lstr,message);
                                strcpy(mail.sstr,"server");
                                for(i=0;i<count;i++)
                                {
                                        if(mailbox_check_full((mailbox_t)c_fd[i])==0)
                                        {
                                                mailbox_send((mailbox_t)c_fd[i],&mail);
                                        }
                                }
			}
			else if(mail.type==4)
			{
				printf("%s LIST\n",mail.sstr);
				char member[100]="";
				for(i=0;i<count-1;i++)
				{
					//strcat(member," ");
					strcat(member,list[i]);
					strcat(member,", ");
				}
				strcat(member,list[count-1]);
				printf("%s\n",member);
				mail.from=0;
				mail.type=4;
				strcpy(mail.sstr,"server");
				strcpy(mail.lstr,member);
				if(mailbox_check_full((mailbox_t)c_fd[temp])==0)
                                {
                                	mailbox_send((mailbox_t)c_fd[temp],&mail);
                                }

			}
			/*for(i=0;i<count;i++)
			{
				printf("%d %s %d\n",i,list[i],c_fd[i]);
			}*/
		}
	}	
	return NULL;
}

int main()
{
	/*
	int s_fd, c_fd[20];
	int i,temp;
	char list[20][50];

	mail_t mail;
	int check=2,count=0;
	*/
	s_fd = (int)mailbox_open(0);
	
//	check=mailbox_recv((mailbox_t)s_fd,&mail);
//	printf("%d\n",check);
/*	while(1) {
//		check=mailbox_check_empty((mailbox_t)s_fd);
//		printf("%d\n",check);
		while(mailbox_check_empty((mailbox_t)s_fd)==0){
			mailbox_recv((mailbox_t)s_fd, &mail);
			for(i=0;i<count;i++)
			{
				if(strcmp(mail.sstr,list[i])==0)
					temp=i;
			}
			//printf("%d",mail.type);
			if(mail.type==1)
			{
			//	printf("%d\n",mail.from);
				c_fd[count] = (int)mailbox_open(mail.from);
				printf("%s JOIN\n",mail.sstr);
				strcpy(list[count],mail.sstr);
				char message[100];
				strcpy(message,mail.sstr);
				strcat(message," JOIN");
				strcpy(mail.lstr,message);
				strcpy(mail.sstr,"server");
				for(i=0;i<count;i++)
                                {
                                        if(mailbox_check_full((mailbox_t)c_fd[i])==0)
                                	{
                                        	mailbox_send((mailbox_t)c_fd[i],&mail);
                                        }
                                }
				count=count+1;
			}
			else if(mail.type==2)
			{	
				printf("%s: %s\n", mail.sstr,mail.lstr);
				for(i=0;i<count;i++)
				{
					if(strcmp(mail.sstr,list[i])!=0){	
						if(mailbox_check_full((mailbox_t)c_fd[i])==0)
						{
							mailbox_send((mailbox_t)c_fd[i],&mail);
							//printf("%d\n",c_fd[i]);
						}	
					}
				}
			}
			else if(mail.type==3)
			{
				printf("%s LEAVE\n",mail.sstr);
				int t;
				for(i=0;i<count;i++)
				{			
					if(strcmp(mail.sstr,list[i])==0)
						t=i;
				}
				for(i=t;i<count-1;i++)
				{
					c_fd[i]=c_fd[i+1];
					strcpy(list[i],list[i+1]);
				}	
				c_fd[count]=0;
				strcpy(list[count],"");
				count=count-1;
				char message[100];
                                strcpy(message,mail.sstr);
                                strcat(message," LEAVE");
                                strcpy(mail.lstr,message);
                                strcpy(mail.sstr,"server");
                                for(i=0;i<count;i++)
                                {
                                        if(mailbox_check_full((mailbox_t)c_fd[i])==0)
                                        {
                                                mailbox_send((mailbox_t)c_fd[i],&mail);
                                        }
                                }
			}
			else if(mail.type==4)
			{
				printf("%s LIST\n",mail.sstr);
				char member[100]="";
				for(i=0;i<count;i++)
				{
					strcat(member," ");
					strcat(member,list[i]);
				}
				printf("%s\n",member);
				mail.from=0;
				mail.type=4;
				strcpy(mail.sstr,"server");
				strcpy(mail.lstr,member);
				if(mailbox_check_full((mailbox_t)c_fd[temp])==0)
                                {
                                	mailbox_send((mailbox_t)c_fd[temp],&mail);
                                }

			}
			//for(i=0;i<count;i++)
			//{
			//	printf("%d %s %d\n",i,list[i],c_fd[i]);
			//}
		}
	}*/
		//check=mailbox_recv(mailbox,mail);
	pthread_t thread1,thread2;
	pthread_create(&thread1,NULL,&p_leave,NULL);
	pthread_create(&thread2,NULL,&p_server,NULL);

	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	return 0;
}
