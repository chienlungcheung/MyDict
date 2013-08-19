#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
/*
 * use the ascii set
 */
#define R 128

/*
 * Node definition of the diction tree
 */
typedef struct TrieNode {
	struct TrieNode *child[R];//each node has R childs
	char *inter;//save the word's interpretation
} TrieNode;

/*
 * Insert a word and it's interpretation into the diction tree。
 * root初始指向树根，word是要插入的单词，inter指向单词对应的意思。
 */
void
InsertDict(TrieNode *root, char *word, char *inter) {
	int wordLen = strlen(word);
	int index;

	/*
	 * 单词长度为0的则无须插入，直接返回。
	 */
	if (wordLen == 0) return;

	/*
	 * 正在处理的字母对应的ascii码值即为它在child分量中的索引。
	 */
	index = (int)(*word);

	if (wordLen > 1) {
		if (root->child[index] != NULL)
			InsertDict(root->child[index], ++word, inter);
		else {
			root->child[index] = (TrieNode*)calloc(1, sizeof(TrieNode));
			InsertDict(root->child[index], ++word, inter);
		}
	} else if (wordLen == 1){//单词长度变为1时要特殊处理，因为要保存对应的含义了。
		if (root->child[index] != NULL) {
			if (root->child[index]->inter == NULL) {
				root->child[index]->inter = (char*)calloc(strlen(inter) + 1, sizeof(char));
				if (root->child[index]->inter == NULL) {
					fprintf(stderr, "FATAL ERROR: malloc new TrieNode->inter(calloc)\n");
					exit(1);
				}
				strcpy(root->child[index]->inter, inter);
				return;
			} else {
				return;
			}
		} else {
			root->child[index] = (TrieNode*)calloc(1, sizeof(TrieNode));
			if (root->child[index] == NULL) {
				fprintf(stderr, "FATAL ERROR: malloc new TrieNode(calloc)\n");
				exit(1);
			}
			root->child[index]->inter = (char*)calloc(strlen(inter) + 1, sizeof(char));
			if (root->child[index]->inter == NULL) {
				fprintf(stderr, "FATAL ERROR: malloc new TrieNode->inter(calloc)\n");
				exit(1);
			}

			strcpy(root->child[index]->inter, inter);
		}
	}
}

/*
 * 读取原始文件创建字典，最后返回字典树的根节点。
 * 内部调用上面的InsertDict函数。
 */
TrieNode *
CreateDict() {
	FILE *fp = NULL;
	char word[300], inter[300];
	size_t wordNumber = 0;
	/*
	 * 创建字典树根节点。
	 */
	TrieNode *root = (TrieNode*)calloc(1, sizeof(TrieNode));
	if (!root) {
		fprintf(stderr, "ERROR: create root error.(calloc)\n");
		exit(1);
	}

	/*
	 * 打开同一个目录下的原始文件。
	 */
	fp = fopen("raw-dict", "r");
	if (!fp) {
		fprintf(stderr, "FATAL ERROR: raw-dict not exist\n");
		exit(1);
	}
	/*
	 * 读取原始文件，单词放到word数组中，对应的中文意思放到inter数组中。
	 */
	while (fgets(word, sizeof(word), fp) && fgets(inter, sizeof(word), fp)) {

		/*
		 * 插入到字典中。
		 */
		word[strlen(word) - 1] = '\0';
		inter[strlen(inter) - 1] = '\0';
		//printf("%s	%s\n", word, inter);
		wordNumber++;
		InsertDict(root, word, inter);
	}
	fclose(fp);
	printf("*****Total number of words is %u.*****\n", wordNumber);
	return root;
}

/*
 * 查找单词。
 * root初始为根节点，word指向要查找的单词.
 */
void
QueryDict(TrieNode *root, char *word) {
	int index;
	int wordLen = strlen(word);
	/*
	 * 要查找的的字符串为空（因为scanf的缘故，程序保证不会出现这个情况），
	 * 或者要查找的结点为空，则该单词不存在。
	 */
	if (wordLen == 0 || root == NULL) {
		fprintf(stdout, "The word not exist\n");
		return;
	}

	/*
	 * 求该字母是所在结点的第几个孩子
	 */
	index = (int)(*word);

	if (wordLen == 1) {
		/*
		 * 查到最后一个字母了，查看这个字母所在结点是否为空，为空则要查的单词不存在；
		 * 结点不为空但是但是不构成单词，则要查的单词也不存在。
		 */
		if (root->child[index] && root->child[index]->inter)
			fprintf(stdout, "%s\n", root->child[index]->inter);
		else
			fprintf(stdout, "The word not exist.\n");
	} else {
		/*
		 * 继续扫描后续字母
		 */
		QueryDict(root->child[index], ++word);
	}
}

/*
 * 检查查询合法性，若合法则转成小写。
 */
int
TestAndTolower(char *word) {
	while (*word) {
		/*
		 * 合法字符为大小写字母和连词符
		 */
		if ((*word >= 'a' && *word <= 'z') || (*word >= 'A' && *word <= 'Z') || *word == '-') {
			if (*word >= 'A' && *word <= 'Z')
				*word = 'a' + (*word - 'A');
		} else {
			return 0;
		}
		word++;
	}
	return 1;
}
int
main(int argc, char *argv[]) {
	char query[200];
	TrieNode *dict;

	struct timeval start, end;
	gettimeofday(&start, NULL);
	dict = CreateDict();
	gettimeofday(&end, NULL);

	printf("*****建立词典耗时 %.4f s.*****\n", 1.0 * (1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)) / 1000000);	
	printf("*****Input --quit for quiting.*****\n");
	do {
		printf(">>>>>>>");
		scanf("%s", query);
		if (TestAndTolower(query) == 0) {
			printf("Invalid input");
			continue;
		}
		if(!strcmp("--quit", query))
			break;
		QueryDict(dict, query);
	} while (1);
	system("cd && clear");
	return 0;
}
