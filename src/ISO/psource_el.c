/*------------------------------------------------------------------------
 *   generate P-wave source or douple couple source at source nodes
 *
 *   O. Hellwig
 *  ----------------------------------------------------------------------*/

#include "fd.h"

void psource(int nt, struct tensor3d ***t,
float ** srcpos_loc, float ** signals, int nsrc_loc, 
float * dx, float * dy, float * dz, float * dxp, float * dyp, float * dzp){

	/* extern variables */
	extern float	DT;
	
	/* local variables */
	int	i, j, k, l, type;
	float	amp;


	/* adding source wavelet to stress components at source points */
	for (l=1;l<=nsrc_loc;l++) {
		type = (int)srcpos_loc[l][7];
		if ((type==1) || ((type>7) && (type<11))){
			i   = (int)srcpos_loc[l][1];
			j   = (int)srcpos_loc[l][2];
			k   = (int)srcpos_loc[l][3];
			amp = DT*signals[l][nt];

			switch (type){
			case 1 :	/* pressure */
				amp *= dx[i]*dy[j]*dz[k];

				t[i][j][k].xx -= amp;
				t[i][j][k].yy -= amp;
				t[i][j][k].zz -= amp;
				break;
			case 8 :	/* double couple in x-y-plane */
				t[i][j][k].xy += amp*dxp[i]*dyp[j]*dz[k];
				break;
			case 9 :	/* double couple in y-z-plane */
				t[i][j][k].yz += amp*dx[i]*dyp[j]*dzp[k];
				break;
			case 10 :	/* double couple in x-z-plane */
				t[i][j][k].xz += amp*dxp[i]*dy[j]*dzp[k];
				break;
			}
		}
	}
}
