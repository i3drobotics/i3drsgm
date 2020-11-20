using System;
using System.Diagnostics;

namespace I3DRSGMSharp {
    public class i3drsgmSharp
    {
        public static bool Run()
        {
            Console.WriteLine("Defining app process...");
            var proc = new Process 
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "C:/Code/I3DR/i3drsgm/install/i3drsgm/i3drsgm-1.0.6/bin/I3DRSGMApp.exe",
                    Arguments = "api",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    CreateNoWindow = true
                }
            };
            Console.WriteLine("Starting app...");
            proc.Start();
            Console.WriteLine("Reading app stdout...");
            while (!proc.StandardOutput.EndOfStream)
            {
                string line = proc.StandardOutput.ReadLine();
                Console.WriteLine(line);
            }
            return true;
        }
    }
}