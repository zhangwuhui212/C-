// encrypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include<time.h>


void password(void)    //����������֤����
{
	char pass[20];
	char word[20];
	int i=0,a=1;
	FILE *pw;
	pw=fopen("pw.obj","ab+");  //�������ļ�   �����½�
	if(pw==NULL)
	{
		printf("�ļ���ʧ��");
	}
	while(fread(&pass[i],sizeof(char),1,pw))  //��ȡ����
		i++;
	pass[i]='\0';
	fclose(pw);
	if(strlen(pass))
	{
		while(a<4)
		{
			printf("���%d/4��������������������(�ûس�������)!\n",a);

			for(i=0;i<20;)
			{
				word[i]=(char)getch();
				if(word[i]==8)
				{
					printf("\b \b");
					i--;
					word[i]='\0';
					continue;
				}
				if(word[i]==13)
				{
					word[i]='\0';
					break;
				}
				else
					printf("*");
				i++;
			}
			if(strcmp(pass,word)==0)
				break;
			else
				printf("\n�����������!\n");
			system("pause");
			system("cls");
			a++;
		}
		if(a==4)
		{
			printf("\n\n\t��ֹ�Ƿ���½!!!\n\n");
			system("pause");
			exit(0);
		}
	}
	else;
	system("attrib pw.obj +h");  //���������ļ���������

}

void addcode(void)    //��ӳ������뺯��
{
	char word1[20],word2[20];
	int i;
	FILE *pw;
	system("attrib pw.obj -h");  //ȥ�������ļ���������
	printf("������������(�Իس�����):");
	for(i=0;i<20;)
	{
		word1[i]=(char)getch();
		if(word1[i]==8)
		{
			printf("\b \b");
			i--;
			word1[i]='\0';
			continue;
		}
		if(word1[i]==13)
		{
			word1[i]='\0';
			break;
		}
		else
			printf("*");
		i++;
	}
	printf("\n���ٴ�����������(�Իس�����):");
	for(i=0;i<20;)
	{
		word2[i]=(char)getch();
		if(word2[i]==8)
		{
			printf("\b \b");
			i--;
			word2[i]='\0';
			continue;
		}
		if(word2[i]==13)
		{
			word2[i]='\0';
			break;
		}
		else
			printf("*");
		i++;
	}
	if(strcmp(word1,word2)==0)
	{
		pw=fopen("pw.obj","wb");
		if(pw==NULL)
		{
			printf("�ļ���ʧ!\n");
		}
		fwrite(word1,1,strlen(word1),pw);
		fclose(pw);
		printf("\n�������óɹ�!\n");
		system("attrib pw.obj +h");     //���������ļ���������

	}
	else
	{
		printf("�����������벻ͬ,��������ʧ��!");
		system("attrib pw.obj +h");
	}
	system("pause");

}

void delcode()  //��������ɾ��
{
	system("attrib pw.obj -h");
	system("del pw.obj");
	printf("����ɾ���ɹ�!");
	fclose(fopen("pw.obj","wb"));  //�½������ļ�
	system("attrib pw.obj +h");
	system("pause");
}



void code(void)   //�����������ú���
{
	unsigned short a,q;
	system("cls");
	password();
	do
	{
		system("cls");
		printf("\t\t*********��ѡ�������Ŀ**************\n");
		printf("\n\t\t*****  \t1---�������롣\t        *****");
		printf("\n\t\t*****  \t2---�޸����롣\t*****");
		printf("\n\t\t*****  \t3---ɾ�����롣\t*****");
		printf("\n\t\t*****  \t0---������һ����\t        *****");
		printf("\n\t\t*************************************\n");
		printf("\n\t\t��ѡ��0--3\n\t\t");
		q=scanf("%d",&a);
		flushall();
		if(q)
		{
			switch(a)
			{
			case 1: addcode();break;   //��������
			case 2: addcode(); break;  //�����޸�
			case 3: delcode(); break;  //����ɾ��
			case 0: return;
			default: 
				printf("\t\tѡ�����!\n\n");
				system("pause");	//������ִ����ͣ�ڴˣ������������
			}
		}
		else
		{
			printf("\t\t��������!\n\n");
			system("pause");
		}
	}while(1);
}
///////////////////////////////////////////////////////////////////////����Ϊ�������벿��


