using System;
using System.Runtime.InteropServices;
using System.Text;

namespace TournamentBuilder
{
	public static class TourmanentBuilder
	{
        [DllImport("tournament_builder_clib", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern void tournament_builder_make_tournament_to_file(
            [MarshalAs(UnmanagedType.LPStr)] string input,
            [MarshalAs(UnmanagedType.LPStr)] string output_filename);

        [DllImport("tournament_builder_clib", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr tournament_builder_make_tournament(
            [MarshalAs(UnmanagedType.LPStr)] string input);

        /*
		Params:
			input: Either a JSON string or a filename containing JSON representing a World.
		Returns:
			A string containing a JSON object representing a World. This must have free() called on it by the user.
	*/
        public static string MakeTournament(ref string input)
        {
            IntPtr makeResult = tournament_builder_make_tournament(input);
            string retResult = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(makeResult);
            return retResult;
        }

        /*
		Params:
			input: Either a JSON string or a filename containing JSON representing a World.
			output_filename: The file to write the output to.
	    */
        public static void MakeTournamentToFile(ref string input, ref string output_filename)
        {
            tournament_builder_make_tournament_to_file(input, output_filename);
        }
    }
}