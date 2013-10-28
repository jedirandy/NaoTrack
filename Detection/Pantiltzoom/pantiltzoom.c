/* pantiltzoom.c */

#include <math.h>
#include "pantiltzoom.h"
/*
fonction pantiltzoom
ppan,ptilt :
	pointeurs sur les angles de pan et de tilt en degré,
	à appliquer à la caméra axis-ptz pour recentrer un objet
u,v :
	coordonnées de l'objet à recentrer (en pixels)
u0,v0 : 
	coordonnées du centre de l'image (en pixels)
pan0,tilt0 : 
	angle de pan et de tilt initiaux en degré
zoom :
	paramètre de réglage du zoom sur
	la caméra axis-ptz (varie entre 1 et 10000)

Description géométrique : voir pages 27 à 29 dans le poly Traitement des Images
http://moodle.supelec.fr/moodle/file.php/398/poly/TraitementDesImages.pdf
*/
void pantiltzoom(double * ppan,double * ptilt,
double u,double v,double u0,double v0,double pan0,double tilt0,double zoom)
{
	double focale,theta,alpha0,beta0,alpha,beta;
	double x,y,z,X,Y,Z,norme;
	theta=4.189301e+001-6.436043e-003*zoom+2.404497e-007*zoom*zoom;
	focale=u0/tan((M_PI*theta/180.0)/2);

	x=u-u0;y=v-v0;z=focale;
	norme=sqrt(x*x+y*y+z*z);
	x/=norme;y/=norme;z/=norme;

	beta0=-(M_PI*pan0/180.0);
	alpha0=-(M_PI*tilt0/180.0);

	X=cos(beta0)*x+sin(alpha0)*sin(beta0)*y-cos(alpha0)*sin(beta0)*z;
	Y=cos(alpha0)*y+sin(alpha0)*z;
	Z=sin(beta0)*x-sin(alpha0)*cos(beta0)*y+cos(alpha0)*cos(beta0)*z;
	alpha=atan2(Y,sqrt(X*X+Z*Z));
	beta=-atan2(X,Z);

	*ppan=-(180.0*beta/M_PI);
	*ptilt=-(180.0*alpha/M_PI);
}
