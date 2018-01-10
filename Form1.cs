using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Strider3000API.Strider3000TopSystem;
using MotionSystems;
using System.IO.Ports;
using System.Runtime.InteropServices;
using System.Globalization;
using System.Timers;

namespace Strider3000API
{
    public partial class Form1 : Form
    {
        private Servo m_Servo;
        private float m_NegativeRange;
        private float m_PositiveRange;

        private ForceSeatMI m_MotionSystem;
        private FSMI_TopTablePositionPhysical m_MSTopTableData;
        private FSMI_PlatformInfo m_PlatformInfo;
        private delegate void MSTXListener(FSMI_TopTablePositionPhysical data);
        private delegate void MSRXListener(FSMI_PlatformInfo data);
        List<MSTXListener> m_MSTXListenerContainer;
        List<MSRXListener> m_MSRXListenerContainer;
        bool m_bPrevMotionSystemState = false;

        System.Windows.Forms.Timer m_MSSendDataTick;
        System.Windows.Forms.Timer m_Timer;
        System.Windows.Forms.Timer m_MotionSystemChecker;
        System.Windows.Forms.Timer m_UpdateMSInfo;
        System.Windows.Forms.Timer m_VelocityChecker;

        float m_Elapsedtime;

        float m_MSPitch;
        float m_MSRoll;
        float m_MSHeave;

        float m_MSHeaveValue;
        float m_MSRollRotation;
        float m_MSPitchRotation;

        float m_ServoZeroPosition;
        float m_MSZeroPosition;
        float m_ServoAndMSDistance;
        float m_CurServoAndMSDistance;

        float m_startHeave;

        public Form1()
        {
            InitializeComponent();
        }

        ~Form1()
        {
            m_Servo.Close();
            m_MotionSystem.EndMotionControl();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //Servo 세팅
            m_Servo = new Servo();

            cbSerialPort.DataSource = SerialPort.GetPortNames();  // 연결된 시리얼 포트 리스트 표시
            cbBaudRate.SelectedIndex = 6;                         //38400  
            tbPortState.Text = "닫힘";
            tbSyncPortState.Text = "닫힘";

            m_Servo.AddPositionChangeCallListener(this.PositionChangeListener);
            m_Servo.AddCommandRecivedCallListener(this.CommandReciveListener);
            //

            //Motion System 세팅
            m_MotionSystem = new ForceSeatMI();
           
            m_PlatformInfo = new FSMI_PlatformInfo();
            m_MSTopTableData = new FSMI_TopTablePositionPhysical();
            m_MSTopTableData.mask = 0;


            m_MSTopTableData.structSize = (byte)Marshal.SizeOf(m_MSTopTableData);
            m_MSTopTableData.mask = FSMI_POS_BIT.STATE | FSMI_POS_BIT.MATRIX | FSMI_POS_BIT.MAX_SPEED;

            m_VelocityChecker = new System.Windows.Forms.Timer();
            m_VelocityChecker.Interval = 1;
            m_VelocityChecker.Tick += new EventHandler(VelocityChecker);

            m_MSSendDataTick = new System.Windows.Forms.Timer();
            m_MSSendDataTick.Interval = 25;
            m_MSSendDataTick.Tick += new EventHandler(SendDataTick);

            m_MSTXListenerContainer = new List<MSTXListener>();
            m_MSTXListenerContainer.Add(delegate (FSMI_TopTablePositionPhysical data)
                {
                    tbTxHeave.Text = m_MSHeave.ToString();
                    tbTxPitch.Text = m_MSPitch.ToString();
                    tbTxRoll.Text = m_MSRoll.ToString();

                    //스크롤바 방향 변경
                    vsbHeave.Value = Convert.ToInt32(m_MSHeave) * -1;
                    vsbPitch.Value = Convert.ToInt32(m_MSPitch) * -1;
                    vsbRoll.Value = Convert.ToInt32(m_MSRoll) * -1;
                }
            );

            m_MSRXListenerContainer = new List<MSRXListener>();
            m_MSRXListenerContainer.Add(delegate (FSMI_PlatformInfo rx)
                {
                    tbRxHeave.Text = rx.fkHeave.ToString();
                    tbRxPitch.Text = RadianToDegree(rx.fkPitch).ToString();
                    tbRxRoll.Text = RadianToDegree(rx.fkRoll).ToString();
                }
            );
            //

            m_MotionSystemChecker = new System.Windows.Forms.Timer();
            m_MotionSystemChecker.Interval = 500;
            m_MotionSystemChecker.Tick += new EventHandler(MotionSystemCheckerFunc);
            m_MotionSystemChecker.Start();

            m_UpdateMSInfo = new System.Windows.Forms.Timer();
            m_UpdateMSInfo.Interval = 500;
            m_UpdateMSInfo.Tick += new EventHandler(delegate (object _sender, EventArgs _e)
                {
                    m_MotionSystem.GetPlatformInfoEx(ref m_PlatformInfo, (uint)Marshal.SizeOf(m_PlatformInfo), 1000);
                    foreach(MSRXListener listener in m_MSRXListenerContainer)
                    {
                        listener(m_PlatformInfo);
                    }

                    m_CurServoAndMSDistance = Math.Abs((m_Servo.GetPosition() - m_PlatformInfo.fkHeave));
                   
                }
            );
        }

