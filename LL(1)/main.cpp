#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_NUM 20

char nonTerminal[MAX_NUM][256];
char terminal[MAX_NUM];
int num = 0;
int numTem = 0;

struct NoneTerminal {
	char nonTerm[256];
	int index;
	NoneTerminal*next;
};

struct SpreadString {
	NoneTerminal*nonTerm;
	bool emptyAble;
	SpreadString*next;
};

SpreadString*spread[MAX_NUM];

struct FirstChar {
	char firstChar;
	int spreadIndex;	//���ĸ�spreadʽ�ӵó�
	FirstChar*next;
};

struct FollowChar {
	char followChar;
	FollowChar*next;
};

FirstChar*first[MAX_NUM];
bool Efirst[MAX_NUM];
int Eindex[MAX_NUM];

FollowChar*follow[MAX_NUM];
bool Efollow[MAX_NUM];

bool finded[MAX_NUM];

struct CheckList {
	int speIndex;	//spread����
};

CheckList cl[MAX_NUM][MAX_NUM];

void deal(char*s) {
	int index = 0;
	char name[256];
	while (*s != '=') {
		if (*s == ' ') {
			s++;
			continue;
		}
		name[index++] = *s;
		s++;
	}
	s++;
	name[index] = 0;
	strcpy(nonTerminal[num], name);
	SpreadString*st = NULL;
	index = 0;
	while ((*s != 0) && (*s != 10)) {
		bool check2 = false;
		bool check3 = false;
		SpreadString*t = new SpreadString;
		t->nonTerm = NULL;
		NoneTerminal*nt = NULL;
		while (true) {
			if (*s == ' ') {	//����ո�
				s++;
				continue;
			}
			if (*s == '&') {
				check3 = true;	//�����յı�־
				break;
			}
			if ((*s >= 'A'&&*s <= 'Z')||check2) {	//����ȡ��һ�����ս���������¶�'�Խ��������ս��������ȷ��
				if ((*s != '\'') && check2) {	//check2���ڱ�ʾ��ǰ���ڶ�'��״̬
					name[index] = 0;	//���������һ������'�Ҵ��ڶ�'��״̬����˵��һ���ս���Ķ����
					NoneTerminal*tnt = new NoneTerminal;
					strcpy(tnt->nonTerm, name);
					tnt->index = -1;
					tnt->next = NULL;
					if (!t->nonTerm) {
						t->nonTerm = tnt;
						nt = tnt;
					}
					else {
						nt->next = tnt;
						nt = nt->next;
					}
					index = 0;
					check2 = false;
					if (!((*s != '|') && (*s != 0) && (*s != 10)))
						break;
					continue;
				}
				check2 = true;	//�����ս��������ΪTrue
				name[index++] = *s;
			}
			else {	//����������ս��
				if (!((*s != '|') && (*s != 0) && (*s != 10)))
					break;
				name[0] = *s;
				name[1] = 0;
				NoneTerminal*tnt = new NoneTerminal;
				strcpy(tnt->nonTerm, name);
				tnt->index = -1;
				tnt->next = NULL;
				if (!t->nonTerm) {
					t->nonTerm = tnt;
					nt = tnt;
				}
				else {
					nt->next = tnt;
					nt = nt->next;
				}
				//break;	//�Ƿ�����ս���ͽ�����һ��spread�Ķ�ȡ
			}
			if (!((*s != '|') && (*s != 0) && (*s != 10)))
				break;
			s++;
		}
		while ((*s != '|') && (*s != 0) && (*s != 10)) {
			s++;
		}
		s++;
		t->next = st;
		t->emptyAble = check3;
		st = t;
	}
	spread[num++] = st;
}

void findIndex() {
	for (int i = 0; i < num; i++) {
		SpreadString*t = spread[i];
		while (t) {
			NoneTerminal*nt = t->nonTerm;
			while (nt) {
				for (int j = 0; j < num; j++) {
					if (!strcmp(nt->nonTerm, nonTerminal[j])) {
						nt->index = j;
						break;
					}
				}
				nt = nt->next;
			}
			t = t->next;
		}
	}
}

