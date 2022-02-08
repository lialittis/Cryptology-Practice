#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define textlen 1000000 /*длина вводимого текста, 150k - Alice, 3.5mln - War And Peace*/
#define keylen 26 /*длина вводимого ключа*/
#define filename 200 /*длина имени файла*/

/*перевод текста в нижний регистр*/
char lowcount(char txt[])
{
	int i=0, n;
	while(txt[i]!='\0')
	{
		if((65<=txt[i])&&(txt[i]<=90))
		{
			txt[i]=txt[i]+32;

		}
	i++;
	}
}

/*нахождение длины ключа*/
int len(char key[])
{
	int i=0;
	while(key[i]!='\0')
	{
		i++;
	}
return i;
}

/*очистка всевозможных знаков "без хвоста"*/
char clean(char a[])
{
	int i=0, j=0, n=0;
	while(a[i]!='\0')
	{
		if((a[i]>=97)&&(a[i]<=122))
		{
			a[j]=a[i];
			j++;
		}
	i++;
	}
		a[j]='\0';
}

/*Удаление одинаковых элементов "без хвоста"*/
char delete(char a[], int n)
{
	int i=0, j=i+1, k;
	while(i<n)
	{
		for(k=0; k<j; k++)
		if(a[i]==a[k])
		{
			break;
		}
		if(k==j)
		{
			a[j]=a[i];
			j++;
		}
	i++;
	}
return j;
}

/*Общий универсальный циклический вывод*/
void output(char a[])
{	
	int i=0;
	while(a[i]!='\0')
	{
		printf("%c", a[i]);
		i++;
	}
	puts("\n");	
}

/*Реверс-алфавит функция*/
void revabc(char abcy[])
{
	char revabc[textlen]="\0";
	int i=0;
	while(i<=26)
	{
		revabc[i]=abcy[i];
		i++;
	}
	i=0;
		while(i<26)
		{
			abcy[revabc[i]-97]=i+97;
			i++;
		}
}

/*Запись в файл в зависимости от выбора*/
void writefile(char txt[], FILE *f)
{	
	int i=0;
		while(txt[i]!='\0')
		{
			fputc(txt[i],f);
			i++;
		}
			fclose(f);
}

/*Запись файла в массив*/
void readfile(char txt[], FILE *f)
{
	int k;
		for(k=0;!feof(f);k++)
		fscanf(f,"%c",&txt[k]);
		fclose(f);
}

/*Создание булева вектора для ключа*/
void makekey(char vector[], char key[])
{
	int i=0;
	while(key[i]!='\0')
	{
		vector[key[i++]-97]=1;
	}
}

/*Заполнение начала алфавита ключом*/
char abcbegining(char abcy[], char key[], int l)
{
	int i=0, j=0;
	while(j!=l)
	{	
		abcy[i]=key[j];
		i++;
		j++;
	}
	return j;
}

/*Заполнение оставшейся части отсутствующими буквами*/
char abcending(char abcy[], char vector[], int j)
{	
	int k=0, i=0;
	while(i<=25)
	{
		if(vector[i]!=1)
		{
			abcy[j]=97+k;
			j++;
		}
	k++;
	i++;
	}
}

/*Шифрование*/
void cipher(char txt[], char abcy[])
{
	int i=0, j;
	while(txt[i]!='\0')
	{
		j=txt[i]-97;
		txt[i++]=abcy[j];
	}
}

/*Функция выбора*/
char choice(char choose[])
{
	int i=0;
	if(choose[i]=='c')
	{
		i=1;
	}
	if(choose[i]=='e')
	{
		i=0;
	}
	return i;
}

/*ввод текста и ключа*/
void main()
{	
	srand(time(NULL));
	int i, l, j, w, t, s;
	/*Переменные:
	i-выбор действия, l-длина ключа, w-длина текста
	j-промежуточный счетчик, t-время, s-количество циклов шифрования
	*/
	puts("Введите [c]-шифровать, [e]-расшифровать");
	char choose[10];
	gets(choose);

	puts("Введите ключ:");
	char key[keylen];
	gets(key);
		lowcount(key);
		clean(key);
		l=len(key);
		l=delete(key, l);

	puts("Введите количество циклов шифрования:");
	scanf("%d",&s);
	
	/*Открытие файлов*/
	FILE *f1, *f2;
	char finput[filename];
	puts("Введите имя файла для чтения:");
	scanf("%s",finput);
	char foutput[filename];
	puts("Введите имя файла для записи:");
	scanf("%s",foutput);
	puts("\nЧтение файлов...");

		f1=fopen(finput,"r");
		f2=fopen(foutput,"w");

		/*Существует ли входной файл?*/
		if(f1==NULL)
		{
			puts("Ошибка: невозможно открыть входной файл. Проверьте его существование.");
			return 0;
		}

	puts("Шифрование...");
	char txt[textlen];
	/*Чтение файла и запись его в массив*/
		readfile(txt, f1);
		lowcount(txt);
		clean(txt);
		w=len(txt);

	/*Создание булева вектора для ключа*/
	char vector[keylen]="\0";
	makekey(vector, key);

	/*Заполнение начала алфавита ключом*/
	char abcy[textlen]="\0";
	j=abcbegining(abcy, key, l);
	
	/*заполнение оставшейся части отсутствующими буквами*/
	abcending(abcy, vector, j);
	
	/*Отсчитывание времени*/
	t=time(NULL);
	/*Реверс-алфавит для расшифровки*/
	i=choice(choose);
	if(i==0)
	{
		revabc(abcy);
	}

	/*Циклическое шифрование*/
	int u=0;
	while(u!=s)
	{
		cipher(txt, abcy);
		u++;
	}
		int q;
		q=len(abcy);

	t=time(NULL)-t;
	/*Запись текста в файл*/
		writefile(txt, f2);
	
	/*Вывод полученного текста*/	
	puts("\nЗавершено!");	
	puts("Полученный алфавит:");	
		output(abcy);		
	printf("Длина текста: %d\n", w);
	printf("Длина алфавита: %d\n", q);
	printf("***Время шифрования: %ds\n", t);
	printf("Длина ключа (без повторов): %d\n", l);
}
