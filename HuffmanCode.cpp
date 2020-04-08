/*********************************************
@Author			CTZ
@Date			2020-04-02
@Description 

Huffman编码
1.txt文本扫描，统计字符出现次数
2.计算每个字符出现的频率，存储在权值表中
3.压缩并按递增序排列权值表
4.根据权值表构造森林，通过合并最小权值树构造Huffman树
5.遍历树生成Huffman编码，保存在链表中
5.将txt文本另存为Huffman编码的文件
*********************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ASCII_CHAR_COUNT_SIZE 128
#define WEIGHTS_SIZE 128
//#define INPUT_TEXT_PATH "./usconstitution.txt"
#define INPUT_TEXT_PATH "./inputText.txt"
//#define INPUT_TEXT_PATH "./inputText1.txt"
#define OUTPUT_HUFFMAN_TEXT_PATH "./outputHuffmanText.txt"
#define OUTPUT_COMPARE_TEXT_PATH "./outputCompareText.txt"
#define OUTPUT_HUFFMAN_CODE_TABLE_PATH "./outputHuffmanCodeTable.txt"
#define OUTPUT_HUFFMAN_FILE_PATH "./outputHuffmanFile.bin"
#define OUTPUT_WEIGHT_TABLE_PATH "./outputWeightTable.txt"
#define KILO_BYTES 1024

/********************************************
	压缩后权值表定义，用单链表表示
********************************************/

typedef struct LinkListNode
{
	char			key;
	float			value;
	LinkListNode	*next;
}*LinkList;

/*
@description : 创建一个带头结点的链表

@return 带头结点的链表
*/
LinkList createLinkList()
{
	LinkListNode *node = (LinkListNode*)malloc(sizeof(LinkListNode));
	node->key = 'n';	// 头结点的key，key值可能会重复
	node->value = 0;	// 记录链表长度
	node->next = NULL;
	return node;
}

/*
@description : 按照递增序插入一个结点

@intput param L : 链表

@input param key : 新结点的key

@input param value : 新结点的value
*/
void insertLinkList(LinkList L, char key, float value)
{
	if(L == NULL) return;
	LinkListNode *pCur = L->next;
	LinkListNode *pPrev = L;

	// 创建新结点
	LinkListNode *pNew = (LinkListNode*)malloc(sizeof(LinkListNode));
	pNew->key = key;
	pNew->value = value;
	pNew->next = NULL;

	// 空链表插入结点
	if(L->value == 0)
	{
		L->next = pNew;
		L->value++;	// 更新链表长度
		return;
	}

	// 跳过头结点，找到合适位置插入（递增序）
	for(int i=1; i<=L->value && pCur!=NULL; ++i)
	{
		if(pNew->value <= pCur->value)			
			break;
		pCur=pCur->next;
		pPrev=pPrev->next;
	}

	// 插入
	pNew->next = pCur;
	pPrev->next = pNew;

	// 头结点更新链表长度
	if(L->key == 'n')
		L->value++;
}
/*
@description : 打印链表

@intput param L : 链表
*/
void printLinkList(LinkList L)
{
	FILE *out = NULL;
	out = fopen(OUTPUT_WEIGHT_TABLE_PATH, "w");
	if(L == NULL) return;
	LinkListNode *pCur = L->next;

	printf("\n-----------------------------链表---------------------------------------------\n");
	if(L->key == 'n')	
		printf("链表长度 : %f\n", L->value);
	int printCounter=0;
	for(int i=1; i<=L->value && pCur != NULL; ++i)
	{
		printf("(key : %2c , value : %f)\n", pCur->key, pCur->value);
		fprintf(out, "key : %2c , value : %f\n", pCur->key, pCur->value);
		printf("|\n");
		printf("v\n");
		pCur=pCur->next;
		printCounter++;
	}
	printf("NULL\n");
	printf("\nprintCounter : %d\n", printCounter);
	printf("\n------------------------------------------------------------------------------\n");
	if(out != NULL)
		fclose(out);
}

/********************************************
	Huffman树定义，用二叉链表表示
********************************************/
typedef struct TreeNode
{
	char		ch;
	float		weight;
	TreeNode	*lChild;
	TreeNode	*rChild;
}*HuffmanTree;

