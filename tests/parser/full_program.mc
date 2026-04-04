int add(int a, int b)
{
    return a + b;
}

int main()
{
    int x = 10;
    int y = 20;
    float z = 3.14;
    char c = 'A';

    if (x < y)
    {
        x = x + 1;
    }
    else
    {
        x = x - 1;
    }

    while (x != y)
    {
        x = x + 1;
    }

    for (int i = 0; i <= 10; i = i + 1)
    {
        y = y * 2;
    }

    int result = add(x, y + 1);
    return result;
}
