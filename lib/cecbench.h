/*
  CEC20 Test Function Suite for Single Objective Bound Constrained Numerical Optimization 
	Author: Not me, I just wrote a wrapper to avoid messing with the various buffers in my code.
	I also wrote the preprocessor macros to wrap the functions.
*/

#ifndef CECBENCH_MODULE
#define CECBENCH_MODULE

#include <stdlib.h>      
#include <stdio.h>
#include <math.h>
#include <malloc.h>

#include "core.h"


struct fun_state {
  struct pcg32_random *prng;
  struct cecbench_state *cecglobals;
};

#define CEC_TEST_CASE(X)\
  void X (const f64 *noalias x,     \
                f64 *noalias f,     \
          const i32           nx,    \
                f64 *noalias Os,    \
                f64 *noalias Mr,    \
          const i32           s_flag,\
          const i32           r_flag,\
                f64 *noalias y,     \
                f64 *noalias z) 

#define CEC_HF_CASE(X) void X (const f64 *noalias x, f64 *f, int nx, f64 *Os,double *Mr,int *S,int s_flag,int r_flag, f64 * y, f64 * z)
#define CEC_CF_CASE(X) void X (const f64 *noalias x, f64 *f, int nx, f64 *Os,double *Mr,int *SS, int r_flag, f64 *y, f64 *z)

CEC_TEST_CASE(sphere_func        );  /* Sphere */
CEC_TEST_CASE(ellips_func        );  /* Ellipsoidal */
CEC_TEST_CASE(bent_cigar_func    );  /* Discus */
CEC_TEST_CASE(discus_func        );  /* Bent_Cigar */
CEC_TEST_CASE(dif_powers_func    );  /* Different Powers */
CEC_TEST_CASE(rosenbrock_func    );  /* Rosenbrock's */
CEC_TEST_CASE(schaffer_F7_func   );  /* Schwefel's F7 */
CEC_TEST_CASE(ackley_func        );  /* Ackley's */
CEC_TEST_CASE(rastrigin_func     );  /* Rastrigin's  */
CEC_TEST_CASE(weierstrass_func   );  /* Weierstrass's  */
CEC_TEST_CASE(griewank_func      );  /* Griewank's  */
CEC_TEST_CASE(schwefel_func      );  /* Schwefel's */
CEC_TEST_CASE(katsuura_func      );  /* Katsuura */
CEC_TEST_CASE(bi_rastrigin_func  );  /* Lunacek Bi_rastrigin */
CEC_TEST_CASE(grie_rosen_func    );  /* Griewank-Rosenbrock  */
CEC_TEST_CASE(escaffer6_func     );  /* Expanded Scaffer s F6  */
CEC_TEST_CASE(step_rastrigin_func);  /* Noncontinuous Rastrigin's  */
CEC_TEST_CASE(happycat_func      );  /* HappyCat */
CEC_TEST_CASE(hgbat_func         );  /* HGBat  */

/* New functions Noor Changes */
CEC_TEST_CASE(sum_diff_pow_func  ); /* Sum of different power */
CEC_TEST_CASE(zakharov_func      ); /* ZAKHAROV */
CEC_TEST_CASE(levy_func          ); /* Levy */
CEC_TEST_CASE(dixon_price_func   ); /* Dixon and Price */

CEC_HF_CASE(hf01);                  /* Hybrid Function 1 */
CEC_HF_CASE(hf02);                  /* Hybrid Function 2 */
CEC_HF_CASE(hf03);                  /* Hybrid Function 3 */
CEC_HF_CASE(hf04);                  /* Hybrid Function 4 */
CEC_HF_CASE(hf05);                  /* Hybrid Function 5 */
CEC_HF_CASE(hf06);                  /* Hybrid Function 6 */
CEC_HF_CASE(hf07);                  /* Hybrid Function 7 */
CEC_HF_CASE(hf08);                  /* Hybrid Function 8 */
CEC_HF_CASE(hf09);                  /* Hybrid Function 9 */
CEC_HF_CASE(hf10);                  /* Hybrid Function 10 */

CEC_CF_CASE(cf01);                  /* Composition Function 1 */
CEC_CF_CASE(cf02);                  /* Composition Function 2 */
CEC_CF_CASE(cf03);                  /* Composition Function 3 */
CEC_CF_CASE(cf04);                  /* Composition Function 4 */
CEC_CF_CASE(cf05);                  /* Composition Function 5 */
CEC_CF_CASE(cf06);                  /* Composition Function 6 */
CEC_CF_CASE(cf07);                  /* Composition Function 7 */
CEC_CF_CASE(cf08);                  /* Composition Function 8 */
CEC_CF_CASE(cf09);                  /* Composition Function 9 */
CEC_CF_CASE(cf10);                  /* Composition Function 10 */

void shiftfunc (const double*,double*,int,double*);
void rotatefunc (const double*,double*,int, double*);
void sr_func (const double *, double *, int, double*, double*, double, int, int, double*); /* shift and rotate */
void asyfunc (const double *, double *x, int, double);
void oszfunc (const double *, double *, int);
void cf_cal(const double *, double *, int, double *,double *,double *,double *,int);

// Obscure old global variables are sandboxed inside this struct.
struct cecbench_state {
  double *OShift, *M, *y, *z;
  int ini_flag, n_flag, func_flag;
  int *SS;
};


// TODO get rig of the pthread depedency
//subroutine
//cecbench_trd_init()
//{
//  struct cecbench_state *state = malloc(sizeof(struct cecbench_state));
//  state->ini_flag = 0;
//  state->OShift = NULL;
//  state->M = NULL;
//  state->SS = NULL;
//  state->y = NULL;
//  state->z = NULL;
//  pthread_setspecific(cecbench_state_key, state);
//}

