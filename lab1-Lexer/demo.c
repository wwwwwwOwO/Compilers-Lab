int main (void)
{
    int a=1, d=2,c;
    if(a <= d)
    {
        c=a;
        a=d;
        d=c;
    }
    a = ~a;
    d *= 2;
    switch(a)
    {
        case 1:
            c--;
            break;
        case 2:
            c++;
            break;
    }
    char ch[10]="ok"; // comment
    char x,y='a';
    c=a+d;  
    return 0;
}