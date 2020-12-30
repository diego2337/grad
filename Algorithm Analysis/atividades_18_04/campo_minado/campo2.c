void open(char **minesweeper, char **board, int n, int m, int x, int y)
{
    if((x >= 0) && (x < n) && (y >= 0) && (y < m))
    {
        if(board[x][y] == '.')
        {
            /* Recursively check adjacent positions */
            /* Check north */
            if((x-1) >= 0)
            {
                open(minesweeper, board, n, m, (x-1), y);
            }
            /* Check northeast */
            if((x-1) >= 0 && (y+1) < m)
            {
                open(minesweeper, board, n, m, (x-1), (y+1));
            }
            /* Check east */
            if((y+1) < m)
            {
                open(minesweeper, board, n, m, x, (y+1));
            }
            /* Check southeast */
            if((x+1) < n && (y+1) < m)
            {
                open(minesweeper, board, n, m, (x+1), (y+1));
            }
            /* Check south */
            if((x+1) < n)
            {
                open(minesweeper, board, n, m, (x+1), y);
            }
            /* Check southwest */
            if((x+1) < n && (y-1) >= 0)
            {
                open(minesweeper, board, n, m, (x+1), (y-1));
            }
            /* Check west */
            if((y-1) >= 0)
            {
                open(minesweeper, board, n, m, x, (y-1));
            }
            /* Check northwest */
            if((x-1) >= 0 && (y-1) >= 0)
            {
                open(minesweeper, board, n, m, (x-1), (y-1));
            }
        }
        else 
        {
            /* Found number; update minesweeper */
            minesweeper[x][y] = board[x][y];
            return;
        }
    }
    else
    {
        return;
    }
}