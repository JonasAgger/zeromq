using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NAudio.Wave;

namespace Server_Service2
{
    class NetworkAudioSender : IDisposable
    {
        private readonly HwClient audioSender;
        private readonly WaveInEvent waveIn;

        public NetworkAudioSender(int inputDeviceNumber, HwClient audioSender)
        {
            this.audioSender = audioSender;
            waveIn = new WaveInEvent
            {
                BufferMilliseconds = 50,
                DeviceNumber = inputDeviceNumber,
                WaveFormat = new WaveFormat(8000, 16, 2)
            };
            waveIn.DataAvailable += OnAudioCaptured;
            waveIn.StartRecording();
        }

        void OnAudioCaptured(object sender, WaveInEventArgs e)
        {
            byte[] encoded = new byte[e.BytesRecorded];
            Array.Copy(e.Buffer, encoded, e.BytesRecorded);
            audioSender.SendData(encoded);
        }

        public void Dispose()
        {
            waveIn.DataAvailable -= OnAudioCaptured;
            waveIn.StopRecording();
            waveIn.Dispose();
            waveIn?.Dispose();
        }
    }

    class NetworkAudioPlayer : IDisposable
    {
        private readonly HwClient receiver;
        private readonly IWavePlayer waveOut;
        private readonly BufferedWaveProvider waveProvider;

        public NetworkAudioPlayer(HwClient receiver)
        {
            this.receiver = receiver;
            receiver.SetUp(OnDataReceived);

            waveOut = new WaveOut();
            waveProvider = new BufferedWaveProvider(new WaveFormat(8000, 16, 2));
            waveOut.Init(waveProvider);
            waveOut.Play();
        }

        private void OnDataReceived(object sender, ZeroMQEventArgs e)
        {
            var received = e.ReceivedZMessage[1].Read();
            waveProvider.AddSamples(received, 0, received.Length);
        }
        /*
        void OnDataReceived(byte[] compressed)
        {
            byte[] decoded = codec.Decode(compressed, 0, compressed.Length);
            waveProvider.AddSamples(decoded, 0, decoded.Length);
        }
        */
        public void Dispose()
        {
            waveOut?.Dispose();
        }
    }
}
