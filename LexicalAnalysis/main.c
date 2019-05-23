#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>


int line_number=1;
static int lexeme_types[6]={0}; ///number of identifiers ,paranthesis,keywords,eol characters,errors,total lexeme number


void write_string(FILE *fptr, FILE *wfptr, char ch) {///tirnak isaretinden sonrasini okur.
    int i=0;
    ch= fgetc(fptr) ;
    char* mystring=malloc(100);
    int max_lenght=100;

    while (ch != '"' && i<=max_lenght && ch!=EOF)
    {

        *(mystring+i)=ch;
        i++;
        ch= fgetc(fptr) ;

    }
    *(mystring+i)='\0';
    if(i<=100 ) {
        if(ch!=EOF) {
            fprintf(wfptr, "String constant (%s)\n", mystring);
            lexeme_types[5]++;
        }
        else{
            lexeme_types[4]++;
            printf("unmatched (\") character at Line: %d \n",line_number);
        }


    }


    free(mystring);
}

void char_detect(char ch, FILE *fptr, FILE *wfptr) {/// alinan char degiskenin parantez,separatör olup olmadigini bulan foksiyon

    if(!(isalnum(ch)) && ch!='-')
        switch (ch)
        {   case ',':
                fprintf(wfptr,"Seperator ','\n");
                lexeme_types[5]++;
                break;
            case '"':

                write_string(fptr,wfptr,ch);/// hocanin vermis oldugu tirnak isareti turkce bir karakter olup C dilinde herhangi bir karsiligi olmadigi icin asil tirnak iþaretini kullandim
                break;



            case '{':
                while ((ch = fgetc(fptr)) != '}') {}
                break;
            case '}':
                printf("Close Comment paranthesis with no start Line:%d\n",line_number);
                lexeme_types[4]++;
                break;
            case '-':case ' ':case'\t':break;
            case '\n': line_number++; break;
            case '[':
                fprintf(wfptr,"Open bracket paranthesis\n");
                lexeme_types[1]++;
                break;
            case ']':
                fprintf(wfptr,"Close bracket paranthesis\n");lexeme_types[1]++;
                break;
            case '.':
                fprintf(wfptr,"End of line character\n");
                lexeme_types[3]++;
                break;

            default:
                if(ch=='_')
                {fprintf(wfptr,"Underscore character\n");
                }
                else
                {
                    lexeme_types[4]++;
                    wprintf(L"Unrecognized Character (%c) Line : %d\n",ch,line_number);
                }
                ch=fgetc(fptr);
                if(isalnum(ch))
                {printf("Invalid identifier word Line : %d\n",line_number);

                    lexeme_types[4]++;
                    while(isalnum(ch) || ch=='_')
                        ch=fgetc(fptr);
                }
                if(ch!=EOF)
                    fseek(fptr,-1,SEEK_CUR);
        }
}
int isalldigit(char* word,FILE *f,FILE* w)
{
    int error=0,i=0;
    while(word[i])
    {
        if(isdigit(word[i])== 0)
        {
            error=1;
            char_detect(word[i], f, w);
            return 0;
        }
        i++;
    }
    return 1;
}

void putintconst(char* word,int minus,FILE* f,FILE* w)
{

    {if(isalldigit(word,f,w))
        {
            if(minus==1)
            {
                fprintf(w,"int constant (%s)\n",word-1);
                lexeme_types[5]++;
            }
            else
            {fprintf(w,"int constant (%s)\n",word);
                lexeme_types[5]++;}
        }
        else
        {
            if(minus==1)
                printf("Invalid identifier word (%s) line:%d\n",word-1,line_number);
            else
                printf("Invalid word (%s) line:%d\n",word,line_number);
            lexeme_types[4]++;
        }
    }
}
int word_parse(FILE * f,char* word,char **keywords,FILE* w) ///verilen kelimenin parcalanmasini ve yanlislarin bulunmasini saglar
{

    if(isdigit(*word)||*word=='-')
    {
        if(*word=='-')
            if(isdigit(*(word+1)))
                putintconst(word+1,1,f,w);
            else
            {
                int a=0;
                ///   printf("Invalid  minus sign usage Line : %d\n",line_number);

                while(word[a])
                {char t = word[a];
                    if(word[a]=='-')
                    {
                        printf("Invalid  minus sign usage Line : %d\n",line_number);
                        lexeme_types[4]++;
                    }
                    else
                    {
                        char_detect(word[a], f, w);
                    }
                    a++;

                }



            }
        else
            putintconst(word,0,f,w);
    }
    else
    {
        int iskeyword=0,i=0;
        for(i=0;i<10;i++)/// keyword control
        {
            char *key=keywords[i];;

            if( strcmp(key,word)==0)
            {
                fprintf(w,"keyword '%s'\n",keywords[i]);
                lexeme_types[2]++;
                iskeyword=1;
                break;
            }
        }
        if(!iskeyword)
        {   if(isdigit(word[0])||word[0]=='_')
            {
                printf("Invalid identifier token (%s) line%d\n",word,line_number);
                lexeme_types[4]++;
                return 0;
            }
            else {
                int error=0;
                for(i=0;i<strlen(word);i++)
                {
                    char a =word[i];
                    if((!isalnum((word[i]))  && word[i]!='_' ))
                    {
                        error=1;
                        char_detect(word[i], f, w);

                        /// if(word[i]=='-')
                        ///  printf("Minus sign\n");
                        ///else

                    }
                }
                if(error==0)
                {fprintf(w,"Identifier '%s'\n",word);
                    lexeme_types[0]++;}
                else
                {
                    printf("Invalid indentifier Token '%s' Line : %d\n",word,line_number);
                    lexeme_types[4]++;
                }
            }
        }

    }

}




int main() {


   char* lexemes[]={"Identifiers","Paranthesis","Keywords","EOL Characters","Errors","Lexemes"};
   char* keywords[]={"move","add","sub","out","loop","to","from","times","int","newline"};
    setlocale(LC_ALL,"Turkish");


    FILE* file;
    FILE* writen;


 char* filename=malloc(100);
 fflush(stdin);
 printf("Insert Source File WITH It's Extention : ");
 gets(filename);







 char ch;

 int chr=0;


 file=fopen(filename,"r");
 writen=fopen("AnalysisResult.txt","w");





 if(file==NULL)
 {
    printf("\nsorry! something gone wrong!\nFile couldn't open :(\n\n");
    return 0;
 }
 else
 {
    printf("\nFile Opened.Reading...\n\n");
 }




 int a=1;

 while((ch=fgetc(file))!=EOF)
 {
     char_detect(ch, file, writen);


     chr=0;
     if(isalnum(ch)|| ch=='-')
     {
         char* ptrword=malloc(100);
         chr=0;
         while(ch!=' '&& ch!='\n'&& ch!='\t'&& ch!=',' && ch!='.')
         {
             *(ptrword+chr)=ch;
             chr++;
             ch=fgetc(file);
             if(ch==EOF)
              {

               break;
              }
         }
      fseek(file,-1,SEEK_CUR);
         *(ptrword+chr)='\0';

         word_parse(file,ptrword,keywords,writen);
         free(ptrword);
     }

 }


 fclose(file);
 fclose(writen);
    printf("\n\nAnalysis Completed With:\n\n");
    for(int k=0;k<4;k++) lexeme_types[5]+=lexeme_types[k];
    for(int k=0;k<6;k++)
        printf(" %s ",lexemes[k]);
    printf("\n");
    for(int k=0;k<6;k++)
        printf("% -12d ",lexeme_types[k]);

    printf("\n\n...Process Completed...\n\n");

    return 0;

}
















