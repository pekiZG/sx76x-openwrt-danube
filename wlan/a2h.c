#include <stdio.h>

int main()
{
FILE *fi;
FILE *fo;
int i=0;
unsigned short a;

fi=fopen("arteep.bin","rb");
fo=fopen("eesim.h","wt");
fprintf(fo,"u16 eesim[2048]=\n");
fprintf(fo,"{\n");

while (!feof(fi)) 
{
fread(&a,sizeof(a),1,fi);
fprintf(fo,"0x%04x",a);
if (!feof(fi)) fprintf(fo,",");
if (i==10) { fprintf(fo,"\n"); i=0; } else i++;
};

fprintf(fo,"};\n");


};
