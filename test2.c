#include<stdio.h>

typedef struct{
    int data;
    int pos;
}Stack;

long largestRectangle(int h_count, int* h) {
    int i, j, k;
    long max = 0;
    Stack inc[100001];
    inc[0].data = 0;
    inc[0].pos = -1;
    int size = 1;
    int flag = 0;
   
    for(i = 0; i < h_count; i++)
    {
        while(h[i] < inc[size-1].data)
        {
            long result = (long)(i - inc[size-2].pos-1)*inc[size-1].data;
            if(result > max)
                max = result;
            size--;
        }
        inc[size].data = h[i];
        inc[size].pos = i;
        size++;
    }
    while(size > 1)
    {
        size--;
        long result = (long)inc[size].data * (h_count - inc[size-1].pos-1);
        if(result > max)
            max = result;
    }
    return max;

}

int main()
{
	int h[] = {11,11,10,10,10};
	printf("%ld", largestRectangle(5, h));
}