/**
  @NOTE(Kaeryv)
  Initializes the cec benchmark globals.
  A priori, there is no drawback for using extensively this functions except
  maybe for performance (it implies next call to cecbench reads (ro)) from files.

  The state may live for the application's runtime but is thread-local.yikes

*/
#ifdef CECBENCH_IMPLEMENTATION
#define CEC_INF 1.0e99
#define CEC_EPS 1.0e-14
#define CEC_EULER  2.7182818284590452353602874713526625
#define CEC_PI 3.1415926535897932384626433832795029

const char conf_cec_data[] = "db/input_data_cec";

subroutine
cecbench_state_init(struct cecbench_state *state)
{
  state->ini_flag = 0;
  state->OShift = NULL;
  state->M      = NULL;
  state->SS     = NULL;
  state->y      = NULL;
  state->z      = NULL;
}

/**
  This one deallocates the state's contents.
*/
#define FREE_EX(X)  { if ((X)) free((X)); } 
subroutine
cecbench_state_free(struct cecbench_state *state)
{
  state->ini_flag = 0;

  // @NOTE(Kaeryv) This may cause a crash (double free) YEAH
  // Because the code frees all over the place with no real logic and no NULLS
//   FREE_EX(state->OShift  );
//   FREE_EX(state->M       );
//   FREE_EX(state->SS      );
//   FREE_EX(state->y       );
//   FREE_EX(state->z       );
}

//subroutine
//cecbench_trd_clear()
//{
//  struct cecbench_state *state = pthread_getspecific(cecbench_state_key);
////   free(state->M);
////   free(state->OShift);
////   free(state->y);
////   free(state->z);
//  //free(state);
//  pthread_setspecific(cecbench_state_key, NULL);
//}

//subroutine
//cecbench_clear()
//{
//  pthread_key_delete(cecbench_state_key);
//}

