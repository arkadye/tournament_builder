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
            CommandLineArgs? cmdArgsRef = CheckCommandLine(args);
            if(cmdArgsRef.HasValue)
            {
                CommandLineArgs cmdArgs = cmdArgsRef.Value;
                if (cmdArgs.testflag && !string.IsNullOrEmpty(cmdArgs.input) && !string.IsNullOrEmpty(cmdArgs.output))
                {
                    Environment.ExitCode = TestExample(cmdArgs.input, cmdArgs.output, cmdArgs.seed);
                    Application.Exit();
                }
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

        private struct CommandLineArgs
        {
            public bool testflag;
            public string input;
            public string output;
            public long seed;
            public CommandLineArgs(bool tf, string i, string o, long s)
            {
                testflag = tf;
                input = i;
                output = o;
                seed = s;
            }
        }

        // Return true if we should continue. False otherwise.
        static private CommandLineArgs? CheckCommandLine(string[] args)
        {
            bool isset = false;
            bool testflag = false;
            string input = null;
            string output = null;
            string last_arg = null;
            long seed = 42;
            foreach(string arg in args)
            {
                if(arg == "-t" || arg == "--test")
                {
                    isset = true;
                    testflag = true;
                }
                else if(last_arg == "-i" || last_arg == "--input")
                {
                    isset = true;
                    input = arg;
                }
                else if(last_arg == "-o" || last_arg == "--output")
                {
                    isset = true;
                    output = arg;
                }
                else if(last_arg == "-s" || last_arg == "--random-seed")
                {
                    isset = true;
                    seed = Int64.Parse(arg);
                }

                last_arg = arg;
            }

            CommandLineArgs? result = null;
            if(isset)
            {
                result = new CommandLineArgs(testflag, input, output, seed);
            }

            return result;
        }

        static private int TestExample(string fileToTest, string referenceFile, long randomSeed)
        {
            string refPath = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, referenceFile));
            try
            {
                using (var referenceStream = File.Open(refPath, FileMode.Open, FileAccess.Read))
                {
                    var extraArgs = new TournamentBuilder.TourmanentBuilder.ExtraArgs();
                    extraArgs.RandomSeed = randomSeed;
                    extraArgs.JsonIndent = 2;
                    string testPath = Path.GetFullPath(Path.Combine(Environment.CurrentDirectory, fileToTest));
                    string testOutput = TournamentBuilder.TourmanentBuilder.MakeTournament(fileToTest, extraArgs);
                    JsonElement testResultJson = JsonSerializer.Deserialize<JsonElement>(testOutput);
                    JsonElement referenceJson = JsonSerializer.Deserialize<JsonElement>(referenceStream);
                    bool result = JsonElement.DeepEquals(testResultJson, referenceJson);
                    if (result)
                    {
                        Console.WriteLine($"SUCCESS: '{refPath}' produces '{testPath}' as expected");
                        return 0;
                    }
                    else
                    {
                        Console.WriteLine($"FAIL: '{refPath}' did not produce '{testPath}' as expected");
                        Console.WriteLine($"Expected:\n{referenceJson.ToString()}\nGot:\n{testResultJson.ToString()}");
                        return 1;
                    }
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine($"FAIL: Could not test '{fileToTest}' because reference '{referenceFile}' could not be opened: {ex.Message}");
                Console.WriteLine($"Error stack: {ex.StackTrace}");
                return 1;
            }
        }
    }
}