/*
@description : 根据权值创建只有一个结点的Huffman树

@return Huffman树
*/
HuffmanTree createHuffmanTree(char ch, float weight)
{
	TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode*));
	node->ch = ch;
	node->weight = weight;
	node->lChild = NULL;
	node->rChild = NULL;
	return node;
}


/*
@description : 根据两颗Huffman树合并成一个新的Huffman树

@return Huffman树
*/
HuffmanTree mergeHuffmanTree(HuffmanTree leftTree, HuffmanTree rightTree)
{
	TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode*));
	node->lChild = leftTree;
	node->rChild = rightTree;
	node->ch = -1;	// 表示非叶子结点
	node->weight = leftTree->weight + rightTree->weight;
	return node;
}

/*
@description : 根据压缩后的权值表创建一个新的Huffman树

@return Huffman树

HuffmanTree createHuffmanTree(LinkList compressedWeights)
{
	LinkListNode *pCur = compressedWeights->next;	// 跳过头结点

	for(int i=1; i<=compressedWeights->value; ++i)
	{
		if(pCur != NULL)
		{
			HuffmanTree tree = createHuffmanTree(pCur->value);
			pCur = pCur->next;
		}
	}
	return NULL; // delete
}*/


/*
@description : 中序遍历打印Huffman树
*/
void printHuffmanTree(HuffmanTree tree)
{
	if(tree->lChild)
		printHuffmanTree(tree->lChild);

	if(tree)
		if(tree->ch == -1)
			printf("[]");
		else
			printf("(%2c,%4f)", tree->ch,tree->weight);
    if(tree->rChild) 
		printHuffmanTree(tree->rChild);
}




/********************************************
	森林的定义，用单链表表示
********************************************/
typedef struct ForestNode
{
	HuffmanTree tree;
	ForestNode *next;	// 指向下一颗树
	
}*Forest;

/*
@description : 创建一个带头结点的森林（链表）

@return 带头结点的森林（链表）
*/
Forest createForest()
{
	// 创建森林的头结点ForestNode，该结点中的树的weight用于存放森林的大小
	HuffmanTree headTree = (TreeNode*)malloc(sizeof(TreeNode));
	headTree->lChild=NULL;
	headTree->rChild=NULL;
	headTree->ch = 'n';
	headTree->weight = 0; // 存放森林大小

	ForestNode *head = (ForestNode*)malloc(sizeof(ForestNode));
	head->tree = headTree;
	head->next = NULL;
	return head;
}


/*
@description : 将树加入森林

@input param forest : 森林

@input param tree : 树
*/
void insertForest(Forest forest, HuffmanTree tree)
{
	if(forest == NULL) return;
	ForestNode *pCur = forest->next;
	ForestNode *pPrev = forest;

	// 创建新结点
	ForestNode *pNew = (ForestNode*)malloc(sizeof(ForestNode));
	pNew->tree = tree;
	pNew->next = NULL;

	// 空森林插入结点
	if(forest->tree->weight == 0)
	{
		forest->next = pNew;
		forest->tree->weight++; // 更新森林大小
		return;
	}

	// 跳过头结点，寻找一个合适的位置插入，使得树按权值递增排列
	for(int i=1; i<=forest->tree->weight && pCur!=NULL; ++i)
	{
		if(pNew->tree->weight <= pCur->tree->weight)
			break;
		pCur=pCur->next;
		pPrev=pPrev->next;
	}
	// 插入
	pNew->next = pCur;
	pPrev->next = pNew;

	// 头结点更新链表长度
	if(forest->tree->ch == 'n')
		forest->tree->weight++;
}




/*
@description : 遍历压缩后权值表，创建单个结点的Huffman树，并加入森林

@input param forest : 森林

@input param compressedWeights : 压缩后权值表
*/
void initialForest(Forest forest, LinkList compressedWeights)
{
	LinkListNode *pCur = compressedWeights->next;	// 跳过头结点
	for(int i=1; i<=compressedWeights->value; ++i)
	{
		if(pCur != NULL)
		{
			HuffmanTree tree = createHuffmanTree(pCur->key, pCur->value); // 创建单个带权值结点的Huffman树
			// 将这棵树加入森林
			insertForest(forest, tree);
			pCur = pCur->next;
		}
	}
}



