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
    sudokuCell(int a) {value=a; writable=FALSE;}
    inline void fillCell(int a) {value=a;writable=TRUE;}
}cell[SIZE][SIZE];

int getCell(int i, int j) {return cell[i][j].value;}

bool checkSafe(int i, int j, int trialNumber)
{
    int boxCol, boxRow;
    boxCol = ((int)j/WIDTH_BOX)*WIDTH_BOX;
    boxRow = ((int)i/HEIGHT_BOX)*HEIGHT_BOX;
    IFOR(k, boxCol, boxCol + WIDTH_BOX)
    {
        IFOR(l, boxRow, boxRow + HEIGHT_BOX)
        {
            if((i==l && j!=k) || (j==k && i!=l))
            {
                if(getCell(l,k)==trialNumber) return FALSE;
            }
        }
    }

            #ifdef DEBUG
            cout<<"\nNot in Box.";
            #endif

    FOR(l, SIZE)
    {
        if((i!=l && (getCell(l,j)==trialNumber))) return FALSE;
    }

            #ifdef DEBUG
            cout<<"\nNot in row.";
            #endif

    FOR(l, SIZE)
    {
        if((j!=l && (getCell(i,l) == trialNumber))) return FALSE;
    }

            #ifdef DEBUG
            cout<<"\nNot in column also.";
            #endif

    return TRUE;
}

void inputSudoku()
{

    FOR(i,SIZE)
    FOR(j,SIZE) cell[i][j].writable=TRUE;
}

/*
*This function will fill Sudoku with appropriate numbers using backtracking
*3rd argument is used in case when a wrong number is filled previously.
*/
void fillSudoku(int i, int j, int except)
{
    //if current cell cannot be edited i.e. given by user then fill next cell.
    if(cell[i][j].writable==FALSE) return fillSudoku(i,j+1,0);

    if(j==-1) {i--;j=0; if(i==-1){return;}}
    else if(j==SIZE) {i++;j=0;}
    if(i==SIZE) {return;}

    /*
    *trialNumber would be increased in each while loop, if this trialNumber
    *can be filled in current cell; then fillSudoku is called for next cell.
    */
    int trialNumber=1;
    while(trialNumber <= SIZE)
    {
        if(trialNumber!=except && cell[i][j].writable==1 && checkSafe(i,j,trialNumber))
        {
            cell[i][j].fillCell(trialNumber);
            cout<<"i= "<<i<<" j= "<<j<<" numberFilled= "<<trialNumber<<endl;
            return fillSudoku(i,j+1,0);
        }
        else trialNumber++;
    }
    if(trialNumber > SIZE)
    {
        /*
        *if trialNumber cannot be filled the backtrack to previous cell and use
        *except argument as the number that cannot be given the previous cell
        *again(i.e. current value of that cell).
        */
        int exc;
        if(j==0 && i==0) return;
        if(j==0) {i--; exc=getCell(i,j);}
        else exc = getCell(i,j-1);
        cout<<"\nB. i= "<<i<<" j= "<<j<<" except= "<<exc<<endl;
        return fillSudoku(i,j-1,exc);
    }
}

void displaySudoku()
{
    FOR(i,SIZE)
    {
        FOR(j,SIZE)
        {
            cout<<getCell(i,j)<<" | ";
            // cout<<cell[i][j].writable;
        }
        cout<<"\n";
    }
}

int main()
{
    memset(cell, 0, SIZE*SIZE*sizeof(sudokuCell));
    inputSudoku();
    fillSudoku(0,0,0);
    displaySudoku();
    return 0;
}
