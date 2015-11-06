#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;


//Macros used.
#define FOR(i,n) for(int i=0;i<n;i++)
#define IFOR(i,a,n) for(int i=a;i<n;i++)
#define FOR_(i,n) for(int i=n;i>0;i--)
#define IFOR_(i,a,n) for(int i=n;i>a;i--)

#define i(n) cin>>n
#define o(n) cout<<n

//Sudoku Dimensions
#define SIZE 9
#define FALSE 0
#define TRUE 1
#define HEIGHT_BOX 3
#define WIDTH_BOX 3

int cell[SIZE][SIZE];

//Function to check whether a nunmber is valid for the box, row and column.
bool checkSafe(int i, int j, int trialNumber)
{
    if(i>SIZE-1 || j>SIZE-1 || i<0 || j<0) return FALSE;
    //box is the sub-grids in which a number cannot be repeated.
    int boxCol, boxRow;
    boxCol = ((int)j/WIDTH_BOX)*WIDTH_BOX;
    boxRow = ((int)i/HEIGHT_BOX)*HEIGHT_BOX;

    //For checking in a box.
    for(int k=boxCol;k<boxCol+WIDTH_BOX;k++)
    {
        for(int l=boxRow; l<boxRow + HEIGHT_BOX;l++)
        {
            if((i==l && j!=k) || (j==k && i!=l))
            {
                if(cell[l][k]==trialNumber) return FALSE;
            }
        }
    }

    //Checking in jth column.
    for(int l=0;l<SIZE;l++)
    {
        if((i!=l && (cell[l][j]==trialNumber))) return FALSE;
    }

    //Checking in ith row.
    for(int l=0;l<SIZE;l++)
    {
        if((j!=l && (cell[i][l] == trialNumber))) return FALSE;
    }
    return TRUE;
}

//Temporary function for inserting manual values by the program;
//for now program is the user.
void insert(int i,int j,int val)
{
    cell[i][j]=val;
}

//Manual input in program
void inputSudoku()
{
    int i,j;
    for(i=0;i<SIZE;i++)
    {
        for(j=0;j<SIZE;j++)
        {
            cell[i][j]=0;
        }
    }
    insert(0,1,6);insert(0,3,3);insert(0,8,6);insert(0,8,4);
    insert(1,0,5);insert(1,1,3);insert(1,2,7);insert(1,4,9);
    insert(2,1,4);insert(2,5,6);insert(2,6,3);insert(2,8,7);
    insert(3,1,9);insert(3,4,5);insert(3,1,9);insert(3,6,2);
    insert(3,7,3);insert(3,8,8);insert(5,0,7);insert(5,1,1);
    insert(5,2,3);insert(5,3,6);insert(5,4,2);insert(5,7,4);
    insert(6,0,3);insert(6,2,6);insert(6,3,4);insert(6,7,1);
    insert(7,4,6);insert(7,6,5);insert(7,7,2);insert(7,8,3);
    insert(8,0,1);insert(8,2,2);insert(8,5,9);insert(8,7,8);
}


//Function to find next unassigned writable value in the grid; also
//to check if grid is full or not.
bool findUnassigned(int &row, int &col)
{
    for(col=0;col<SIZE;col++)
    {
        for(row=0;row<SIZE;row++)
        {
            if(cell[row][col]==0) {return TRUE;}
        }
    }
    return FALSE;
}

//Main solver function
bool fillSudoku()
{
    int row, col;
    //Check whether grid is filled; if not then return first unassigned value
    if(!findUnassigned(row, col))
    {
        return TRUE;
    }

    //Backtracking is used here.
    int num;
    for(num=1;num<SIZE+1;num++)
    {
            if(checkSafe(row,col,num))
            {
                cell[row][col]=num;
                if(fillSudoku()) return TRUE;
                cell[row][col]=0;
            }
    }
    return FALSE;
}

//Displays Sudoku.
void displaySudoku()
{
    FOR(i,SIZE)
    {
        FOR(j,SIZE)
        {
            cout<<cell[i][j]<<" | ";
        }
        cout<<"\n";
    }
}

int main()
{
    memset(cell, 0, SIZE*SIZE*sizeof(int));
    inputSudoku();
    fillSudoku();
    displaySudoku();
    return 0;
}