void cec20_test_func(const double *noalias x, double *f, int nx, int mx,int func_num0, struct cecbench_state *state)
{
  int cf_num=10,i,j,func_num;
  int Func_num[]={1,2,3,7,4,16,6,22,24,25};
  if (func_num0<1||func_num0>10)
  {
  	printf("\nError: Test function %d is not defined.\n", func_num0);
  	exit(EXIT_FAILURE);
  }
    // Transform the func_num to suite the codes   
    func_num=Func_num[func_num0-1];
//     printf("\n Input func_num %d Running fun num%d  \n", func_num0, func_num);
	if (state->ini_flag==1)
	{
		if ((state->n_flag!=nx)||(state->func_flag!=func_num))
		{
			state->ini_flag=0;
		}
	}

	if (state->ini_flag==0)
	{
		FILE *fpt;
		char FileName[256];
		if(state->M) free(state->M);
		if(state->OShift) free(state->OShift);
		if(state->y) free(state->y);
		if(state->z) free(state->z);
		state->y=(double *)malloc(sizeof(double)  *  nx);
		state->z=(double *)malloc(sizeof(double)  *  nx);

		if (!(nx==2||nx==5||nx==10||nx==15||nx==20||nx==30||nx==50||nx==100))
		{
			printf("\nError: Test functions are only defined for D=2,5,10,15,20,30,50,100.\n");
		}
		if (nx==2&&(func_num==4||func_num==16||func_num==6))
		{
			printf("\nError:  NOT defined for D=2.\n");
		}

		/* Load Matrix M*/
		sprintf(FileName, "%s/M_%d_D%d.txt", conf_cec_data, func_num,nx);
		fpt = fopen(FileName,"r");
		if (fpt==NULL)
		{
		    printf("\n Error: Cannot open M_%d_D%d.txt for reading \n",func_num,nx);
		}
		if (func_num<20)
		{
			state->M=(double*)malloc(nx*nx*sizeof(double));
			if (state->M==NULL)
				printf("\nError: there is insufficient memory available!\n");
			for (i=0; i<nx*nx; i++)
			{
				fscanf(fpt,"%lf",&state->M[i]);
			}
		}
		else
		{
			state->M=(double*)malloc(cf_num*nx*nx*sizeof(double));
			if (state->M==NULL)
				printf("\nError: there is insufficient memory available!\n");
			for (i=0; i<cf_num*nx*nx; i++)
			{
				fscanf(fpt,"%lf",&state->M[i]);
			}
		}
		fclose(fpt);
		
		/* Load shift_data */
		sprintf(FileName, "%s/shift_data_%d.txt", conf_cec_data, func_num);
		fpt = fopen(FileName,"r");
		if (fpt==NULL)
		{
			printf("\n Error: Cannot open shift_data_%d.txt for reading \n",func_num);
		}

		if (func_num<20)
		{
			state->OShift=(double *)malloc(nx*sizeof(double));
			if (state->OShift==NULL)
			printf("\nError: there is insufficient memory available!\n");
			for(i=0;i<nx;i++)
			{
				fscanf(fpt,"%lf",&state->OShift[i]);
			}
		}
		else
		{
			state->OShift=(double *)malloc(nx*cf_num*sizeof(double));
			if (state->OShift==NULL)
			printf("\nError: there is insufficient memory available!\n");
			for(i=0;i<cf_num-1;i++)
			{
				for (j=0;j<nx;j++)
				{
					fscanf(fpt,"%lf",&state->OShift[i*nx+j]);
				}
				fscanf(fpt,"%*[^\n]%*c"); 
			}
			for (j=0;j<nx;j++)
			{
				fscanf(fpt,"%lf",&state->OShift[(cf_num-1)*nx+j]);
			}
				
		}
		fclose(fpt);


		/* Load Shuffle_data */
		
		if (func_num==4||func_num==6||(func_num>=11&&func_num<=20))//4 hf01 6 hf03 in cec2020**
		{
			sprintf(FileName, "%s/shuffle_data_%d_D%d.txt", conf_cec_data, func_num, nx);
			fpt = fopen(FileName,"r");
			if (fpt==NULL)
			{
				printf("\n Error: Cannot open shuffle_data_%d_D%d.txt for reading \n", func_num, nx);
			}
			state->SS=(int *)malloc(nx*sizeof(int));
			if (state->SS==NULL)
				printf("\nError: there is insufficient memory available!\n");
			for(i=0;i<nx;i++)
			{
				fscanf(fpt,"%d",&state->SS[i]);
			}
			fclose(fpt);	
		}
		else if (func_num==29||func_num==30)
		{
			sprintf(FileName, "%s/shuffle_data_%d_D%d.txt", conf_cec_data, func_num, nx);
			fpt = fopen(FileName,"r");
			if (fpt==NULL)
			{
				printf("\n Error: Cannot open shuffle_data_%d_D%d.txt for reading \n", func_num, nx);
			}
			state->SS=(int *)malloc(nx*cf_num*sizeof(int));
			if (state->SS==NULL)
				printf("\nError: there is insufficient memory available!\n");
			for(i=0;i<nx*cf_num;i++)
			{
				fscanf(fpt,"%d",&state->SS[i]);
			}
			fclose(fpt);
		}
		

		state->n_flag=nx;
		state->func_flag=func_num;
		state->ini_flag=1;
		//printf("Function has been initialized!\n");
	}


	for (i = 0; i < mx; i++)
	{
		switch(func_num)
		{
		case 1:	
			bent_cigar_func(&x[i*nx],&f[i],nx,state->OShift,state->M,1,1, state->y, state->z);
			f[i]+=100.0;
			break;
		case 2:	
			schwefel_func(&x[i*nx],&f[i],nx,state->OShift,state->M,1,1, state->y, state->z);//F11 in CEC2014
			f[i]+=1100.0;
			break;
		case 3:	
			bi_rastrigin_func(&x[i*nx],&f[i],nx,state->OShift,state->M,1,1, state->y, state->z);//F7 in CEC 2017
			f[i]+=700.0;
			break;
		case 4:	
			hf01(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);//F17 in cec 2014 (hf1 in cec 2014)
           
			f[i]=f[i]+1700.0;
          
//             printf("f[%d]=%f\n",i,1.0);            
			break;
		case 5:
			rastrigin_func(&x[i*nx],&f[i],nx,state->OShift,state->M,1,1, state->y, state->z);
			f[i]+=500.0;
			break;
		case 6:
			hf05(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);//F21 in cec 2014 (hf5 in cec 2014)
			f[i]+=2100.0;

			break;
		case 7:	
                        grie_rosen_func(&x[i*nx],&f[i],nx,state->OShift,state->M,1,1, state->y, state->z);//f19 in cec2017 
			f[i]+=1900.0;
			break;
		case 8:	
			step_rastrigin_func(&x[i*nx],&f[i],nx,state->OShift,state->M,1,1, state->y, state->z);
			f[i]+=800.0;
			break;
		case 9:	
			levy_func(&x[i*nx],&f[i],nx,state->OShift,state->M,1,1, state->y, state->z);
			f[i]+=900.0;
			break;
		case 10:	
			schwefel_func(&x[i*nx],&f[i],nx,state->OShift,state->M,1,1, state->y, state->z);
			f[i]+=1000.0;

			break;
		case 11:	
			hf01(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1100.0;
			break;
		case 12:	
			hf02(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1200.0;
			break;
		case 13:	
			hf03(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1300.0;
			break;
		case 14:	
			hf04(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1400.0;
			break;
		case 15:	
			hf05(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1500.0;
			break;
		case 16:	
			hf06(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1600.0;
			break;
		case 17:	
			hf07(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1700.0;
			break;
		case 18:	
			hf08(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1800.0;
			break;
		case 19:	
			hf09(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=1900.0;
			break;
		case 20:	
			hf10(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1,1, state->y, state->z);
			f[i]+=2000.0;
			break;
		case 21:	
			cf01(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2100.0;
			
			break;
		case 22:	
			cf02(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2200.0;
			
			break;
		case 23:	
			cf03(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2300.0;
			break;
		case 24:	
			cf04(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2400.0;
			break;
		case 25:	
			cf05(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2500.0;
			break;
		case 26:
			cf06(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2600.0;
			break;
		case 27:
			cf07(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2700.0;
			break;
		case 28:
			cf08(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2800.0;
			break;
		case 29:
			cf09(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=2900.0;
			break;
		case 30:
			cf10(&x[i*nx],&f[i],nx,state->OShift,state->M,state->SS,1, state->y, state->z);
			f[i]+=3000.0;
			break;
		default:
			printf("\nError: There are only 30 test functions in this test suite!\n");
			f[i] = 0.0;
			break;
		}
		
	}

}

CEC_TEST_CASE(sphere_func)
{
	int i;
	f[0] = 0.0;
	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */	
	for (i=0; i<nx; i++)
	{					
		f[0] += z[i]*z[i];
	}

}



CEC_TEST_CASE(ellips_func)
{
    int i;
	f[0] = 0.0;
	sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag, y); /* shift and rotate */
	for (i=0; i<nx; i++)
	{
       f[0] += pow(10.0,6.0*i/(nx-1.))*z[i]*z[i];
	}
}

CEC_TEST_CASE(sum_diff_pow_func)
{
    int i;
	sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag, y); // shift and rotate 
	f[0] = 0.0; 
	double sum = 0.0;
	for (i=0; i<nx; i++)
	{
		double xi = z[i];
		double newv = pow((abs(xi)),(i+1));
		sum = sum + newv;
	}
	
	f[0] = sum;
}

CEC_TEST_CASE(zakharov_func)
{
	int i;
	sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag, y); // shift and rotate 
	f[0] = 0.0; 
	double sum1 = 0.0;
	double sum2 = 0.0;
	for (i=0; i<nx; i++)
	{
		double xi = z[i];
		sum1 = sum1 + pow(xi,2);
		sum2 = sum2 + 0.5*(i+1)*xi;
	}
	
	f[0] = sum1 + pow(sum2,2) + pow(sum2,4);
}

/* Levy function */
CEC_TEST_CASE(levy_func)
{
    int i;
	f[0] = 0.0;
	sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag, y); /* shift and rotate */
	
	double *w;
	w=(double *)malloc(sizeof(double)  *  nx);

	for (i=0; i<nx; i++)
	{
	   w[i] = 1.0 + (z[i] - 1.0)/4.0;
	}
	
	double term1 = pow((sin(CEC_PI*w[0])),2);
	double term3 = pow((w[nx-1]-1),2) * (1+pow((sin(2*CEC_PI*w[nx-1])),2));
	
	double sum = 0.0;

	for (i=0; i<nx-1; i++)
	{
		double wi = w[i];
        double newv = pow((wi-1),2) * (1+10*pow((sin(CEC_PI*wi+1)),2));
		sum = sum + newv;
	}
	
	f[0] = term1 + sum + term3;
	free(w);   // ADD THIS LINE to free memory! Thanks for Dr. Janez
}

/* Dixon and Price */
CEC_TEST_CASE(dixon_price_func)
{
	int i;
	sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag, y); // shift and rotate 
	f[0] = 0;
	double x1 = z[0];;
	double term1 = pow((x1-1),2);
	
	double sum = 0;
	for (i=1; i<nx; i++)
	{
		double xi = z[i];
		double xold = z[i-1];
		double newv = i * pow((pow(2*xi,2) - xold),2);
		sum = sum + newv;
	}
	
	f[0] = term1 + sum;
}

CEC_TEST_CASE(bent_cigar_func)
{
    int i;
	sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag, y); /* shift and rotate */
	f[0] = z[0]*z[0];
	for (i=1; i<nx; i++)
	{
		f[0] += pow(10.0,6.0)*z[i]*z[i];
	}

}

CEC_TEST_CASE(discus_func)
{
    int i;
	sr_func (x, z, nx, Os, Mr,1.0, s_flag, r_flag, y); /* shift and rotate */
	f[0] = pow(10.0,6.0)*z[0]*z[0];
	for (i=1; i<nx; i++)
	{
		f[0] += z[i]*z[i];
	}
}

CEC_TEST_CASE(dif_powers_func)
{
	int i;
	f[0] = 0.0;
	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		f[0] += pow(fabs(z[i]),2+4*i/(nx-1));
	}
	f[0]=pow(f[0],0.5);
}


CEC_TEST_CASE(rosenbrock_func)
{
    int i;
	double tmp1,tmp2;
	f[0] = 0.0;
	sr_func (x, z, nx, Os, Mr, 2.048/100.0, s_flag, r_flag, y); /* shift and rotate */
	z[0] += 1.0;//shift to orgin
	for (i=0; i<nx-1; i++)
	{
		z[i+1] += 1.0;//shift to orgin
		tmp1=z[i]*z[i]-z[i+1];
		tmp2=z[i]-1.0;
		f[0] += 100.0*tmp1*tmp1 +tmp2*tmp2;
	}
}

CEC_TEST_CASE(schaffer_F7_func)
{
    int i;
	double tmp;
    f[0] = 0.0;
	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */
	for (i=0; i<nx-1; i++)	
	{
		z[i]=pow(y[i]*y[i]+y[i+1]*y[i+1],0.5);
		tmp=sin(50.0*pow(z[i],0.2));
		f[0] += pow(z[i],0.5)+pow(z[i],0.5)*tmp*tmp ;
	}
	f[0] = f[0]*f[0]/(nx-1)/(nx-1);
}

CEC_TEST_CASE(ackley_func)
{
    int i;
    double sum1, sum2;
    sum1 = 0.0;
    sum2 = 0.0;

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		sum1 += z[i]*z[i];
		sum2 += cos(2.0*CEC_PI*z[i]);
	}
	sum1 = -0.2*sqrt(sum1/nx);
	sum2 /= nx;
		f[0] =  CEC_EULER - 20.0*exp(sum1) - exp(sum2) +20.0;
}


CEC_TEST_CASE(weierstrass_func)
{
    int i,j,k_max;
    double sum,sum2=0.0, a, b;
    a = 0.5;
    b = 3.0;
    k_max = 20;
    f[0] = 0.0;

	sr_func (x, z, nx, Os, Mr, 0.5/100.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		sum = 0.0;
		sum2 = 0.0;
		for (j=0; j<=k_max; j++)
		{
			sum += pow(a,j)*cos(2.0*CEC_PI*pow(b,j)*(z[i]+0.5));
			sum2 += pow(a,j)*cos(2.0*CEC_PI*pow(b,j)*0.5);
		}
		f[0] += sum;
	}
	f[0] -= nx*sum2;
}


CEC_TEST_CASE(griewank_func)
{
    int i;
    double s, p;
    s = 0.0;
    p = 1.0;

	sr_func (x, z, nx, Os, Mr, 600.0/100.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		s += z[i]*z[i];
		p *= cos(z[i]/sqrt(1.0+i));
	}
	f[0] = 1.0 + s/4000.0 - p;
}

CEC_TEST_CASE(rastrigin_func)
{
    int i;
	f[0] = 0.0;

	sr_func (x, z, nx, Os, Mr, 5.12/100.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		f[0] += (z[i]*z[i] - 10.0*cos(2.0*CEC_PI*z[i]) + 10.0);
	}
}

CEC_TEST_CASE(step_rastrigin_func)
{
    int i;
	f[0]=0.0;
	for (i=0; i<nx; i++)
	{
		if (fabs(y[i]-Os[i])>0.5)
		y[i]=Os[i]+floor(2*(y[i]-Os[i])+0.5)/2;
	}

	sr_func (x, z, nx, Os, Mr, 5.12/100.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		f[0] += (z[i]*z[i] - 10.0*cos(2.0*CEC_PI*z[i]) + 10.0);
	}
}

CEC_TEST_CASE(schwefel_func)
{
    int i;
	double tmp;
	f[0]=0.0;

	sr_func (x, z, nx, Os, Mr, 1000.0/100.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		z[i] += 4.209687462275036e+002;
		if (z[i]>500)
		{
			f[0]-=(500.0-fmod(z[i],500))*sin(pow(500.0-fmod(z[i],500),0.5));
			tmp=(z[i]-500.0)/100;
			f[0]+= tmp*tmp/nx;
		}
		else if (z[i]<-500)
		{
			f[0]-=(-500.0+fmod(fabs(z[i]),500))*sin(pow(500.0-fmod(fabs(z[i]),500),0.5));
			tmp=(z[i]+500.0)/100;
			f[0]+= tmp*tmp/nx;
		}
		else
			f[0]-=z[i]*sin(pow(fabs(z[i]),0.5));
		}
		f[0] +=4.189828872724338e+002*nx;

}

CEC_TEST_CASE(katsuura_func)
{
    int i,j;
	double temp,tmp1,tmp2,tmp3;
	f[0]=1.0;
	tmp3=pow(1.0*nx,1.2);

	sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag, y); /* shift and rotate */

    for (i=0; i<nx; i++)
	{
		temp=0.0;
		for (j=1; j<=32; j++)
		{
			tmp1=pow(2.0,j);
			tmp2=tmp1*z[i];
			temp += fabs(tmp2-floor(tmp2+0.5))/tmp1;
		}
		f[0] *= pow(1.0+(i+1)*temp,10.0/tmp3);
    }
	tmp1=10.0/nx/nx;
    f[0]=f[0]*tmp1-tmp1;

}

CEC_TEST_CASE(bi_rastrigin_func)
{
    int i;
	double mu0=2.5,d=1.0,s,mu1,tmp,tmp1,tmp2;
	double *tmpx;
	tmpx=(double *)malloc(sizeof(double)  *  nx);
	s=1.0-1.0/(2.0*pow(nx+20.0,0.5)-8.2);
	mu1=-pow((mu0*mu0-d)/s,0.5);

	if (s_flag==1)
		shiftfunc(x, y, nx, Os);
	else
	{
		for (i=0; i<nx; i++)//shrink to the orginal search range
		{
			y[i] = x[i];
		}
	}
	for (i=0; i<nx; i++)//shrink to the orginal search range
    {
        y[i] *= 10.0/100.0;
    }

	for (i = 0; i < nx; i++)
    {
		tmpx[i]=2*y[i];
        if (Os[i] < 0.0)
            tmpx[i] *= -1.;
    }
	for (i=0; i<nx; i++)
	{
		z[i]=tmpx[i];
		tmpx[i] += mu0;
	}
    tmp1=0.0;tmp2=0.0;
	for (i=0; i<nx; i++)
	{
		tmp = tmpx[i]-mu0;
		tmp1 += tmp*tmp;
		tmp = tmpx[i]-mu1;
		tmp2 += tmp*tmp;
	}
	tmp2 *= s;
	tmp2 += d*nx;
	tmp=0.0;

	if (r_flag==1)
	{
		rotatefunc(z, y, nx, Mr);
		for (i=0; i<nx; i++)
		{
			tmp+=cos(2.0*CEC_PI*y[i]);
		}	
		if(tmp1<tmp2)
			f[0] = tmp1;
		else
			f[0] = tmp2;
		f[0] += 10.0*(nx-tmp);
	}
	else
	{
		for (i=0; i<nx; i++)
		{
			tmp+=cos(2.0*CEC_PI*z[i]);
		}	
		if(tmp1<tmp2)
			f[0] = tmp1;
		else
			f[0] = tmp2;
		f[0] += 10.0*(nx-tmp);
	}

	free(tmpx);
}

CEC_TEST_CASE(grie_rosen_func)
{
    int i;
    double temp,tmp1,tmp2;
    f[0]=0.0;

	sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag, y); /* shift and rotate */

	z[0] += 1.0;//shift to orgin
    for (i=0; i<nx-1; i++)
    {
		z[i+1] += 1.0;//shift to orgin
		tmp1 = z[i]*z[i]-z[i+1];
		tmp2 = z[i]-1.0;
        temp = 100.0*tmp1*tmp1 + tmp2*tmp2;
         f[0] += (temp*temp)/4000.0 - cos(temp) + 1.0;
    }
	tmp1 = z[nx-1]*z[nx-1]-z[0];
	tmp2 = z[nx-1]-1.0;
    temp = 100.0*tmp1*tmp1 + tmp2*tmp2;;
    f[0] += (temp*temp)/4000.0 - cos(temp) + 1.0 ;
}


CEC_TEST_CASE(escaffer6_func)
{
    int i;
    double temp1, temp2;

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

    f[0] = 0.0;
    for (i=0; i<nx-1; i++)
    {
        temp1 = sin(sqrt(z[i]*z[i]+z[i+1]*z[i+1]));
		temp1 =temp1*temp1;
        temp2 = 1.0 + 0.001*(z[i]*z[i]+z[i+1]*z[i+1]);
        f[0] += 0.5 + (temp1-0.5)/(temp2*temp2);
    }
    temp1 = sin(sqrt(z[nx-1]*z[nx-1]+z[0]*z[0]));
	temp1 =temp1*temp1;
    temp2 = 1.0 + 0.001*(z[nx-1]*z[nx-1]+z[0]*z[0]);
    f[0] += 0.5 + (temp1-0.5)/(temp2*temp2);
}

CEC_TEST_CASE(happycat_func)
/* original global optimum: [-1,-1,...,-1] */
{
	int i;
	double alpha,r2,sum_z;
	alpha=1.0/8.0;
	
	sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag, y); /* shift and rotate */

	r2 = 0.0;
	sum_z=0.0;
    for (i=0; i<nx; i++)
    {
		z[i]=z[i]-1.0;//shift to orgin
        r2 += z[i]*z[i];
		sum_z += z[i];
    }
    f[0]=pow(fabs(r2-nx),2*alpha) + (0.5*r2 + sum_z)/nx + 0.5;
}

CEC_TEST_CASE(hgbat_func)
/* original global optimum: [-1,-1,...,-1] */
{
	int i;
	double alpha,r2,sum_z;
	alpha=1.0/4.0;

	sr_func (x, z, nx, Os, Mr, 5.0/100.0, s_flag, r_flag, y); /* shift and rotate */

	r2 = 0.0;
	sum_z=0.0;
    for (i=0; i<nx; i++)
    {
		z[i]=z[i]-1.0;//shift to orgin
        r2 += z[i]*z[i];
		sum_z += z[i];
    }
    f[0]=pow(fabs(pow(r2,2.0)-pow(sum_z,2.0)),2*alpha) + (0.5*r2 + sum_z)/nx + 0.5;
}

CEC_HF_CASE(hf01)
{
	int i,tmp,cf_num=3;
	double fit[3];
	int G[3],G_nx[3];
	double Gp[3]={0.3,0.3,0.4};

	tmp=0;
	for (i=1; i<cf_num; i++)
	{
		G_nx[i] = ceil(Gp[i]*nx);
		tmp += G_nx[i];
	}
	//G_nx[cf_num-1]=nx-tmp;
    G_nx[0]=nx-tmp;
	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	i=0;
	schwefel_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
//     printf("G_nx[%d]=%d\n",i,G_nx[i]);
	i=1;
	rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
//     printf("G_nx[%d]=%d\n",i,G_nx[i]);
	i=2;
	ellips_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
//     printf("G_nx[%d]=%d\n",i,G_nx[i]);
//     printf("fit[2]=%f\n",fit[2]);
	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
     
}

CEC_HF_CASE(hf02)
{
	int i,tmp,cf_num=3;
	double fit[3];
	int G[3],G_nx[3];
	double Gp[3]={0.3,0.3,0.4};

	tmp=0;
	for (i=0; i<cf_num-1; i++)
	{
		G_nx[i] = ceil(Gp[i]*nx);
		tmp += G_nx[i];
	}
	G_nx[cf_num-1]=nx-tmp;

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	i=0;
	ellips_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	schwefel_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	bent_cigar_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
}

CEC_HF_CASE(hf03)
{
	int i,tmp,cf_num=3;
	double fit[3];
	int G[3],G_nx[3];
	double Gp[3]={0.3,0.3,0.4};

	tmp=0;
	for (i=0; i<cf_num-1; i++)
	{
		G_nx[i] = ceil(Gp[i]*nx);
		tmp += G_nx[i];
	}
	G_nx[cf_num-1]=nx-tmp;

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	
	i=0;
	bent_cigar_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	rosenbrock_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	bi_rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);

	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
	
}

CEC_HF_CASE(hf04)
{
	int i,tmp,cf_num=4;
	double fit[4];
	int G[4],G_nx[4];
	double Gp[4]={0.2,0.2,0.2,0.4};

	tmp=0;
	for (i=0; i<cf_num-1; i++)
	{
		G_nx[i] = ceil(Gp[i]*nx);
		tmp += G_nx[i];
	}
	G_nx[cf_num-1]=nx-tmp;

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	i=0;
	ellips_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	ackley_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	schaffer_F7_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=3;
	rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	
	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
}

CEC_HF_CASE(hf05)
{
	int i,tmp,cf_num=5;
	double fit[5] = {0.};
	int G[5],G_nx[5];
	double Gp[5]={0.1,0.2,0.2,0.2,0.3};

	if (nx==5) // deal with D=6**04/01/2020
    { 
        G_nx[0]=1;
        G_nx[1]=1;
        G_nx[2]=1;
        G_nx[3]=1;
        G_nx[4]=1;
      
    }// deal with D=5**04/01/2020   
    else
    {
        tmp=0;
        for (i=1; i<cf_num; i++)
        {
            G_nx[i] = ceil(Gp[i]*nx);
            tmp += G_nx[i];
        }
        G_nx[0]=nx-tmp;
    }

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	i=0;
	escaffer6_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	hgbat_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	rosenbrock_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=3;
	schwefel_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=4;

	ellips_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	//printf("fit: %lf\n",fit[4]);

	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{

		f[0] += fit[i];
	}
}
CEC_HF_CASE(hf06)
{
	int i,tmp,cf_num=4;
	double fit[4];
	int G[4],G_nx[4];
	double Gp[4]={0.2,0.2,0.3,0.3};

	if (nx==5) // deal with D=5**04/01/2020
    { 
        
            G_nx[0] = 1;
            G_nx[1] = 1;
            G_nx[2] = 1;
            G_nx[3] = 2;
        
    }// deal with D=6**04/01/2020   
    else
    {
        tmp=0;
        for (i=1; i<cf_num; i++)
        {
            G_nx[i] = ceil(Gp[i]*nx);
            tmp += G_nx[i];
        }
        G_nx[0]=nx-tmp;
    }

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	i=0;
	escaffer6_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	hgbat_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	rosenbrock_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=3;
	schwefel_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	
	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
}

CEC_HF_CASE(hf07)
{
	int i,tmp,cf_num=5;
	double fit[5];
	int G[5],G_nx[5];
	double Gp[5]={0.1,0.2,0.2,0.2,0.3};

	tmp=0;
	for (i=0; i<cf_num-1; i++)
	{
		G_nx[i] = ceil(Gp[i]*nx);
		tmp += G_nx[i];
	}
	G_nx[cf_num-1]=nx-tmp;

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	i=0;
	katsuura_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	ackley_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	grie_rosen_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=3;
	schwefel_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=4;
	rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);

	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
}

CEC_HF_CASE(hf08)
{
	int i,tmp,cf_num=5;
	double fit[5];
	int G[5],G_nx[5];
	double Gp[5]={0.2,0.2,0.2,0.2,0.2};

	tmp=0;
	for (i=0; i<cf_num-1; i++)
	{
		G_nx[i] = ceil(Gp[i]*nx);
		tmp += G_nx[i];
	}
	G_nx[cf_num-1]=nx-tmp;

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	
	i=0;
	ellips_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	ackley_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=3;
	hgbat_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=4;
	discus_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);

	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
}

CEC_HF_CASE(hf09)
{
	int i,tmp,cf_num=5;
	double fit[5];
	int G[5],G_nx[5];
	double Gp[5]={0.2,0.2,0.2,0.2,0.2};

	tmp=0;
	for (i=0; i<cf_num-1; i++)
	{
		G_nx[i] = ceil(Gp[i]*nx);
		tmp += G_nx[i];
	}
	G_nx[cf_num-1]=nx-tmp;

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	
	i=0;
	bent_cigar_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	grie_rosen_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=3;
	weierstrass_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=4;
	escaffer6_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);

	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
}


CEC_HF_CASE(hf10)
{
	int i,tmp,cf_num=6;
	double fit[6];
	int G[6],G_nx[6];
	double Gp[6]={0.1,0.1,0.2,0.2,0.2,0.2};

	tmp=0;
	for (i=0; i<cf_num-1; i++)
	{
		G_nx[i] = ceil(Gp[i]*nx);
		tmp += G_nx[i];
	}
	G_nx[cf_num-1]=nx-tmp;

	G[0]=0;
	for (i=1; i<cf_num; i++)
	{
		G[i] = G[i-1]+G_nx[i-1];
	}

	sr_func (x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and rotate */

	for (i=0; i<nx; i++)
	{
		y[i]=z[S[i]-1];
	}
	
	i=0;
	hgbat_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=1;
	katsuura_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=2;
	ackley_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=3;
	rastrigin_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=4;
	schwefel_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);
	i=5;
	schaffer_F7_func(&y[G[i]],&fit[i],G_nx[i],Os,Mr,0,0,y,z);

	f[0]=0.0;
	for(i=0;i<cf_num;i++)
	{
		f[0] += fit[i];
	}
}

CEC_CF_CASE(cf01)
{
	int i,cf_num=3;
	double fit[3];
	double delta[3] = {10, 20, 30};
	double bias[3] = {0, 100, 200};
	
	i=0;
	rosenbrock_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	i=1;
	ellips_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+10;
	i=2;
	rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num); 
}

CEC_CF_CASE(cf02)
{
	int i,cf_num=3;
	double fit[3];
	double delta[3] = {10,20,30};
	double bias[3] = {0, 100, 200};

	i=0;
	rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	i=1;
	griewank_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/100;
	i=2;
	schwefel_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
}

CEC_CF_CASE(cf03)
{
	int i,cf_num=4;
	double fit[4];
	double delta[4] = {10,20,30,40};
	double bias[4] = {0, 100, 200, 300};
	
	i=0;
	rosenbrock_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	i=1;
	ackley_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/100;
	i=2;
	schwefel_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	i=3;
	rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num); 
	
}
CEC_CF_CASE(cf04)
{
	int i,cf_num=4;
	double fit[4];
	double delta[4] = {10,20,30,40};
	double bias[4] = {0, 100, 200, 300};
	
	i=0;
	ackley_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/100;
	i=1;
	ellips_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+10;
	i=2;
	griewank_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/100;
	i=3;
	rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
}

CEC_CF_CASE(cf05)
{
	int i,cf_num=5;
	double fit[5];
	double delta[5] = {10,20,30,40,50};
	double bias[5] = {0, 100, 200, 300, 400};
	i=0;
	rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+3;
	i=1;
	happycat_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/1e+3;
	i=2;
	ackley_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/100;
	i=3;
	discus_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+10;	
	i=4;
	rosenbrock_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
}		


CEC_CF_CASE(cf06)
{
	int i,cf_num=5;
	double fit[5];
	double delta[5] = {10,20,20,30,40};
	double bias[5] = {0, 100, 200, 300, 400};
	i=0;
	escaffer6_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/2e+7;
	i=1;
	schwefel_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	i=2;
	griewank_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/100;
	i=3;
	rosenbrock_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	i=4;
	rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+3;
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
}

CEC_CF_CASE(cf07)
{
	int i,cf_num=6;
	double fit[6];
	double delta[6] = {10,20,30,40,50,60};
	double bias[6] = {0, 100, 200, 300, 400, 500};
	i=0;
	hgbat_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1000;
	i=1;
	rastrigin_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+3;
	i=2;
	schwefel_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/4e+3;
	i=3;
	bent_cigar_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+30;
	i=4;
	ellips_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+10;
	i=5;
	escaffer6_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/2e+7;
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num); 
}

