#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <locale.h>

//Navratove hodnoty

#define RET_OK    0x00000000      	//all OK
#define RET_FILE  0x00000001		//chyba pri otvarani suboru
#define RET_ARGS  0x00000002		//chyba parametrov prikazoveho riadku
#define RET_ALK   0x00000004		//chyba pri alokacii
#define RET_INP   0x00000008		//chybny vstup alebo format dat v subore
#define RET_ERR   0x00000016		//nespecifikovana chyba

const unsigned char firstpass[] = {
	[(unsigned char)'A'] = 1, [(unsigned char)'a'] = 1, [(unsigned char)'Á'] = 1,
	[(unsigned char)'á'] = 1, [(unsigned char)'B'] = 2, [(unsigned char)'b'] = 2, 
	[(unsigned char)'C'] = 3, [(unsigned char)'c'] = 3, [(unsigned char)'È'] = 4, 
	[(unsigned char)'è'] = 4, [(unsigned char)'d'] = 5, [(unsigned char)'D'] = 5,
	[(unsigned char)'Ï'] = 5, [(unsigned char)'ï'] = 5,
	[(unsigned char)'E'] = 6, [(unsigned char)'e'] = 6, 
	[(unsigned char)'É'] = 6, [(unsigned char)'é'] = 6,[(unsigned char)'ì'] = 6, 
	[(unsigned char)'Ì'] = 6, [(unsigned char)'F'] = 7, [(unsigned char)'f'] = 7,
	[(unsigned char)'G'] = 8, [(unsigned char)'g'] = 8, [(unsigned char)'H'] = 9,
	[(unsigned char)'h'] = 9, 
	[(unsigned char)'@'] = 10, [(unsigned char)'$'] = 10,
	[(unsigned char)'I'] = 11, [(unsigned char)'i'] = 11,
	[(unsigned char)'Í'] = 11,[(unsigned char)'í'] = 11,[(unsigned char)'J'] = 12,
	[(unsigned char)'j'] = 12,[(unsigned char)'K'] = 13, [(unsigned char)'k'] = 13,
	[(unsigned char)'L'] = 14,[(unsigned char)'l'] = 14,[(unsigned char)'M'] = 15,
	[(unsigned char)'m'] = 15,[(unsigned char)'N'] = 16, [(unsigned char)'n'] = 16,
	[(unsigned char)'O'] = 17,[(unsigned char)'o'] = 17,[(unsigned char)'Ó'] = 17,
	[(unsigned char)'ó'] = 17,[(unsigned char)'P'] = 18, [(unsigned char)'p'] = 18,
	[(unsigned char)'Q'] = 19,[(unsigned char)'q'] = 19, [(unsigned char)'R'] = 20,
	[(unsigned char)'r'] = 20,[(unsigned char)'Ø'] = 21,[(unsigned char)'ø'] = 21,
	[(unsigned char)'S'] = 22,[(unsigned char)'s'] = 22, [(unsigned char)'©'] = 23,
	[(unsigned char)'¹'] = 23,[(unsigned char)'T'] = 24,[(unsigned char)'t'] = 24,
	[(unsigned char)'«'] = 24,[(unsigned char)'»'] = 24,
	[(unsigned char)'U'] = 25,[(unsigned char)'u'] = 25,
	[(unsigned char)'Ú'] = 25,[(unsigned char)'ú'] = 25,[(unsigned char)'Ù'] = 25,
	[(unsigned char)'ù'] = 25,[(unsigned char)'V'] = 26,[(unsigned char)'v'] = 26,
	[(unsigned char)'W'] = 27,[(unsigned char)'w'] = 27, [(unsigned char)'X'] = 28,
	[(unsigned char)'x'] = 28,[(unsigned char)'Y'] = 29,[(unsigned char)'y'] = 29,
	[(unsigned char)'Ý'] = 29,[(unsigned char)'ý'] = 29, [(unsigned char)'Z'] = 30,
	[(unsigned char)'z'] = 30,[(unsigned char)'®'] = 31,[(unsigned char)'¾'] = 31,
	[(unsigned char)'0'] = 32,[(unsigned char)'1'] = 33,[(unsigned char)'2'] = 34,
	[(unsigned char)'3'] = 35,[(unsigned char)'4'] = 36, [(unsigned char)'5'] = 37,
	[(unsigned char)'6'] = 38,[(unsigned char)'7'] = 39,[(unsigned char)'8'] = 40,
	[(unsigned char)'9'] = 41
}; 