/*
@description : 将树从森林中删除

@input param forest : 森林

@input param tree : 树
*/
void deleteFromForest(Forest forest, HuffmanTree tree)
{
	if(forest==NULL || tree==NULL) return;
	// 如果森林中只剩一颗树，且该树正好是要删除的
	if(forest->tree->weight == 1 && forest->next->tree == tree) 
		forest->next = NULL;
	else if(forest->tree->weight == 1 && forest->next->tree != tree) 
		return;

	// 跳过头结点，遍历森林
	ForestNode *pCur = forest->next;
	ForestNode *pPrev = forest;
	for(int i=1; i<=forest->tree->weight && pCur != NULL; ++i)
	{
		// 将这颗树从森林中删除
		if(pCur->tree == tree)
		{
			pPrev->next = pCur->next;
			pCur->next = NULL;

			// 更新森林大小
			forest->tree->weight--;
			return;
		}
		pPrev = pPrev->next;
		pCur = pCur->next;
	}
	return;
}
/********************************************
	字符串操作定义
********************************************/

typedef struct MyString
{
	char	*s;			// 字符串
	int		size;		// 字符串长度
	int		maxSize;	// 字符串最大容量
}*String;

/*
@description 创建一个字符串

@return 新字符串
*/
String createString()
{
	String str = (MyString*)malloc(sizeof(MyString));
	str->s = (char*)malloc(sizeof(char));
	str->s[0] = '\0';
	str->size = 0;
	str->maxSize = 1;

	return str;
}

/*
@description 根据字符串创建新的字符串

@input param ss : 字符串

@return 新字符串
*/
String createString(String ss)
{
	if(ss == NULL) return NULL;
	String str = (MyString*)malloc(sizeof(MyString));
	// 深拷贝
	str->s = (char*)malloc(sizeof(char) * ss->maxSize);
	memcpy(str->s, ss->s, sizeof(char) * ss->maxSize);

	str->size = ss->size;
	str->maxSize = ss->maxSize;

	return str;
}

/*
@description 创建一个指定最大容量的字符串，并用字符串ss的内容初始化

@input param maxSize : 新字符串的最大容量

@input param ss : 字符串

@return 新字符串
*/
String createString(int maxSize, String ss)
{
	String str = (MyString*)malloc(sizeof(MyString));
	str->s = (char*)malloc(sizeof(char) * maxSize);
	
	// 用ss的内容初始化
	memcpy(str->s, ss->s, sizeof(char) * ss->maxSize);

	str->size = ss->size;
	str->maxSize = maxSize;

	return str;
}


/*
@description 添加一个字符到char*字符串的尾部

@input param str : 字符串

@input param ch : 字符

@return String字符串
*/
String appendChar2String(String str, char ch)
{
	if(str == NULL) return NULL;
	int strCurmaxSize = str->size + 1;	// 字符串占用的内容空间, +1因为还有字符串尾部还有一个'\0'

	if(strCurmaxSize + 1 > str->maxSize)	// 若添加字符后，超出字符串分配的内存空间，则将内存空间扩大2倍
	{
		/*
		str->maxSize *= 2;
		// 申请新的内存空间并将原字符串内容拷贝到新字符串上
		//char *newStr = (char*)malloc(sizeof(char) * strmaxSize);
		//memcpy(newStr, str, strSize);
		String newStr = (MyString*)malloc(sizeof(MyString));
		// 深拷贝
		newStr->str = (char*)malloc(sizeof(char) * str->maxSize);
		memcpy(newStr->s, str->s, str->size);

		newStr->size = str->size;
		newStr->maxSize = str->maxSize;

		// 释放原字符串内存空间
		free(str->s);
		free(str);
		
		// 重新指向新的内存空间
		str = newStr;
		*/

		// 创建一个新的字符串，且内存空间是原来的2倍
		String newStr = createString(str->maxSize*2, str);
		
		// 释放原字符串内存空间
	//	free(str->s);
		if(str)
			free(str);

		// str指向新的内存空间
		str = newStr;
	}
	
	// 添加字符
	if(strCurmaxSize < str->maxSize)
	{
		str->s[str->size] = ch;
		str->s[str->size+1] = '\0';

		// 更新字符串长度
		str->size += 1;
	}

		
	return str;
}

