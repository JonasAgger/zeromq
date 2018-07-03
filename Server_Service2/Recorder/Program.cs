using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using NAudio.Wave;
using Timer = System.Timers.Timer;

namespace Recorder
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine(WaveIn.DeviceCount);
            
            var rec = new Recorder(0, "Test");
            //var rec = new RecordingPanel();
            var timer = new Timer(10000);
            timer.Elapsed += rec.RecordEnd;
            //timer.Elapsed += (object o, ElapsedEventArgs e) => rec.StopRecording();
            timer.AutoReset = false;

            //EventHandler handler = new EventHandler(rec.OnButtonStartRecordingClick);
            //handler.Invoke(new object(), new EventArgs());

            

            Console.WriteLine("Recording started");
            rec.StartRecording();
            timer.Start();
            while(rec.IsRunning)
            {
            }
            Console.WriteLine("Recording ended");
            
            var player = new Player();

            player.Play();

        }
    }
}