const unsigned char secondpass[] = {
	[(unsigned char)'A'] = 1, [(unsigned char)'a'] = 1, [(unsigned char)'Á'] = 2,
	[(unsigned char)'á'] = 2, [(unsigned char)'B'] = 3, [(unsigned char)'b'] = 3, 
	[(unsigned char)'C'] = 4, [(unsigned char)'c'] = 4, [(unsigned char)'È'] = 5, 
	[(unsigned char)'è'] = 5, [(unsigned char)'d'] = 6, [(unsigned char)'D'] = 6,
	[(unsigned char)'Ï'] = 7, [(unsigned char)'ï'] = 7,
	[(unsigned char)'E'] = 8, [(unsigned char)'e'] = 8, 
	[(unsigned char)'É'] = 9, [(unsigned char)'é'] = 9,[(unsigned char)'ì'] = 10, 
	[(unsigned char)'Ì'] = 10, [(unsigned char)'F'] = 11, [(unsigned char)'f'] =11,
	[(unsigned char)'G'] = 12, [(unsigned char)'g'] = 12, [(unsigned char)'H'] = 13,
	[(unsigned char)'h'] = 13, 
	[(unsigned char)'@'] = 14, [(unsigned char)'$'] = 14,
	[(unsigned char)'I'] = 15, [(unsigned char)'i'] = 15,
	[(unsigned char)'Í'] = 16,[(unsigned char)'í'] = 16,[(unsigned char)'J'] = 17,
	[(unsigned char)'j'] = 17,[(unsigned char)'K'] = 18, [(unsigned char)'k'] = 18,
	[(unsigned char)'L'] = 19,[(unsigned char)'l'] = 19,[(unsigned char)'M'] = 20,
	[(unsigned char)'m'] = 20,[(unsigned char)'N'] = 21, [(unsigned char)'n'] = 21,
	[(unsigned char)'O'] = 22,[(unsigned char)'o'] = 22,[(unsigned char)'Ó'] = 23,
	[(unsigned char)'ó'] = 23,[(unsigned char)'P'] = 24, [(unsigned char)'p'] = 24,
	[(unsigned char)'Q'] = 25,[(unsigned char)'q'] = 25, [(unsigned char)'R'] = 26,
	[(unsigned char)'r'] = 26,[(unsigned char)'Ø'] = 27,[(unsigned char)'ø'] = 27,
	[(unsigned char)'S'] = 28,[(unsigned char)'s'] = 28, [(unsigned char)'©'] = 29,
	[(unsigned char)'¹'] = 29,[(unsigned char)'T'] = 30,[(unsigned char)'t'] = 30,
	[(unsigned char)'«'] = 31,[(unsigned char)'»'] = 31,
	[(unsigned char)'U'] = 32,[(unsigned char)'u'] = 32,
	[(unsigned char)'Ú'] = 33,[(unsigned char)'ú'] = 33,[(unsigned char)'Ù'] = 34,
	[(unsigned char)'ù'] = 34,[(unsigned char)'V'] = 35,[(unsigned char)'v'] = 35,
	[(unsigned char)'W'] = 36,[(unsigned char)'w'] = 36, [(unsigned char)'X'] = 37,
	[(unsigned char)'x'] = 37,[(unsigned char)'Y'] = 38,[(unsigned char)'y'] = 38,
	[(unsigned char)'Ý'] = 39,[(unsigned char)'ý'] = 39, [(unsigned char)'Z'] = 40,
	[(unsigned char)'z'] = 40,[(unsigned char)'®'] = 41,[(unsigned char)'¾'] = 41,
	[(unsigned char)'0'] = 42,[(unsigned char)'1'] = 43,[(unsigned char)'2'] = 44,
	[(unsigned char)'3'] = 45,[(unsigned char)'4'] = 46, [(unsigned char)'5'] = 47,
	[(unsigned char)'6'] = 48,[(unsigned char)'7'] = 49,[(unsigned char)'8'] = 50,
	[(unsigned char)'9'] = 51
};
//Strukturi

