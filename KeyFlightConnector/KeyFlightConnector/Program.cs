using System;
using System.Windows.Forms;

namespace KeyFlightConnector
{
    /// <summary>
    /// Main entry point for KeyFlight Connector application
    /// MobiFlight-inspired keyboard command configurator
    /// </summary>
    internal static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.SetHighDpiMode(HighDpiMode.SystemAware);

            // Show main form
            Application.Run(new Forms.MainForm());
        }
    }
}
