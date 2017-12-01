using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace LightRidersManager
{
    public partial class Form1 : Form
    {
        private const int FIELD_HEIGHT = 16;
        private const int FIELD_WIDTH = 16;
        private const string OPENINGS = @"C:\Users\Alcides Schulz\Documents\AI\LightRider\files\openings.txt";
        private const string ERRFILE0 = @"C:\Temp\Err0.txt";
        private const string ERRFILE1 = @"C:\Temp\Err1.txt";

        private GameManager mGameManager = new GameManager(FIELD_HEIGHT, FIELD_WIDTH);
        private System.IO.StreamReader mOpenings = new System.IO.StreamReader(OPENINGS);

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = true;
            startInfo.UseShellExecute = false;
            startInfo.RedirectStandardInput = true;
            startInfo.RedirectStandardOutput = true;
            startInfo.RedirectStandardError = true;
            startInfo.FileName = @"C:\Alcides\light_riders\Debug\light_riders.exe";

            Process process = new Process();
            process.StartInfo = startInfo;
            process.Start();

            //settings timebank 10000
            //settings time_per_move 500
            //settings player_names player0,player1
            //settings your_bot player0
            //settings your_botid 0
            //settings field_width 16
            //settings field_height 16

            //update game round 0
            //update game field .,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,0,.,.,.,.,.,.,.,.,1,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.

            //action move 10000
            process.StandardInput.WriteLine("settings timebank 10000");
            process.StandardInput.WriteLine("settings time_per_move 100");
            process.StandardInput.WriteLine("settings player_names player0,player1");
            process.StandardInput.WriteLine("settings your_bot player0");
            process.StandardInput.WriteLine("settings your_botid 0");
            process.StandardInput.WriteLine("settings field_width 16");
            process.StandardInput.WriteLine("settings field_height 16");
            process.StandardInput.WriteLine("update game round 0");
            process.StandardInput.WriteLine("update game field .,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,0,.,.,.,.,.,.,.,.,1,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.");
            process.StandardInput.WriteLine("action move 10000");
            while (true)
            {
                process.StandardInput.WriteLine("action move 10000");

                var resp = process.StandardOutput.ReadLine();
                Console.WriteLine(resp);
                //if (!process.StandardError.EndOfStream)
                //{
                //    var err = process.StandardError.ReadLine();
                //    Console.WriteLine(err);
                //}
                //process.WaitForExit();
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            for (int i = 0; i < (FIELD_HEIGHT * FIELD_WIDTH); i++)
            {
                var l = new Label();
                l.Width = 15;
                l.Height = 15;
                //l.Dock = DockStyle.Fill;
                l.TextAlign = ContentAlignment.MiddleCenter;
                l.Text = " ";
                l.BackColor = Color.LightBlue;
                cBoard.Controls.Add(l);
            }
        }

        private void bNew_Click(object sender, EventArgs e)
        {
            Process engine0 = InitEngine(cEng0.Text, ERRFILE0);
            Process engine1 = InitEngine(cEng1.Text, ERRFILE1);

            mGameManager.Engine0 = engine0;
            mGameManager.Engine1 = engine1;

            string pos = mOpenings.ReadLine();
            Console.WriteLine(pos);
            mGameManager.NewGame(pos);

            VisualizeGameState();

            Console.WriteLine("Log0: " + ERRFILE0);
            Console.WriteLine("Log1: " + ERRFILE1);
            Console.WriteLine("New game OK");
        }

        private void VisualizeGameState()
        {
            int x = 0;
            int y = 0;

            for (int i = 0; i < (FIELD_HEIGHT * FIELD_WIDTH); i++)
            {
                Label l = (Label)cBoard.Controls[i];
                l.Text = "" + mGameManager.GetFieldCell(x, y);
                x++;
                if (x >= FIELD_WIDTH)
                {
                    x = 0;
                    y++;
                }
            }
        }

        private Process InitEngine(string filename, string stderr_file)
        {
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = true;
            startInfo.UseShellExecute = false;
            startInfo.RedirectStandardInput = true;
            startInfo.RedirectStandardOutput = true;
            startInfo.RedirectStandardError = true;
            startInfo.FileName = filename;

            Console.WriteLine(filename);
            
            Process process = new Process();
            process.StartInfo = startInfo;
            process.Start();

            process.StandardInput.WriteLine("stderr " + stderr_file);
            return process;
        }

        private void bPlayRound_Click(object sender, EventArgs e)
        {
            if (mGameManager.IsGameOver())
            {
                MessageBox.Show("Game is over. Win=" + mGameManager.GetWinner(), "Game", MessageBoxButtons.OK);
                return;
            }
            mGameManager.DoRound();
            VisualizeGameState();
        }

        private void bPlayGame_Click(object sender, EventArgs e)
        {
            while(!mGameManager.IsGameOver())
            {
                mGameManager.DoRound();
                VisualizeGameState();
                Application.DoEvents();
            }

            //List<string> hist = mGameManager.FieldHistory();
            //foreach(string f in hist)
            //{
            //    Console.WriteLine(f);
            //}

            MessageBox.Show("Game is over. Win=" + mGameManager.GetWinner(), "Game", MessageBoxButtons.OK);
        }

        private void bRunBatch_Click(object sender, EventArgs e)
        {
            mGameManager = new GameManager(16, 16);

            mGameManager.Engine0 = InitEngine(cEng0.Text, ERRFILE0);
            mGameManager.Engine1 = InitEngine(cEng1.Text, ERRFILE1);

            Console.WriteLine("Log0: " + ERRFILE0);
            Console.WriteLine("Log1: " + ERRFILE1);

            var games = int.Parse(cGames.Text);
            int win0 = 0;
            int win1 = 0;
            int draw = 0;

            var hist_file = new System.IO.StreamWriter(@"C:\Users\Alcides Schulz\Documents\AI\LightRider\files\positions.txt");

            for (int i = 1; i <= games; i++)
            {
                var ol = mOpenings.ReadLine();
                if (ol == null)
                {
                    mOpenings.Close();
                    mOpenings = new System.IO.StreamReader(OPENINGS);
                    ol = mOpenings.ReadLine();
                }
                mGameManager.NewGame(mOpenings.ReadLine());
                while (!mGameManager.IsGameOver())
                {
                    mGameManager.DoRound();
                    Application.DoEvents();
                }
                //VisualizeGameState();
                Application.DoEvents();

                var res = mGameManager.GetWinner();

                if (res == GameManager.WIN0) win0++;
                if (res == GameManager.WIN1) win1++;
                if (res == GameManager.DRAW) draw++;

                double win_pct = (double)win0 / (double)i * 100;

                var score = "Eng0=" + win0 + " Eng1=" + win1 + " Draw=" + draw + " WinPct=" + win_pct.ToString("N") + " Win0-Win1=" + (win0 - win1);

                var rs = "d";
                if (res == GameManager.WIN0) rs = "0";
                if (res == GameManager.WIN1) rs = "1";

                cBatchConsole.AppendText("Game " + i + "/" + games + "  Result=" + rs + "   " + score + Environment.NewLine);

                var hist = mGameManager.FieldHistory();
                int round = 0;
                foreach(string field in hist)
                {
                    if (res == GameManager.WIN0) hist_file.Write("0 ");
                    if (res == GameManager.WIN1) hist_file.Write("1 ");
                    if (res == GameManager.DRAW) hist_file.Write("d ");
                    hist_file.WriteLine(round.ToString("000") + " " + field);
                    round++;
                }
                hist_file.Flush();

                mGameManager.NextStartPosition();
            }

            hist_file.Close();
        }

        private void bNextPosition_Click(object sender, EventArgs e)
        {
            mGameManager.NextStartPosition();
        }
    }
}
