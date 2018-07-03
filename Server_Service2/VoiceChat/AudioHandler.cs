using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NAudio.Wave;
using VoiceChat;

namespace Server_Service2
{
    class NetworkAudioSender : IDisposable
    {
        private readonly IAudioClient audioSender;
        private readonly WaveInEvent waveIn;

        public NetworkAudioSender(int inputDeviceNumber, IAudioClient audioSender)
        {
            this.audioSender = audioSender;
            waveIn = new WaveInEvent
            {
                BufferMilliseconds = 100,
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
            audioSender.Send(encoded);
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
        private readonly IAudioClient receiver;
        private readonly IWavePlayer waveOut;
        private readonly BufferedWaveProvider waveProvider;

        public NetworkAudioPlayer(IAudioClient receiver)
        {
            this.receiver = receiver;
            receiver.SetReceived(OnDataReceived);

            waveOut = new WaveOut();
            waveProvider = new BufferedWaveProvider(new WaveFormat(8000, 16, 2));
            waveOut.Init(waveProvider);
            waveOut.Play();
        }

        private void OnDataReceived(byte[] audioData)
        {
            waveProvider.AddSamples(audioData, 0, audioData.Length);
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
