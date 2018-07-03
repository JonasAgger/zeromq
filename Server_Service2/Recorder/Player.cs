using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using NAudio.Wave;

namespace Recorder
{
    public class Player
    {
        private WaveOut wout;

        public Player()
        {
            
        }

        public void Play()
        {
            wout = new WaveOut();

            var audioData = new AudioFileReader("Test.wav");

            wout.Init(audioData);
            wout.Play();

            while (wout.PlaybackState == PlaybackState.Playing)
            {
                Thread.Sleep(250);
            }
        }
    }
}
