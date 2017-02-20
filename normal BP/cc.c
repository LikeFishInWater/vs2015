#include <stdio.h>
#include <math.h>
#include <stdlib.h>   //用到随机数函数srand
#include <time.h>      //用到time函数
 
#define NODES               9216  
#define J                    3   
#define K                    6  
#define PI 3.1415926
#define limited 1e-50
int test_num;
int max_iter=30;

int H_mcw;
int H_mrw;
int n=9216;                         
int k=4608;                          
int M,N;  
int sum;                     

struct parent_node 
{
    int size;
    int index[J];
    double v[4608];
}
code_node[NODES];

struct child_node 
{
    int size;
    int index[K];                    
    double u[NODES];     
 }
check_node[4608];

double code_out[NODES];
int encoded_out[NODES],decoded[NODES];
int matrix[4608][9216],message[4608];


void encode_ldpc();
int decode_ldpc(double);
double F(double);


main()
{
 int i,j,num,noise,error,success;
 double no,u1,u0,gaussnoise,ber,snr,aver_iter;
 long error_bit,z,now;
 FILE *fp,*fp1;
  printf("read the H matrix......\n");
 /**********************读取H矩阵*****************************/
     
  if((fp=fopen("0.5LDPC_h.txt","r"))!= NULL)
    {
      fscanf(fp, "%d %d", &N, &M);           //从文件中读取数据，赋给N=9216,M=4608
      fscanf(fp, "%d %d", &H_mcw, &H_mrw);   //从文件中读取数据，赋给H_mcw=3,H_mrw=6 
     // printf("N=%d,M=%d\n",N,M);
     // printf("H_mcw=%d,H_mrw=%d\n",H_mcw,H_mrw);
      for (i=0; i<N; i++)
      {
        fscanf(fp, "%d", &code_node[i].size);          //9216个3,code_node[i].size=3
        //printf("code_node[i].size=%d",code_node[i].size);
      }
      for (i=0; i<M; i++)
      {
          fscanf(fp, "%d", &check_node[i].size);       //4608个6 ,check_node[i].size=6
      }
      
      for (i=0; i<N; i++)
      {
        for (j=0; j<code_node[i].size; j++)
    	{
          fscanf(fp, "%d", &code_node[i].index[j]);     
             //printf("j=%d,code_node[i].index[j]=%d\n",j,code_node[i].index[j]);
    	}
      }

      for (i=0; i<M; i++)
      {
        for (j=0; j<check_node[i].size; j++)
    	{
          fscanf(fp, "%d", &check_node[i].index[j]);
           //printf("j=%d,check_node[i].index[j]=%d\n",j,check_node[i].index[j]);
    	}
      }
      fclose(fp);
    }
  else 
    { 
      printf("get the H matrix error....\n"); 
      exit(0);
    }
/**********************读取G矩阵*************************/

  printf("read the G matrix......\n");
  if((fp=fopen("9216_4608_g.txt","r"))!= NULL)
  { 
      for (i=0;i<n-k;i++)
      {
         for (j=0;j<n;j++)
    	 {
             fscanf(fp,"%d ",&matrix[i][j]);
    	 }
      }
  }
  else 
    { 
      printf("get the G matrix error....\n"); 
      exit(0);
    }
  fclose(fp);
/**********************读取G矩阵完成*************************/

 srand(time(&now));
 printf("start simulation......\n");
 fp1=fopen("result.txt","w");
 for(noise=20;noise<=1000;noise++)
 {
    snr=0.5*noise;
    /*snr=1.2;*/
   no=pow(10,(double)-snr/10)*2;    //计算10的-snr/10次幂的2倍
  
   sum=0;
   ber=0;
   error_bit=0;
   success=0;
   error=0;


   if(snr<1.2)
       test_num=200;
  else if(snr<1.3)
     test_num=300;
  else if(snr<1.4)
     test_num=400;
  else if(snr<1.65)
      test_num=5000;
  else if(snr<1.7) 
     test_num=20000;
  else
      test_num=50000;
   for(num=0;num<test_num;num++)
                                
   {
/******************产生随机数,作为信息矢量m*******************/
    for(i=0;i<k;i++)                 /*  k=6 */ 
    
       message[i]=rand()%2;             //产生随机数0、1,
     
    
/******************进行编码******************/
    encode_ldpc();



/****************BPSK映射************************/

    for(j=0;j<n;j++)
    {
    encoded_out[j]=(1-2*encoded_out[j]);
    }
    /*for(i=0;i<10;i++)*/
/*printf(" %d ",encoded_out[i]); */
/****************加高斯白噪声*******************/

    for(i=0;i<n;i++)
    {
    u0=(double)rand()/(double)RAND_MAX;
    u1=(double)rand()/(double)RAND_MAX;
      if(u0<limited)
      {
        u0=limited;
      }
    gaussnoise=sqrt(no*log(1.0/u0))*sin(2*PI*u1);
    /*gaussnoise=0;*/
     /* printf(" %f ",gaussnoise);*/
      /*printf(" %f ",no); */
    code_out[i]=(double)encoded_out[i]+gaussnoise;
    }
   /* for(i=0;i<5;i++) */
   /* printf(" %f ",code_out[i]); */
/*************************进行译码***********************/
   z=0;
   z=decode_ldpc(no);
   if(z==0)
   {
    success++;
   }
   else
   {
    error++;
   }
   error_bit+=z;
   ber=error_bit/(double)((num+1)*4608);
   aver_iter=sum/(double)(num+1);
   if((num+1)%100==0)
   {
   printf("snr=%f num=%d success=%d error_bit=%d error_code=%d ber=%.8f aver_iter=%f\n",snr,num+1,success,error_bit,error,ber,aver_iter);
   fprintf(fp1,"snr=%f   num=%d  success=%d    error_bit=%d  error_code=%d   ber=%.8f   aver_iter=%f\n",snr,num+1,success,error_bit,error,ber,aver_iter);
   fflush(fp1);
   }
   }
   }
 fclose(fp1);
   }
