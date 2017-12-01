using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace LightRidersManager
{
    public class GameManager
    {
        public const int WIN0 = 0;
        public const int WIN1 = 1;
        public const int DRAW = -1;
        public const int NONE = -2;

        private int mWidth;
        private int mHeight;

        private const char EMPTY = '.';
        private const char WALL0 = 'o';
        private const char WALL1 = 'x';
        private const char PLAYER0 = '0';
        private const char PLAYER1 = '1';

        private int mTimeBank = 100;
        private int mTimePerMove = 100;
        private string mName0 = "Engine0";
        private string mName1 = "Engine1";
        private int mTime0 = 0;
        private int mTime1 = 0;

        private int mStartRow = 1;
        private int mStartCol = 1;

        private Stopwatch mStopWatch = new Stopwatch();

        private char[][] mField;
        private int mRound;

        private List<string> mFieldHistory = new List<string>();
        
        private Process mEngine0;
        private Process mEngine1;

        private Random mRand = new Random();

        public Process Engine0
        {
            get { return mEngine0; }
            set { mEngine0 = value; }
        }

        public Process Engine1
        {
            get { return mEngine1; }
            set { mEngine1 = value; }
        }

        public GameManager(int width, int height)
        {
            mWidth = width;
            mHeight = height;
            mField = new char[height][];
            for (int i = 0; i < height; i++)
            {
                mField[i] = new char[width];
            }
        }

        public char GetFieldCell(int x, int y)
        {
            return mField[y][x];
        }

        public int FieldHeight()
        {
            return mHeight;
        }
        
        public int FieldWidth()
        {
            return mWidth;
        }

        public List<string> FieldHistory()
        {
            return mFieldHistory;
        }

        public void NewGame(string initial_position)
        {
            InitGame(initial_position.Replace(",", ""));
            SendSettings(mEngine0, 0, mName0);
            SendSettings(mEngine1, 1, mName1);
            mTime0 = mTimeBank;
            mTime1 = mTimeBank;
            mFieldHistory.Clear();
        }

        private void SendSettings(Process engine, int botid, string your_bot) 
        {
            engine.StandardInput.WriteLine("settings timebank " + mTimeBank);
            engine.StandardInput.WriteLine("settings time_per_move " + mTimePerMove);
            engine.StandardInput.WriteLine("settings player_names " + mName0 + "," + mName1);
            engine.StandardInput.WriteLine("settings your_bot " + your_bot);
            engine.StandardInput.WriteLine("settings your_botid " + botid);
            engine.StandardInput.WriteLine("settings field_width " + mWidth);
            engine.StandardInput.WriteLine("settings field_height " + mHeight);
        }

        private void InitGame(string initial_position) 
        {
            //for (int y = 0; y < mHeight; y++)
            //{
            //    for (int x = 0; x < mWidth; x++)
            //    {
            //        mField[y][x] = EMPTY;
            //    }
            //}

            //mField[mStartRow][mStartCol] = PLAYER0;
            //mField[mStartRow][mWidth - 1 - mStartCol] = PLAYER1;
            int i = 0;
            for (int y = 0; y < mHeight; y++)
            {
                for (int x = 0; x < mWidth; x++)
                {
                    mField[y][x] = initial_position.ElementAt(i++);
                }
            }

            mRound = 0;
        }

        public void NextStartPosition()
        {
            mStartCol++;
            if (mStartCol < mWidth / 2) return;
            mStartCol = 1;
            mStartRow++;
            if (mStartRow < mHeight) return;
            mStartRow = 1;
        }

        public void DoRound()
        {
            string fs = FieldString();

            mStopWatch.Restart();
            SendReceiveMove(mEngine0, 0, fs, mTime0);
            //SendReceiveMove(mEngine0, 0, fs, mRand.Next(10, 50));
            mStopWatch.Stop();

            //Console.Write("start: " + mTime0);
            //mTime0 -= (int)mStopWatch.ElapsedMilliseconds;
            mTime0 = mTimePerMove;
            //Console.WriteLine(" elapsed: " + mStopWatch.ElapsedMilliseconds + " end: " + mTime0);

            mStopWatch.Restart();
            SendReceiveMove(mEngine1, 1, fs, mTime1);
            //SendReceiveMove(mEngine1, 1, fs, mRand.Next(10, 50));
            mStopWatch.Stop();

            //mTime1 -= (int)mStopWatch.ElapsedMilliseconds;
            mTime1 = mTimePerMove;

            mRound++;

            mFieldHistory.Add(FieldString());
        }

        private string FieldString()
        {
            string fs = "";
            for (int y = 0; y < mHeight; y++)
            {
                for (int x = 0; x < mWidth; x++)
                {
                    if (fs != "") fs += ",";
                    if (mField[y][x] == WALL0 || mField[y][x] == WALL1)
                        fs += "x";
                    else
                        fs += mField[y][x];
                }
            }
            return fs;
        }

        private void SendReceiveMove(Process engine, int botid, string field, int time)
        {
            //update("game", "round", "0");
            //update("game", "field", ".,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,0,.,.,.,.,.,.,.,.,1,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.");
            engine.StandardInput.WriteLine("update game round " + mRound);
            engine.StandardInput.WriteLine("update game field " + field);
            engine.StandardInput.WriteLine("action move " + time);
            string move = engine.StandardOutput.ReadLine();
            //Console.WriteLine("engine" + botid + " move = " + move);
            engine.StandardError.ReadToEnd();
            MakeMove(botid, move);
        }

        private void MakeMove(int botid, string move)
        {
            if (move == null) return;
            if (move != "up" && move != "down" && move != "right" && move != "left") return;

            char player = '0';
            if (botid == 1) player = '1';
            char wall = WALL0;
            if (botid == 1) wall = WALL1;

            for (int y = 0; y < mHeight; y++)
            {
                for (int x = 0; x < mWidth; x++)
                {
                    if (mField[y][x] == player)
                    {
                        if (move == "up") { TryMove(x, y, x, y - 1, wall); return; }
                        if (move == "down") { TryMove(x, y, x, y + 1, wall); return; }
                        if (move == "right") { TryMove(x, y, x + 1, y, wall); return; }
                        if (move == "left") { TryMove(x, y, x - 1, y, wall); return; }
                    }
                }
            }

        }

        private void TryMove(int xf, int yf, int xt, int yt, char wall)
        {
            if (xt < 0 || xt >= mWidth) return;
            if (yt < 0 || yt >= mHeight) return;
            if (mField[yt][xt] != EMPTY) return;
            mField[yt][xt] = mField[yf][xf];
            mField[yf][xf] = wall;
        }

        public bool IsGameOver()
        {
            if (!PlayerCanMove(PLAYER0)) return true;
            if (!PlayerCanMove(PLAYER1)) return true;
            return false;
        }

        private bool PlayerCanMove(char player)
        {
            for (int y = 0; y < mHeight; y++)
            {
                for (int x = 0; x < mWidth; x++)
                {
                    if (mField[y][x] == player)
                    {
                        if (CanMoveTo(x, y - 1)) return true;
                        if (CanMoveTo(x, y + 1)) return true;
                        if (CanMoveTo(x + 1, y)) return true;
                        if (CanMoveTo(x - 1, y)) return true;
                        return false;
                    }
                }
            }
            return false;
        }

        private bool CanMoveTo(int x, int y)
        {
            if (x < 0 || x >= mWidth) return false;
            if (y < 0 || y >= mHeight) return false;
            return (mField[y][x] == EMPTY);
        }

        public int GetWinner()
        {
            bool p0move = PlayerCanMove(PLAYER0);
            bool p1move = PlayerCanMove(PLAYER1);

            if (p0move == false && p1move == false) return DRAW;
            if (p0move == true && p1move == false) return WIN0;
            if (p0move == false && p1move == true) return WIN1;

            return NONE;
        }
    }
}