/*
@description 删除char*字符串的最后一个字符

@input param str : 字符串

@input param ch : 字符

*/
void removeCharFromString(String str, char ch)
{
	if(str->size <= 0 || str->maxSize <=0)	return;

	
	if(str->s)
	{
		// 删除最后一个字符
		str->s[str->size-1] = str->s[str->size];
		// 更新字符串长度
		str->size -= 1;
	}
}


/********************************************
	Huffman编码定义，用单链表表示
********************************************/
/*
typedef struct HuffmanCodeNode
{
	char			ch;			// 字符
	char			*code;		// 字符对应的编码 / 头结点该字段不用
	int				codeSize;	// 编码长度 / 头结点该字段表示HuffmanCode链表的长度
	HuffmanCodeNode *next;	
	
}*HuffmanCode;
*/

typedef struct HuffmanCodeNode
{
	char			ch;			// 字符
	String			code;		// 字符对应的编码（自定义字符串表示） / 头结点的code->size用于表示HuffmanCode链表的长度
	HuffmanCodeNode *next;	
	
}*HuffmanCode;


/*
@description 根据压缩后权值表，构造HuffmanCode链表（生成一个头结点）

@input param compreesedWeights : 压缩后权值表

@return HuffmanCode链表
*/
HuffmanCode createHuffmanCode(LinkList compressedWeights)
{
	HuffmanCodeNode *node = (HuffmanCodeNode*)malloc(sizeof(HuffmanCodeNode));
	node->ch = 'n';
	node->code = createString();
	node->code->size = 0;	// 头结点的该字段表示链表长度
	node->next = NULL;
	return node;
}

/*
@description 插入一个结点到HuffmanCode链表

@input param list : HuffmanCode链表

@input param ch : 字符

@input param code : 字符对应的Huffman编码
*/
void insertHuffmanCode(HuffmanCode list, char ch, String code)
{
	HuffmanCodeNode *pCur = list->next; // 跳过头结点

	// 生成新结点
	HuffmanCodeNode *pNew = (HuffmanCodeNode*)malloc(sizeof(HuffmanCodeNode));
	pNew->ch = ch;

	// 深拷贝
	pNew->code = createString(code);
	pNew->next = NULL;

	// 头插法
	pNew->next = list->next;
	list->next = pNew;

	// 更新链表长度
	list->code->size++;
}

/*
@description 打印HuffmanCode链表

@input param list : HuffmanCode链表
*/
void printHuffmanCode(HuffmanCode list)
{
	if(list == NULL) return;
	HuffmanCodeNode *pCur = list->next;	// 跳过头结点
	printf("\n-----------------------------HuffmanCode链表------------------------------------\n");
	printf("链表长度 : %d\n", list->code->size);
	int printCount = 0;
	for(int i=1; i<=list->code->size; ++i)
	{
		if(pCur)
		{
			printf("(");
			printf("%c", pCur->ch);
			printf(",");
			printf("%s", pCur->code->s);			
			printf(")");
			printf("\n");
			printf("|\n");
			printf("v\n");
			printCount++;
			pCur = pCur->next;
		}
	}
	printf("NULL\n");
	printf("printCount : %d\n", printCount);
	printf("\n--------------------------------------------------------------------------------\n");

}

/*
@description HuffmanCode按照链表存放顺序依次写入文件

@input param list : HuffmanCode链表

@input param outputFilePath : 输出文件路径
*/
void saveHuffmanCode(HuffmanCode list, char *outputFilePath)
{
	FILE *out = NULL;
	out = fopen(outputFilePath, "w");

	fprintf(out, "total char : %d\n", list->code->size);
	fprintf(out, "char : HuffmanCode\n");

	HuffmanCodeNode *pCur = list->next;
	for(int i=1; i<=list->code->size; ++i)
	{
		fprintf(out, "%c : %s\n", pCur->ch, pCur->code->s);
		pCur = pCur->next;
	}
	fclose(out);
}




