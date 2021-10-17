#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MaxTableSize 10 /*散列表的最大长度*/
typedef int ElemType; /*关键词的数据类型为整型*/
typedef int Index; /*散列表的地址类型为整型*/
typedef int Position; /*数据的位置类型与散列表的地址类型一致*/ 

/*散列表中单元格的状态*/
typedef enum {
	Legitimate, Empty, Deleted
} EntryType;

/*散列表单元格的数据结构*/
typedef struct HashEntry Cell;
struct HashEntry {
	ElemType Data; /*单元格存放的元素*/
	EntryType Info; /*单元格的状态*/
};

/*散列表的存储结构*/
typedef struct TableNode *HashTable;
struct TableNode {
	Cell *Cells; /*存放散列表的数组*/
	int TableSize; /*散列表的最大长度*/
}; 

/*函数声明*/
int NextPrime(int N); /*返回大于N且不超过MaxTableSize的最小素数*/
HashTable CreateTable(int TableSize); /*初始化散列表*/
void BulidHashTable(HashTable H); /*构建散列表*/
void PrintHashTable(HashTable H); /*输出散列表*/
int Hash(ElemType Data, int TableSize); /*散列函数*/
Position SquareFind(HashTable H, ElemType Data); /*平方探测法*/
bool Insert(HashTable H, ElemType Data); /*散列表的插入操作*/
bool Delete(HashTable H); /*散列表的删除操作*/
Position UserSquareFind(HashTable H, ElemType Data); /*用户使用平方探测法查找*/

int main(int argc, char const *argv[]) 
{
	int i, data, findPos;
	bool result;
	
	HashTable H=CreateTable(MaxTableSize); /*初始化一个散列表*/
	printf("H->TableSize=%d\n", H->TableSize);
	BulidHashTable(H); /*构建散列表*/
	PrintHashTable(H); /*输出散列表*/
	
	printf("\n请输入要查找的元素：");
	scanf("%d", &data);
	findPos=UserSquareFind(H, data);
	if (data!=-1) {
		printf("查找元素:%d 散列地址%d", data, findPos);
	}
	printf("\n\n请输入要插入的元素：");
	scanf("%d", &data);
	Insert(H, data);
	
	result=Delete(H); /*删除一个元素*/
	if (result) {
		printf("删除成功!");
		PrintHashTable(H); 
	}
	
	return 0;
}
/*回大于N且不超过MaxTableSize的最小素数*/ 
int NextPrime(int N) 
{
	int i, p;
	
	p=(N%2)? N+2: N+1; /*获得大于N的最小奇数*/
	
	/*然后从这个最小奇数开始寻找下一个素数*/
	while (p<=MaxTableSize) {
		for (i=p-1; i>2; i--) {
			if (!(p%i)) {
				break;
			}
		}
		/*如果是素数*/
		if (i==2) {
			break;
		} else {
			p+=2; /*然测下一个奇数是否是素数*/
		}
	}
}

/*初始化散列表*/
HashTable CreateTable(int TableSize) 
{
	int i=0; 
	HashTable H;
	
	H=(HashTable)malloc(sizeof(struct TableNode));
	/*使得散列表的最大长度是素数*/
	H->TableSize=NextPrime(TableSize); 
	H->Cells=(Cell*)malloc(H->TableSize*sizeof(Cell));
	
	/*初始化散列表中每一个单元格的状态为空*/
	for (i=0; i<=H->TableSize; i++) {
		H->Cells[i].Info=Empty;
	} 
	
	return H;
}

/*构建散列表*/
void BulidHashTable(HashTable H) 
{
	int i=0; 
	
	printf("\n请创建散列表:");
	while (i!=-1) {
		scanf("%d", &i);
		Insert(H, i);
	}
	
	return;
}

/*输出散列表*/
void PrintHashTable(HashTable H) 
{
	int i=0;
	float count=0;
	
	printf("\n散列表为:\n"); 
	for(i=0; i<=H->TableSize; i++) {
		if (H->Cells[i].Info==Legitimate) {
			printf("S[%d]：", i);
			printf("%d", H->Cells[i].Data);
			count++;
		} else if (H->Cells[i].Info==Empty) {
			printf("S[%d]：", i);
		} else {
			printf("S[%d]：单元格状态：Deleted", i);
		}
		printf("\n");
	}
	printf("装填因子=%f\n", count/H->TableSize);
	
	return;
}

