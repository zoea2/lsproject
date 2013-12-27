#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>  
char* filename[1000000];
int filenameLen[1000000];
int widthPerColumn[48];
void countFilenameLen(char* filename,int n){
	int count = 0;
	for(int i = 0;i < strlen(filename);i++){
		if(filename[i] < 0 || filename[i] > 127)
		{
			count++;
			i+=2;
		}
		count++;
	}
	filenameLen[n] = count;
}
char* strupr(char* a){
	char* p =(char *)malloc(256 * sizeof(char)) ;
	strcpy(p,a);
	int i =0;
	while(p[i]){
		p[i] = toupper(p[i]);
		i++;
	}
	//printf("%s\n",p);
	return p;
}

int cmp ( const void *a , const void *b ) 
{
	char* p = strupr(*(char**)a);
	char* q = strupr(*(char**)b);
	int res = strcmp(p,q);
	free(p);
	free(q);
	return res;

} 
void display(int numFile){
	int row = 1;
	struct winsize ws;  
	if (ioctl(0,TIOCGWINSZ,&ws)!=0) {
		    
		      fprintf(stderr,"TIOCGWINSZ:%s/n",strerror(errno));  
			      exit(1);  
	}  
	while(1){
		memset(widthPerColumn,0,sizeof(widthPerColumn));
		int column = 0;
		int i = 0;
		while(i < numFile){
			for(int j = 0;j < row;j++){
				if(i < numFile){
					if(widthPerColumn[column] < filenameLen[i])
						widthPerColumn[column] = filenameLen[i];
					i++;
		
				}
				else
					break;
			}
			column++;
		}
		int sum = 0;
		for(int j = 0;j < column;j++){
			sum += widthPerColumn[j]+2;
		}
		if(sum <= ws.ws_col){
			for(int j = 0;j < row;j++){
				for(int k = 0;k < column;k++){
					if(k * row + j < numFile){
						printf("%s  ",filename[k * row + j]);
						for(int t = 0;t < widthPerColumn[k] - filenameLen[k*row+j];t++)
							printf(" ");
					}

				}
				printf("\n");
			}
			return;
		}
		else row++;

	}
	

}
void testPerm(int mode,char* modes){
	if(mode & 4)
		modes[0] = 'r';
	if(mode & 2)
		modes[1] = 'w';
	if(mode & 1)
		modes[2] = 'x';
}
char* filemode(int mode){
	static char modes[11];
	strcpy(modes,"----------");
	char type;
	switch(mode & S_IFMT){
		case S_IFREG:	type = '-';	break;
		case S_IFDIR:	type = 'd';	break;
		case S_IFCHR:	type = 'c';	break;	
		case S_IFBLK:	type = 'b';	break;
		default:		type = '?';	break;
	}
	modes[0] = type;
	
	testPerm( mode>>6 , modes+1 );	
	testPerm( mode>>3 , modes+4 );	
	testPerm( mode    , modes+7 );
	return modes;
}
char* uid_to_name( short uid )

{
	struct passwd  *pw_ptr;

	if ( ( pw_ptr = getpwuid( uid ) ) == NULL )
		return "Unknown" ;
	else
		return pw_ptr->pw_name ;
}

char* gid_to_name( short gid )
{

	struct group *grp_ptr;

	if ( ( grp_ptr = getgrgid(gid) ) == NULL )
		return "Unknown" ;
	else
		return grp_ptr->gr_name;
}

void displayWithL(int numFile,char* path){
	for(int i = 0;i < numFile;i++){
		struct stat fileinfo;
		char temp[500];
		strcpy(temp,path);
		strcat(temp,"/");
		strcat(temp,filename[i]);
		if(stat(temp,&fileinfo) == -1)
			perror(temp);
		else{
			printf( "%s"    , filemode(fileinfo.st_mode) );
			printf( "%4d "  , (int) fileinfo.st_nlink);	
			printf( "%8s " , uid_to_name(fileinfo.st_uid) );
			printf( "%8s " , gid_to_name(fileinfo.st_gid) );
			printf( "%8ld " , (long)fileinfo.st_size);
			printf( "%.12s ", 4+ctime(&fileinfo.st_mtime));
			printf( "%s\n"  , filename[i] );
		}
	}
}
void do_ls(char* path,int isL){
	DIR *dir_ptr;
	struct dirent *direntp;
	int numFile = 0;
	if((dir_ptr = opendir(path)) == NULL){
		fprintf(stderr,"ls1: cannot open %s\n", path);
	}
	else{
		while ( ( direntp = readdir( dir_ptr ) ) != NULL ){
			if((direntp->d_name)[0] != '.' ){
				filename[numFile++] = direntp->d_name;
			
			}
		}
		qsort(filename,numFile,sizeof(char*),cmp);
	
		if(!isL){
			for(int i = 0;i < numFile;i++){
				countFilenameLen(filename[i],i);
			}
			display(numFile);
		}
		else
			displayWithL(numFile,path);
		closedir(dir_ptr);

	}
}
main(int ac,char* av[]){
	if(ac == 1)
		do_ls(".",0);
	if(ac == 2 && strcmp(av[1],"-l") == 0) 
		do_ls(".",1);
	else{

		int isL = 0;
		int numDir = 0;
		for(int i = 1;i < ac;i++){
			if(strcmp(av[i],"-l") == 0)
				isL = 1;
			else
				numDir++;
		}
		int br = numDir;
		while(--ac){
			//printf("%s\n",*(++av));
			av++;
			if(strcmp(*av,"-l") != 0){
				if(numDir > 1)
					printf("%s:\n",*av);
				do_ls(*av,isL);
				if(br-- > 1)
					printf("\n");
			}
		}
	}

}