typedef struct entry *p_TEntry;

typedef int (*entryComparator)(p_TEntry entry1, p_TEntry entry2, char key[5]);

typedef char *String;

typedef struct entry		//struktura matice
{
   String name;
   String surname;
   int gender;
   int possesion;
   int debt;
   
   p_TEntry next;
   p_TEntry previous;
} TEntry;

typedef struct entry_list {
	p_TEntry head;
	p_TEntry tail;
	int entries;
}TEntry_list, *p_TEntry_list;

typedef struct params {
	String pathin;
	String pathout;
	char key[5];
} TParams, *p_TParams;

int errMsg(int code){
}

int copyData(p_TEntry dest, p_TEntry src){
	dest->name = src->name;
	dest->surname = src->surname;
	dest->gender = src->gender;
	dest->possesion = src->possesion;
	dest->debt = src->debt;
	return RET_OK;
}

int initList(p_TEntry_list list){
	list->entries = 0;
	list->head = NULL;
	list->tail = NULL;
	return RET_OK;
}

int addLast(p_TEntry_list list, p_TEntry entry){

	p_TEntry tmp = malloc(sizeof(TEntry));
	if(tmp == NULL) return RET_ALK;

	copyData(tmp, entry);

	if (list->entries == 0) { 
		list->head = tmp;
		list->tail = tmp;
		tmp->previous = NULL;
		tmp->next = NULL;
		list->entries++;
		return RET_OK;
		
	}

	tmp->next = NULL;
	tmp->previous = list->tail;
	
	list->tail->next = tmp;
	
	list->tail = tmp;
	list->entries++;
	
	return RET_OK;
	}
	
int swapEntry(p_TEntry_list list ,p_TEntry first, p_TEntry second){
	p_TEntry tmp1;
	p_TEntry tmp2;
	
	tmp1 = first->previous;
	tmp2 = second->next;
	
	if (tmp1 != NULL) { tmp1->next = second;}

	second->previous = tmp1;
	
	second->next = first->next;
	first->previous = second->previous;
	
	first->next = tmp2;
	
	if (tmp2 != NULL) { tmp2->previous = first;}
	return RET_OK;
}
	
int swapData(p_TEntry first, p_TEntry second){
	TEntry tmp;
	copyData(&tmp,first);
	copyData(first,second);
	copyData(second,&tmp);
}
	
int freeList(p_TEntry_list list){
	p_TEntry tmp;
	for (tmp = list->head;tmp != NULL; tmp = tmp->next){
		free(tmp->name);
		free(tmp->surname);
		free(tmp);	
	}
		initList(list);
		
	
return RET_OK;
}
int dumpList(FILE *fp, p_TEntry_list list);

int orderList(p_TEntry_list list, entryComparator ec, char key[5]){
	p_TEntry tmp;
	int c;
	int i;
	int flag = 1;
	while(flag){
		flag = 0;
	for (tmp = list->head;tmp->next != NULL; tmp = tmp->next){
		if (ec(tmp,tmp->next,key) > 0) {
		swapData(tmp,tmp->next);
			flag = 1;}
	}
	}
}
	
int printEntry(FILE * fp, p_TEntry entry){
	fprintf(fp,"%s:",entry->name);
	fprintf(fp,"%s:",entry->surname);
	fprintf(fp,"%c:",entry->gender == 0 ? 'M' : '®');
	fprintf(fp,"%d:",entry->possesion);
	fprintf(fp,"%d",entry->debt);
	fprintf(fp,"\n");
	return RET_OK;
}

int dumpList(FILE *fp, p_TEntry_list list){
	p_TEntry tmp;
	for (tmp = list->head;tmp != NULL; tmp = tmp->next)
		printEntry(fp,tmp);	
	return RET_OK;
}