CEC_CF_CASE(cf08)
{	
	int i,cf_num=6;
	double fit[6];
	double delta[6] = {10,20,30,40,50,60};
	double bias[6] = {0, 100, 200, 300, 400, 500};
	i=0;
	ackley_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/100;
	i=1;
	griewank_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/100;
	i=2;
	discus_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/1e+10;
	i=3;
	rosenbrock_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	i=4;
	happycat_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=1000*fit[i]/1e+3;
	i=5;
	escaffer6_func(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],1,r_flag,y,z);
	fit[i]=10000*fit[i]/2e+7;
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
}


CEC_CF_CASE(cf09)
{
	
	int i,cf_num=3;
	double fit[3];
	double delta[3] = {10,30,50};
	double bias[3] = {0, 100, 200};
	i=0;
	hf05(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],&SS[i*nx],1,r_flag, y, z);
	i=1;
	hf06(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],&SS[i*nx],1,r_flag, y, z);
	i=2;
	hf07(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],&SS[i*nx],1,r_flag, y, z);
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
		
}

CEC_CF_CASE(cf10)
{
	int i,cf_num=3;
	double fit[3];
	double delta[3] = {10,30,50};
	double bias[3] = {0, 100, 200};
	i=0;
	hf05(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],&SS[i*nx],1,r_flag, y, z);
	i=1;
	hf08(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],&SS[i*nx],1,r_flag, y, z);
	i=2;
	hf09(x,&fit[i],nx,&Os[i*nx],&Mr[i*nx*nx],&SS[i*nx],1,r_flag, y, z);
	cf_cal(x, f, nx, Os, delta,bias,fit,cf_num);
}