        void VelocityChecker(object sender, EventArgs e)
        {
            m_Elapsedtime += 1;
        }

        // 쓰레드풀의 작업 스레드가 지정된 시간 간격으로
        // 아래 이벤트 핸들러 실행
        void Tick(object sender, EventArgs e)
        {
            m_Servo.PositionCall();
        }

        void MotionSystemCheckerFunc(object sender, EventArgs e)
        {
            if (m_MotionSystem.IsLoaded() == false)
            {
                m_bPrevMotionSystemState = false;

                tbMotionSystemState.Text = "닫힘";
                tbSyncMotionSystemState.Text = "닫힘";

                tbSyncMove.Enabled = false;
                tbSyncSpeed.Enabled = false;
                hsbSyncSpeed.Enabled = false;
                btSyncAbs.Enabled = false;
                btSyncInc.Enabled = false;
                btSyncSpeed.Enabled = false;
                btSyncStop.Enabled = false;
                btSyncHome.Enabled = false;

                m_MotionSystem.EndMotionControl();

                m_UpdateMSInfo.Stop();
                m_MSSendDataTick.Stop();
            }
            else if(m_bPrevMotionSystemState == false)
            {
                tbMotionSystemState.Text = "열림";
                tbSyncMotionSystemState.Text = "열림";

                tbSyncMove.Enabled = true;
                tbSyncSpeed.Enabled = true;
                hsbSyncSpeed.Enabled = true;
                btSyncAbs.Enabled = true;
                btSyncInc.Enabled = true;
                btSyncSpeed.Enabled = true;
                btSyncStop.Enabled = true;
                btSyncHome.Enabled = true;

                m_bPrevMotionSystemState = true;
                m_UpdateMSInfo.Start();

                m_MSTopTableData.mask = FSMI_POS_BIT.STATE | FSMI_POS_BIT.MATRIX | FSMI_POS_BIT.MAX_SPEED;
                m_MotionSystem.BeginMotionControl();
                m_MSSendDataTick.Start();
            }
        }

        #region Servo Tab

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkboxAutoSpeed.Checked)
            {
                btSpeed.Enabled = false;
            }
            else
            {
                btSpeed.Enabled = true;
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            tbTXD.Text = m_Servo.MoveToHome();
        }

        private void btOpen_Click(object sender, EventArgs e)
        {
            m_Servo.Open(cbSerialPort.Text, Convert.ToInt32(cbBaudRate.Text), this);
            tbPortState.Text = "열림";
            tbSyncPortState.Text = "열림";

            m_Timer = new System.Windows.Forms.Timer();
            m_Timer.Interval = 100;
            m_Timer.Tick += new EventHandler(Tick);
            m_Timer.Start();

            tbStretch.Enabled = true;
            tbSpeed.Enabled = true;
            hsbSpeed.Enabled = true;
            btAbsMove.Enabled = true;
            btIncMove.Enabled = true;
            btSpeed.Enabled = true;
            btStop.Enabled = true;
            btHome.Enabled = true;
        }

        private void button7_Click(object sender, EventArgs e)
        {
            m_Servo.Close();
            tbPortState.Text = "닫힘";
            tbSyncPortState.Text = "닫힘";

            tbStretch.Enabled = false;
            tbSpeed.Enabled = false;
            hsbSpeed.Enabled = false;
            btAbsMove.Enabled = false;
            btIncMove.Enabled = false;
            btSpeed.Enabled = false;
            btStop.Enabled = false;
            btHome.Enabled = false;

            m_Timer.Stop();
        }

