using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Mime;
using System.Text;
using System.Threading.Tasks;
using NAudio.CoreAudioApi;
using NAudio.Wave;

namespace Recorder
{
    
    public class Recorder
    {

        private IWaveIn _sourceStream;
        WaveFileWriter _waveWriter;
        readonly String _fileName;
        private readonly int _inputDeviceIndex;
        private object locker = new object();
        private bool _isRunning = true;
        public bool IsRunning
        {
            get
            {
                lock (locker)
                    return _isRunning;

            }
            set
            {
                lock (locker)
                    _isRunning = value;
            }
        }

        public Recorder(int inputDeviceIndex, String fileName)
        {
            this._inputDeviceIndex = inputDeviceIndex;
            this._fileName = fileName;
        }

        public void StartRecording()
        {
            Console.WriteLine("Setting up");
            _sourceStream = CreateWaveInDevice();

            var path = Directory.GetCurrentDirectory();

            _waveWriter = new WaveFileWriter(path + "/" + _fileName + ".wav", _sourceStream.WaveFormat);
            Console.WriteLine("Starting...");
            IsRunning = true;
            _sourceStream.StartRecording();
        }

        public void SourceStreamDataAvailable(object sender, WaveInEventArgs e)
        {
            if (_waveWriter == null) return;
            _waveWriter.Write(e.Buffer, 0, e.BytesRecorded);
            _waveWriter.Flush();
        }

        public void RecordEnd(object sender, EventArgs e)
        {
            if (_sourceStream != null)
            {
                _sourceStream.StopRecording();
                _sourceStream.Dispose();
                _sourceStream = null;
            }
            if (this._waveWriter == null)
            {
                return;
            }
            this._waveWriter.Dispose();
            this._waveWriter = null;
            IsRunning = false;
        }

        private IWaveIn CreateWaveInDevice()
        {
            IWaveIn newWaveIn;
            var deviceNumber = 0;
            //newWaveIn = new WaveIn() { DeviceNumber = deviceNumber };
            newWaveIn = new WaveInEvent() { DeviceNumber = deviceNumber };
            var sampleRate = 44100;
            var channels = 2;
            newWaveIn.WaveFormat = new WaveFormat(sampleRate, channels);
            newWaveIn.DataAvailable += SourceStreamDataAvailable;
            newWaveIn.RecordingStopped += RecordEnd;
            return newWaveIn;
        }
    }
}
