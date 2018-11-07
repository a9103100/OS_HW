#include "Mailbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SIZE_OF_SHORT_STRING 64
#define SIZE_OF_LONG_STRING 512

/*
typedef struct __MAIL {
	int from;
	int type;
	char sstr[SIZE_OF_SHORT_STRING];
	char lstr[SIZE_OF_LONG_STRING];
} mail_t;
*/
//typedef void *mailbox_t;  


mailbox_t mailbox_open(int id)
{
	char name[50];
	int shm_fd=0;
	sprintf(name,"__mailbox_%d",id);

	shm_fd = shm_open(name, O_CREAT|O_RDWR, S_IRUSR | S_IWUSR);
	if(shm_fd == -1)
	{
		if(errno == EEXIST)
			shm_fd=shm_open(name,O_RDWR,S_IRUSR | S_IWUSR);
	}
	if(shm_fd == -1)
		return NULL;
	return (mailbox_t)shm_fd; 
}

int mailbox_unlink(int id)
{
	char name[50];
	int shm_fd;
	sprintf(name,"__mailbox_%d",id);
	int err=0;
	err=shm_unlink(name);
	return err;
}

int mailbox_close(mailbox_t box)
{
	int output=0;
	output=close((int)box);
	return output;
}

/*
int mailbox_check_empty(mailbox_t box)
{
	int check;
	if(box==NULL)
		return -1;

	check=lseek((int)box,0,SEEK_CUR);
	if(check==0)
		return 1;
	else
		return 0;	
}

int mailbox_check_full(mailbox_t box)
{
	int check;
	if(box==NULL)
		return -1;
	check=lseek((int)box,0,SEEK_END);
	if(check==-1)
		return 1;
	else 	
		return 0;
}*/

int mailbox_send(mailbox_t box, mail_t *mail)
{
	int check;
	if(box==NULL)
	{
		printf("box NULL\n");	
		return -1;
	} 
	lseek((int)box,0,SEEK_END);
	check=write((int)box,mail,sizeof(mail_t));
	//printf("%d\n",(int)lseek((int)box,0,SEEK_CUR));
	if(check==-1)
		return -1;
	else 
		return 0;
}

int mailbox_recv(mailbox_t box, mail_t *mail)
{
	int temp=0, check;
	
	if(box==NULL)
		return -1;
	temp=lseek((int)box,0,SEEK_END);
//	printf("%d\n",temp);
//	lseek((int)box,temp-sizeof(mail_t),SEEK_SET);
	lseek((int)box,0,SEEK_SET);
	check=read((int)box,mail,sizeof(mail_t));
	if(check==-1)
		return -1;

	char buf[temp-sizeof(mail_t)];

	lseek((int)box,sizeof(mail_t),SEEK_SET);
	read((int)box,buf,temp-sizeof(mail_t));

	ftruncate((int)box, temp-sizeof(mail_t));
	lseek((int)box,0,SEEK_SET);
	write((int)box,buf,temp-sizeof(mail_t));
/*	temp=lseek((int)box,sizeof(mail),SEEK_END);
	lseek((int)box,sizeof(mail),SEEK_SET);
	read((int)box,buf,temp);
	lseek((int)box,0,SEEK_SET);
	write((int)box,buf,temp); 
*/
//	printf("from %s:%s\n",mail->sstr,mail->lstr);
	return 0;
}

int mailbox_check_empty(mailbox_t box)
{
        int check;
        if(box==NULL)
                return -1;

        check=lseek((int)box,0,SEEK_END);
	if(check==0)
                return 1;
        else
                return 0;
}

int mailbox_check_full(mailbox_t box)
{
        int check;
        if(box==NULL)
                return -1;
        check=lseek((int)box,0,SEEK_END);
        if(check==-1)
                return 1;
        else
                return 0;
}



