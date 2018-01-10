using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Windows.Forms;

namespace Strider3000API.Strider3000TopSystem
{
    class Servo
    {
        public string RxMessage;
        public string TxMessage;
        public delegate void DataRecived(object sender, SerialDataReceivedEventArgs e);
        public delegate bool RxListener(string rx);
        public delegate void PositionChangedCall(float pos);
        public delegate void CommandRecivedCall(string rx);

        List<RxListener> m_RxListenerContainer;
        List<PositionChangedCall> m_PositionChangeCallContainer;
        List<CommandRecivedCall> m_CommandRecivedCallContainer;

        SerialPort m_SerialPort;
        string RXContainer;
        float m_Position;
        Form m_Form;

        public Servo()
        {
            m_SerialPort = new SerialPort();
            m_RxListenerContainer = new List<RxListener>();
            m_CommandRecivedCallContainer = new List<CommandRecivedCall>();
            m_PositionChangeCallContainer = new List<PositionChangedCall>();

            m_RxListenerContainer.Add(PosListener);
            m_RxListenerContainer.Add(FeedbackListener);
        }

        public void AddRxListener(RxListener listener)
        {
            m_RxListenerContainer.Add(listener);
        }

        public void AddPositionChangeCallListener(PositionChangedCall listener)
        {
            m_PositionChangeCallContainer.Add(listener);
        }

        public void AddCommandRecivedCallListener(CommandRecivedCall listener)
        {
            m_CommandRecivedCallContainer.Add(listener);
        }

        public float GetPosition()
        {
            return m_Position;
        }

        public bool GetServoState()
        {
            return m_SerialPort.IsOpen;
        }

        public void Open(string portNumber, int baudRate, Form form)
        {
            m_Form = form;

            m_SerialPort.BaudRate = baudRate;
            m_SerialPort.PortName = portNumber;
            m_SerialPort.StopBits = StopBits.One;
            m_SerialPort.Parity = Parity.None;
            m_SerialPort.ReadBufferSize = 4096; // 4096이 가장 큰 값이다.
            m_SerialPort.ReadTimeout = 10000; // 기본값은 500m
            m_SerialPort.DataReceived += M_SerialPort_DataReceived;
            try
            {
                m_SerialPort.Open();
            }
            catch (UnauthorizedAccessException)
            {
                MessageBox.Show("포트에 액세스할 수 없습니다.", "Error", MessageBoxButtons.OK);
            }
            catch (ArgumentException)
            {
                MessageBox.Show("포트 이름이 올바르지 않습니다.", "Error", MessageBoxButtons.OK);
            }
            catch (InvalidOperationException)
            {
                MessageBox.Show("포트가 이미 열려있습니다.", "Error", MessageBoxButtons.OK);
            }
        }

        private void M_SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            m_Form.Invoke(new EventHandler(ListenRX)); //쓰레드 위반을 피하기 위함임.
        }

        public void Close()
        {
            m_SerialPort.Close();
        }

        public void PositionCall()
        { 
            m_SerialPort.Write(Convert.ToChar(0x02) + "POS 0,0,0,0" + Convert.ToChar(0x03));
        }

        public string MoveAbs(int mm, int speed)
        {
            string command = Convert.ToChar(0x02) + "ABS " + Convert.ToString(mm) +"," + speed + ",0,0" + Convert.ToChar(0x03);
            SendData(command);

            return command;
        }

        public string MoveInc(int mm, int speed)
        {
            string command = Convert.ToChar(0x02) + "INC " + Convert.ToString(mm) + "," + speed + ",0,0" + Convert.ToChar(0x03);
            SendData(command);

            return command;
        }

        public string MoveToHome()
        {
            string command = Convert.ToChar(0x02) + "HOM 0,0,0,0" + Convert.ToChar(0x03);
            SendData(command);

            return command;
        }

        public string MoveStop()
        {
            string command = Convert.ToChar(0x02) + "STP 0,0,0,0" + Convert.ToChar(0x03);
            SendData(command);

            return command;
        }

        public string SpeedChange(int speed)
        {
            string command = Convert.ToChar(0x02) + "SPD " + Convert.ToString(speed) + ",0,0,0" + Convert.ToChar(0x03) + "\r";
            SendData(command);

            return command;
        }

        private void SendData(string data)
        {
            TxMessage = data;
            m_SerialPort.Write(data);
        }

        public void ListenRX(object sender, EventArgs e)
        {
            RXContainer += m_SerialPort.ReadExisting();
            int length  = RXContainer.Length;

            if(length > 30)
            {
                RXContainer = null;
            }

            foreach(RxListener listener in m_RxListenerContainer)
            {
                if(listener(RXContainer) == false)
                {
                    RXContainer = null;
                    break;
                }
            }
        }

        private bool PosListener(string rx)
        {
            int i = rx.IndexOf("POS");
            int RxLeng = 0;

            if (i >= 0)
            {
                RxLeng = rx.IndexOf("\r");
                if (RxLeng >= 1)
                {
                    if ((i >= 0) && (i < RxLeng))
                    {
                        try
                        {
                            string tempStr = rx.Substring(i + 4, RxLeng - (i + 4)).Trim();
                            long original = Convert.ToInt64(tempStr);
                            long integerPart = original / 100;
                            long fixedPointPart = original % 100;


                            m_Position = original * 0.01f;
                            foreach (PositionChangedCall listener in m_PositionChangeCallContainer)
                            {
                                listener(m_Position);
                            }

                            RxMessage = integerPart.ToString() + '.' + fixedPointPart.ToString();
                            return false;
                        }
                        catch
                        {
                            return false;
                        }
                    }
                    return false;
                }
            }
            return true;
        }

        private bool FeedbackListener(string rx)
        {
            int RxLeng = 0;
            RxLeng = rx.IndexOf("\r");
            if (RxLeng >= 1)
            {
                foreach (CommandRecivedCall listener in m_CommandRecivedCallContainer)
                {
                    listener(rx);
                }
                RxMessage = rx;
                return false;
            }
            return true;
        }

    }
}
