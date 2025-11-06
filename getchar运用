int main()
{
    int letter = 0, digit = 0, other = 0, N;
    scanf("%d", &N);
	while (getchar() != '\n'); //清除缓冲区中的换行符
    for (int i = 0; i < N; ++i)
    {
        char ch = getchar();
        if (isalpha(ch))
            letter++;
        else if (isdigit(ch))
            digit++;
        else
            other++;
    }
    printf("letter = %d, digit = %d, other = %d", letter, digit, other);
    return 0;
}
