/*------------------------------------------------------------------------
 *   output source signal e.g. for cross-correlation, deconvolution 
 *   or comparison with analytical solutions
 *
 *   T. Bohlen, modified by O. Hellwig
 *  ----------------------------------------------------------------------*/
 
#include "fd.h"
#include "segy.h"

void  output_source_signal(FILE *fp, float **signals, float **srcpos_loc, int ns, int seis_form, float *xg, float *yg, float *zg, float *xpg, float *ypg, float *zpg){

	/* extern variables */
	extern float	DT;
	extern int	NX[3], POS[3];
	extern int	NDT, NSRC_LOC;
	extern int	FFID;

	/* local variables */
	int		i, j, k;
	int		tracl;
	segy		tr;
	float		xs, ys, zs;
	const float	p = 3.0;


	switch(seis_form){
	case 1 :	/*SEGY (without file-header)*/
		for(tracl=1;tracl<=NSRC_LOC;tracl++){
			i = POS[0]*NX[0] + srcpos_loc[tracl][1];
			j = POS[1]*NX[1] + srcpos_loc[tracl][2];
			k = POS[2]*NX[2] + srcpos_loc[tracl][3];

			switch ((int)srcpos_loc[tracl][7]){
			case 1: /* explosion */
				xs = xpg[i];
				ys = ypg[j];
				zs = zpg[k];
				break;
			case 2: /* force in x-direction */
				xs =  xg[i];
				ys = ypg[j];
				zs = zpg[k];
				break;
			case 3: /* force in y-direction */
				xs = xpg[i];
				ys =  yg[j];
				zs = zpg[k];
				break;
			case 4: /* force in z-direction */
				xs = xpg[i];
				ys = ypg[j];
				zs =  zg[k];
				break;
			case 5: /* double couple in x-y-plane */
				xs =  xg[i];
				ys =  yg[j];
				zs = zpg[k];
				break;
			case 6: /* double couple in y-z-plane */
				xs = xpg[i];
				ys =  yg[j];
				zs =  zg[k];
				break;
			case 7: /* double couple in x-z-plane */
				xs =  xg[i];
				ys = ypg[j];
				zs =  zg[k];
				break;
			default:
				xs = xg[i];
				ys = yg[j];
				zs = zg[k];
			}

			tr.tracl	= tracl;					/* trace sequence number within line */
			tr.tracr	= tracl;					/* trace sequence number within reel */
			tr.fldr		= (signed int)FFID;				/* original field record number */
			tr.tracf	= tracl;					/* trace number within original field record */
			tr.ep		= (signed int)FFID;				/* energy source point number */
			tr.cdp		= 0;						/* ensemble number: CDP, CMP, CRP, etc. */
			tr.cdpt		= 0;						/* trace number within CDP ensemble */
			tr.trid		= (short)6;					/* trace identification code: 
												-1=other
												 0=unknown
												 1=seismic
												 2=dead
												 3=dummy
												 4=time-break
												 5=uphole
												 6=sweep
												 7=timing
												 8=water-break
												 etc. */
			tr.nvs		= 1;						/* number of vertically summed traces */
			tr.nhs		= 1;						/* number of horizontally summed traces */
			tr.duse		= 1;						/* data use:
												1 = production
												2 = test */
			tr.offset	= (signed int)0;				/* Distance from source point center to receiver group center (negative if opposite to direction in which line is shot) */
			tr.gelev	= (signed int)iround(zs*powf(10.0,p));		/* receiver group elevation from sea level (above sea level is positive) */
			tr.selev	= (signed int)iround(zs*powf(10.0,p));		/* surface elevation at source	(above sea level is positive) */
			tr.sdepth	= 0;						/* source depth below surface (positive) */
			tr.gdel		= 0;						/* datum elevation at receiver group */
			tr.sdel		= 0;						/* datum elevation at source */
			tr.swdep	= 0;						/* water depth at source */
			tr.gwdep	= 0;						/* water depth at group */
			//tr.scalel	= (signed short)-p;
			//tr.scalco	= (signed short)-p;
			tr.scalel	= (signed short)(-fsign(p)*powf(10,abs(p)));	/* Scalar to be applied to all elevations and depths in bytes 41-68 to give real value. Scaler = 1, +/- 10, ... +/- 10,000. Positive=multiplier; negative=divisor */
			tr.scalco	= (signed short)(-fsign(p)*powf(10,abs(p)));	/* Scalar to be applied to all coordinates in bytes 73-88 to give real value. Scaler = 1, +/- 10, ... +/- 10,000. Positive=multiplier; negative=divisor */
			tr.sx		= (signed int)iround(xs*powf(10.0,p));		/* X source coordinate */
			tr.sy		= (signed int)iround(ys*powf(10.0,p));		/* Y source coordinate */
			tr.gx		= (signed int)iround(xs*powf(10.0,p));		/* X group coordinate */
			tr.gy		= (signed int)iround(ys*powf(10.0,p));		/* Y group coordinate */
			tr.counit	= 0;	/* coordinate units code:
							for previous four entries
							1 = length (meters or feet)
							2 = seconds of arc (in this case, the
							X values are longitude and the Y values
							are latitude, a positive value designates
							the number of seconds east of Greenwich
							or north of the equator
							3=decimal degrees
							4=degrees, minutes, seconds (DMS) */
			tr.wevel	= 0;	/* weathering velocity */
			tr.swevel	= 0;	/* subweathering velocity */
			tr.sut		= 0;	/* uphole time at source */
			tr.gut		= 0;	/* uphole time at receiver group */
			tr.sstat	= 0;	/* source static correction (ms) */
			tr.gstat	= 0;	/* group static correction (ms) */
			tr.tstat	= 0;	/* total static applied (ms) */
			tr.laga		= 0;	/* lag time A, time in ms between end of 240-
							byte trace identification header and time
							break, positive if time break occurs after
							end of header, time break is defined as
							the initiation pulse which maybe recorded
							on an auxiliary trace or as otherwise
							specified by the recording system */
			tr.lagb		= 0;	/* lag time B, time in ms between the time break
							and the initiation time of the energy source,
							may be positive or negative */
			tr.delrt	= 0;	/* delay recording time, time in ms between
							initiation time of energy source and time
							when recording of data samples begins
							(for deep water work if recording does not
							start at zero time) */
			tr.muts		= 0;	/* mute time--start (ms) */
			tr.mute		= 0;	/* mute time--end (ms) */
			tr.ns		= (unsigned short)iround(ns/NDT);			/* number of samples in this trace */
			if ((NDT*DT)>=1.0e-6)
				tr.dt	= (unsigned short)iround(((float)NDT*DT)*1.0e6);	/* sample interval in micro-seconds */
			else 
				tr.dt	= 1;
			tr.gain		= 0;	/* gain type of field instruments code:
							1 = fixed
							2 = binary
							3 = floating point
							4 ---- N = optional use */
			tr.igc		= 0;	/* instrument gain constant (dB) */
			tr.igi		= 0;	/* instrument early or initial gain (dB) */
			tr.corr		= 0;	/* correlated:
							1 = no
							2 = yes */
			tr.sfs		= 0;	/* sweep frequency at start (Hz) */
			tr.sfe		= 0;	/* sweep frequency at end (Hz) */
			tr.slen		= 0;	/* sweep length in ms */
			tr.styp		= 0;	/* sweep type code:
							1 = linear
							2 = parabolic
							3 = exponential
							4 = other */
			tr.stas		= 0;	/* sweep trace taper length at start in ms */
			tr.stae		= 0;	/* sweep trace taper length at end in ms */
			tr.tatyp	= 0;	/* taper type: 
							1=linear
							2=cos^2
							3=other */
			tr.afilf	= 0;	/* alias filter frequency (Hz) if used */
			tr.afils	= 0;	/* alias filter slope (dB/octave) */
			tr.nofilf	= 0;	/* notch filter frequency (Hz) if used */
			tr.nofils	= 0;	/* notch filter slope (dB/octave) */
			tr.lcf		= 0;	/* low cut frequency (Hz) if used */
			tr.hcf		= 0;	/* high cut frequncy (Hz) if used */
			tr.lcs		= 0;	/* low cut slope (dB/octave) */
			tr.hcs		= 0;	/* high cut slope (dB/octave) */
			tr.year		= 0;	/* year data recorded */
			tr.day		= 0;	/* day of year */
			tr.hour		= 0;	/* hour of day (24 hour clock) */
			tr.minute	= 0;	/* minute of hour */
			tr.sec		= 0;	/* second of minute */
			tr.timbas	= 0;	/* time basis code:
							1 = local
							2 = GMT
							3 = other
							4 = UTC (Coordinated Universal Time) */
			tr.trwf		= 0;	/* trace weighting factor, defined as 1/2^N
							volts for the least sigificant bit */
			tr.grnors	= 0;	/* geophone group number of roll switch
							position one */
			tr.grnofr	= 0;	/* geophone group number of trace one within
							original field record */
			tr.grnlof	= 0;	/* geophone group number of last trace within
							original field record */
			tr.gaps		= 0;	/* gap size (total number of groups dropped) */
			tr.otrav	= 0;	/* overtravel taper code:
							1 = down (or behind)
							2 = up (or ahead) */
			tr.d1		= (float)NDT*DT;	/* sample spacing for non-seismic data */
			tr.f1		= 0.0;			/* first sample location for non-seismic data */
			tr.d2		= 0.0;			/* sample spacing between traces */
			tr.f2		= 0.0;			/* first trace location */
			tr.ungpow	= 0.0;			/* negative of power used for dynamic range compression */
			tr.unscale	= 0.0;			/* reciprocal of scaling factor to normalize range */
			tr.ntr		= 0;			/* number of traces */
			tr.mark		= 0;


			for(j=1;j<=tr.ns;j++) 
				tr.data[j] = signals[tracl][j*NDT];

			fwrite(&tr,240,1,fp);
			fwrite(&tr.data[1],4,tr.ns,fp);
		}
		break;
	case 2 :	/*ASCII ONE COLUMN*/
		for(i=1;i<=NSRC_LOC;i++){
			for(j=1;j<=ns;j+=NDT) 
				fprintf(fp,"%e\n", signals[i][j]);
		}
		break;
	case 3 :	/*BINARY */
		for(i=1;i<=NSRC_LOC;i++)
			for(j=1;j<=ns;j+=NDT){
				fwrite(&signals[i][j],sizeof(float),1,fp);
			}
		break;
	default :
		fprintf(stdout," Message from output_source_signal: Don't know data format for seismograms !\n");
		fprintf(stdout," No output written. ");
	}

	fclose(fp);
}
