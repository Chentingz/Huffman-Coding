/*********************************************
@Author			CTZ
@Date			2020-04-02
@Description 

Huffman����
1.txt�ı�ɨ�裬ͳ���ַ����ִ���
2.����ÿ���ַ����ֵ�Ƶ�ʣ��洢��Ȩֵ����
3.ѹ����������������Ȩֵ��
4.����Ȩֵ����ɭ�֣�ͨ���ϲ���СȨֵ������Huffman��
5.����������Huffman���룬������������
5.��txt�ı����ΪHuffman������ļ�
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
	ѹ����Ȩֵ���壬�õ������ʾ
********************************************/

typedef struct LinkListNode
{
	char			key;
	float			value;
	LinkListNode	*next;
}*LinkList;

/*
@description : ����һ����ͷ��������

@return ��ͷ��������
*/
LinkList createLinkList()
{
	LinkListNode *node = (LinkListNode*)malloc(sizeof(LinkListNode));
	node->key = 'n';	// ͷ����key��keyֵ���ܻ��ظ�
	node->value = 0;	// ��¼������
	node->next = NULL;
	return node;
}

/*
@description : ���յ��������һ�����

@intput param L : ����

@input param key : �½���key

@input param value : �½���value
*/
void insertLinkList(LinkList L, char key, float value)
{
	if(L == NULL) return;
	LinkListNode *pCur = L->next;
	LinkListNode *pPrev = L;

	// �����½��
	LinkListNode *pNew = (LinkListNode*)malloc(sizeof(LinkListNode));
	pNew->key = key;
	pNew->value = value;
	pNew->next = NULL;

	// �����������
	if(L->value == 0)
	{
		L->next = pNew;
		L->value++;	// ����������
		return;
	}

	// ����ͷ��㣬�ҵ�����λ�ò��루������
	for(int i=1; i<=L->value && pCur!=NULL; ++i)
	{
		if(pNew->value <= pCur->value)			
			break;
		pCur=pCur->next;
		pPrev=pPrev->next;
	}

	// ����
	pNew->next = pCur;
	pPrev->next = pNew;

	// ͷ������������
	if(L->key == 'n')
		L->value++;
}
/*
@description : ��ӡ����

@intput param L : ����
*/
void printLinkList(LinkList L)
{
	FILE *out = NULL;
	out = fopen(OUTPUT_WEIGHT_TABLE_PATH, "w");
	if(L == NULL) return;
	LinkListNode *pCur = L->next;

	printf("\n-----------------------------����---------------------------------------------\n");
	if(L->key == 'n')	
		printf("������ : %f\n", L->value);
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
	Huffman�����壬�ö��������ʾ
********************************************/
typedef struct TreeNode
{
	char		ch;
	float		weight;
	TreeNode	*lChild;
	TreeNode	*rChild;
}*HuffmanTree;

/*
@description : ����Ȩֵ����ֻ��һ������Huffman��

@return Huffman��
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
@description : ��������Huffman���ϲ���һ���µ�Huffman��

@return Huffman��
*/
HuffmanTree mergeHuffmanTree(HuffmanTree leftTree, HuffmanTree rightTree)
{
	TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode*));
	node->lChild = leftTree;
	node->rChild = rightTree;
	node->ch = -1;	// ��ʾ��Ҷ�ӽ��
	node->weight = leftTree->weight + rightTree->weight;
	return node;
}

/*
@description : ����ѹ�����Ȩֵ����һ���µ�Huffman��

@return Huffman��

HuffmanTree createHuffmanTree(LinkList compressedWeights)
{
	LinkListNode *pCur = compressedWeights->next;	// ����ͷ���

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
@description : ���������ӡHuffman��
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
	ɭ�ֵĶ��壬�õ������ʾ
********************************************/
typedef struct ForestNode
{
	HuffmanTree tree;
	ForestNode *next;	// ָ����һ����
	
}*Forest;

/*
@description : ����һ����ͷ����ɭ�֣�����

@return ��ͷ����ɭ�֣�����
*/
Forest createForest()
{
	// ����ɭ�ֵ�ͷ���ForestNode���ý���е�����weight���ڴ��ɭ�ֵĴ�С
	HuffmanTree headTree = (TreeNode*)malloc(sizeof(TreeNode));
	headTree->lChild=NULL;
	headTree->rChild=NULL;
	headTree->ch = 'n';
	headTree->weight = 0; // ���ɭ�ִ�С

	ForestNode *head = (ForestNode*)malloc(sizeof(ForestNode));
	head->tree = headTree;
	head->next = NULL;
	return head;
}


