using System;
using System.IO;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace I3DRSGMSharp
{
    public class i3drsgmSharp
    {
        private string left_cal_filepath;
        private string right_cal_filepath;
        private string output_folder;
        private Process appProcess;

        public i3drsgmSharp(string left_cal_filepath, string right_cal_filepath, string output_folder, string app_path = null)
        {
            this.left_cal_filepath = left_cal_filepath;
            this.right_cal_filepath = right_cal_filepath;
            this.output_folder = output_folder;

            if (app_path == null)
            {
                string cwd = Directory.GetCurrentDirectory();
                app_path = cwd + "/I3DRSGMApp.exe";
            }
            Console.WriteLine(app_path);
            this.appProcess = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = app_path,
                    Arguments = "api",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardInput = true,
                    CreateNoWindow = true
                }
            };
            Console.WriteLine("Starting app...");
            this.appProcess.Start();
            string response;
            this.apiRequest("INIT", out response);
        }

        private static string removePrefix(string text, string prefix)
        {
            return text.StartsWith(prefix) ? text.Substring(prefix.Length) : text;
        }

        public bool apiWaitResponse(out string response)
        {
            Console.WriteLine("Reading app stdout...");
            while (true)
            {
                string line = this.appProcess.StandardOutput.ReadLine();
                if (line != null)
                {
                    string line_norm = Regex.Replace(line, @"\s", "");
                    Console.WriteLine(line_norm);
                    if (line_norm == "API_READY")
                    {
                        response = line;
                        return true;
                    }
                    else if (line_norm.StartsWith("API_RESPONSE:"))
                    {
                        string api_response = removePrefix(line_norm, "API_RESPONSE:");
                        if (api_response.StartsWith("ERROR,"))
                        {
                            string error_msg = removePrefix(api_response, "API_RESPONSE:");
                            response = error_msg;
                            return false;
                        }
                        else
                        {
                            response = api_response;
                            return true;
                        }
                    }
                    else
                    {
                        Console.WriteLine("stout:" + line_norm);
                    }
                }
            }
        }

        public bool apiRequest(string request, out string response)
        {
            bool valid = this.apiWaitResponse(out response);
            if (valid)
            {
                Console.WriteLine("sending api request...");
                this.appProcess.StandardInput.WriteLine(request);
                this.appProcess.StandardInput.Flush();
                Console.WriteLine("waiting for api response...");
                valid = this.apiWaitResponse(out response);
            }
            return valid;
        }

        public bool forwardMatchFiles(string left_filepath, string right_filepath)
        {
            string appOptions = "FORWARD_MATCH," + left_filepath + "," + right_filepath + "," + this.left_cal_filepath + "," + this.right_cal_filepath + "," + this.output_folder + ",0,1";
            string response;
            bool valid = this.apiRequest(appOptions, out response);
            Console.WriteLine(response);
            return valid;
        }

        public bool setMinDisparity(int val)
        {
            string appOptions = "SET_MIN_DISPARITY," + val.ToString();
            string response;
            bool valid = this.apiRequest(appOptions, out response);
            Console.WriteLine(response);
            return valid;
        }

        public bool setDisparityRange(int val)
        {
            string appOptions = "SET_DISPARITY_RANGE," + val.ToString();
            string response;
            bool valid = this.apiRequest(appOptions, out response);
            Console.WriteLine(response);
            return valid;
        }

        public void close()
        {
            Console.WriteLine("Closing I3DRSGM process...");
            this.appProcess.Kill();
            Console.WriteLine("I3DRSGM process closed.");
        }
    }
}