void Encryption(void)  //���ܺ���
{
	FILE *fp,*temp;
	int i=0,a=0,j=0,k=0;
	short swap1,swap2;
	char ch,strfilename[30],strtempbuff[256],pass[20],word2[20],t[22];
	clock_t start,finish;
	double duration;
	while(1)  //����Ҫ�����ļ�������
	{
		system("cls");
		printf("������Ҫ���ܵ��ļ���(�Ӻ�׺��):\n");
		flushall();
		gets(strfilename);
		if(0==strlen(strfilename))
		{
			printf("����Ϊ��,����������!\n");
			system("pause");
		}
		else break;
	}
	if((fp=fopen(strfilename,"rb+"))==NULL)  
	{
		printf("�ļ�%s������!\n",strfilename);
		system("pause");
		return;
	}
	if((temp=fopen("tempfile.txt","wb+"))==NULL)
	{
		printf("��ʱ�ļ�����ʧ��!\n");
		system("pause");
		return;
	}
	while(1)             //�����������
	{
		system("cls");
		printf("�������������:\n");
		for(i=0;i<20;)
		{
			pass[i]=(char)getch();
			if(pass[i]==8)
			{
				printf("\b \b");
				i--;
				pass[i]='\0';
				continue;
			}
			if(pass[i]==13)
			{
				pass[i]='\0';
				break;
			}
			else
				printf("*");
			i++;
		}
		if(0==strlen(pass))
		{
			printf("���벻��Ϊ��!\n");
			system("pause");
			continue;
		}
		printf("\n���ٴ������������:\n");
		for(i=0;i<20;)
		{
			word2[i]=(char)getch();
			if(word2[i]==8)
			{
				printf("\b \b");
				i--;
				word2[i]='\0';
				continue;
			}
			if(word2[i]==13)
			{
				word2[i]='\0';
				break;
			}
			else
				printf("*");
			i++;
		}
		if(strcmp(pass,word2)!=0)
		{
			printf("\n�����������벻ͬ,����������!\n");
			system("pause");
		}
		else break;
	}
	///////////////////////////////////////////////////////////////////////////////
	k=strlen(pass);
	printf("\n���Եȣ�������...\n");
	start=clock();  //���ܼ�ʱ��ʼ����
	t[0]=k;  //��¼���볤��
	for(i=1;i<=k;i++)
	{
		t[i]=pass[i-1];
	}
	t[i]='\0';
	i=0;
	while(t[i]!='\0')
	{
		if(j==k)
			j=0;
		swap1=t[i];
		swap1&=255;
		swap2=pass[j];
		swap1=swap1+swap2;
		swap1=~swap1;
		fwrite(&swap1,sizeof(short),1,temp);
		j++;
		i++;
	}
	while(fread(&ch,sizeof(char),1,fp)!=0)
	{
		if(j==k)
			j=0;                                //�����㷨������
		swap1=ch;
		swap1&=255;
		swap2=pass[j];
		swap1=swap1+swap2;
		swap1=~swap1;
		fwrite(&swap1,sizeof(short),1,temp);
		j++;
	}
	//////////////////////////////////////////////////////////////////////////////
	fclose(temp);
	fclose(fp);
	finish=clock();   //���ܽ�������
	sprintf(strtempbuff,"del %s",strfilename);
	system(strtempbuff);
	sprintf(strtempbuff,"ren tempfile.txt %s",strfilename);
	system(strtempbuff);
	printf("���ܳɹ�!\n");
	printf("���μ�����ʱ:\t");
	duration=(double)(finish-start)/CLOCKS_PER_SEC;  //�������ʱ��
	if(0==duration)
		duration=1;
	printf("%f��\n",duration);
	system("pause");
	return;
}


