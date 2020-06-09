#include <stdio.h>
#include <stdlib.h>

FILE *handle,*outfile;

static int dump(FILE* fp)
{
    int c,count;
    count=0;
    while ((c = getc(fp)) != EOF) 
    {
        if (c==10) count++;
        putc(c,outfile);
    }
    return count;
}

static void makestring(char* d,char* s,char* t)
{
    while ((*d = *s++) != 0) d++;
    while ((*d = *t++) != 0) d++;
    *d++ = 0;
}

int main (int argc, char** argv)
{
    char nouns[100],msgs[100],scns[100],npcs[100],outs[100];
    int nouncount,msgcount,scencount,npccount;

    if (argc != 2)
    {
        printf("usage %s scenario\n",argv[0]);
        exit (-1);
    }

    makestring (outs,argv[1],".text");
    if ((outfile = fopen(outs,"w")) == NULL)
    {
        printf("%s: can't open output file %s!\n",argv[0],outs);
        exit (-1);
    }

    makestring (nouns,argv[1],".tmp");
    if ((handle = fopen(nouns,"r")) == NULL)
    {
        printf("%s: can't open %s\n",argv[0],nouns);
        exit (-1);
    }
    nouncount = dump(handle);
    fclose(handle);

    makestring (msgs,argv[1],".rest");
    if ((handle = fopen(msgs,"r")) == NULL)
    {
        printf("%s: can't open %s\n",argv[0],msgs);
        exit (-1);
    }
    msgcount = dump(handle);
    fclose(handle);

    makestring (scns,argv[1],".scen");
    if ((handle = fopen(scns,"r")) == NULL)
    {
        printf("%s: can't open %s\n",argv[0],scns);
        exit (-1);
    }
    scencount = dump(handle);
    fclose(handle);

    makestring (npcs,argv[1],".npc");
    if ((handle = fopen(npcs,"r")) == NULL)
    {
        printf("%s: can't open %s\n",argv[0],npcs);
        exit (-1);
    }
    npccount = dump(handle);
    fclose(handle);

    fclose(outfile);

    if ((handle = fopen("msginfo.i","w")) == NULL)
    {
        printf("%s: can't open output file msginfo.i!\n",argv[0]);
        exit (-1);
    }

    fprintf (handle,"MsgMSGBASE\tEQU\t%d\n",nouncount);
    //fprintf (handle,"MsgSCNBASE\tEQU\t%d\n",msgcount+nouncount);
    //fprintf (handle,"MsgNPCBASE\tEQU\t%d\n",scencount+msgcount+nouncount);

    // NB: values do not include nouncount for the guild!
    fprintf (handle,"MsgSCNBASE\tEQU\t%d\n",msgcount);
    fprintf (handle,"MsgNPCBASE\tEQU\t%d\n",scencount+msgcount);
    
    fprintf (handle,"MsgEND\tEQU\t%d\n",npccount+scencount+msgcount+nouncount);

    fclose (handle);
    fclose (outfile);
    return 0;
}