/********************************************
	函数定义
********************************************/


/*
@description : 读取文本并统计所有字符的个数，以及每种ASCII字符出现的次数

@input param asciiCharCount : ASCII字符统计表，统计每种ASCII字符出现次数

@input param size : asciiCharCount数组的大小

@return 文本中所有字符的个数
*/
int readAndCount(int *asciiCharCount, int size)
{
	FILE *in = NULL;
	char buf[KILO_BYTES];
 
	in = fopen(INPUT_TEXT_PATH, "r");

	int readTotalCount = 0; // 记录文本中所有字符的个数
	// 每次读取1024字节
	while (fgets(buf, sizeof(buf), in) != NULL)
	{
		printf("%s", buf);
		// 统计每种ASCII字符出现次数
		char *pCur = buf;
		for(int i=0; i<sizeof(buf) && *pCur != '\0'; ++i)
		{
			if(*pCur >=0 && *pCur <=128)
			{
				asciiCharCount[*pCur]++;
				readTotalCount++;
			}
			pCur++;
			
		}
	}
    fclose(in);


	printf("\n读取文本中的字符总数 : %d\n",readTotalCount);
    return readTotalCount;
}


/*
@description : 打印ASCII字符统计表

@intput param asciiCharCount : ASCII字符统计表，统计每种ASCII字符出现次数

@input param size : asciiCharCount数组的大小
*/
void printAsciiCharCount(int *asciiCharCount, int size)
{
	int *pCur = asciiCharCount;
	printf("\n---------------------------ASCII字符统计表--------------------------------------\n");
	for(int i=0; i<size; ++i)
	{
		if(i==0)
			printf("|");
		if(i!=0 && i%4==0)
		{
			printf("\n");
			printf("|");
		}
		printf("%4c : %4d  |", i, *pCur);

		pCur++;
	}
	printf("\n--------------------------------------------------------------------------------\n");
}


/*
@description : 计算权值表（每种字符在文本中出现的概率 = 某种字符出现次数 / 总字符数）

@input param asciiCharCount : ASCII字符统计表，统计每种ASCII字符出现次数

@input param size : asciiCharCount数组的大小

@input param totalCharCount : 文本所有字符个数

@output param weights : 权值表
*/
void getWeights(int *asciiCharCount, int size, int totalCharCount, float *weights)
{
	//int *pCur = asciiCharCount;	// *pCur = 当前指向的ASCII字符出现次数
	for(int i=0; i<size; ++i)
	{
		weights[i] = (asciiCharCount[i] * 1.0f / totalCharCount);
	}
}

/*
@description : 打印权值表（每种字符在文本中出现的概率 = 某种字符出现次数 / 总字符数）

@input param weights : 权值表

@input param size : 权值表大小
*/
void printWeights(float *weights, int size)
{
	float totalWeights = (float)0.0;
	printf("\n-------------------------------权值表----------------------------------------\n");
	for(int i=0; i<size; ++i)
	{
		if(i==0)
			printf("|");
		if(i!=0 && i%4==0)
		{
			printf("\n");
			printf("|");
		}
		printf("%4c : %4f  |", i, weights[i]);
		totalWeights += weights[i];
	}
	printf("\n------------------------------------------------------------------------------\n");
	printf("总的权值 : %f\n",totalWeights);
}


/*
@description : 压缩权值表（去掉为0的权值）并递增排序

@input param weights : 权值表

@input param size : 权值表大小

@input param compressedWeights : 待压缩的空权值表（链表）

@output param compressedWeights : 压缩并排序后的权值表（链表）
*/
void getCompressedWeights(float *weights, int size, LinkList compressedWeights)
{
	for(int i=0; i<size; ++i)
	{
		// 插入链表中
		if(weights[i] != 0.0f)
			insertLinkList(compressedWeights, i, weights[i]);
	}
}

