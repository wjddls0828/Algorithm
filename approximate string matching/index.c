#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	FILE* infp;
	infp = fopen("words.txt", "rt");

	if (infp == NULL)
	{
		fprintf(stderr, "Error: cannot open file words.txt\n");
		return 1;
	}

	//words.txt에서 단어 읽기
	int index_of_read_word = -1;
	char read_one_word[100];
	char text_file_name[30];
	char alreadyput[100][3];
	int index_already_put = 0;
	char text_txt[] = ".txt";
	char firstch;
	char secondch;
	FILE* fp;
	int new = 1;
	//int flag = 0;

	//read_one_word에 단어 저장됨
	while (!feof(infp)) {
		//flag++;
		//printf("%d 단어입니다\n", flag);
		fscanf(infp, "%s", read_one_word); //words.txt에서 단어 하나 읽기
		index_of_read_word++; //단어의 인덱스값 설정
		index_already_put = 0;
		for (int y = 0; y < 10; y++) {
			for (int x = 0; x < 3; x++) {
				alreadyput[y][x] = '\0';
			}
		}
		for (int i = 0;; i++) {
			if (strlen(read_one_word) == 1) {
				firstch = read_one_word[i];
				for (int i = 97; i <= 122; i++) {
					text_file_name[0] = firstch;
					text_file_name[1] = (char)i;
					text_file_name[2] = '\0';
					new = 1;
					for (int already = 0; already < index_already_put; already++) {
						if (firstch == alreadyput[already][0] && (char)i == alreadyput[already][1])
							new = -1;
					}

					if (new == 1) {
						strcat(text_file_name, text_txt);
						fp = fopen(text_file_name, "a");
						fprintf(fp, "%s\n", read_one_word);
						alreadyput[index_already_put][0] = firstch;
						alreadyput[index_already_put][1] = (char)i;
						index_already_put++;
						fclose(fp);
					}
				}

				for (int i = 97; i <= 122; i++) {
					text_file_name[0] = (char)i;
					text_file_name[1] = firstch;
					text_file_name[2] = '\0';
					new = 1;
					for (int already = 0; already < index_already_put; already++) {
						if ((char)i == alreadyput[already][0] && firstch == alreadyput[already][1])
							new = -1;
					}

					if (new == 1) {
						strcat(text_file_name, text_txt);
						fp = fopen(text_file_name, "a");
						fprintf(fp, "%s\n", read_one_word);
						alreadyput[index_already_put][0] = (char)i;
						alreadyput[index_already_put][1] = firstch;
						index_already_put++;
						fclose(fp);
					}
				}
			}
			if (read_one_word[i + 1] == '\0')
				break;

			//단어 내부에서 두글자씩 비교
			firstch = read_one_word[i];
			secondch = read_one_word[i + 1];
			new = 1;
			for (int already = 0; already < index_already_put; already++) {
				if (firstch == alreadyput[already][0] && secondch == alreadyput[already][1])
					new = -1;
			}

			if (new == 1) {
				text_file_name[0] = firstch;
				text_file_name[1] = secondch;
				text_file_name[2] = '\0';
				strcat(text_file_name, text_txt);
				fp = fopen(text_file_name, "a");
				fprintf(fp, "%s\n", read_one_word);
				alreadyput[index_already_put][0] = firstch;
				alreadyput[index_already_put][1] = secondch;
				index_already_put++;
				fclose(fp);
			}
		}

	}
	fclose(infp);
	
	return 0;
}