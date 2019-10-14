#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <pthread.h>
typedef struct customer
{
	int c_custkey;    	   //顾客编号
	char c_ment[20]; //对应的某个市场部门
}cus;				   //顾客结构体 

typedef struct orders
{
	int o_orderkey;    	 //订单号 
	long int o_custkey;    	 //顾客编号
	char odate[10];//订货日期 
}order;				 //订单

typedef struct lineitem
{
	int l_orderkey;//订单号
	double l_price;//额外价格
	char sdate[10];;//发货日期 
}line; //商品信息 

typedef struct
{
	int l_orderkey;//订单号
	char o_orderdate[10];//订货日期 
	double l_extendedprice;//额外价格
}select_result;

void * read_customer_txt(void * r_txt) //读取customer。txt内容 
{
	FILE * fp;
	customer *a=NULL;
	a = (customer *)malloc(101*sizeof(customer));
	int i=0;
	char b;
	fp = fopen("customer.txt","r");
	if(NULL==fp)
	{
		printf("cannot open customer.txt!");
		return NULL;
	}
	while(!feof(fp))
	{	
		fscanf(fp,"%d%c%s",&a[i].c_custkey, &a[i].c_ment);
		i++;
	}
	fclose(fp);
	return a;
}

void * read_orders_txt(void * r_txt)//读取orders.txt内容 
{
	int i =0; 
	orders * a=NULL;
	a = (orders * )malloc(4001*sizeof(orders));
	char b,c;
	long long d;
	FILE *fp;
	fp = fopen("orders.txt","r");
	if(fp == NULL)
	{
		printf("cannot open orders.txt!");
		return NULL;
	}
	while(!feof(fp))
	{	
		fscanf(fp,"%d%c%lld%c%s",&a[i].o_orderkey,&b,&d,&c,&a[i].odate);
		a[i].o_custkey=d%100;
		i++;
	}
	fclose(fp);
	return a;
}

void * read_lineitem_txt(void * r_txt)//读取lineitem.txt内容
{
	FILE * fp;
	lineitem * l=NULL;
	l = (lineitem *)malloc(1001*sizeof(lineitem));
	int i=0;
	char b,c;
	fp = fopen("lineitme.txt","r");
	if(fp==NULL)
	{
		printf("cannot open lineitem.txt!");
		return NULL;
	}
	while(!feof(fp))
	{
		//printf("%d!",i);
		fscanf(fp,"%d%c%lf%c%s",&l[i].l_orderkey,&c,&l[i].l_price,&b,&l[i].sdate);
		//printf("%d,%lf,%s\n",l[i].l_orderkey,l[i].l_extendedprice,l[i].l_shipdate);
		i++;
	}
	fclose(fp);
	return l; 
}

char** argvs;


void* thread(void* t)
{
	int q = *(int*)(void*)t;
//	printf("ID:%d\n", q);
	char order_date[] = " ";
	char ship_date[] = " ";
	int d; 
	int k, j, l = 0, m = 0, i = 0;
	strcpy(order_date, argvs[6 + q * 4]);
	strcpy(ship_date, argvs[7 + q * 4]);
	cus customer[1000];
	line lineitme[3000];
	order orders[10000];
	
	select_result result1[2000];
	select_result result2[2000];
	//多线程读取文件
	pthread_t cus_id,ord_id,item_id; 
	
	pthread_create(&cus_id,NULL,read_customer_txt,(void*)&q);  //创建读取customer.txt的线程，导入客户表 
	pthread_create(&ord_id,NULL,read_orders_txt,(void*)&q);    //创建读取orders.txt的线程，导入客户表 
	pthread_create(&item_id,NULL,read_lineitem_txt,(void*)&q); //创建读取lineitem.txt的线程，导入客户表 
	pthread_join(cus_id,NULL);
	pthread_join(ord_id,NULL);                              //pthread_join的作用在于 阻塞当前进程
	pthread_join(item_id,NULL);                             //避免调用进程还没结束就执行下一步导致程序出错  
	
	
	for (j = 0; j < 5000; j++)//orders j
	{
		for (k = 0; k < 1000; k++)//lineitme k
			if (customer[i].c_custkey == (orders[j].o_custkey % 100) && orders[j].o_orderkey == lineitme[k].l_orderkey && (strcmp(orders[j].odate, order_date) < 0) && (strcmp(lineitme[k].sdate, ship_date) > 0))
			{
				result1[l].l_orderkey = lineitme[k].l_orderkey;
				strcpy(result1[l].o_orderdate, orders[j].odate);
				result1[l].l_extendedprice = lineitme[k].l_price;
				l++;

			}
	}

	//	}

	for (i = 0; i < l; i++)
	{
		if (i == 0)
		{
			result2[m].l_orderkey = result1[i].l_orderkey;
			strcpy(result2[m].o_orderdate, result1[i].o_orderdate);
			result2[m].l_extendedprice = result1[i].l_extendedprice;
			continue;
		}
		if (result1[i].l_orderkey == result1[i - 1].l_orderkey)
		{
			result2[m].l_extendedprice = result2[m].l_extendedprice + result1[i].l_extendedprice;

		}
		else
		{

			m++;
			result2[m].l_orderkey = result1[i].l_orderkey;
			strcpy(result2[m].o_orderdate, result1[i].o_orderdate);
			result2[m].l_extendedprice = result1[i].l_extendedprice;

		}

	}
	//printf("%s",order_date);
	//printf("%-10d|%-11s|%-20.2lf\n",result2[0].l_orderkey,result2[0].o_orderdate,result2[0].l_extendedprice);
	printf("l_orderkey|o_orderdate|revenue\n");
	//q=0;
	//printf("%d\n",q);
	//printf("%d",atoi(argv[8+4*q]));
	for (i = 0; i < atoi(argvs[8 + 4 * q]); i++)
	{

		if (result2[i].l_orderkey == 0)
		{
			continue;
		}
		else
		{
			printf("%-10d|%-11s|%-20.2lf\n", result2[i].l_orderkey, result2[i].o_orderdate, result2[i].l_extendedprice);
		}

	}
}

int main(int argc, char** argv)
{
	argvs = argv;
	int  q, p;
	int i;
	void* r;
	p = atoi(argv[4]);
	//printf("%d ", p);
	pthread_t* pthreads = (pthread_t*)malloc(sizeof(pthread_t) * p);
	for (q = 0; q < p; q++)
	{
		pthread_t d;
		pthreads[q] = d;
		
		int errorCode = pthread_create(&d, NULL, thread, (void*)& q);
		if (errorCode)
		{
			printf("create pthread faild code:%d\n", errorCode);
			continue;
		}
		pthread_join(pthreads[q], &r);
	}
	return 0;
}