/*
@description : 构造Huffman树

@input param forest : 森林

@input param compressedWeights : 压缩后权值表

@return Huffman树
*/
HuffmanTree getHuffmanTree(Forest forest, LinkList compressedWeights)
{
	initialForest(forest, compressedWeights);	// 第一轮

	float round = forest->tree->weight;
	// 经过round轮即可构造出一颗Huffman树
	for(float i=2; i<=round; ++i)
	{
		// 挑森林（递增排序）中最前面的两棵树构造新的Huffman树，并加入到森林中 
		if(forest->tree->weight >=2 )
		{
			ForestNode *pFirstNode = forest->next;
			ForestNode *pSecondNode = forest->next->next;

			// 根据两颗权值最小的树，构造新的树
			HuffmanTree newTree = mergeHuffmanTree(pFirstNode->tree, pSecondNode->tree);
			
			// 从森林中删除这两颗树
			deleteFromForest(forest, pFirstNode->tree);
			deleteFromForest(forest, pSecondNode->tree);

			// 将新树加入到森林
			insertForest(forest, newTree);
		}
	}
	// 最后森林中只有一棵树，此时Huffman树构造完成
	return forest->next->tree;
}


/*
@description : 中序遍历Huffman树,计算Huffman编码

@input param tree : Huffman树

@input param code : 自定义字符串表示的编码

@output param list : Huffman编码链表

@return code : 编码的内存空间在遍历过程中可能会因为扩容而改变，需要动态修改code指针的指向
*/
String getHuffmanCode(HuffmanTree tree, String code, HuffmanCode list)
{
	if(tree->lChild)
	{
		// code + '0'
		code = appendChar2String(code, '0');

		code = getHuffmanCode(tree->lChild, code, list);

		// code - '0'
		removeCharFromString(code, '0');
	}
	if(tree)
	{
		if(tree->ch == -1)	// ch == -1表示非叶子结点
			printf("[]");
		else // 叶子结点
		{
			printf("(%2c,%4f)", tree->ch,tree->weight);
			// code -> list
			insertHuffmanCode(list, tree->ch, code);
		}
	}		
    if(tree->rChild) 
	{
		// code + '1'
		code = appendChar2String(code, '1');

		code = getHuffmanCode(tree->rChild, code, list);
		// code - '1'
		removeCharFromString(code, '1');
	}
	return code;

}


/*
@description 分别按照ASCII码和Huffman编码打印指定文件中的文本，并写入文件

@input param list : HuffmanCode链表

@input param inputTextPath : 输入的文本路径（输入ASCII文本）

@input param outputTextPath : 输出的文本路径（输出ASCII和Huffman的对比文本）

@input param outputText2Path : 输出的文本路径（输出Huffman文本）
*/
void printText(HuffmanCode list, char *inputTextPath, char *outputCompareTextPath, char *outputHuffmanTextPath)
{
	FILE *in = NULL;
	char buf[KILO_BYTES];
	in = fopen(inputTextPath, "r");


	FILE *out = NULL;
	out = fopen(outputCompareTextPath, "w");

	FILE *out2 = NULL;
	out2 = fopen(outputHuffmanTextPath, "w");


	printf("ASCII码文本 : \n");
	fprintf(out, "ASCII码文本 : \n");

	// 每次读取1024字节
	while (fgets(buf, sizeof(buf), in) != NULL)
	{
		// ASCII码打印
		printf("%s", buf);
		fprintf(out, "%s", buf);
	}
	fclose(in);

	
	in = fopen(inputTextPath, "r");
	printf("\n\nHuffman编码文本 : \n");
	fprintf(out, "\n\nHuffman编码文本 : \n");
	// 每次读取1024字节
	while (fgets(buf, sizeof(buf), in) != NULL)
	{
		// 打印ASCII码字符对应的Huffman编码
		char *pCur = buf;
		for(int i=0; i<sizeof(buf) && *pCur != '\0'; ++i)
		{
			if(*pCur >=0 && *pCur <=128)
			{
				// 扫描Huffman编码表
				HuffmanCodeNode *pCurNode = list->next;
				while(pCurNode)
				{
					if(pCurNode->ch == *pCur)
					{
						printf("%s",pCurNode->code->s);
						fprintf(out, "%s", pCurNode->code->s);
						fprintf(out2, "%s", pCurNode->code->s);
						break;
					}

					pCurNode = pCurNode->next;
				}
			}
			pCur++;
			
		}
	}
	fclose(in);
	fclose(out);
	fclose(out2);
}

