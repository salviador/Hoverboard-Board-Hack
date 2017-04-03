using Android.App;
using Android.Widget;
using Android.OS;
using Android.Content;
using Android.Bluetooth;
using System;
using Android.Runtime;
using Android.Views;
using System.Collections.Generic;
using System.Threading;
using Android.Bluetooth.LE;
using Android.Util;
using wheel.Resources;
using Java.Util;
using System.Linq;


namespace wheel
{
    [Activity(Label = "wheel")]
    public class joystickapp : Activity
    {
        private TextView t;

        private BluetoothUUID myUUID;
        private BluetoothConnessione ble;

        private BluetoothDevice device;
        private BluetoothServiceDevice joyBLE;
        private BluetoothServiceDevice telemetryBLE; 

        private Activity activity;

        private ControllerView controller_view;
        private TextView battery_textview;
        private TextView current_mL_textview;
        private TextView current_mR_textview;

        private string Quale_Layout = String.Empty;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);

            activity = this;

            this.device = (BluetoothDevice)Intent.GetParcelableExtra("wheellobjBle");
            SetContentView(Resource.Layout.Connection);

            t = FindViewById<TextView>(Resource.Id.textViewNameConnect);
            t.Text = device.Name;
            t.SetTextColor(Android.Graphics.Color.Cyan);

            ble = new BluetoothConnessione(this, this, null);
            myUUID = new BluetoothUUID();