void DFS(int index) {
	Eindex[index] = -1;
	finded[index] = true;
	first[index] = NULL;
	SpreadString*t = spread[index];
	int count = 0;
	while (t) {
		if (t->emptyAble) {
			Efirst[index] = true;
			Eindex[index] = count;
		}
		else {
			NoneTerminal*nt = t->nonTerm;
			while (nt) {
				if (nt->index == -1) {
					FirstChar*ft = new FirstChar;
					ft->firstChar = nt->nonTerm[0];
					ft->spreadIndex = count;
					ft->next = first[index];
					first[index] = ft;
					break;
				}
				else {
					if (!finded[nt->index])
						DFS(nt->index);
					FirstChar*ft = first[nt->index];
					while (ft) {
						FirstChar*ftt = new FirstChar;
						ftt->firstChar = ft->firstChar;
						ftt->next = first[index];
						ftt->spreadIndex = count;
						first[index] = ftt;
						ft = ft->next;
					}
					if (!Efirst[nt->index])
						break;
					else {
						Efirst[index] = true;
					}
				}
				nt = nt->next;
			}
		}
		t = t->next;
		count++;
	}
}

void FindFollowOne() {
	follow[0] = new FollowChar;
	follow[0]->followChar = '$';
	follow[0]->next = NULL;
	for (int i = 0; i < num; i++) {
		SpreadString*t = spread[i];
		while (t) {
			if (t->emptyAble) {
				t = t->next;
				continue;
			}
			NoneTerminal*nt = t->nonTerm;
			int lastIndex = -1;
			while (nt) {
				if ((nt->index == -1) && (lastIndex != -1)) {	//����ҵ�һ���ս������ǰ��һ���Ƿ��ս�� ��ֱ����ȡ
					FollowChar*ft = new FollowChar;
					ft->followChar = nt->nonTerm[0];
					ft->next = follow[lastIndex];
					follow[lastIndex] = ft;
				}
				else {
					lastIndex = nt->index;
				}
				nt = nt->next;
			}
			t = t->next;
		}
	}
}

bool isin(FollowChar*f, char c) {
	if (!f)
		return false;
	while (f) {
		if (f->followChar == c)
			return true;
		f = f->next;
	}
	return false;
}

int FindFollowDFS(NoneTerminal*nt, int index) {
	int check = 0;
	if (nt->next) {
		check = FindFollowDFS(nt->next, index);
	}
	else {
		check = -1;	//��β��λ��
	}
	if (check == 1)	//��β��һ���ս��
		return 1;
	if (nt->index == -1) {	//��β��ǰ��һ���ս��
		return 1;
	}
	if (Efirst[nt->index] && (check == 2 || check == -1)) {	//��β�ķ��ս�������Ƴ���,״̬�����������ϴο��Ƴ��ջ��ߵ�ǰ����ĩβ
		FollowChar*fit = follow[index];	//��ͷ��follow�������ǰλ�õ�follow
		while (fit) {
			if (isin(follow[nt->index], fit->followChar)) {
				fit = fit->next;
				continue;
			}
			FollowChar*flt = new FollowChar;
			flt->followChar = fit->followChar;
			flt->next = follow[nt->index];
			follow[nt->index] = flt;
			fit = fit->next;
		}
		if (check != -1) {	//�����������ĩβ
			int nextIndex = nt->next->index;
			FirstChar*fit = first[nextIndex];	//����һ����first�������ǰλ�õ�follow
			while (fit) {
				if (isin(follow[nt->index], fit->firstChar)) {
					fit = fit->next;
					continue;
				}
				FollowChar*flt = new FollowChar;
				flt->followChar = fit->firstChar;
				flt->next = follow[nt->index];
				follow[nt->index] = flt;
				fit = fit->next;
			}
		}
		return 2;
	}
	if (!Efirst[nt->index] && (check == 2 || check == -1 || check == 3)) {	//��ǰλ�ò����ƿս�����״̬���뵽3
		if (check == 2) {
			FollowChar*fit = follow[index];	//��ͷ��follow�������ǰλ�õ�follow
			while (fit) {
				if (isin(follow[nt->index], fit->followChar)) {
					fit = fit->next;
					continue;
				}
				FollowChar*flt = new FollowChar;
				flt->followChar = fit->followChar;
				flt->next = follow[nt->index];
				follow[nt->index] = flt;
				fit = fit->next;
			}
		}
		if (check != -1) {	//�����������ĩβ
			int nextIndex = nt->next->index;
			FirstChar*fit = first[nextIndex];	//����һ����first�������ǰλ�õ�follow
			while (fit) {
				if (isin(follow[nt->index], fit->firstChar)) {
					fit = fit->next;
					continue;
				}
				FollowChar*flt = new FollowChar;
				flt->followChar = fit->firstChar;
				flt->next = follow[nt->index];
				follow[nt->index] = flt;
				fit = fit->next;
			}
		}
		else {
			FollowChar*fit = follow[index];	//��ͷ��follow�������ǰλ�õ�follow
			while (fit) {
				if (isin(follow[nt->index], fit->followChar)) {
					fit = fit->next;
					continue;
				}
				FollowChar*flt = new FollowChar;
				flt->followChar = fit->followChar;
				flt->next = follow[nt->index];
				follow[nt->index] = flt;
				fit = fit->next;
			}
		}
		return 3;
	}
}