/********************************************
	MyChar的定义，方便位域操作
	Little-Endian存储：低地址存低位
	例如：存储"0001 0010"，在MyChar的存储为0100 1000
********************************************/
typedef struct MyChar
{
	unsigned char bit0:1;		// 低位
	unsigned char bit1:1;
	unsigned char bit2:1;
	unsigned char bit3:1;
	unsigned char bit4:1;
	unsigned char bit5:1;
	unsigned char bit6:1;
	unsigned char bit7:1;
}MyChar;


/*
@description 创建并初始化MyChar
*/
MyChar *createMyChar()
{	
	MyChar *ch = (MyChar*)malloc(sizeof(char));
	memset(ch, 0, sizeof(char));
	return ch;
}

/*
@description 销毁MyChar
*/
void deleteMyChar(MyChar *ch)
{	
	if(ch == NULL)	return;
	else
	{
		free(ch);
		ch = NULL;
	}
	return;
}

char MyChar2Char(MyChar* ch)
{
	char *tmp = (char*)malloc(sizeof(char));
	memcpy(tmp, ch, sizeof(MyChar));
	return *tmp;
}

/*
@description 设置MyChar的pos位的bit位

@input param ch : MyChar类型表示的1字节变量

@input param pos : 设置bit的位置(0-7)

@input param bit : 设置bit的值(0或1)
*/
void MyCharSetBit(MyChar *ch, int pos, char bit)
{
	if(pos < 0 || pos >7)	return;
	if(bit !=0 && bit !=1)	return;

	if(pos==0)
		bit == 0 ? ch->bit0 = 0 : ch->bit0 = 1;
	else if(pos==1)
		bit == 0 ? ch->bit1 = 0 : ch->bit1 = 1;
	else if(pos==2)
		bit == 0 ? ch->bit2 = 0 : ch->bit2 = 1;
	else if(pos==3)
		bit == 0 ? ch->bit3 = 0 : ch->bit3 = 1;
	else if(pos==4)
		bit == 0 ? ch->bit4 = 0 : ch->bit4 = 1;
	else if(pos==5)
		bit == 0 ? ch->bit5 = 0 : ch->bit5 = 1;
	else if(pos==6)
		bit == 0 ? ch->bit6 = 0 : ch->bit6 = 1;
	else if(pos==7)
		bit == 0 ? ch->bit7 = 0 : ch->bit7 = 1;
}



/*
@description 8字节字符串转换成1个8位的2进制位串

@input param eightByteString : 8字节字符串

@input param size : 字符串实际长度

@return 转换后的8位二进制位串
*/
char eightByteString2Char(char *eightByteString, int size)
{
	if(eightByteString == NULL)	return NULL;
	MyChar *bitStream = createMyChar();	// 设置为全0的二进制位串


	// 遍历字符串
	for(int i=0; i<size; ++i)
	{
		if( eightByteString[i] == '0' )
			MyCharSetBit(bitStream, 7-i, 0);	// 注意MyChar中按Little-Endian顺序存放位,7-i表示从后往前存
		else if(eightByteString[i] == '1' )
			MyCharSetBit(bitStream, 7-i, 1);
	}
	/*
	if(size!=8)// 例如，字符串"010101??"，有2位没有填充，实际长度不足8，用0填充
	{
		// 低位低地址填充
		int paddingSize = 8 - size;
		for(int i=0; i<paddingSize; ++i)
		{
			int pos = i;
			MyCharSetBit(bitStream, pos, 0);
		}
	}
	*/
	char result = MyChar2Char(bitStream);
	deleteMyChar(bitStream);
	return result;
}