/*
@description : ��������ɭ��

@input param forest : ɭ��

@input param tree : ��
*/
void insertForest(Forest forest, HuffmanTree tree)
{
	if(forest == NULL) return;
	ForestNode *pCur = forest->next;
	ForestNode *pPrev = forest;

	// �����½��
	ForestNode *pNew = (ForestNode*)malloc(sizeof(ForestNode));
	pNew->tree = tree;
	pNew->next = NULL;

	// ��ɭ�ֲ�����
	if(forest->tree->weight == 0)
	{
		forest->next = pNew;
		forest->tree->weight++; // ����ɭ�ִ�С
		return;
	}

	// ����ͷ��㣬Ѱ��һ�����ʵ�λ�ò��룬ʹ������Ȩֵ��������
	for(int i=1; i<=forest->tree->weight && pCur!=NULL; ++i)
	{
		if(pNew->tree->weight <= pCur->tree->weight)
			break;
		pCur=pCur->next;
		pPrev=pPrev->next;
	}
	// ����
	pNew->next = pCur;
	pPrev->next = pNew;

	// ͷ������������
	if(forest->tree->ch == 'n')
		forest->tree->weight++;
}




/*
@description : ����ѹ����Ȩֵ��������������Huffman����������ɭ��

@input param forest : ɭ��

@input param compressedWeights : ѹ����Ȩֵ��
*/
void initialForest(Forest forest, LinkList compressedWeights)
{
	LinkListNode *pCur = compressedWeights->next;	// ����ͷ���
	for(int i=1; i<=compressedWeights->value; ++i)
	{
		if(pCur != NULL)
		{
			HuffmanTree tree = createHuffmanTree(pCur->key, pCur->value); // ����������Ȩֵ����Huffman��
			// �����������ɭ��
			insertForest(forest, tree);
			pCur = pCur->next;
		}
	}
}



/*
@description : ������ɭ����ɾ��

@input param forest : ɭ��

@input param tree : ��
*/
void deleteFromForest(Forest forest, HuffmanTree tree)
{
	if(forest==NULL || tree==NULL) return;
	// ���ɭ����ֻʣһ�������Ҹ���������Ҫɾ����
	if(forest->tree->weight == 1 && forest->next->tree == tree) 
		forest->next = NULL;
	else if(forest->tree->weight == 1 && forest->next->tree != tree) 
		return;

	// ����ͷ��㣬����ɭ��
	ForestNode *pCur = forest->next;
	ForestNode *pPrev = forest;
	for(int i=1; i<=forest->tree->weight && pCur != NULL; ++i)
	{
		// ���������ɭ����ɾ��
		if(pCur->tree == tree)
		{
			pPrev->next = pCur->next;
			pCur->next = NULL;

			// ����ɭ�ִ�С
			forest->tree->weight--;
			return;
		}
		pPrev = pPrev->next;
		pCur = pCur->next;
	}
	return;
}
/********************************************
	�ַ�����������
********************************************/

typedef struct MyString
{
	char	*s;			// �ַ���
	int		size;		// �ַ�������
	int		maxSize;	// �ַ����������
}*String;

/*
@description ����һ���ַ���

@return ���ַ���
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
@description �����ַ��������µ��ַ���

@input param ss : �ַ���

@return ���ַ���
*/
String createString(String ss)
{
	if(ss == NULL) return NULL;
	String str = (MyString*)malloc(sizeof(MyString));
	// ���
	str->s = (char*)malloc(sizeof(char) * ss->maxSize);
	memcpy(str->s, ss->s, sizeof(char) * ss->maxSize);

	str->size = ss->size;
	str->maxSize = ss->maxSize;

	return str;
}

/*
@description ����һ��ָ������������ַ����������ַ���ss�����ݳ�ʼ��

@input param maxSize : ���ַ������������

@input param ss : �ַ���

@return ���ַ���
*/
String createString(int maxSize, String ss)
{
	String str = (MyString*)malloc(sizeof(MyString));
	str->s = (char*)malloc(sizeof(char) * maxSize);
	
	// ��ss�����ݳ�ʼ��
	memcpy(str->s, ss->s, sizeof(char) * ss->maxSize);

	str->size = ss->size;
	str->maxSize = maxSize;

	return str;
}


