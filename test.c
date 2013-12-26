#include <stdio.h>
#include <string.h>	
#include <ctype.h>
main(){
	char* a = "百度一下你就知道";
	char* b = "ssssssssssssssss";
	printf("%s %d %s %d\n",a,strlen(a),b,strlen(b));
	printf("%d %d\n",sizeof('白'),strlen("白"));
	char * p = strupr(b);
	printf("%s\n",p);
}
