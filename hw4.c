#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct data
{
	int VPN;
	int PFN;
	int in_use;
	int present;
}data;

int main(int argc,char *argv[])
{
	//printf("%s\n",argv[1]);
	FILE *fp;
	char s[100];  //讀一行字串用
	char type[100];
	char *t=NULL;  //type
	char *num=NULL;
	int i=0,j=0;
	int num_page=0,num_frame=0;
	//開檔
	fp=fopen(argv[1],"r");
	if(!fp)	
	{
		printf("open failed\n");
		return 0;
	}
	//讀type
	fgets(s,100,fp);
	//printf("%s",s);
	strtok(s," ");
	t=strtok(NULL," ");
	strcpy(type,t);
	//printf("%s",type);
	//讀num_page
	fgets(s,100,fp);
	strtok(s," ");strtok(NULL," ");strtok(NULL," ");strtok(NULL," ");
	num=strtok(NULL," ");   //取第五個字串（數字）
	while(num[i]!=10)  //字串轉數字,10是new line
	{
		j=num[i]-'0';
		num_page=num_page*10+j;
		i=i+1;
	}
	//printf("%d\n",num_page);
	i=0;j=0;num=NULL;
	//讀num_frame
	fgets(s,100,fp);	
	strtok(s," ");strtok(NULL," ");strtok(NULL," ");strtok(NULL," ");
	num=strtok(NULL," ");
	while(num[i]!=10)  //字串轉數字,10是new line
	{
		j=num[i]-'0';
		num_frame=num_frame*10+j;
		i=i+1;
	}
	//printf("%d\n",num_frame);
	i=0;j=0;num=NULL;
	fgets(s,100,fp); //trace那行分隔線 QQ	
	int number;
	data d[num_page];  //table
	int disk[num_page]; //disk
	int memory[num_frame]; //physical frame
	for(i=0;i<num_page;i++)
	{
		disk[i]=-1;  //初始化 -1 沒用過
		d[i].in_use=0;  //初始化 0 沒用過
		d[i].present=0; //初始化 0 
		d[i].PFN=-1;   //初始化 -1
		d[i].VPN=i;   //每個vpn
	}	
	for(i=0;i<num_frame;i++)
	{
		memory[i]=-1;  //初始化 -1 沒東西
	}
	i=0;j=0;num=NULL;
	/*-------------------------------------------*/	
	/* FIFO */
	if(strcmp(type,"FIFO\n")==0)
	{
		int count=0;   //FIFO被踢出的順序滿了,要歸0
		int k;
		float page_fault_rate;
		float hit=0,miss=0;
		number=0; //此次的vpn      
		//fgets(s,100,fp);   
		//while(!feof(fp))
		while(fgets(s,100,fp)!=NULL)
		{
			//fgets(s,100,fp);   
			//printf("%s",s);
			int c=0;  //c:算還沒用到的physical frame
			int disk_c=num_page-1;
			strtok(s," ");
			num=strtok(NULL," ");
			while(isdigit(num[i])&&num[i]!=10)  //字串轉數字,10是new line
			{
				j=num[i]-'0';
				number=number*10+j;
				i=i+1;
			}
			//printf("%d\n",number);
			//d[number].VPN=number;
			d[number].in_use=1;
			for(k=0;k<num_frame;k++)  //算有沒有空位
			{
				if(memory[k]==-1)
				{
					c=c+1;
				}
			}
			//printf("%d\n",number);
			//有空位直接放進去
			if(c>0)  
			{
				//不在frame的
				if(d[number].present==0){ 
					miss=miss+1;          
					printf("Miss, %d, -1>>-1, %d<<-1\n",count,number);  //還沒出現過的
					memory[count]=number;  //把VPN存進frame
					d[number].PFN=count;   //紀錄vpn放在哪個frame
					d[number].present=1;   //在frame中
					count=count+1;	  
				}
				//在frame的
				else if(d[number].present==1){
					hit=hit+1;
					printf("Hit, %d=>%d\n",number,d[number].PFN);    
				}
			}
			//沒空位了
			else{
				//不在frame的
				if(d[number].present==0){
					miss=miss+1;
					for(k=0;k<num_page;k++)  //找出最早在disk中最早遇到的空位
					{
						if(disk[k]==-1)
						{
							if(k < disk_c)	
								disk_c=k;
						}
					}
					disk[disk_c]=memory[count];	//把被踢掉的,存進disk中
					printf("Miss, %d, %d>>%d, %d<<%d\n",count,memory[count],disk_c,number,d[number].PFN);
					if(d[number].in_use==1)         //若原來在disk中要存進frame,把原來disk的位置空間空出來 
						disk[d[number].PFN]=-1;              
					d[memory[count]].PFN=disk_c;   //把被踢掉的PFN改成在disk的位置
					d[memory[count]].present=0;   //把被踢掉的改成不在frame中
					d[number].present=1;		//要放進的,tag改在frame中
					d[number].PFN=count;		//要放進的,pfn改在他要放進的位置
					memory[count]=number;		//把新的vpn存進frame
					count=count+1;
				}
				else if(d[number].present==1){
					hit=hit+1;
					printf("Hit, %d=>%d\n",number,d[number].PFN);
				}
			}
			if(count==num_frame)
				count=0;
			//printf("%d\n",d[number].VPN);
			//fgets(s,100,fp);  
			//printf("%c",s[1]); 
			i=0;j=0;num=NULL;number=0;  //初始化
		}
		printf("Page Fault Rate: %.3f\n",miss/(miss+hit));	
	}
	/*-------------------------------------------*/
	/* LRU */
	if(strcmp(type,"LRU\n")==0)
	{
		//printf("LRU\n");
		int order[num_frame];  //紀錄順序
		int count=0;   //FIFO被踢出的順序滿了,要歸0
		int k;
		float page_fault_rate;
		float hit=0,miss=0;
		number=0; //此次的vpn      
		for(k=0;k<num_frame;k++)
			order[k]=-1;
		//fgets(s,100,fp);   
		//while(!feof(fp))
		while(fgets(s,100,fp)!=NULL)
		{
			//fgets(s,100,fp);   
			//printf("%s",s);
			int c=0;  //c:算還沒用到的physical frame
			int disk_c=num_page-1;
			int temp;
			strtok(s," ");
			num=strtok(NULL," ");
			while(isdigit(num[i])&&num[i]!=10)  //字串轉數字,10是new line
			{
				j=num[i]-'0';
				number=number*10+j;
				i=i+1;
			}
			//printf("%d\n",number);
			//d[number].VPN=number;
			d[number].in_use=1;
			for(k=0;k<num_frame;k++)  //算有沒有空位
			{
				if(memory[k]==-1)
				{
					c=c+1;
				}
			}
			//printf("%d\n",number);
			//有空位直接放進去
			if(c>0)  
			{
				//不在frame的
				if(d[number].present==0){ 
					miss=miss+1;          
					printf("Miss, %d, -1>>-1, %d<<-1\n",count,number);  //還沒出現過的
					memory[count]=number;  //把VPN存進frame
					order[count]=number;   //把此次的放進order中排序	 
					d[number].PFN=count;   //紀錄vpn放在哪個frame
					d[number].present=1;   //在frame中
					count=count+1;
				}
				//在frame的
				else if(d[number].present==1){
					hit=hit+1;
					//看hit的是order中哪一個
					for(k=0;k<num_frame;k++)
					{
						if(number==order[k])
							temp=k;
					}
					//從他的下一個往前移,還有空位不會到最後一個
					for(k=temp+1;k<num_frame-1;k++)
					{
						order[k-1]=order[k];
					}
					order[num_frame-c-1]=number;  //把hit的那個在放到最後面
					printf("Hit, %d=>%d\n",number,d[number].PFN);    
				}
			}
			//沒空位了
			else{
				//不在frame的
				if(d[number].present==0){
					miss=miss+1;
					//選出要被踢掉的（順序1）,看是在哪個pfn中
					for(k=0;k<num_frame;k++)
					{
						if(order[0]==memory[k])
							count=k;
					}
					for(k=0;k<num_page;k++)  //找出最早在disk中最早遇到的空位
					{
						if(disk[k]==-1)
						{
							if(k < disk_c)	
								disk_c=k;
						}
					}
					disk[disk_c]=memory[count];	//把被踢掉的,存進disk中
					printf("Miss, %d, %d>>%d, %d<<%d\n",count,memory[count],disk_c,number,d[number].PFN);
					if(d[number].in_use==1)         //若原來在disk中要存進frame,把原來disk的位置空間空出來 
						disk[d[number].PFN]=-1;              
					d[memory[count]].PFN=disk_c;   //把被踢掉的PFN改成在disk的位置
					d[memory[count]].present=0;   //把被踢掉的改成不在frame中
					d[number].present=1;		//要放進的,tag改在frame中
					d[number].PFN=count;		//要放進的,pfn改在他要放進的位置
					memory[count]=number;		//把新的vpn存進frame
					//count=count+1;
					//把順序全部往前移一個,且放入新的在最後
					for(k=1;k<num_frame;k++)
					{
						order[k-1]=order[k];
					}
					order[num_frame-1]=number;
				}
				else if(d[number].present==1){
					hit=hit+1;
					//先找到hit的order位置
					for(k=0;k<num_frame;k++)
					{
						if(order[k]==number)
							temp=k;
					}
					//把之後的往前移一格,hit的放到最後面
					for(k=temp+1;k<num_frame;k++)
					{
						order[k-1]=order[k];
					}
					order[num_frame-1]=number;
					printf("Hit, %d=>%d\n",number,d[number].PFN);
				}
			}
			if(count==num_frame)
				count=0;
			//for(k=0;k<num_frame;k++)
			//	printf("%d",order[k]);
			//printf("\n");
			//printf("%d\n",d[number].VPN);
			//fgets(s,100,fp);  
			//printf("%c",s[1]); 
			i=0;j=0;num=NULL;number=0;  //初始化
		}
		printf("Page Fault Rate: %.3f\n",miss/(miss+hit));	
	}
	/*-------------------------------------------*/
	/* RANDOM */
	if(strcmp(type,"Random\n")==0)
	{
		//printf("Random\n");
		srand(time(NULL));
		int count=0;   //FIFO被踢出的順序滿了,要歸0
		int k;
		float page_fault_rate;
		float hit=0,miss=0;
		number=0; //此次的vpn      
		//fgets(s,100,fp);   
		//while(!feof(fp))
		while(fgets(s,100,fp)!=NULL)
		{
			//fgets(s,100,fp);   
			//printf("%s",s);
			int c=0;  //c:算還沒用到的physical frame
			int disk_c=num_page-1;
			strtok(s," ");
			num=strtok(NULL," ");
			while(isdigit(num[i])&&num[i]!=10)  //字串轉數字,10是new line
			{
				j=num[i]-'0';
				number=number*10+j;
				i=i+1;
			}
			//printf("%d\n",number);
			//d[number].VPN=number;
			d[number].in_use=1;
			for(k=0;k<num_frame;k++)  //算有沒有空位
			{
				if(memory[k]==-1)
				{
					c=c+1;
				}
			}
			//printf("%d\n",number);
			//有空位直接放進去
			if(c>0)  
			{
				//不在frame的
				if(d[number].present==0){ 
					miss=miss+1;          
					printf("Miss, %d, -1>>-1, %d<<-1\n",count,number);  //還沒出現過的
					memory[count]=number;  //把VPN存進frame
					d[number].PFN=count;   //紀錄vpn放在哪個frame
					d[number].present=1;   //在frame中
					count=count+1;	  
				}
				//在frame的
				else if(d[number].present==1){
					hit=hit+1;
					printf("Hit, %d=>%d\n",number,d[number].PFN);    
				}
			}
			//沒空位了
			else{
				//不在frame的
				if(d[number].present==0){
					count=rand()%num_frame;
					miss=miss+1;
					for(k=0;k<num_page;k++)  //找出最早在disk中最早遇到的空位
					{
						if(disk[k]==-1)
						{
							if(k < disk_c)	
								disk_c=k;
						}
					}
					disk[disk_c]=memory[count];	//把被踢掉的,存進disk中
					printf("Miss, %d, %d>>%d, %d<<%d\n",count,memory[count],disk_c,number,d[number].PFN);
					if(d[number].in_use==1)         //若原來在disk中要存進frame,把原來disk的位置空間空出來 
						disk[d[number].PFN]=-1;              
					d[memory[count]].PFN=disk_c;   //把被踢掉的PFN改成在disk的位置
					d[memory[count]].present=0;   //把被踢掉的改成不在frame中
					d[number].present=1;		//要放進的,tag改在frame中
					d[number].PFN=count;		//要放進的,pfn改在他要放進的位置
					memory[count]=number;		//把新的vpn存進frame
					//count=rand()%num_frame;
				}
				else if(d[number].present==1){
					hit=hit+1;
					printf("Hit, %d=>%d\n",number,d[number].PFN);
				}
			}
			if(count==num_frame)
				count=0;
			//printf("%d\n",d[number].VPN);
			//fgets(s,100,fp);  
			//printf("%c",s[1]);
			printf(" count=%d\n",count); 
			i=0;j=0;num=NULL;number=0;  //初始化
		}
		printf("Page Fault Rate: %.3f\n",miss/(miss+hit));	
	}
	fclose(fp);
	return 0;
}