void shiftfunc (const double *x, double *xshift, int nx,double *Os)
{
	int i;
    for (i=0; i<nx; i++)
    {
        xshift[i]=x[i]-Os[i];
    }
}

void rotatefunc (const double *x, double *xrot, int nx,double *Mr)
{
	int i,j;
    for (i=0; i<nx; i++)
    {
        xrot[i]=0;
			for (j=0; j<nx; j++)
			{
				xrot[i]=xrot[i]+x[j]*Mr[i*nx+j];
			}
    }
}

void sr_func (const double *x, double *sr_x, int nx, double *Os,double *Mr, double sh_rate, int s_flag,int r_flag, double *y ) /* shift and rotate */
{
	int i;
	if (s_flag==1)
	{
		if (r_flag==1)
		{	
			shiftfunc(x, y, nx, Os);
			for (i=0; i<nx; i++)//shrink to the orginal search range
			{
				y[i]=y[i]*sh_rate;
			}
			rotatefunc(y, sr_x, nx, Mr);
		}
		else
		{
			shiftfunc(x, sr_x, nx, Os);
			for (i=0; i<nx; i++)//shrink to the orginal search range
			{
				sr_x[i]=sr_x[i]*sh_rate;
			}
		}
	}
	else
	{	

		if (r_flag==1)
		{	
			for (i=0; i<nx; i++)//shrink to the orginal search range
			{
				y[i]=x[i]*sh_rate;
			}
			rotatefunc(y, sr_x, nx, Mr);
		}
		else
		for (i=0; i<nx; i++)//shrink to the orginal search range
		{
			sr_x[i]=x[i]*sh_rate;
		}
	}
}