        private void btAbsMove_Click(object sender, EventArgs e)
        {
            tbTXD.Text = m_Servo.MoveAbs(Convert.ToInt32(tbStretch.Text), Convert.ToInt32(tbSpeed.Text));
        }

        private void btIncMove_Click(object sender, EventArgs e)
        {
            tbTXD.Text = m_Servo.MoveInc(Convert.ToInt32(tbStretch.Text), Convert.ToInt32(tbSpeed.Text));
        }

        private void btSpeed_Click(object sender, EventArgs e)
        {
            tbTXD.Text = m_Servo.SpeedChange(Convert.ToInt32(tbSpeed.Text));
            tbCurSpeed.Text = tbSpeed.Text;
        }

        private void btStop_Click(object sender, EventArgs e)
        {
            tbTXD.Text = m_Servo.MoveStop();
        }

        private void hScrollBar1_Scroll(object sender, ScrollEventArgs e)
        {
            tbSpeed.Text = hsbSpeed.Value.ToString();
            if (checkboxAutoSpeed.Checked)
            {
                m_Servo.SpeedChange(hsbSpeed.Value);
                tbCurSpeed.Text = tbSpeed.Text;
            }
        }

        private void CommandReciveListener(string rx)
        {
            rtbRXD.Text = rtbRXD.Text + rx;
            rtbRXD.SelectionStart = rtbRXD.Text.Length;
            rtbRXD.ScrollToCaret();
        }

        public void PositionChangeListener(float pos)
        {
            tbCurPos.Text = pos.ToString();
            m_CurServoAndMSDistance = Math.Abs(pos - m_PlatformInfo.fkHeave);
        }

        private void tbNagRange_TextChanged(object sender, EventArgs e)
        {
            bool isNum = float.TryParse(tbNagRange.Text, out m_NegativeRange);
            if (!isNum) return;

            if(m_NegativeRange != 0 && m_PositiveRange != 0)
            {
                tbStretch.Enabled = true;
                tbSpeed.Enabled = true;
                hsbSpeed.Enabled = true;
                btAbsMove.Enabled = true;
                btIncMove.Enabled = true;
                btSpeed.Enabled = true;
                btStop.Enabled = true;
                btHome.Enabled = true;

                tbSyncMove.Enabled = true;
                tbSyncSpeed.Enabled = true;
                hsbSyncSpeed.Enabled = true;
                btSyncAbs.Enabled = true;
                btSyncInc.Enabled = true;
                btSyncSpeed.Enabled = true;
                btSyncStop.Enabled = true;
                btSyncHome.Enabled = true;
            }
        }

        private void tbPosiRange_TextChanged(object sender, EventArgs e)
        {
            bool isNum = float.TryParse(tbPosiRange.Text, out m_PositiveRange);
            if (!isNum) return;

            if (m_NegativeRange != 0 && m_PositiveRange != 0)
            {
                tbStretch.Enabled = true;
                tbSpeed.Enabled = true;
                hsbSpeed.Enabled = true;
                btAbsMove.Enabled = true;
                btIncMove.Enabled = true;
                btSpeed.Enabled = true;
                btStop.Enabled = true;
                btHome.Enabled = true;

                tbSyncMove.Enabled = true;
                tbSyncSpeed.Enabled = true;
                hsbSyncSpeed.Enabled = true;
                btSyncAbs.Enabled = true;
                btSyncInc.Enabled = true;
                btSyncSpeed.Enabled = true;
                btSyncStop.Enabled = true;
                btSyncHome.Enabled = true;
            }
        }

        #endregion

        #region MotionSystem

        private void SendDataTick(object sender, EventArgs e)
        {
            MoveMotionSystem();
        }

        private void btPitch_Front_Click(object sender, EventArgs e)
        {
            m_MSPitch += m_MSPitchRotation;
            if (m_MSPitch > 17)
            {
                m_MSPitch = 17;
            }
            m_MSTopTableData.pitch = DegreeToRadian(m_MSPitch);
        }

        private void btPitch_Back_Click(object sender, EventArgs e)
        {
            m_MSPitch -= m_MSPitchRotation;
            if (m_MSPitch < -17)
            {
                m_MSPitch = -17;
            }
            m_MSTopTableData.pitch = DegreeToRadian(m_MSPitch);
        }