/*散列函数*/
int Hash(ElemType Data, int TableSize) 
{
	return (int)(Data%TableSize);
}

/*平方探测法*/
Position SquareFind(HashTable H, ElemType Data) 
{
	/*前者指示初始插入位置,后置指示实际插入位置*/
	Position currentPos, newPos;
	int collisionNum=0; /*记录发生冲突的次数*/
	int numCount=0;
	
	if (Data!=-1) {
		/*首先根据散列函数计算出插入位置*/
		newPos=currentPos=Hash(Data, H->TableSize);
		/*得到初始插入位置后,判断该位置是否为空或者是否发生冲突*/
		while (H->Cells[newPos].Info!=Empty && H->Cells[newPos].Data!=Data) {
			collisionNum++;
			/*如果是奇数次操作,也就是正数的平方探测*/
			if (++numCount%2) {
				/*12 -12 22 -22 32 -32 42 -42*/
				/*1   2  3   4  5   6  7   8*/ 
				newPos=currentPos+(numCount+1)*(numCount+1)/4;
				/*判断插入位置是否有越界,有就调整*/
				if (newPos>=H->TableSize) {
					newPos=newPos%H->TableSize;
				} 
			} else {
				/*如果是偶数次操作,也就是负数的平方探测*/
				newPos=currentPos-numCount*numCount/4;
				/*下标越界判断*/
				while (newPos<0) {
					newPos+=H->TableSize;
				}
			}
		} 
		printf("元素%d插入操作:\t散列地址=%d\t实际地址=%d\t冲突次数=%d\n", Data,currentPos, newPos, collisionNum);
	} 
	
	return newPos;
}

/*散列表的插入操作*/
bool Insert(HashTable H, ElemType Data) 
{
	/*插入前先探测插入位置*/
	Position Pos=SquareFind(H, Data);
	
	if (Data!=-1 && H->Cells[Pos].Info!=Legitimate) {
		/*如果这个单元格没有被占用,说明Data可以插入到这个Pos位置*/
		/*插入*/
		H->Cells[Pos].Data=Data;
		H->Cells[Pos].Info=Legitimate;
		return true; 
	} else if (Data==-1) {
		return false;
	}
}

/*散列表的删除操作*/
bool Delete(HashTable H) 
{
	ElemType Data;
	
	printf("\n请输入要删除的元素：");
	scanf("%d", &Data);
	/*删除前查找元素是否存在*/
	Position Pos=UserSquareFind(H, Data);
	
	if (Data!=-1 && H->Cells[Pos].Info!=Empty && H->Cells[Pos].Data==Data) {
		/*如果这个单元格元素不空且等于要删除的元素*/
		/*修改这个单元格的状态为"删除"*/
		H->Cells[Pos].Info=Deleted;
		return true;
	} else {
		return false;
	}
}

/*用户使用平方探测法查找*/
Position UserSquareFind(HashTable H, ElemType Data) 
{
	/*前者指示初始插入位置,后置指示实际插入位置*/
	Position currentPos, newPos;
	int collisionNum=0; /*记录发生冲突的次数*/
	int numCount=0;
	
	if (Data!=-1) {
		/*首先根据散列函数计算出插入位置*/
		newPos=currentPos=Hash(Data, H->TableSize);
		/*得到初始插入位置后,判断该位置是否为空或者是否发生冲突*/
		while (H->Cells[newPos].Info!=Empty && H->Cells[newPos].Data!=Data) {
			collisionNum++;
			/*如果是奇数次操作,也就是正数的平方探测*/
			if (++numCount%2) {
				/*12 -12 22 -22 32 -32 42 -42*/
				/*1   2  3   4  5   6  7   8*/ 
				newPos=currentPos+(numCount+1)*(numCount+1)/4;
				/*判断插入位置是否有越界,有就调整*/
				if (newPos>=H->TableSize) {
					newPos=newPos%H->TableSize;
				} 
			} else {
				/*如果是偶数次操作,也就是负数的平方探测*/
				newPos=currentPos-numCount*numCount/4;
				/*下标越界判断*/
				while (newPos<0) {
					newPos+=H->TableSize;
				}
			}
		} 
	} 
	
	return newPos;
}

