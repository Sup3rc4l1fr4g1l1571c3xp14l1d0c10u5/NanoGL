/**
 * nwget - nano wget like tools in windows
 * 
 * How to compile without VisualStudio
 *   1. Perform either
 *      A) Open the "Command Prompt" from the Start menu . 
 *      B) Press the Windows key + R key , and enter 'cmd.exe' to shown dialog box.
 *   2. enter %windir%\Microsoft.NET\Framework\v3.5\csc.exe nwget.cs
 */

/**
 * 
 *  NYSL Version 0.9982
 * 
 * A. This software is "Everyone'sWare". It means:
 *   Anybody who has this software can use it as if he/she is
 *   the author.
 * 
 *   A-1. Freeware. No fee is required.
 *   A-2. You can freely redistribute this software.
 *   A-3. You can freely modify this software. And the source
 *       may be used in any software with no limitation.
 *   A-4. When you release a modified version to public, you
 *       must publish it with your name.
 * 
 * B. The author is not responsible for any kind of damages or loss
 *   while using or misusing this software, which is distributed
 *   "AS IS". No warranty of any kind is expressed or implied.
 *   You use AT YOUR OWN RISK.
 * 
 * C. Copyrighted to Kazuya Fukuhara
 * 
 * D. Above three clauses are applied both to source and binary
 *   form of this software.
 *
 */

namespace nwget
{
    static class nwget
    {
        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                System.Console.WriteLine("nwget <output> <URI>");
                return;
            }

            try
            {
                System.Uri remote;
                if (System.Uri.TryCreate(args[1], System.UriKind.Absolute, out remote) == false)
                {
                    System.Console.Error.WriteLine("**error: URI {0} is bad format.", args[1]);
                    System.Environment.Exit(-1);
                }
                System.Net.HttpWebRequest myHttpWebRequest = null;
                try
                {
                    myHttpWebRequest = (System.Net.HttpWebRequest)System.Net.WebRequest.Create(remote);
                }
                catch (System.Exception)
                {
                    System.Console.Error.WriteLine("**error: URI {0} is bad format or not supported.", args[1]);
                    System.Environment.Exit(-1);
                }
                myHttpWebRequest.MaximumAutomaticRedirections = 10;
                myHttpWebRequest.AllowAutoRedirect = true;

                System.Console.WriteLine("recive from {0}", remote.AbsoluteUri);
                var myHttpWebResponse = (System.Net.HttpWebResponse)myHttpWebRequest.GetResponse();
                using (var output = System.IO.File.OpenWrite(args[0]))
                using (var input = myHttpWebResponse.GetResponseStream())
                {
                    if (input != null)
                    {
                        var contentLength = myHttpWebResponse.ContentLength;
                        long reciveLength = 0;
                        var buffer = new byte[8192];
                        int bytesRead;
                        while ((bytesRead = input.Read(buffer, 0, buffer.Length)) > 0)
                        {
                            reciveLength += bytesRead;
                            output.Write(buffer, 0, bytesRead);
                            System.Console.Write("Recive: {0}  / {1} [byte] ({2}%)\r", reciveLength, contentLength, reciveLength * 100 / contentLength);
                        }
                        System.Console.WriteLine("\nRecive completed");
                        System.Environment.Exit(0);
                    }
                    else
                    {
                        System.Console.Error.WriteLine("**error: Cannot recive data.");
                        System.Environment.Exit(-1);
                    }
                }
            }
            catch (System.Net.WebException e)
            {
                if (e.Status == System.Net.WebExceptionStatus.ProtocolError)
                {
                    System.Net.HttpWebResponse myHttpWebResponse = (System.Net.HttpWebResponse)e.Response;
                    System.Console.Error.WriteLine("**error: {0} - {1}", myHttpWebResponse.StatusCode,
                        myHttpWebResponse.StatusDescription);
                    System.Environment.Exit(-1);
                } else if (e.Status == System.Net.WebExceptionStatus.Timeout)
                {
                    System.Console.Error.WriteLine("**error: Download is timeout.");
                    System.Environment.Exit(-1);
                }
                else
                {
                    ExceptionHandler(e, "Recive Failed.");
                }
            }
            catch (System.Exception e)
            {
                ExceptionHandler(e, "Recive Failed.");
            }
        }

        private static void ExceptionHandler(System.Exception e, string message)
        {
            System.Console.WriteLine("*error: exception has occurred.");
            System.Console.WriteLine("Message: {0}", e.Message);
            System.Console.WriteLine("StackTrace: {0}", e.StackTrace);
            System.Console.WriteLine(message);
            System.Environment.Exit(-1);
        }
    }
}
