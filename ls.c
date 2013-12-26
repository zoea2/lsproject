#include <stdio.h>
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
void do_ls(char* path){
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
		for(int i = 0;i < numFile;i++){
			countFilenameLen(filename[i],i);
		//	printf("%s %d\n",filename[i],filenameLen[i]);

		}
		display(numFile);
		closedir(dir_ptr);

	}
}
main(int ac,char* av[]){
	if(ac == 1)
		do_ls(".");
	else{
		int isL = false;
		int numDir = 0;
		for(int i = 1;i < ac;i++){
			if(av[i] == "-l")
				isL = true;
			else
				numDir++;
		}
		while(--ac){
			//printf("%s\n",*(++av));
			av++;
			if(numDir > 1)
				printf("%s:\n",*av);
			if(*av != "-l"){
				if(!isL)
					do_ls(*av);
			}
			if(ac != 1)
				printf("\n");
		}
	}

}