        private void btRoll_Right_Click(object sender, EventArgs e)
        {
            m_MSRoll += m_MSRollRotation;
            if (m_MSRoll > 20)
            {
                m_MSRoll = 20;
            }
            m_MSTopTableData.roll = DegreeToRadian(m_MSRoll);
        }

        private void btRoll_Left_Click(object sender, EventArgs e)
        {
            m_MSRoll -= m_MSRollRotation;
            if (m_MSRoll < -20)
            {
                m_MSRoll = -20;
            }
            m_MSTopTableData.roll = DegreeToRadian(m_MSRoll);
        }

        private void btHeaveUp_Click(object sender, EventArgs e)
        {
            m_MSHeave += m_MSHeaveValue;
            if (m_MSHeave > 133)
            {
                m_MSHeave = 133;
            }
            m_MSTopTableData.heave = m_MSHeave;
        }

        private void btHeaveDown_Click(object sender, EventArgs e)
        {
            m_MSHeave -= m_MSHeaveValue;
            if (m_MSHeave < -133)
            {
                m_MSHeave = -133;
            }
            m_MSTopTableData.heave = DegreeToRadian(m_MSHeave);
        }

        private float DegreeToRadian(float angle)
        {
            return (float)(Math.PI * angle / 180.0);
        }

        private float RadianToDegree(float radian)
        {
            return (float)(radian * (180.0 / Math.PI));
        }

        private void vsbHeave_Scroll(object sender, ScrollEventArgs e)
        {
            //스크롤바 방향 변경
            m_MSHeave = (float)vsbHeave.Value * -1;
            m_MSTopTableData.heave = m_MSHeave;
        }

        private void vsbPitch_Scroll(object sender, ScrollEventArgs e)
        {
            //스크롤바 방향 변경
            m_MSPitch = (float)vsbPitch.Value * -1;
            m_MSTopTableData.pitch = DegreeToRadian(m_MSPitch);
        }

        private void vsbRoll_Scroll(object sender, ScrollEventArgs e)
        {
            //스크롤바 방향 변경
            m_MSRoll = (float)vsbRoll.Value * -1;
            m_MSTopTableData.roll = DegreeToRadian(m_MSRoll);
        }

        private void tbRotateSync_TextChanged(object sender, EventArgs e)
        {
            float rotatement = float.Parse(tbRotateSync.Text, CultureInfo.InvariantCulture.NumberFormat);
            if (rotatement > 17)
            {
                rotatement = 17;
                tbRotateSync.Text = rotatement.ToString();
            }

            tbPitchRotateValue.Text = tbRotateSync.Text;
            tbRollRotateValue.Text = tbRotateSync.Text;

            m_MSRollRotation = rotatement;
            m_MSPitchRotation = rotatement;
        }

        private void tbHeaveValue_TextChanged(object sender, EventArgs e)
        {
            m_MSHeaveValue = float.Parse(tbHeaveValue.Text, CultureInfo.InvariantCulture.NumberFormat);
            if (m_MSHeaveValue > 133)
            {
                m_MSHeaveValue = 133;
                tbHeaveValue.Text = m_MSHeaveValue.ToString();
            }
        }

        private void tbPitchRotateValue_TextChanged(object sender, EventArgs e)
        {
            m_MSPitchRotation = float.Parse(tbPitchRotateValue.Text, CultureInfo.InvariantCulture.NumberFormat);
            if (m_MSPitchRotation > 17)
            {
                m_MSPitchRotation = 17;
                tbPitchRotateValue.Text = m_MSPitchRotation.ToString();
            }
        }

        private void tbRollRotateValue_TextChanged(object sender, EventArgs e)
        {
            m_MSRollRotation = float.Parse(tbRollRotateValue.Text, CultureInfo.InvariantCulture.NumberFormat);
            if (m_MSRollRotation > 20)
            {
                m_MSRollRotation = 20;
                tbRollRotateValue.Text = m_MSRollRotation.ToString();
            }
        }

