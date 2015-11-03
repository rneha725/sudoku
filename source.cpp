#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

#define FOR(i,n) for(int i=0;i<n;i++)
#define IFOR(i,a,n) for(int i=a;i<n;i++)
#define FOR_(i,n) for(int i=n;i>0;i--)
#define IFOR_(i,a,n) for(int i=n;i>a;i--)

#define i(n) cin>>n
#define o(n) cout<<n

#define SIZE 9
#define FALSE 0
#define TRUE 1
#define HEIGHT_BOX 3
#define WIDTH_BOX 3

class sudokuCell
{
public:
    int value;
    bool writable;
    sudokuCell(){}
    void readCell(int a) {value=a; writable=FALSE;}
    inline void fillCell(int a) {value=a;writable=TRUE;}
}cell[SIZE][SIZE];

int getCell(int i, int j) {return cell[i][j].value;}

bool checkSafe(int i, int j, int trialNumber)
{
    if(i>SIZE-1 || j>SIZE-1 || i<0 || j<0) return FALSE;
    int boxCol, boxRow;
    boxCol = ((int)j/WIDTH_BOX)*WIDTH_BOX;
    boxRow = ((int)i/HEIGHT_BOX)*HEIGHT_BOX;
    for(int k=boxCol;k<boxCol+WIDTH_BOX;k++)
    {
        for(int l=boxRow; l<boxRow + HEIGHT_BOX;l++)
        {
            if((i==l && j!=k) || (j==k && i!=l))
            {
                if(getCell(l,k)==trialNumber) return FALSE;
            }
        }
    }

    for(int l=0;l<SIZE;l++)
    {
        if((i!=l && (getCell(l,j)==trialNumber))) return FALSE;
    }

    for(int l=0;l<SIZE;l++)
    {
        if((j!=l && (getCell(i,l) == trialNumber))) return FALSE;
    }
    return TRUE;
}


void insert(int i,int j,int val)
{
    cell[i][j].readCell(val);
}

void inputSudoku()
{
    int i,j;
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
            cell[i][j].fillCell(0);
        }
    }
    cell[0][1].readCell(6);
    cell[0][3].readCell(3);
    cell[0][6].readCell(8);
    cell[0][8].readCell(4);
    cell[1][0].readCell(5);
    cell[1][1].readCell(3);
    cell[1][2].readCell(7);
    cell[1][4].readCell(9);
    cell[2][1].readCell(4);
    cell[2][5].readCell(6);
    cell[2][6].readCell(3);
    cell[2][8].readCell(7);
    insert(3,1,9);
    insert(3,4,5);
    insert(3,1,9);
    insert(3,6,2);
    insert(3,7,3);
    insert(3,8,8);
    insert(5,0,7);
    insert(5,1,1);
    insert(5,2,3);
    insert(5,3,6);
    insert(5,4,2);
    insert(5,7,4);
    insert(6,0,3);
    insert(6,2,6);
    insert(6,3,4);
    insert(6,7,1);
    insert(7,4,6);
    insert(7,6,5);
    insert(7,7,2);
    insert(7,8,3);
    insert(8,0,1);
    insert(8,2,2);insert(8,5,9);insert(8,7,8);
}

bool findUnassigned(int &row, int &col)
{
    for(col=0;col<SIZE;col++)
    {
        for(row=0;row<SIZE;row++)
        {
            if(getCell(row,col)==0) {cout<<"row= col="<<row<<" "<<col<<"\n";return TRUE;}
        }
    }
    return FALSE;
}

bool fillSudoku()
{
    int row, col;
    if(!findUnassigned(row, col))
    {
        return TRUE;
    }
    cout<<"row = "<<row<<" "<<col<<endl;
    int num;
    for(num=1;num<SIZE+1;num++)
    {
            if(checkSafe(row,col,num))
            {
                cell[row][col].fillCell(num);
                if(fillSudoku()) return TRUE;
                cell[row][col].fillCell(0);
            }
    }
    return FALSE;
}

void displaySudoku()
{
    FOR(i,SIZE)
    {
        FOR(j,SIZE)
        {
            cout<<getCell(i,j)<<" | ";
        }
        cout<<"\n";
    }
}

int main()
{
    memset(cell, 0, SIZE*SIZE*sizeof(sudokuCell));
    inputSudoku();
    fillSudoku();
    displaySudoku();
    cout<<"get ready for gui !!!  "<<endl<<"coming soon";
    return 0;
}
