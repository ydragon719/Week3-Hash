#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define MaxTableSize 10 /*散列表的最大长度*/
typedef char ElemType; /*关键词的数据类型为字符串型*/
typedef int Index; /*散列表的地址类型为整型*/
typedef int Position; /*数据所在的位置类型与散列表的地址类型一致*/

/*散列表中单元格的状态类型,Legitimate为有合法元素;Empty为空单元格;Deleted为有已删除元素*/
typedef enum {
	Legitimate, Empty, Deleted
} EntryType; 

/*散列表单元格的存储结构*/
typedef struct HashEntry Cell;
struct HashEntry {
	ElemType Data[15]; /*单元格存放的元素*/
	EntryType Info; /*单元格的状态*/
};

/*散列表的存储结构*/
typedef struct TableNode *HashTable;
struct TableNode {
	Cell *Cells; /*存放散列表数据的数组*/
	int TableSize; /*散列表的最大长度*/
};

/*函数声明*/
int NextPrime(int N); /*返回大于N且不超过MaxTableSize的最小素数*/
HashTable CreateTable(int TableSize); /*创建并初始化散列表*/
void BuildHashTable(HashTable H); /*构建散列表*/
void PrintHashTable(HashTable H); /*输出散列表*/
int Hash(ElemType Data[], int TableSize); /*散列函数*/ 
Position SquareFind(HashTable H, ElemType Data[]); /*平方探测查找*/
bool Insert(HashTable H, ElemType Data[]); /*散列表的插入操作*/
bool Delete(HashTable H); /*散列表的删除操作*/
Position UserSquareFind(HashTable H, char Data[]); /*用户使用平方探测法查找*/

int main(int argc, char const *argv[]) 
{
	char data[15];
	bool deleteResult;
	Position findPos;
	
	HashTable H=CreateTable(MaxTableSize); /*初始化一个空的散列表*/ 
	//printf("H->TableSize=%d\n", H->TableSize);
	BuildHashTable(H); /*构建散列表*/
	PrintHashTable(H);
	
	printf("\n请输入要查找的元素:");
	gets(data);
	findPos=UserSquareFind(H, data);
	if (findPos!=-1) {
		printf("查找元素:");
		puts(data);
		printf("散列地址:%d\n", findPos);
	}
	printf("\n请输入要插入的元素:");
	gets(data);
	Insert(H, data);
	
	deleteResult=Delete(H);
	if (deleteResult) {
		printf("删除成功!");
		PrintHashTable(H); 
	}
	
	return 0;
}

/*返回大于N且不超过MaxTableSize的最小素数*/
int NextPrime(int N) 
{
	int i, p;
	
	p=(N%2)? N+2: N+1; /*获得大于N的最小奇数*/
	
	//找出从p开始的下一个素数 
	while (p<=MaxTableSize) {
		for (i=p-1; i>2; i--) {
			//如果p不是素数,break; 
			if (!(p%i)) {
				break;
			}
		}
		//如果是素数
		if (i==2) {
			break;
		} else {
			//探测下一个奇数是否是素数 
			p+=2;
		}
	}	
}

/*创建始化散列表*/
HashTable CreateTable(int TableSize) 
{
	int i=0;
	HashTable H;
	
	H=(HashTable)malloc(sizeof(struct TableNode));
	H->TableSize=NextPrime(TableSize); /*使得散列表的最大长度是素数*/ 
	H->Cells=(Cell*)malloc(H->TableSize*sizeof(Cell));
	
	/*初始化每一个单元格的状态为空*/
	for (i=0; i<H->TableSize; i++) {
		H->Cells[i].Info=Empty;
	}
	
	return H;
}

/*构建散列表*/
void BuildHashTable(HashTable H) 
{	
	printf("\n请创建散列表:\n"); 
	while (1) {
		char i[15];
		gets(i);
		if (strcmp(i, "finish")!=0) {
			Insert(H, i);
		} else {
			break;
		}
	} 
	
	return;
} 

/*输出散列表*/
void PrintHashTable(HashTable H) 
{
	int i=0;
	float count=0; 
	
	printf("\n散列表为:\n");
	for (i=0; i<H->TableSize; i++) {
		if (H->Cells[i].Info==Legitimate) {
			printf("S[%d]：", i);
			puts(H->Cells[i].Data);
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
int Hash(ElemType Data[], int TableSize) 
{
	int Pos=Data[0]-'a'; //字符串首字母转换成数字 
	
	return (int)(Pos%TableSize);
}

/*平方探测查找*/
Position SquareFind(HashTable H, ElemType Data[]) 
{
	/*前者指示初始插入位置,后置指示实际插入位置*/
	Position currentPos, newPos;
	int collisionNum=0; /*记录发生冲突的次数*/
	int numCount=0;
	
	if (strcmp(Data, "-1")!=0) {
		/*根据散列函数找到该元素应该放的初始位置*/
		newPos=currentPos=Hash(Data, H->TableSize); 
		/*如果该位置的单元格非空,并且不是要找的元素时,即发生了冲突*/
		while (H->Cells[newPos].Info!=Empty && strcmp(H->Cells[newPos].Data, Data)!=0) {
			/*冲突次数+1*/
			collisionNum++;
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
		//puts(Data);
		printf("元素插入操作:\t散列地址=%d\t实际地址=%d\t冲突次数=%d,\n", currentPos, newPos, collisionNum);
	}	
	return newPos; /*此时的newPos位置是Data的位置,或者是一个空单元格的位置*/
} 

/*散列表的插入操作*/
bool Insert(HashTable H, ElemType Data[]) 
{
	Position Pos=SquareFind(H, Data); /*探测Data是否已存在散列表中*/
	
	if (strcmp(Data, "finish")!=0 && H->Cells[Pos].Info!=Legitimate) {
		/*如果这个单元格没有被占用,说明Data可以插入在这个Pos位置*/ 
		/*插入*/
		strcpy(H->Cells[Pos].Data, Data); /*字符串复制进去*/ 
		H->Cells[Pos].Info=Legitimate;
		return true;
	} else if (strcmp(Data, "finish")==0) {
		return;
	} else {
		printf("该元素已存在于散列表中.\n");
		return false;
	}
}

/*散列表的删除操作*/
bool Delete(HashTable H) 
{
	char i[15];
	
	printf("\n请输入要删除的元素：");
	gets(i);
	/*删除前查找元素是否存在*/
	Position Pos=UserSquareFind(H, i);
	
	if (strcmp(i, "finish")!=0 && H->Cells[Pos].Info!=Empty && strcmp(H->Cells[Pos].Data, i)==0) {
		/*如果这个单元格元素不空且等于要删除的元素*/
		/*修改这个单元格的状态为"删除"*/
		H->Cells[Pos].Info=Deleted;
		return true;
	} else {
		return false;
	}
}

/*用户使用线性探测查找*/
Position UserSquareFind(HashTable H, char Data[]) 
{
	Position currentPos, newPos; /*一个指示初始位置,一个指示新位置*/
	int collisionNum=0; /*记录发生冲突的次数*/
	int numCount=0;

	if (strcmp(Data, "finish")!=0) {
		/*根据散列函数找到该元素应该放的初始位置*/
		newPos=currentPos=Hash(Data, H->TableSize); 
		/*如果该位置的单元格非空,并且不是要找的元素时,即发生了冲突*/
		while (H->Cells[newPos].Info!=Empty && strcmp(H->Cells[newPos].Data, Data)!=0) {
			/*冲突次数+1*/
			collisionNum++;
			/*线性探测,增量+1*/
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