        float prevHeave;
        private void MoveMotionSystem()
        {
           // m_MSTopTableData.mask

            if((m_MSTopTableData.heave - prevHeave) > 0)
            {
                m_MSTopTableData.heave += 1.301901f;
            }

            m_MotionSystem.SendTopTablePosPhy(ref m_MSTopTableData);
            foreach (MSTXListener listener in m_MSTXListenerContainer)
            {
                listener(m_MSTopTableData);
            }

            m_MotionSystem.GetPlatformInfoEx(ref m_PlatformInfo, (uint)Marshal.SizeOf(m_PlatformInfo), 1000);
            foreach (MSRXListener listener in m_MSRXListenerContainer)
            {
                listener(m_PlatformInfo);
            }

            prevHeave = m_MSTopTableData.heave;
        }

        private void tbMaxSpeed_TextChanged(object sender, EventArgs e)
        {
            int maxSpeed = Convert.ToInt32(tbMaxSpeed.Text);
            if (maxSpeed > 64000)
            {
                m_MSTopTableData.maxSpeed = 64000;
                tbMaxSpeed.Text = "64000";
            }
            else if (maxSpeed < 0)
            {
                m_MSTopTableData.maxSpeed = 0;
                tbMaxSpeed.Text = "0";
            }
            else
            {
                m_MSTopTableData.maxSpeed = (ushort)maxSpeed;
            }
        }

        #endregion

        float m_MSSpeedConst = 64000 / 100;

        #region Sync

        System.Windows.Forms.Timer m_ErrorChecker = null;
        float m_LimitTopPos;
        float m_LimitBottomPos;
        int m_Ratio;

        #endregion

        private void btZeroSetting_Click(object sender, EventArgs e)
        {
            if(!String.IsNullOrEmpty(tbSyncServoZeroPos.Text) && !tbSyncServoZeroPos.Text.Equals("OUT OF RANGE"))
            {
                if(!String.IsNullOrEmpty(tbSyncMsZeroPos.Text) && !tbSyncServoZeroPos.Text.Equals("OUT OF RANGE"))
                {
                    checkbox_IsZero.Checked = true;
                    m_ServoAndMSDistance = Math.Abs(m_ServoZeroPosition - m_MSZeroPosition);

                    if(m_NegativeRange < 133)
                    {
                        m_LimitBottomPos = m_NegativeRange;
                    }
                    else
                    {
                        m_LimitBottomPos = -133;
                    }

                    if(m_PositiveRange < 133)
                    {
                        m_LimitTopPos = m_PositiveRange;
                    }
                    else
                    {
                        m_LimitBottomPos = 133;
                    }


                    m_Servo.MoveAbs((int)m_ServoZeroPosition, 10);
                    m_MSTopTableData.heave = m_MSZeroPosition;
                    m_MSTopTableData.maxSpeed = 12000;
                }
            }
        }

        private void tbSyncZeroPos_TextChanged(object sender, EventArgs e)
        {
            bool isNum = float.TryParse(tbSyncServoZeroPos.Text, out m_ServoZeroPosition);
            if(isNum)
            {
                if (m_ServoZeroPosition > m_PositiveRange)
                {
                    tbSyncServoZeroPos.Text = "OUT OF RANGE";
                }
                else if(m_ServoZeroPosition < m_NegativeRange)
                {
                    tbSyncServoZeroPos.Text = "OUT OF RANGE";
                }
            }
        }

        private void SyncMoveAbs(int mm, int speed)
        {
            int servoMoement = (int)m_ServoZeroPosition + mm;
            float msMovement = m_MSZeroPosition + mm;

            if (servoMoement < m_NegativeRange)
            {
                servoMoement = (int)m_NegativeRange;
                msMovement   = m_NegativeRange;
            }
            else if (servoMoement > m_PositiveRange)
            {
                servoMoement = (int)m_PositiveRange;
                msMovement   = m_PositiveRange;
            }
            else if (msMovement < m_NegativeRange)
            {
                servoMoement = (int)m_NegativeRange;
                msMovement = m_NegativeRange;
            }
            else if (msMovement > m_PositiveRange)
            {
                servoMoement = (int)m_PositiveRange;
                msMovement = m_PositiveRange;
            }

            m_Servo.MoveAbs(servoMoement, speed);

            //64000 / ((speed / 100) * m_Ratio)
            m_MSTopTableData.maxSpeed = (ushort)((m_MSSpeedConst - m_Ratio) * speed);
            m_MSTopTableData.heave = msMovement; 
        }