/*
@description ���һ���ַ���char*�ַ�����β��

@input param str : �ַ���

@input param ch : �ַ�

@return String�ַ���
*/
String appendChar2String(String str, char ch)
{
	if(str == NULL) return NULL;
	int strCurmaxSize = str->size + 1;	// �ַ���ռ�õ����ݿռ�, +1��Ϊ�����ַ���β������һ��'\0'

	if(strCurmaxSize + 1 > str->maxSize)	// ������ַ��󣬳����ַ���������ڴ�ռ䣬���ڴ�ռ�����2��
	{
		/*
		str->maxSize *= 2;
		// �����µ��ڴ�ռ䲢��ԭ�ַ������ݿ��������ַ�����
		//char *newStr = (char*)malloc(sizeof(char) * strmaxSize);
		//memcpy(newStr, str, strSize);
		String newStr = (MyString*)malloc(sizeof(MyString));
		// ���
		newStr->str = (char*)malloc(sizeof(char) * str->maxSize);
		memcpy(newStr->s, str->s, str->size);

		newStr->size = str->size;
		newStr->maxSize = str->maxSize;

		// �ͷ�ԭ�ַ����ڴ�ռ�
		free(str->s);
		free(str);
		
		// ����ָ���µ��ڴ�ռ�
		str = newStr;
		*/

		// ����һ���µ��ַ��������ڴ�ռ���ԭ����2��
		String newStr = createString(str->maxSize*2, str);
		
		// �ͷ�ԭ�ַ����ڴ�ռ�
	//	free(str->s);
		if(str)
			free(str);

		// strָ���µ��ڴ�ռ�
		str = newStr;
	}
	
	// ����ַ�
	if(strCurmaxSize < str->maxSize)
	{
		str->s[str->size] = ch;
		str->s[str->size+1] = '\0';

		// �����ַ�������
		str->size += 1;
	}

		
	return str;
}

/*
@description ɾ��char*�ַ��������һ���ַ�

@input param str : �ַ���

@input param ch : �ַ�

*/
void removeCharFromString(String str, char ch)
{
	if(str->size <= 0 || str->maxSize <=0)	return;

	
	if(str->s)
	{
		// ɾ�����һ���ַ�
		str->s[str->size-1] = str->s[str->size];
		// �����ַ�������
		str->size -= 1;
	}
}


/********************************************
	Huffman���붨�壬�õ������ʾ
********************************************/
/*
typedef struct HuffmanCodeNode
{
	char			ch;			// �ַ�
	char			*code;		// �ַ���Ӧ�ı��� / ͷ�����ֶβ���
	int				codeSize;	// ���볤�� / ͷ�����ֶα�ʾHuffmanCode����ĳ���
	HuffmanCodeNode *next;	
	
}*HuffmanCode;
*/

typedef struct HuffmanCodeNode
{
	char			ch;			// �ַ�
	String			code;		// �ַ���Ӧ�ı��루�Զ����ַ�����ʾ�� / ͷ����code->size���ڱ�ʾHuffmanCode����ĳ���
	HuffmanCodeNode *next;	
	
}*HuffmanCode;


/*
@description ����ѹ����Ȩֵ������HuffmanCode��������һ��ͷ��㣩

@input param compreesedWeights : ѹ����Ȩֵ��

@return HuffmanCode����
*/
HuffmanCode createHuffmanCode(LinkList compressedWeights)
{
	HuffmanCodeNode *node = (HuffmanCodeNode*)malloc(sizeof(HuffmanCodeNode));
	node->ch = 'n';
	node->code = createString();
	node->code->size = 0;	// ͷ���ĸ��ֶα�ʾ������
	node->next = NULL;
	return node;
}

/*
@description ����һ����㵽HuffmanCode����

@input param list : HuffmanCode����

@input param ch : �ַ�

@input param code : �ַ���Ӧ��Huffman����
*/
void insertHuffmanCode(HuffmanCode list, char ch, String code)
{
	HuffmanCodeNode *pCur = list->next; // ����ͷ���

	// �����½��
	HuffmanCodeNode *pNew = (HuffmanCodeNode*)malloc(sizeof(HuffmanCodeNode));
	pNew->ch = ch;

	// ���
	pNew->code = createString(code);
	pNew->next = NULL;

	// ͷ�巨
	pNew->next = list->next;
	list->next = pNew;

	// ����������
	list->code->size++;
}