void asyfunc (const double *x, double *xasy, int nx, double beta)
{
	int i;
    for (i=0; i<nx; i++)
    {
		if (x[i]>0)
        xasy[i]=pow(x[i],1.0+beta*i/(nx-1)*pow(x[i],0.5));
    }
}

void oszfunc (const double *x, double *xosz, int nx)
{
	int i,sx;
	double c1,c2,xx=0.0;
    for (i=0; i<nx; i++)
    {
		if (i==0||i==nx-1)
        {
			if (x[i]!=0)
				xx=log(fabs(x[i]));
			if (x[i]>0)
			{	
				c1=10;
				c2=7.9;
			}
			else
			{
				c1=5.5;
				c2=3.1;
			}	
			if (x[i]>0)
				sx=1;
			else if (x[i]==0)
				sx=0;
			else
				sx=-1;
			xosz[i]=sx*exp(xx+0.049*(sin(c1*xx)+sin(c2*xx)));
		}
		else
			xosz[i]=x[i];
    }
}


void cf_cal(const double *x, double *f, int nx, double *Os,double * delta,double * bias,double * fit, int cf_num)
{
	int i,j;
	double *w;
	double w_max=0,w_sum=0;
	w=(double *)malloc(cf_num * sizeof(double));
	for (i=0; i<cf_num; i++)
	{
		fit[i]+=bias[i];
		w[i]=0;
		for (j=0; j<nx; j++)
		{
			w[i]+=pow(x[j]-Os[i*nx+j],2.0);
		}
		if (w[i]!=0)
			w[i]=pow(1.0/w[i],0.5)*exp(-w[i]/2.0/nx/pow(delta[i],2.0));
		else
			w[i]=CEC_INF;
		if (w[i]>w_max)
			w_max=w[i];
	}

	for (i=0; i<cf_num; i++)
	{
		w_sum=w_sum+w[i];
	}
	if(w_max==0)
	{
		for (i=0; i<cf_num; i++)
			w[i]=1;
		w_sum=cf_num;
	}
	f[0] = 0.0;
    for (i=0; i<cf_num; i++)
    {
		f[0]=f[0]+w[i]/w_sum*fit[i];
    }
	free(w);
}
#undef CEC_TEST_CASE
#endif
#endif