/*************************主函数结束***********************/



double F(double x)
{
  double data;
  if (x==0.0) return(1e10);
  if (fabs(x)>30)
    data=0;
  else
    data=log((exp(x)+1.0)/(exp(x)-1.0));
  if(data>1e10)
  {
   data=1e10;
  }
  else if(data<-1e10)
  {
   data=-1e10;
  }
  return(data);
}
/*************************F（）的输出为-e的10次方~e的10次方***********************/



/*************************译码***********************/
int decode_ldpc(double no)                   
{
long error_bit;
int num;
int i,j,l,iter ;
/*,q=0;*/
int aux,auy;
double delt,v,st;
int  sign;
double LLR[NODES],q1[NODES];    //LLR为对数似然比
              

/**********************初始化**************************/
    for(i=0;i<n;i++)
    {		
     LLR[i]=4.0*code_out[i]/no;
         for(j=0;j<code_node[i].size;j++)
    	 {
           aux=code_node[i].index[j];        //矩阵中每行信息位的位置
           code_node[i].v[aux-1]=LLR[i];     //矩阵中每行信息位的取值均为LLR[i]
    	 
    	 }
     
    }
 /**********************code_node[i]中有3位取值为LLR[i]**************************/   
    
    
     
  

/********************开始迭代*********************/
for(iter=0;iter<max_iter;iter++)       //max_iter=30
{
    /*校验节点更新*/
  for(i=0;i<M;i++)
  {
    for(j=0;j<check_node[i].size;j++)
     {
      delt = 0;
      sign = 0; 
      auy=check_node[i].index[j];
     for (l=0;l<check_node[i].size;l++)
       {
         if(l!=j)
    	 {
             aux=check_node[i].index[l];
             v=code_node[aux-1].v[i];          //列节点信息
            if (v<0.0)
                sign^= 1;
            delt+=F(fabs(v));      //fabs(v)为取v的绝对值，F(fabs(v))为0~~e的10次方
    	 }
     }
      
      if(sign==0)
    	 
        check_node[i].u[auy-1]=F(delt);
      else
        check_node[i].u[auy-1]=-F(delt);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    }

  }
    
    /*  printf("%f",check_node[0].u[0]);  */

     /*信息节点更新 */
  for(i=0;i<N;i++)
  {

    for(j=0;j<code_node[i].size;j++)
    {
         st=0;
         auy=code_node[i].index[j];
            for(l=0;l<code_node[i].size;l++)
    		{
               if(l!=j)
        	   {

                 aux=code_node[i].index[l];
                 st+=check_node[aux-1].u[i];    //用除本身以外的校验节点更新信息节点
        	   }	
    		}

            code_node[i].v[auy-1]=LLR[i]+st;     //???3个节点 
    }

  }
  
/******************进行判决*************************/
  for(j=0;j<N;j++)
  {
      q1[j]=0;
      for(i=0;i<code_node[j].size;i++)
      {
          aux=code_node[j].index[i];
          q1[j]+=check_node[aux-1].u[j];

      }
      q1[j]+=LLR[j];
       
    if(q1[j]< 0.0) 
      decoded[j]=1;
    else 
      decoded[j]=0;
  } 
 /*printf("%f ",q1[0]); */


/********************判断译码是否成功*********************/
    	
        error_bit=0;
        for(j=0;j<n-k;j++)
    	{
            num=0;
            for(l=0;l<check_node[j].size;l++)
    		{
               if(decoded[check_node[j].index[l]-1]==1)
                num++;
    		}
            if((num%2)==1)
            error_bit++;

    	}
     if(error_bit==0)
    {
    /*printf(" %d",iter+1)*/
     break;}
}
 
sum+=iter;
  /*printf(" %d",iter); */
return (error_bit);

}





void  encode_ldpc()                //编码
{
 int temp,i,j;
 for(i=0;i<n;i++)
 {
   temp=0;
    for(j=0;j<k;j++)
    {
     temp=temp+message[j]*matrix[j][i];           // 信息矢量m与生成矩阵G相乘
    }
  encoded_out[i]=temp%2;                          //模2加输出，编码完成

 }
/*for(i=0;i<10;i++)*/
 /*printf(" %d",encoded_out[i]);  */

}
