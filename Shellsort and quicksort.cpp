//two types of sorting
#include<stdio.h>
void shellsort(int a[],int n)
{
	int gap , i , j , temp;
	for(gap = n/2; gap > 0; gap /= 2)//gap 就是分出来的组数 
		for(i = gap; i < n; i++)
			for(j = i - gap;j >= 0 && a[j] > a[j+gap]; j -= gap)//组内插入排序 
			{
				temp = a[j];
				a[j] = a[j+gap];
				a[j+gap] = temp;
			}
}
void swap(int a[], int i, int j)
{
	int temp;
	temp = a[i];
	a[i] = a[j];
	a[j] = temp;	
}
void qsort(int a[], int left, int right)// quicksort
	int i, last;
	if(left >= right)
		return;
	swap(a, left, (left + right) /2); //中间量移到最左 
	last = left;
	for(i = left+1;i <= right;i++)
		if(a[i] < a[left])
			swap(a, ++last, i); //保证last后的数一定大于等于a[left] ，last跟踪小于a[left]的数，使其从a[left+1]顺次排序 
	swap(a, left , last);//将二者互换，左边和右边以之间的中间量为界分为两组 
	qsort(a, left, last-1);
	qsort(a, last+1, right);
	
 } 
int main()
{
	int n,a[100];
	scanf("%d",&n);
	int i=0;
	for(; i < n; i++)
		scanf("%d", &a[i]);
	qsort(a, 0, n-1);
	//shellsort(a,n);
	for(i = 0; i < n; i++)
		printf("%d ", a[i]);
}
