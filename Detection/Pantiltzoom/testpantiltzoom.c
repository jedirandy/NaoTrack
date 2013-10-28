/* testpantiltzoom.c 
compilation :
gcc pantiltzoom.c testpantiltzoom.c -o testpantiltzoom.exe
exécution :
testpantiltzoom.exe
*/


#include <stdio.h>
#include <stdlib.h>
#include "pantiltzoom.h"

int main(int ac,char ** av)
{
	double pan,tilt,pan0,tilt0,u,v,u0,v0,zoom;
	if (ac!=8)
	{
		fprintf(stderr,"usage : testpantiltzoom u0 v0 pan0 tilt0 u v zoom\n");
		exit(-1);
	}
	u0=atof(av[1]);
	v0=atof(av[2]);
	pan0=atof(av[3]);
	tilt0=atof(av[4]);
	u=atof(av[5]);
	v=atof(av[6]);
	zoom=atof(av[7]);

	pantiltzoom(&pan,&tilt,u,v,u0,v0,pan0,tilt0,zoom);
	fprintf(stderr,"pan=%lf deg. tilt=%lf deg.\n",pan,tilt);
	return 0;
}