int readLine(FILE *fp, String *read){
	int c;
	int i = 0;
	if ((c=fgetc(fp)) == EOF )
	{
		return RET_INP;
	} 
	else
	{
		ungetc (c, fp);
	}
	*read = NULL;
	
	while (((c = fgetc(fp)) != EOF) && (c != '\n')){
		if ((i % 32) == 0)
		{
			*read = realloc (*read, i + 32);
			if (*read == NULL)
			{
				return RET_ALK;
			}
			*(*(read) + i) = c;
		}
		else
		{
			*(*(read) + i) = c;
		};
		i++;
	
	}
	
	*(*(read) + i) = '\0';
	return RET_OK;
	
	
}

int parseLine(String line, p_TEntry entry){
	int i = 0;
	int code;
	String tmp = line;
	String tmp2 = line;	
	//najdi meno
	while((*tmp != ':') && (*tmp != '\0')) {i++;tmp++;}
	if (*tmp == '\0') return RET_INP;
	if((entry->name = malloc((i+1) * sizeof(char))) == NULL) return RET_ALK;
	strncpy(entry->name, line, i);
	*(entry->name+i) = '\0';


	i = 0;

	tmp++; //preskoc ":"
	tmp2 = tmp;//nastav druhu zalozku
	
	
	//najdi priezvisko
	while((*tmp != ':') && (*tmp != '\0')) {i++;tmp++;}
	if (*tmp == '\0') return RET_INP;
	if((entry->surname = malloc((i+1) * sizeof(char))) == NULL) return RET_ALK;

	strncpy(entry->surname, tmp2, i);
	*(entry->surname+i) = '\0';
	i = 0;
	
	tmp++;
	
	//pohlavie
	if (*tmp == '\0') return RET_INP;
	if (*tmp == 'M') entry->gender = 0; else if
				(*tmp == '®') entry->gender = 1;
				else return RET_INP;		
	tmp++;
	if((code = sscanf(tmp,":%d:",&(entry->possesion))) != 1) return RET_INP;

	
	
	if((code = sscanf(tmp,":%*d:%d",&(entry->debt))) != 1) return RET_INP;
	
	return RET_OK;
}

int parseFile(String path, p_TEntry_list list){
	printf("SOM TU \n");
	int i = 0;
	int code = 0;
	FILE *fp;
	TEntry tmp;
	String tmpline;
	
	if((fp = fopen(path,"rb"))==NULL){
		return RET_FILE;
	}
	
	do {
		if((code |= readLine(fp, &tmpline)) != RET_OK) {};
		//printf("nacitany riadok %s\n", tmpline);
		if((code |= parseLine(tmpline, &tmp)) != RET_OK) {};
		//printf("asi aj spracovany");
		if((code |= addLast(list, &tmp)) != RET_OK) {};
		
		free(tmpline);
	}while ((code ==0) && (feof(fp) == 0));
	fclose(fp);
	if (code !=0) {return RET_INP;}
	return RET_OK;
	
}

int substCh(String str){
	String tmp;
	const int len = strlen(str);
	int i = 0;
	int j,code;
	while (*(str+i+1) != '\0'){
		if ((strncmp(str+i,"ch",2) == 0)) {*(str+i)='$'; memmove(str+i+1, str+i+2, len-i+1);} 
		if ((strncmp(str+i,"Ch",2) == 0)) {*(str+i)='@'; memmove(str+i+1, str+i+2, len-i+1);} 
		i++;
		}

}

int recreateCh(String str){
	String tmp;
	const int len = strlen(str);
	int i = 0;
	int j,code;
	while (*(str+i+1) != '\0'){
		if (*(str+i)=='$') {memmove(str+i+2, str+i+1, len-i+1);*(str+i) = 'c';*(str+i+1) = 'h';} 
		if (*(str+i)=='@') {memmove(str+i+2, str+i+1, len-i+1);*(str+i) = 'C';*(str+i+1) = 'h';}  
		i++;
	}

}

int pass1(String first,String second);
int pass2(String first,String second);


