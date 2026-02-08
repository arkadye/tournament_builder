using System;
using System.Runtime.InteropServices;
using System.Text;

namespace TournamentBuilder
{
	public static class TourmanentBuilder
	{
        public struct ExtraArgs
        {
            public int? JsonIndent;
        };

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        private struct ExtraArgsImpl
        {
            public ExtraArgsImpl(ExtraArgs args) : this(args.JsonIndent)
            {}

            ExtraArgsImpl(int? initialIndent)
            {
                JsonIndent = initialIndent.GetValueOrDefault(-1);
            }
            public int JsonIndent;
        }

        private static IntPtr GetMarshalledArgs(ExtraArgs? args)
        {
            if(args is null)
            {
                return IntPtr.Zero;
            }
            ExtraArgsImpl argsImpl = new ExtraArgsImpl(args.Value);
            IntPtr result = Marshal.AllocCoTaskMem(Marshal.SizeOf(argsImpl));
            Marshal.StructureToPtr(argsImpl, result, false);
            return result;
        }

        [DllImport("tournament_builder_clib", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void tournament_builder_make_tournament_to_file(
            [MarshalAs(UnmanagedType.LPStr)] string input,
            [MarshalAs(UnmanagedType.LPStr)] string output_filename);

        [DllImport("tournament_builder_clib", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern void tournament_builder_make_tournament_to_file_ext(
            [MarshalAs(UnmanagedType.LPStr)] string input,
            [MarshalAs(UnmanagedType.LPStr)] string output_filename,
            IntPtr extra_args);

        [DllImport("tournament_builder_clib", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr tournament_builder_make_tournament(
            [MarshalAs(UnmanagedType.LPStr)] string input);

        [DllImport("tournament_builder_clib", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr tournament_builder_make_tournament_ext(
            [MarshalAs(UnmanagedType.LPStr)] string input, IntPtr extra_args);

        /*
		Params:
			input: Either a JSON string or a filename containing JSON representing a World.
		Returns:
			A string containing a JSON object representing a World. This must have free() called on it by the user.
	*/
        public static string MakeTournament(string input, ExtraArgs? extra_args = null)
        {
            IntPtr extraArgsPtr = GetMarshalledArgs(extra_args);
            IntPtr makeResult = tournament_builder_make_tournament_ext(input, extraArgsPtr);
            Marshal.FreeCoTaskMem(extraArgsPtr);
            string retResult = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(makeResult);
            return retResult;
        }

        /*
		Params:
			input: Either a JSON string or a filename containing JSON representing a World.
			output_filename: The file to write the output to.
	    */
        public static void MakeTournamentToFile(string input, string output_filename, ExtraArgs? extra_args = null)
        {
            IntPtr extraArgsPtr = GetMarshalledArgs(extra_args);
            tournament_builder_make_tournament_to_file_ext(input, output_filename, extraArgsPtr);
            Marshal.FreeCoTaskMem(extraArgsPtr);
        }
    }
}