            if (this.device != null)
            {
                ble.Discovered_Event += Blecallback_Discovered_Event;
                ble.Notification_Event += Ble_Notification_Event;
                ble.Errore_Password_Event += Ble_Errore_Password_Event;
                ble.Connetti(this.device);                
            }
            Quale_Layout = "TryConnection";
        }

        protected override void OnResume()
        {
            base.OnResume();

            Window.AddFlags(WindowManagerFlags.KeepScreenOn |
               WindowManagerFlags.DismissKeyguard |
               WindowManagerFlags.ShowWhenLocked |
               WindowManagerFlags.TurnScreenOn);

            if (((joyBLE != null) && (telemetryBLE != null)) && (joyBLE.gatt != null)&&(telemetryBLE.gatt != null))
            {
                //Riconnetti
                battery_textview.Text = "--";
                current_mL_textview.Text = "--";
                current_mR_textview.Text = "--";
                ble.Connetti(this.device);
            }
        }

        protected override void OnStop()
        {
            base.OnStop();
            if ((joyBLE != null) && (telemetryBLE != null))
            {
                if (joyBLE.gatt != null)
                {
                    joyBLE.gatt.Disconnect();
                    joyBLE.gatt.Close();
                }
                if (telemetryBLE.gatt != null)
                {
                    telemetryBLE.gatt.Disconnect();
                    telemetryBLE.gatt.Close();
                }
            }
        }


        //*******************************************************************************************
        //*******************************************************************************************
        //*******************************************************************************************
        // ACTIVITY **********************JOYSTICK***************************************************
        //*******************************************************************************************
        //*******************************************************************************************
        //*******************************************************************************************
        private void Controller_view__event(int x, int y)
        {
            //Log.Info("-JOYSTICK-","x:" + x + " ; y:" + y);  // -160; +160
            if (joyBLE.gatt != null)
            {
                //STATE
                //scale 0-320 
                int xs = x + 160;
                int ys = y + 160;
                if (xs < 0) xs = 0;
                if (ys < 0) ys = 0;
                if (xs > 320) xs = 320;
                if (ys > 320) ys = 320;
                ys = 320 - ys;
                //scale 0 - 150
                xs = (int)((float)xs * 0.46875);
                ys = (int)((float)ys * 0.46875);

                byte[] intBytesx = BitConverter.GetBytes(xs);
                byte[] intBytesy = BitConverter.GetBytes(ys);

                byte[] value = new byte[2];
                value[0] = intBytesx[0];
                value[1] = intBytesy[0];

                joyBLE.characteristic.SetValue(value);
                joyBLE.gatt.WriteCharacteristic(joyBLE.characteristic);
            }
        }

        //Bluetooth callback Telemetry -> Battery , Current Motor
        private void Ble_BLE_Event_Char_Callback(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic)
        {
            if ((current_mL_textview != null) && (current_mR_textview != null))
            {
                if (myUUID.Telemetry_UUID.Equals(characteristic.Uuid))
                {
                    byte[] br = characteristic.GetValue();
                    float[] myFloat = new float[3];
                    myFloat[0] = System.BitConverter.ToSingle(br, 0);
                    myFloat[1] = System.BitConverter.ToSingle(br, 4);
                    myFloat[2] = System.BitConverter.ToSingle(br, 8);

                    RunOnUiThread(() => {
                        battery_textview.Text = myFloat[0].ToString("0.0");
                        current_mL_textview.Text = myFloat[2].ToString("0.0");
                        current_mR_textview.Text = myFloat[1].ToString("0.0");
                    });
                }
            }
        }

        //*******************************************************************************************
        //*******************************************************************************************
        //*******************************************************************************************
        // ACTIVITY **********************TRY CONNESSIONE********************************************
        //*******************************************************************************************
        //*******************************************************************************************
        //*******************************************************************************************

        //Call Back BLE Notification buon fine
        private void Ble_Notification_Event(bool stato)
        {
            if (stato)
            {
                //Visualizza Activity
                if (Quale_Layout == "TryConnection")
                {
                    activity.RunOnUiThread(() => Action_SetContentViewJOY());
                }
            }
            else {
                activity.RunOnUiThread(() => Action_Error("Errore Setting Notification!!"));
            }
        }
        //Call Back BLE onDiscovered
        private void Blecallback_Discovered_Event(List<BluetoothServiceDevice> obj)
        {
            bool findDeviceJOY = false;
            bool findDeviceTELEMETRY = false;

            //Verifica Servizi se sono compatibili
            foreach (BluetoothServiceDevice s in obj)
            {
                string myservice = myUUID.DEVICE_SERVICE_UUID.Replace("-", "").ToLower();
                if (s.service.Uuid.ToString().Replace("-", "").ToLower() == myservice)
                {
                    if (s.characteristic.Uuid.ToString().Replace("-", "").ToLower() == myUUID.JOY_CHARACTERISTIC_CONFIG.Replace("-", "").ToLower())
                    {
                        findDeviceJOY = true;
                        if (joyBLE == null) {
                            joyBLE = new BluetoothServiceDevice();
                        }
                        joyBLE.service = s.service;
                        joyBLE.characteristic = s.characteristic;
                        joyBLE.gatt = s.gatt;
                    }
                    else if (s.characteristic.Uuid.ToString().Replace("-", "").ToLower() == myUUID.TELEMETRY_CHARACTERISTIC_CONFIG.Replace("-", "").ToLower())
                    {
                        findDeviceTELEMETRY = true;
                        if (telemetryBLE == null) {
                            telemetryBLE = new BluetoothServiceDevice();
                        }
                        telemetryBLE.service = s.service;
                        telemetryBLE.characteristic = s.characteristic;
                        telemetryBLE.gatt = s.gatt;
                    }
                }
            }

            if ((findDeviceJOY) && (findDeviceTELEMETRY))
            {
                //Richietsa Attivavazione Char Notification 
                ble.SetCharacteristicNotification(telemetryBLE.gatt, telemetryBLE.characteristic, myUUID.CLIENT_UUID);
            }
            else {
                // **************************************************************************************************************************************************************************************************************************************************************
                /*              Handler mHandler = new Handler();
                                mHandler.PostDelayed(new Action(delegate {
                                    Finish();
                                }), 2000);
                */
                activity.RunOnUiThread(() => Action_Error("Errore Connessione!!!"));
            }
        }
        private void Ble_Errore_Password_Event()
        {
            Handler mHandler = new Handler();
            mHandler.PostDelayed(new Action(delegate {
                Intent i = new Intent(this, typeof(MainActivity));
                StartActivity(i);
            }), 2000);
            activity.RunOnUiThread(() => Action_Error("Errore Password!!!"));
        }
        private void Action_SetContentViewJOY()
        {
            activity.SetContentView(Resource.Layout.joystick);
            Quale_Layout = "Joystick";

            controller_view = FindViewById<ControllerView>(Resource.Id.controllerView1);
            controller_view._event += Controller_view__event;
            battery_textview = FindViewById<TextView>(Resource.Id.textBatt);
            current_mL_textview = FindViewById<TextView>(Resource.Id.CurrentLeft);
            current_mR_textview = FindViewById<TextView>(Resource.Id.CurrentRight);

            ble.BLE_Event_Char_Callback += Ble_BLE_Event_Char_Callback;
        }
        private void Action_Error(string s)
        {
            Toast.MakeText(this, s, ToastLength.Short).Show();
        }

    }
}