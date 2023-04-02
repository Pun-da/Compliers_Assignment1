%{
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
char* current_custID;
char* current_date;
char* input_date;
int current_val;
int space_counter;
char* max_custID="";
int max_val = -1;
int k = 0;
int i=0;
%}
digit [0-9] 
uppercase [A-Z]
DD (30|[0-2][0-9]) 
MM (0[1-9]|1[0-2])
%S RECORD
%S CUSTOMERID
%S DATE
%S VALUE
%S VALID
%x COMMENT

%%


[$][\ ]* {
    BEGIN RECORD;
}

<COMMENT>[.]*[\n] {
    BEGIN INITIAL;
}

<RECORD>{uppercase}+{digit}+[\ ]* {
    space_counter=0;
    while(yytext[space_counter]!=' '){
        space_counter++;
    }
    // printf("%d", space_counter);
    // printf("%s", yytext);
    current_custID=(char*)malloc((space_counter+1)*sizeof(char));
    strncpy(current_custID, yytext, space_counter);
    BEGIN CUSTOMERID;
}
<RECORD>. {
    BEGIN INITIAL;
}

<CUSTOMERID>{DD}[/]{MM}[\ ]* {
    current_date=(char*)malloc((5)*sizeof(char));
    strncpy(current_date, yytext, 5);
    // printf("\ndate: %s_%s", current_date, input_date);
    BEGIN DATE;
}
<CUSTOMERID>. {
    return -1;
}
<DATE>{digit}+ {
    current_val=atoi(yytext);
    // printf("\nvalue: %d", current_val);
    BEGIN VALUE;
}

<VALUE>[;] {
        if(strcmp(input_date, current_date)==0){
        // printf("entered");
        k++;
        // printf("\n%s:%d", current_date, k);
        if(current_val>max_val){
            max_val=current_val;
            max_custID=current_custID;
        }
    }
    BEGIN VALID;
}

<VALID>[\ ]*[\n] {
    BEGIN INITIAL;
}

<VALID>[\ ]* {
    BEGIN CUSTOMERID;
}


%%

int yywrap()
{
    return 1;
}

int main()
{
    FILE* fileptr1 = fopen("input.txt", "r");
    size_t sza = 5 * sizeof(char);
    input_date = (char*)malloc(5 * sizeof(char));
    getline(&input_date, &sza, fileptr1);
    yyin = fopen("data.txt", "r");
    int x = yylex();
    FILE *fileptr2 = fopen("output.txt", "w+");

    if(x == -1){
        fprintf(fileptr2, "0");
        return 0;
    }
   
    fprintf(fileptr2,"$%d$%s#", k, max_custID);
    fclose(fileptr1);
    fclose(fileptr2);
    return 0;
}