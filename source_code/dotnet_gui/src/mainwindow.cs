using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Threading.Tasks;
using System.Windows.Forms;
using TournamentBuilder;

namespace tournament_builder_gui
{    public partial class mainwindow : Form
    {
        static public void Main(string[] args)
        {
            int? checkArgsResult = CheckCommandLine(args);
            if(checkArgsResult.HasValue)
            {
                Environment.ExitCode = checkArgsResult.Value;
                Application.Exit();
            }
            else
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new mainwindow());
            }
        }

        public mainwindow()
        {
            InitializeComponent();
        }

        // Return true if we should continue. False otherwise.
        static private int? CheckCommandLine(string[] args)
        {
            bool testflag = false;
            string input = null;
            string output = null;
            string last_arg = null;
            long seed = 42;
            Console.WriteLine("Args:");
            foreach(string arg in args)
            {
                Console.WriteLine("    {0}", arg);
                if(arg == "-t" || arg == "--test")
                {
                    testflag = true;
                }
                else if(last_arg == "-i" || last_arg == "--input")
                {
                    input = arg;
                }
                else if(last_arg == "-o" || last_arg == "--output")
                {
                    output = arg;
                }
                else if(last_arg == "-s" || last_arg == "--random-seed")
                {
                    seed = Int64.Parse(arg);
                }

                last_arg = arg;
            }

            if(testflag && !string.IsNullOrEmpty(input) && !string.IsNullOrEmpty(output))
            {
                string testResult = TestExample(input, output, seed);
                Console.WriteLine(testResult);
                return testResult.StartsWith("SUCCESS:") ? 0 : 1;
            }

            return null;
        }

        static private string TestExample(string fileToTest, string referenceFile, long randomSeed)
        {
            try
            {
                using (var referenceStream = File.Open(referenceFile, FileMode.Open, FileAccess.Read))
                {
                    var extraArgs = new TournamentBuilder.TourmanentBuilder.ExtraArgs();
                    extraArgs.RandomSeed = randomSeed;
                    string testOutput = TournamentBuilder.TourmanentBuilder.MakeTournament(fileToTest, extraArgs);
                    JsonElement referenceJson = JsonSerializer.Deserialize<JsonElement>(referenceStream);
                    bool result = referenceJson.ValueEquals(testOutput);
                    if (result)
                    {
                        return string.Format("SUCCESS: '{0}' produces '{1}' as expected", fileToTest, referenceFile);
                    }
                    else
                    {
                        return string.Format("FAIL: '{0}' did not produce '{1}' as expected", fileToTest, referenceFile);
                    }
                }
            }
            catch(Exception ex)
            {
                return string.Format("FAIL: Could not test '{0}' because reference '{1}' could not be opened: {2}", fileToTest, referenceFile, ex.Message);
            }
        }
    }
}