/*
@description 按照Huffman编码将文本写入文件

@input param list : HuffmanCode链表

@input param huffmanTextPath : 输入的Huffman文本路径

@input param huffmanFilePath : 输出的Huffman二进制文件路径
*/
void saveAsHuffmanCodeFile(HuffmanCode list, char *huffmanTextPath, char *huffmanFilePath)
{
	FILE *in = NULL;
	char buf[8] = {0};
	in = fopen(huffmanTextPath, "r");

	FILE *out = NULL;
	out = fopen(huffmanFilePath, "wb");	// 写入二进制文件

	// 每次读取8字节的字符串（相当于读取8位的二进制编码）
	// 例如读取字符串"01010101"，相当于读取8位的二进制位串
	int readBytes = 8;	// 每次读取8字节
	int n = 0;			// 记录每次读取字节数
	int count = 0;		// 记录轮数
	//fseek(in,0,SEEK_SET); 
	while (n = (fread(buf, sizeof(char), readBytes, in))) // 文件内容为空时，n = 0
	{
	/*
	char ch='\0';
	while( (ch = getc(in)) != EOF)
	{
		buf[n++] = ch;
		if(n == readBytes)
		{
		*/
	
			// 8字节字符串转换成1字节二进制位串(char变量存储)
			char bitStream = eightByteString2Char(buf, n);
			fwrite(&bitStream, sizeof(char),1,out);
		//	n=0;
	//	}
	//	count++;
	}
	fclose(out);
	fclose(in);
}


/*
@description 按照Huffman编码将文本写入文件

@input param weights : 内存中权值表

@input param huffmanTextPath : Huffman文本（Huffman编码表示的字符串）路径
*/
void parseHuffmanCodeFile(LinkList weights, char * huffmanTextPath)
{

}



int main()
{
	// 存储128个ASCII码字符的计数器
	int *asciiCharCount = (int*)malloc(sizeof(int) * ASCII_CHAR_COUNT_SIZE);
	memset(asciiCharCount, 0, sizeof(int) * ASCII_CHAR_COUNT_SIZE);	// 数组初始化

	// 读取文本
	int totalCharCount = readAndCount(asciiCharCount, ASCII_CHAR_COUNT_SIZE);
	printAsciiCharCount(asciiCharCount, ASCII_CHAR_COUNT_SIZE);

	// 权值表初始化
	float *weights = (float*)malloc(sizeof(int) * WEIGHTS_SIZE);
	memset(weights, 0, sizeof(float) * WEIGHTS_SIZE);	// 数组初始化

	// 计算权值表
	getWeights(asciiCharCount, ASCII_CHAR_COUNT_SIZE, totalCharCount, weights);
	printWeights(weights, WEIGHTS_SIZE);


	// 压缩权值表并递增排序
	LinkList compressedWeights = NULL;
	compressedWeights = createLinkList();
	getCompressedWeights(weights, WEIGHTS_SIZE, compressedWeights);
	printf("-----------------------------压缩后权值表-----------------------------\n");
	printLinkList(compressedWeights);

	// 遍历压缩后权值表，构造Huffman树
	Forest forest = createForest();
	HuffmanTree tree = getHuffmanTree(forest, compressedWeights);
	printf("-----------------------------中序遍历的Huffman树-----------------------------\n");
	printHuffmanTree(tree);

	// 遍历Huffman树，计算Huffman编码
	HuffmanCode list = createHuffmanCode(compressedWeights);
	String code = createString();
	getHuffmanCode(tree, code, list);
	printHuffmanCode(list);
	saveHuffmanCode(list, OUTPUT_HUFFMAN_CODE_TABLE_PATH);

	// 根据Huffman编码表，打印Huffman编码表示的文本
	printText(list, INPUT_TEXT_PATH, OUTPUT_COMPARE_TEXT_PATH, OUTPUT_HUFFMAN_TEXT_PATH);

	// 根据Huffman编码表，另存为Huffman编码的二进制文件
	saveAsHuffmanCodeFile(list, OUTPUT_HUFFMAN_TEXT_PATH, OUTPUT_HUFFMAN_FILE_PATH);

	// todo:根据频率表建Huffman树，解析Huffman编码文件
	parseHuffmanCodeFile(compressedWeights, OUTPUT_HUFFMAN_TEXT_PATH);

	return 0;
}