int strlloccom(String first,String second){
	substCh(first);
	substCh(second);
	int code;

	if ((code=pass1(first,second))==0) {code = pass2(first,second);}
	
	recreateCh(second);
	recreateCh(first);
	return code;
}

int pass1(String first,String second){

	int code = 0;
	int i,j;
	while ((*first != '\0') && (*second != '\0')){
		if (firstpass[(unsigned char)(*first)]>firstpass[(unsigned char)(*second)]) return 1;
		if (firstpass[(unsigned char)(*first)]<firstpass[(unsigned char)(*second)]) return -1;
		first++;
		second++;
	}
	
	if (strlen(first) != strlen(second)) return strlen(first)-strlen(second);
	
	return 0;
	
}

int pass2(String first,String second){

	int code = 0;
	int i,j;
	while ((*first != '\0') && (*second != '\0')){
		if (secondpass[(unsigned char)(*first)]>secondpass[(unsigned char)(*second)]) return 1;
		if (secondpass[(unsigned char)(*first)]<secondpass[(unsigned char)(*second)]) return -1;
		first++;
		second++;
	}
	
	if (strlen(first) != strlen(second)) return strlen(first)-strlen(second);
	
	return 0;
	
}

int comGen(int first, int second){
	if (first > second) return 1;
	if (first < second) return -1;
return 0;
}

int comNum(int first, int second){
	if (first > second) return 1;
	if (first < second) return -1;
return 0;
}

int localedCmp(p_TEntry first, p_TEntry second, char key[5]){
	int i = 0;
	int j = 0;
	while ((i == 0) && (j < 5)) {
		switch (key[j]) {
			case '0' : i = 0;break;
			case '1' : i = strlloccom(first->name,second->name);break;
			case '2' : i = strlloccom(first->surname,second->surname);break;
			case '3' : i = comGen(first->gender,second->gender);break;
			case '4' : i = comNum(first->possesion,second->possesion);break;
			case '5' : i = comNum(first->debt,second->debt);break;
		}
		j++;
	}
	
return i;
}



int
processParams (p_TParams ptp, int count, char **vector)
{
	int i;
  if (count != 4) {return RET_ARGS;}
  
  int j;
  j = strlen(vector[1]);
  if ((j<6) || (j>10)) return RET_ARGS;
  
  for (i = 0; i < 5;i++ ) ptp->key[i]=0;

  for (i = 5; i < j ; i++ ) {
	  if (vector[1][i] > '5' || vector[1][i] < '1') {return RET_ARGS;}
	  ptp->key[i-5] = vector[1][i];
  }
  
  
       
  	ptp->pathin = vector[2];
	ptp->pathout = vector[3];
	
	

  return RET_OK;


}

int
main (int argc, char *argv[])
{
	int code;
	TParams tp;
	TEntry_list data;
	if ((code = processParams (&tp, argc, argv)) != RET_OK) {/*errMsg(code);*/return EXIT_FAILURE;}
	initList(&data);
	setlocale(LC_ALL, "cs_CZ");
	
	
	if ((code = parseFile (tp.pathin,&data)) != RET_OK) {/*errMsg(code);*/printf("asi chyba\n");return EXIT_FAILURE;}
	

	
	orderList(&data, &localedCmp, tp.key);
	
	dumpList(stdout,&data);

	freeList(&data);
	



		/*TEntry tent;
	parseLine("Cukor:Matusko:M:3500:-1200",&tent);
	addLast(&data, &tent);*/
/*parseLine("Cukorikiki:Matusko:M:3700:-1200",&tent);addLast(&data, &tent);parseLine("Cukorikiki:Matusko:M:1800:-1200",&tent);
	
	addLast(&data, &tent);parseLine("Cukorikiki:Matusko:M:5000:-1200",&tent);addLast(&data, &tent);
	printf("cau\n");
	orderList(&data,&localedCmp,"12345");
	dumpList(stdout,&data);*/
	//String s;
	//readLine(stdin,&s);
	//printf("exit code> %d\n",parseLine(s,&tent));
	//printEntry(stdout,&tent);
	//printf("localized :  %c\n",0xF0);
  return EXIT_SUCCESS;
}
