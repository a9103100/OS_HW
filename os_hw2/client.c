#include "Mailbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define NONE "\033[m"
#define LIGHT_GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"

static int s_fd,c_fd;
static int c_id;
        
static mail_t mail;
static mail_t recv_mail;


void *p_send(void *argu)
{
	while(1)
	{
		//scanf("%s",mail.lstr);
		char ans[100]="";
		int pos=0;
         	while(1)
		{
			int c=getchar();
			if(c!=EOF&&c!=10)
				ans[pos++]=c;
			if(c==10)
				break;
		}
		strcpy(mail.lstr,ans);
		if(strcmp(mail.lstr,"LEAVE")==0)
         	{
         		mail.type=3; //LEAVE = 3
                	if(mailbox_check_full((mailbox_t)s_fd)==0)        
				mailbox_send((mailbox_t)s_fd, &mail);
                	mailbox_unlink(c_id);
              		printf(YELLOW"Bye!\n");
               	  	break;
         	}
		else if(strcmp(mail.lstr,"LIST")==0)
		{
			mail.type=4; //LIST = 4
			if(mailbox_check_full((mailbox_t)s_fd)==0)
                                mailbox_send((mailbox_t)s_fd, &mail);
		}
         	else
         	{
                	mail.type=2; //BROADCAST = 2
                	if(mailbox_check_full((mailbox_t)s_fd)==0)
                        	 mailbox_send((mailbox_t)s_fd, &mail);
         	}
	}
	return NULL;
}

void *p_recv(void *argu)
{
	while(1)
	{
		if(strcmp(mail.lstr,"LEAVE")==0)
                {
                  	break;
                }
		else if(mailbox_check_empty((mailbox_t)c_fd)==0)
		{
			mailbox_recv((mailbox_t)c_fd,&recv_mail);
			printf(YELLOW"from %s "NONE":%s\n",recv_mail.sstr,recv_mail.lstr);
		}
	}
	return NULL;
}

int main()
{
	//int check=2;	
	/*
	int s_fd,c_fd;
	int c_id;
	*/
	srand(time(NULL));
	c_id=(rand()%10000)+1;
	s_fd = (int) mailbox_open(0);
	c_fd = (int) mailbox_open(c_id);

/*
	mailbox_t mailbox;
	mail_t mail;
	mail_t recv_mail;
	*/
	printf(YELLOW"Welcome!!^^\nPlease enter your name "NONE":");	
	//scanf("%s",mail.sstr);
	char ans[100]="";
        int pos=0;
        while(1)
        {
        	int c=getchar();
                if(c!=EOF&&c!=10)
              		ans[pos++]=c;
                if(c==10)
                        break;
        }
	printf(LIGHT_GREEN"-------------------------------"NONE"\n");
	printf(YELLOW"Enter \"LIST\" to check members\nEnter \"LEAVE\" to quit"NONE"\n");
	printf(LIGHT_GREEN"-------------------------------"NONE"\n");
	strcpy(mail.sstr,ans);
	mail.from=c_id;
	mail.type=1;  //JOIN = 1
	//strcpy(mail.lstr,"i am in!");

	if(mailbox_check_full((mailbox_t)s_fd)==0)
		mailbox_send((mailbox_t)s_fd, &mail);
	
	/*while(1){
		scanf("%s",mail.lstr);
		if(strcmp(mail.lstr,"LEAVE")==0)
		{	
			mail.type=3; //LEAVE = 3
			if(mailbox_check_full((mailbox_t)s_fd)==0)
                		mailbox_send((mailbox_t)s_fd, &mail);
			mailbox_unlink(c_id);
			printf("Bye!\n");
			break;
		}
		else
		{
			mail.type=2; //BROADCAST = 2
			if(mailbox_check_full((mailbox_t)s_fd)==0)
                		mailbox_send((mailbox_t)s_fd, &mail);
		}
		if(mailbox_check_empty((mailbox_t)c_fd)==0)
		{
			mailbox_recv((mailbox_t)c_fd,&recv_mail);
			printf("from %s :%s\n",recv_mail.sstr,recv_mail.lstr);
		}
	}*/

	pthread_t thread1,thread2;
	pthread_create(&thread1,NULL,&p_send,NULL);
	pthread_create(&thread2,NULL,&p_recv,NULL);
	
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	return 0;
}