void Decrypt(void)    //���ܺ���
{
	FILE *fp,*temp;
	int j=0,k=0,i=0,m=0;
	short swap,f=1;
	char ch,strfilename[30],strtempbuff[256],pass[20],word[21];
	clock_t start,finish;
	double duration;
	while(1)
	{
		system("cls");
		printf("������Ҫ���ܵ��ļ���(�Ӻ�׺��):\n");
		gets(strfilename);
		if(0==strlen(strfilename))
		{
			printf("����Ϊ��,����������!\n");
			system("pause");
		}
		else break;
	}
	if((fp=fopen(strfilename,"rb+"))==NULL)
	{
		printf("�ļ�%s������!\n",strfilename);
		system("pause");
		return;
	}
	if((temp=fopen("tempfile.txt","wb+"))==NULL)
	{
		printf("��ʱ�ļ�����ʧ��!\n");
		system("pause");
		return;
	}
	do 
	{
		system("cls");
		printf("�������������:\n");
		for(i=0;i<20;)
		{
			pass[i]=(char)getch();
			if(pass[i]==8)
			{
				printf("\b \b");
				i--;
				pass[i]='\0';
				continue;
			}
			if(pass[i]==13)
			{
				pass[i]='\0';
				break;
			}
			else
				printf("*");
			i++;
		}
		k=strlen(pass);
		printf("\n������...\n");
		i=0;
		j=0;
		rewind(fp);
		while(i<=k&&fread(&swap,sizeof(short),1,fp)!=0)  //�����������볤�ȡ�������
		{
			if(j==k)
				j=0;
			swap=~swap;
			word[i]=swap-pass[j];
			j++;
			i++;
		}
		if(k!=word[0])
		{
			printf("�������!\n");
			system("pause");
			continue;
		}
		for(i=0,m=1;i<k;i++,m++)
		{
			if(pass[i]!=word[m])
			{
				printf("�������!\n");
				f=0;
				system("pause");
				break;
			}
			else
				f=1;
		}
		if(f)
			break;
	}while(1);
	start=clock();    //���ܼ�ʱ��ʼ
	///////////////////////////////////////////////////////////////////////////////
	while(fread(&swap,sizeof(short),1,fp)!=0)
	{
		if(j==k)
			j=0;                                //�����㷨������
		swap=~swap;
		ch=swap-pass[j];
		fwrite(&ch,sizeof(char),1,temp);
		j++;
	}
	//////////////////////////////////////////////////////////////////////////////
	fclose(temp);
	fclose(fp);
	finish=clock();     //���ܼ�ʱ����
	sprintf(strtempbuff,"del %s",strfilename);
	system(strtempbuff);
	sprintf(strtempbuff,"ren tempfile.txt %s",strfilename);
	system(strtempbuff);
	printf("�������!\n");
	printf("���ν�����ʱ:\t");
	duration=(double)(finish-start)/CLOCKS_PER_SEC;
	if(0==duration)
		duration=1;
	printf("%f��\n",duration);
	system("pause");
	return;
}


void menu()  //�˵���������
{
	unsigned short a,q;
	do
	{
		system("cls");
		printf("\n\n\n\t\t    ��ӭʹ�������ļ����ܽ��ܳ���\n\n");
		printf("\t\t*********��ѡ�������Ŀ**************");
		printf("\n\t\t*****  \t   1---�����ļ�\t        *****");
		printf("\n\t\t*****  \t   2---�����ļ�\t        *****");
		printf("\n\t\t*****  \t   3---��������\t        *****");
		printf("\n\t\t*****  \t   0---�˳�ϵͳ\t        *****");
		printf("\n\t\t*************************************\n");
		printf("\n\t\t��ѡ��0--5\n\t\t");
		q=scanf("%d",&a);
		flushall();
		if(q)
		{
			switch(a)
			{
			case 1: Encryption();break; //���ܺ���
			case 2: Decrypt(); break;   //���ܺ���
			case 3: code(); break;      //�����������ú���
			case 0: printf("\n\t\t��ӭ�´�ʹ�ã��ټ�!\n\n\n");
				system("pause");
				exit(0);
			default: printf("\t\tѡ�����!\n\n");
				system("pause");
			}
		}
		else
		{
			printf("\t\t��������!\n\n");
			system("pause");
		}
	}while(1);
}



int main()
{
	password();  //��֤��������
	menu();      //��ʾ�˵�
}