        private void SyncMoveInc(int mm, int speed)
        {
            int servoMoement = (int)m_Servo.GetPosition() + mm;
            float msMovement = m_MSTopTableData.heave + mm;

            if (servoMoement < m_NegativeRange)
            {
                servoMoement = (int)m_NegativeRange;
                msMovement = m_NegativeRange;
            }
            else if (servoMoement > m_PositiveRange)
            {
                servoMoement = (int)m_PositiveRange;
                msMovement = m_PositiveRange;
            }
            else if (msMovement < m_NegativeRange)
            {
                servoMoement = (int)m_NegativeRange;
                msMovement = m_NegativeRange;
            }
            else if (msMovement > m_PositiveRange)
            {
                servoMoement = (int)m_PositiveRange;
                msMovement = m_PositiveRange;
            }

            m_Servo.MoveAbs(servoMoement, speed);

            m_MSTopTableData.maxSpeed = (ushort)((m_MSSpeedConst * speed) - m_Ratio);
            m_MSTopTableData.heave = msMovement;
        }

        private void SyncMoveStop()
        {
            m_Servo.MoveStop();
            m_MSTopTableData.heave = m_PlatformInfo.fkHeave;
        }

        private void SyncChangeSpeed(int speed)
        {
            m_Servo.SpeedChange(speed);

            m_MSTopTableData.maxSpeed = (ushort)(speed * m_Ratio);
        }

        private void SyncMoveZero()
        {
            SyncMoveAbs(0, 10);
        }

        private void tbSyncMsZeroPos_TextChanged(object sender, EventArgs e)
        {
            bool isNum = float.TryParse(tbSyncMsZeroPos.Text, out m_MSZeroPosition);
            if (isNum)
            {
                if (m_MSZeroPosition < -133)
                {
                    tbSyncMsZeroPos.Text = "OUT OF RANGE";
                }
                else if (m_MSZeroPosition > 133)
                {
                    tbSyncMsZeroPos.Text = "OUT OF RANGE";
                }
            }
        }

        private void checkBox_checkError_CheckedChanged(object sender, EventArgs e)
        {
            if(m_ErrorChecker == null)
            {
                m_ErrorChecker = new System.Windows.Forms.Timer();
                m_ErrorChecker.Interval = 500;
                m_ErrorChecker.Tick += new EventHandler(delegate(object _sender, EventArgs _e)
                    {
                        tbSyncServoMSDistance.Text = (m_CurServoAndMSDistance - m_ServoAndMSDistance).ToString();
                    }
                );
                m_ErrorChecker.Start();
            }
            else if(checkBox_checkError.Checked)
            {
                m_ErrorChecker.Start();
            }
            else
            {
                m_ErrorChecker.Stop();
            }
        }

        private void tbDebug_TextChanged(object sender, EventArgs e)
        {
            try
            {
                m_Ratio = Convert.ToInt32(tbDebug.Text);
            }
            catch
            {
                m_Ratio = 1;
            }
        }

        private void hsbDebug_Scroll(object sender, ScrollEventArgs e)
        {
            m_Ratio = hsbDebug.Value;
            tbDebug.Text = m_Ratio.ToString();
        }

        private void btSyncAbs_Click(object sender, EventArgs e)
        {
            int mm = 0;
            int speed = 0;

            bool isNumMM = int.TryParse(tbSyncMove.Text, out mm);
            bool isNumSpeed = int.TryParse(tbSyncSpeed.Text, out speed);
            if(isNumMM && isNumSpeed)
            {
                SyncMoveAbs(mm,speed);
            }
        }

        private void btSyncInc_Click(object sender, EventArgs e)
        {
            int mm = 0;
            int speed = 0;

            bool isNumMM = int.TryParse(tbSyncMove.Text, out mm);
            bool isNumSpeed = int.TryParse(tbSyncSpeed.Text, out speed);
            if (isNumMM && isNumSpeed)
            {
                SyncMoveInc(mm, speed);
            }
        }

        private void btSyncSpeed_Click(object sender, EventArgs e)
        {
            int speed = 0;

            bool isNumSpeed = int.TryParse(tbSyncSpeed.Text, out speed);
            if (isNumSpeed)
            {
                SyncChangeSpeed(speed);
            }
        }

        private void btSyncStop_Click(object sender, EventArgs e)
        {
            SyncMoveStop();
        }

        private void btSyncHome_Click(object sender, EventArgs e)
        {
            SyncMoveZero();
        }
    }
}
