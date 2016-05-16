#include<stdio.h>
#include<stdlib.h>
#include<string.h>

FILE *fpr=NULL,*fpw=NULL,*fp1=NULL,*fp2=NULL;

char addr(char ch)
{
    int n;

    n=0;
    n=ch-'0';
    n+=8;

    if(n==8)
        return '8';
    else if(n==9)
        return '9';
    else if(n==10)
        return 'A';
    else if(n==11)
        return 'B';
    else if(n==12)
        return 'C';
    else if(n==13)
        return 'D';
    else if(n==14)
        return 'E';
    else if(n==15)
        return 'F';
}

int main()
{
    char label[10],mnemonic[10] ,operand[10],program_name[10],sym_label[30][10],sym_address[30][10],
    opcodes[1000][3],opcode[10],str[80],st[10],ch,c,chr[10],text[80][80];

    int address=0,line=0,length,x=0,first_address=0,last_address=0,pa=0,obj_code=0,i=0,j=0,k=0,pos1=0,
    pos2=0,flag1=0,flag2=0,p1=0,p2=0,l=0,z=0,n=0,count=0,l1=0,l2=0;

    char op_mnemonic[26][10]={"LDAC","STAC","SUBJ","MULT","STRL","DIVD","ADDA","STRCH","DT",
    "JMPEQ","LOADCH","DW","SUBR","FIXR","FIX","JMPLT","COMP","CLR","LOADB","J","DR","COMPR","STREX","LOADT","LOADL","LOADX"};

    char op_opcode[26][10]={"10","1C","58","30","24","34","28","44","F0","40","60","EC",
    "4C","C8","3C","39","38","C4","78","3C","E8","D0","20","84","08","04"};

    /*Adressing*/

    fpr=fopen("new_input.txt","r");
    fpw=fopen("addressed_file.txt","w");

    fscanf(fpr,"%s%s%X",&program_name,&mnemonic,&address);  /*Reading data from input.txt*/
    line++;
    fprintf(fpw,"%d\t\t%s\t%s\t%.4X\n",line,program_name,mnemonic,address);  /*Writing data into addressed_file.txt */

    while(strcmp(mnemonic,"END")!=0)
    {
        if(strcmp(label,".")!=0)        /*For checking comments*/
        {
            fscanf(fpr,"%s%s%s",&label,&mnemonic,&operand);
            line++;

            fprintf(fpw,"%d\t%.4X\t%s\t%s\t%s\n",line,address,label,mnemonic,operand);  /*Writing data into addressed_file.txt */

            /*checking conditions for various mnemonics*/

            if(strcmp(mnemonic,"BYTE")==0)
            {
                length=0;
                length=strlen(operand);
                length-=3;          /*excluding C'' ,X'' */

                if(operand[0]=='C')
                        address+=length;
                else
                {
                    length=(length/2);
                    address+=length;
                }
            }
            else if(strcmp(mnemonic,"WORD")==0)
                address+=3;
            else if(strcmp(mnemonic,"RESERW")==0)
            {
                x=atoi(operand);    /*converting character into integer */
                address+=(x*3);     /*1 WORD = 3 BYTES */
            }
            else if(strcmp(mnemonic,"RESERB")==0)
            {
                x=atoi(operand);    /*change RESERB's string mnemonic into integer --> x */
                address+=x;
            }
            else
                address+=3;
        }
        else
            fprintf(fpw,"\n");
    }
    fclose(fpr);
    fclose(fpw);

    /*Creating Symtab*/

    fpr=fopen("addressed_file.txt","r");
    fpw=fopen("symtab.txt","w");

    fscanf(fpr,"%d%s%s%s",&line,&program_name,&mnemonic,&operand);

    while(strcmp(mnemonic,"END")!=0)
    {
        fscanf(fpr,"%d%X%s%s%s",&line,&address,&label,&mnemonic,&operand);

        if(line==2)
            first_address=address;     /*Will be used during object file creation */

        if(strcmp(label,"--")!=0)
        {
            fprintf(fpw,"%s  %X\n",label,address);
            j++;
        }
    }
    last_address=address;      /*Will be used during object file creation */

    fclose(fpr);
    fclose(fpw);

    /*Extracting data from symtab*/

    fp1=fopen("symtab.txt","r");
    i=0;

    while(j--)
    {
        fscanf(fp1,"%s%s",&sym_label[i],&sym_address[i]);
        i++;
    }

    fclose(fp1);

    /*Generating opcodes*/

    fpr=fopen("addressed_file.txt","r");
    fpw=fopen("intermediate.txt","w");

    fscanf(fpr,"%d%s%s%s",&line,&program_name,&mnemonic,&operand);
    fprintf(fpw,"%d\t\t%s\t%s\t%s\t\t--\n",line,program_name,mnemonic,operand);

    while(strcmp(mnemonic,"END")!=0)
    {
        fscanf(fpr,"%d%X%s%s%s",&line,&address,&label,&mnemonic,&operand);
        l=strlen(operand);

        if(strcmp(mnemonic,"WORD")==0)
            fprintf(fpw,"%d\t%.4X\t%s\t%s\t%s\t\t%.6X\n",line,address,label,mnemonic,operand,atoi(operand));

        else if(strcmp(mnemonic,"BYTE")==0)
        {
            l=strlen(operand);
            k=0;

            for(i=2;i<l-1;i++)
                opcode[k++]=operand[i];
            opcode[k]='\0';

            if(operand[0]=='C')
                fprintf(fpw,"%d\t%.4X\t%s\t%s\t%s\t\t454F46\n",line,address,label,mnemonic,operand);

            if(operand[0]=='X')
                fprintf(fpw,"%d\t%.4X\t%s\t%s\t%s\t\t%s\n",line,address,label,mnemonic,operand,opcode);
        }

        else if(strcmp(mnemonic,"RESERW")==0 || strcmp(mnemonic,"RESERB")==0 || strcmp(mnemonic,"START")==0 || strcmp(mnemonic,"END")==0)
            fprintf(fpw,"%d\t%.4X\t%s\t%s\t%s\t\t--\n",line,address,label,mnemonic,operand);

        else if(strcmp(mnemonic,"SUBR")==0)
            fprintf(fpw,"%d\t%.4X\t%s\t%s\t%s\t\t4C0000\n",line,address,label,mnemonic,operand);

        else if(operand[l-1]=='X' && operand[l-2]==',')
        {
            strcpy(st,operand);
            st[l-2]='\0';

            for(j=0;j<17;j++)
            {
                if(strcmp(st,sym_label[j])==0)
                {
                    pos2=j;
                    break;
                }
            }

            strcpy(str,sym_address[pos2]);
            ch=str[0];
            str[0]=addr(ch);

            fprintf(fpw,"%d\t%.4X\t%s\t%s\t%s\t%s%s\n",line,address,label,mnemonic,operand,op_opcode[pos1],str);
        }

        else
        {
            pos1=pos2=0;
            for(i=0;i<26;i++)
            {
                if(strcmp(mnemonic,op_mnemonic[i])==0)
                {
                    pos1=i;
                    break;
                }
            }
            for(j=0;j<17;j++)
            {
                if(strcmp(operand,sym_label[j])==0)
                {
                    pos2=j;
                    break;
                }
            }
            fprintf(fpw,"%d\t%.4X\t%s\t%s\t%s\t\t%s%s\n",line,address,label,mnemonic,operand,op_opcode[pos1],sym_address[pos2]);
        }
    }

    fclose(fpr);
    fclose(fpw);

    /*Generating Object File*/

    fpr=fopen("intermediate.txt","r");
    fscanf(fpr,"%d%s%s%s%s",&line,&program_name,&mnemonic,&operand,&opcode);
    k=count=0;
    fpw=fopen("object_file.txt","w");

    chr[0]='^';
    chr[1]=' ';
    chr[2]='\0';
    i=k=0;

    fprintf(fpw,"H ^ %s ^ %.6X ^ %.6X\n\n",program_name,first_address,last_address-first_address);

    while(strcmp(mnemonic,"END")!=0)
    {
        fscanf(fpr,"%d%X%s%s%s%s",&line,&address,&label,&mnemonic,&operand,&opcode);

        if(i<30 && strcmp(opcode,"--")!=0)
        {
            if(i==28 || i==29)
            {
                fprintf(fpw,"T ^ %.6X ^ %.2X %s\n\n",address-i,i,text[k]);
                k++;
                i=0;
                strcat(text[k],chr);
                i+=(strlen(opcode))/2;
                strcat(text[k],opcode);
            }
            else
            {
                strcat(text[k],chr);
                i+=(strlen(opcode))/2;
                strcat(text[k],opcode);
            }
        }
        if(i>=30 || strcmp(opcode,"--")==0)
        {
            if(strlen(text[k])!=0)
            {
                if(i<30)
                    fprintf(fpw,"T ^ %.6X ^ %.2X %s\n\n",address-i,i,text[k]);
                else
                    fprintf(fpw,"T ^ %.6X ^ %.2X %s\n\n",address-i+3,i,text[k]);
            }
            k++;
            i=0;
        }
    }
    fprintf(fpw,"E ^ %.6X",first_address);

    fclose(fpw);
    fclose(fpr);

    /*Splitting opcodes in strings of length = 2*/

    fpr=fopen("intermediate.txt","r");

    fscanf(fpr,"%d%s%s%s%s",&line,&program_name,&mnemonic,&operand,&opcode);
    k=count=i=0;

    while(strcmp(mnemonic,"END")!=0)
    {
        fscanf(fpr,"%d%X%s%s%s%s",&line,&address,&label,&mnemonic,&operand,&opcode);

        if(strcmp(opcode,"--")!=0 && strlen(opcode)==6)
        {
            if(address-pa>500)
            {
                ch=(pa%16)+48;
                opcodes[k][0]='+';
                opcodes[k][1]='+';
                opcodes[k][2]='\0';
                k++;
                opcodes[k][0]='0';
                opcodes[k][1]=ch;
                opcodes[k][2]='\0';
                k++;
            }
            for(i=0;i<6;)
            {
                opcodes[k][0]=opcode[i];i++;
                opcodes[k][1]=opcode[i];
                opcodes[k][2]='\0';i++;
                k++;
            }
        }
        if(strcmp(opcode,"--")==0)
        {}
        if(strcmp(opcode,"--")!=0 && strlen(opcode)==2)
        {
            strcpy(opcodes[k],opcode);
            k++;
        }
        pa=address;
    }
    fclose(fpr);

    /*Creating loader.txt*/

    j=l1=c=0;
    fpw=fopen("loader.txt","w");
    fprintf(fpw,"0000\txxxxxxxx\txxxxxxxx\txxxxxxxx\txxxxxxxx\n");
    for(i=0;i<k;i++)
    {
        if(strcmp(opcodes[i],"++")!=0)
            fprintf(fpw,"%s",opcodes[i]);
        else
        {
            l1=0;
            for(j=0;j<3;j++)
                fprintf(fpw,"\n  .  \t    .    \t    .    \t    .    \t    .    \t\n");
            c++;
            i++;

            if(c==2)
            {
                first_address-=16;
                first_address+=4096;
                fprintf(fpw,"%.4X\t",first_address);
                first_address+=16;
            }
            else
            {
                fprintf(fpw,"%.4X\t",first_address);
                first_address+=16;
            }

            for(j=0;j<atoi(opcodes[i]);j++)
            {
                l1+=2;
                fprintf(fpw,"xx");

                if(l1%8==0)
                    fprintf(fpw,"\t");
            }
            continue;
        }
        l1+=strlen(opcodes[i]);
        if(l1%8==0)
            fprintf(fpw,"\t");
        if(l1%32==0)
            {fprintf(fpw,"\n%.4X\t",first_address);first_address+=16;}
    }
    fclose(fpw);

    return 0;
}