/*
@description ��ӡHuffmanCode����

@input param list : HuffmanCode����
*/
void printHuffmanCode(HuffmanCode list)
{
	if(list == NULL) return;
	HuffmanCodeNode *pCur = list->next;	// ����ͷ���
	printf("\n-----------------------------HuffmanCode����------------------------------------\n");
	printf("������ : %d\n", list->code->size);
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
@description HuffmanCode����������˳������д���ļ�

@input param list : HuffmanCode����

@input param outputFilePath : ����ļ�·��
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
	��������
********************************************/


/*
@description : ��ȡ�ı���ͳ�������ַ��ĸ������Լ�ÿ��ASCII�ַ����ֵĴ���

@input param asciiCharCount : ASCII�ַ�ͳ�Ʊ�ͳ��ÿ��ASCII�ַ����ִ���

@input param size : asciiCharCount����Ĵ�С

@return �ı��������ַ��ĸ���
*/
int readAndCount(int *asciiCharCount, int size)
{
	FILE *in = NULL;
	char buf[KILO_BYTES];
 
	in = fopen(INPUT_TEXT_PATH, "r");

	int readTotalCount = 0; // ��¼�ı��������ַ��ĸ���
	// ÿ�ζ�ȡ1024�ֽ�
	while (fgets(buf, sizeof(buf), in) != NULL)
	{
		printf("%s", buf);
		// ͳ��ÿ��ASCII�ַ����ִ���
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


	printf("\n��ȡ�ı��е��ַ����� : %d\n",readTotalCount);
    return readTotalCount;
}


/*
@description : ��ӡASCII�ַ�ͳ�Ʊ�

@intput param asciiCharCount : ASCII�ַ�ͳ�Ʊ�ͳ��ÿ��ASCII�ַ����ִ���

@input param size : asciiCharCount����Ĵ�С
*/
void printAsciiCharCount(int *asciiCharCount, int size)
{
	int *pCur = asciiCharCount;
	printf("\n---------------------------ASCII�ַ�ͳ�Ʊ�--------------------------------------\n");
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
@description : ����Ȩֵ��ÿ���ַ����ı��г��ֵĸ��� = ĳ���ַ����ִ��� / ���ַ�����

@input param asciiCharCount : ASCII�ַ�ͳ�Ʊ�ͳ��ÿ��ASCII�ַ����ִ���

@input param size : asciiCharCount����Ĵ�С

@input param totalCharCount : �ı������ַ�����

@output param weights : Ȩֵ��
*/
void getWeights(int *asciiCharCount, int size, int totalCharCount, float *weights)
{
	//int *pCur = asciiCharCount;	// *pCur = ��ǰָ���ASCII�ַ����ִ���
	for(int i=0; i<size; ++i)
	{
		weights[i] = (asciiCharCount[i] * 1.0f / totalCharCount);
	}
}

/*
@description : ��ӡȨֵ��ÿ���ַ����ı��г��ֵĸ��� = ĳ���ַ����ִ��� / ���ַ�����

@input param weights : Ȩֵ��

@input param size : Ȩֵ���С
*/
void printWeights(float *weights, int size)
{
	float totalWeights = (float)0.0;
	printf("\n-------------------------------Ȩֵ��----------------------------------------\n");
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
	printf("�ܵ�Ȩֵ : %f\n",totalWeights);
}


/*
@description : ѹ��Ȩֵ��ȥ��Ϊ0��Ȩֵ������������

@input param weights : Ȩֵ��

@input param size : Ȩֵ���С

@input param compressedWeights : ��ѹ���Ŀ�Ȩֵ������

@output param compressedWeights : ѹ����������Ȩֵ������
*/
void getCompressedWeights(float *weights, int size, LinkList compressedWeights)
{
	for(int i=0; i<size; ++i)
	{
		// ����������
		if(weights[i] != 0.0f)
			insertLinkList(compressedWeights, i, weights[i]);
	}
}

/*
@description : ����Huffman��

@input param forest : ɭ��

@input param compressedWeights : ѹ����Ȩֵ��

@return Huffman��
*/
HuffmanTree getHuffmanTree(Forest forest, LinkList compressedWeights)
{
	initialForest(forest, compressedWeights);	// ��һ��

	float round = forest->tree->weight;
	// ����round�ּ��ɹ����һ��Huffman��
	for(float i=2; i<=round; ++i)
	{
		// ��ɭ�֣�������������ǰ��������������µ�Huffman���������뵽ɭ���� 
		if(forest->tree->weight >=2 )
		{
			ForestNode *pFirstNode = forest->next;
			ForestNode *pSecondNode = forest->next->next;

			// ��������Ȩֵ��С�����������µ���
			HuffmanTree newTree = mergeHuffmanTree(pFirstNode->tree, pSecondNode->tree);
			
			// ��ɭ����ɾ����������
			deleteFromForest(forest, pFirstNode->tree);
			deleteFromForest(forest, pSecondNode->tree);

			// ���������뵽ɭ��
			insertForest(forest, newTree);
		}
	}
	// ���ɭ����ֻ��һ��������ʱHuffman���������
	return forest->next->tree;
}


/*
@description : �������Huffman��,����Huffman����

@input param tree : Huffman��

@input param code : �Զ����ַ�����ʾ�ı���

@output param list : Huffman��������

@return code : ������ڴ�ռ��ڱ��������п��ܻ���Ϊ���ݶ��ı䣬��Ҫ��̬�޸�codeָ���ָ��
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
		if(tree->ch == -1)	// ch == -1��ʾ��Ҷ�ӽ��
			printf("[]");
		else // Ҷ�ӽ��
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
@description �ֱ���ASCII���Huffman�����ӡָ���ļ��е��ı�����д���ļ�

@input param list : HuffmanCode����

@input param inputTextPath : ������ı�·��������ASCII�ı���

@input param outputTextPath : ������ı�·�������ASCII��Huffman�ĶԱ��ı���

@input param outputText2Path : ������ı�·�������Huffman�ı���
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


	printf("ASCII���ı� : \n");
	fprintf(out, "ASCII���ı� : \n");

	// ÿ�ζ�ȡ1024�ֽ�
	while (fgets(buf, sizeof(buf), in) != NULL)
	{
		// ASCII���ӡ
		printf("%s", buf);
		fprintf(out, "%s", buf);
	}
	fclose(in);

	
	in = fopen(inputTextPath, "r");
	printf("\n\nHuffman�����ı� : \n");
	fprintf(out, "\n\nHuffman�����ı� : \n");
	// ÿ�ζ�ȡ1024�ֽ�
	while (fgets(buf, sizeof(buf), in) != NULL)
	{
		// ��ӡASCII���ַ���Ӧ��Huffman����
		char *pCur = buf;
		for(int i=0; i<sizeof(buf) && *pCur != '\0'; ++i)
		{
			if(*pCur >=0 && *pCur <=128)
			{
				// ɨ��Huffman�����
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
	MyChar�Ķ��壬����λ�����
	Little-Endian�洢���͵�ַ���λ
	���磺�洢"0001 0010"����MyChar�Ĵ洢Ϊ0100 1000
********************************************/
typedef struct MyChar
{
	unsigned char bit0:1;		// ��λ
	unsigned char bit1:1;
	unsigned char bit2:1;
	unsigned char bit3:1;
	unsigned char bit4:1;
	unsigned char bit5:1;
	unsigned char bit6:1;
	unsigned char bit7:1;
}MyChar;


/*
@description ��������ʼ��MyChar
*/
MyChar *createMyChar()
{	
	MyChar *ch = (MyChar*)malloc(sizeof(char));
	memset(ch, 0, sizeof(char));
	return ch;
}

/*
@description ����MyChar
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
@description ����MyChar��posλ��bitλ

@input param ch : MyChar���ͱ�ʾ��1�ֽڱ���

@input param pos : ����bit��λ��(0-7)

@input param bit : ����bit��ֵ(0��1)
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
@description 8�ֽ��ַ���ת����1��8λ��2����λ��

@input param eightByteString : 8�ֽ��ַ���

@input param size : �ַ���ʵ�ʳ���

@return ת�����8λ������λ��
*/
char eightByteString2Char(char *eightByteString, int size)
{
	if(eightByteString == NULL)	return NULL;
	MyChar *bitStream = createMyChar();	// ����Ϊȫ0�Ķ�����λ��


	// �����ַ���
	for(int i=0; i<size; ++i)
	{
		if( eightByteString[i] == '0' )
			MyCharSetBit(bitStream, 7-i, 0);	// ע��MyChar�а�Little-Endian˳����λ,7-i��ʾ�Ӻ���ǰ��
		else if(eightByteString[i] == '1' )
			MyCharSetBit(bitStream, 7-i, 1);
	}
	/*
	if(size!=8)// ���磬�ַ���"010101??"����2λû����䣬ʵ�ʳ��Ȳ���8����0���
	{
		// ��λ�͵�ַ���
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
@description ����Huffman���뽫�ı�д���ļ�

@input param list : HuffmanCode����

@input param huffmanTextPath : �����Huffman�ı�·��

@input param huffmanFilePath : �����Huffman�������ļ�·��
*/
void saveAsHuffmanCodeFile(HuffmanCode list, char *huffmanTextPath, char *huffmanFilePath)
{
	FILE *in = NULL;
	char buf[8] = {0};
	in = fopen(huffmanTextPath, "r");

	FILE *out = NULL;
	out = fopen(huffmanFilePath, "wb");	// д��������ļ�

	// ÿ�ζ�ȡ8�ֽڵ��ַ������൱�ڶ�ȡ8λ�Ķ����Ʊ��룩
	// �����ȡ�ַ���"01010101"���൱�ڶ�ȡ8λ�Ķ�����λ��
	int readBytes = 8;	// ÿ�ζ�ȡ8�ֽ�
	int n = 0;			// ��¼ÿ�ζ�ȡ�ֽ���
	int count = 0;		// ��¼����
	//fseek(in,0,SEEK_SET); 
	while (n = (fread(buf, sizeof(char), readBytes, in))) // �ļ�����Ϊ��ʱ��n = 0
	{
	/*
	char ch='\0';
	while( (ch = getc(in)) != EOF)
	{
		buf[n++] = ch;
		if(n == readBytes)
		{
		*/
	
			// 8�ֽ��ַ���ת����1�ֽڶ�����λ��(char�����洢)
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
@description ����Huffman���뽫�ı�д���ļ�

@input param weights : �ڴ���Ȩֵ��

@input param huffmanTextPath : Huffman�ı���Huffman�����ʾ���ַ�����·��
*/
void parseHuffmanCodeFile(LinkList weights, char * huffmanTextPath)
{

}



int main()
{
	// �洢128��ASCII���ַ��ļ�����
	int *asciiCharCount = (int*)malloc(sizeof(int) * ASCII_CHAR_COUNT_SIZE);
	memset(asciiCharCount, 0, sizeof(int) * ASCII_CHAR_COUNT_SIZE);	// �����ʼ��

	// ��ȡ�ı�
	int totalCharCount = readAndCount(asciiCharCount, ASCII_CHAR_COUNT_SIZE);
	printAsciiCharCount(asciiCharCount, ASCII_CHAR_COUNT_SIZE);

	// Ȩֵ���ʼ��
	float *weights = (float*)malloc(sizeof(int) * WEIGHTS_SIZE);
	memset(weights, 0, sizeof(float) * WEIGHTS_SIZE);	// �����ʼ��

	// ����Ȩֵ��
	getWeights(asciiCharCount, ASCII_CHAR_COUNT_SIZE, totalCharCount, weights);
	printWeights(weights, WEIGHTS_SIZE);


	// ѹ��Ȩֵ����������
	LinkList compressedWeights = NULL;
	compressedWeights = createLinkList();
	getCompressedWeights(weights, WEIGHTS_SIZE, compressedWeights);
	printf("-----------------------------ѹ����Ȩֵ��-----------------------------\n");
	printLinkList(compressedWeights);

	// ����ѹ����Ȩֵ������Huffman��
	Forest forest = createForest();
	HuffmanTree tree = getHuffmanTree(forest, compressedWeights);
	printf("-----------------------------���������Huffman��-----------------------------\n");
	printHuffmanTree(tree);

	// ����Huffman��������Huffman����
	HuffmanCode list = createHuffmanCode(compressedWeights);
	String code = createString();
	getHuffmanCode(tree, code, list);
	printHuffmanCode(list);
	saveHuffmanCode(list, OUTPUT_HUFFMAN_CODE_TABLE_PATH);

	// ����Huffman�������ӡHuffman�����ʾ���ı�
	printText(list, INPUT_TEXT_PATH, OUTPUT_COMPARE_TEXT_PATH, OUTPUT_HUFFMAN_TEXT_PATH);

	// ����Huffman��������ΪHuffman����Ķ������ļ�
	saveAsHuffmanCodeFile(list, OUTPUT_HUFFMAN_TEXT_PATH, OUTPUT_HUFFMAN_FILE_PATH);

	// todo:����Ƶ�ʱ�Huffman��������Huffman�����ļ�
	parseHuffmanCodeFile(compressedWeights, OUTPUT_HUFFMAN_TEXT_PATH);

	return 0;
}