void FindFollowTwo(int index) {
	SpreadString*t = spread[index];
	while (t) {
		if (t->emptyAble) {
			t = t->next;
			continue;
		}
		NoneTerminal*nt = t->nonTerm;
		FindFollowDFS(nt, index);	//����һ��spread
		t = t->next;
	}
}

void AddToTerm(char c) {
	bool check = false;
	for (int j = 0; j < numTem; j++) {
		if (terminal[j] == c) {
			check = true;
			break;
		}
	}
	if (!check) {
		terminal[numTem++] = c;
	}
}

int FindTermIndex(char c) {
	for (int j = 0; j < numTem; j++) {
		if (terminal[j] == c) {
			return j;
		}
	}
	return -1;
}

void GenCheckList() {
	terminal[numTem++] = '$';
	for (int i = 0; i < num; i++) {
		printf("First(%s) : ", nonTerminal[i]);
		if (Efirst[i])
			printf("&:%d ", Eindex[i]);
		FirstChar*t = first[i];
		while (t) {
			printf("%c:%d ", t->firstChar, t->spreadIndex);
			AddToTerm(t->firstChar);
			t = t->next;
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 0; i < num; i++) {
		printf("Follow(%s) : ", nonTerminal[i]);
		FollowChar*t = follow[i];
		while (t) {
			printf("%c ", t->followChar);
			AddToTerm(t->followChar);
			t = t->next;
		}
		printf("\n");
	}
	printf("\n");
	terminal[numTem++] = '&';
	//����Ϊterm���Ĺ�����first,follow����������
	for (int i = 0; i < num; i++)
		for (int j = 0; j < numTem; j++)
			cl[i][j].speIndex = -1;
	for (int i = 0; i < num; i++) {
		if (Efirst[i]) {	//�������Ϊ�գ����follow��Ӧ�ķ�����Ϊ�յ�����ʽ
			FollowChar*ft = follow[i];
			while (ft) {
				int index = FindTermIndex(ft->followChar);
				cl[i][index].speIndex = Eindex[i];
				ft = ft->next;
			}
		}
		FirstChar*t = first[i];
		while (t) {	//����first������ÿ��Ԫ�صĶ�Ӧ����ʽ����cl
			AddToTerm(t->firstChar);
			int index = FindTermIndex(t->firstChar);
			cl[i][index].speIndex = t->spreadIndex;
			t = t->next;
		}
	}
}

int cStack[100];	//�﷨����ջ
int sTop = 0;	//ջ��

void AddNonEmpty(NoneTerminal*n) {
	if (n->next)
		AddNonEmpty(n->next);
	if (n->index == -1) {	//�����ս��
		cStack[sTop++] = FindTermIndex(n->nonTerm[0]) + num;
		return;
	}
	cStack[sTop++] = n->index;	//������ս��
}

void AddToCheckStack(SpreadString*s) {
	if (s->emptyAble) {	//�����
		cStack[sTop++] = FindTermIndex('&') + num;
		return;
	}
	AddNonEmpty(s->nonTerm);
}

void PrintSpread(int index, SpreadString*t) {
	printf("%s -> ", nonTerminal[index]);
	if (t->emptyAble) {
		printf("&\n");
		return;
	}
	for (NoneTerminal*n = t->nonTerm; n; n = n->next) {
		printf("%s", n->nonTerm);
	}
	printf("\n");
}

void WriteError(char stack, char string) {
	if (stack != 'i'&&string == 'i'){
		printf("Unexpect ID\n");
		return;
	}
	if (stack == ')'){
		printf("Expect a ')'\n");
		return;
	}
	printf("Unexpect Error!!\n");
}

void WriteError1(int t, char string) {
	if (t == 2 && string == 'i') {
		printf("Unexpect ID\n");
		return;
	}
	if (t == 4) {
		printf("Expect a ID\n");
		return;
	}
	if (t == 3){
		printf("Expect a '*'\n");
	return;
	}
	if (t == 1){
		printf("Expect a '+'\n");
		return;
	}
	printf("Unexpect Error!!\n");
}

bool Check(char*s) {
	int len = strlen(s);
	s[len - 1] = '$';	//�����ս��
	s[len] = 0;
	cStack[sTop++] = num;	//����ѹ���ս��
	cStack[sTop++] = 0;	//��ѹ����ʼ���ս��
	while (*s) {
		int ord = cStack[sTop - 1];
		if (ord >= num) {	//˵��Ϊ�ս��
			if (terminal[ord - num] == *s) {	//ջ����ʽ��������ͬ
                printf("match %c\n", terminal[ord - num]);
				sTop--;
				s++;
				if (sTop == 0)	//ջ����Ϊ�Ϸ�
					return true;
			}
			else if (terminal[ord - num] == '&') {	//ջ��Ϊ�գ���ʽ�Ӳ����ϣ���ֱ������
				sTop--;
			}
			else {
				WriteError(terminal[ord - num], *s);
				return false;	//����Ƿ�
			}
		}
		else {	//˵��Ϊ���ս��
			int index = FindTermIndex(*s);
			if (cl[ord][index].speIndex == -1) {
				WriteError1(index, *s);
				return false;	//δ�ܴ��﷨�����ҵ����ʵ���Ŀ����Ϊ�Ƿ�
			}
			int speIndex = cl[ord][index].speIndex;
			sTop--;
			SpreadString*t = spread[ord];
			while (speIndex) {	//�ҵ���Ӧ��spread
				t = t->next;
				speIndex--;
			}
			PrintSpread(ord, t);
			AddToCheckStack(t);
		}
	}
}

int main() {
	char tmp[256];
	FILE*f = fopen("Data.txt", "r");
	while (!feof(f)) {
		memset(tmp, 0, sizeof(char) * 256);
		fgets(tmp, 256, f);
		if (!strcmp(tmp, "#"))
			break;
		deal(tmp);
	}
	fclose(f);
	findIndex();
	memset(finded, false, sizeof(bool)*num);
	memset(Efirst, false, sizeof(bool)*num);
	for (int i = 0; i < num; i++)
		if(!finded[i])
			DFS(i);
	memset(Efollow, false, sizeof(bool)*num);
	FindFollowOne();	//�����ս������
	for (int i = 0; i < num; i++)
		FindFollowTwo(i);
	GenCheckList();

	f = fopen("Data2.txt", "r");	//������жϵķ�����
	while (!feof(f)) {
		memset(tmp, 0, sizeof(char) * 256);
		fgets(tmp, 256, f);
		if (!strcmp(tmp, "#"))
			break;
		bool check = Check(tmp);
		if (check)
			printf("OK\n");
		printf("______________________________________________________\n");
	}
	fclose(f);
	system("pause");
}
