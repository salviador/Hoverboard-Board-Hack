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
    [Activity(Label = "wheel", MainLauncher = true, Icon = "@drawable/icon")]
    public class MainActivity : Activity
    {
        private Bluetooth ble;

        private ListView mListView;
        public DeviceAdapter adapterList;
        private ControllerView controller_view;
        private TextView battery_textview;
        private TextView current_mL_textview;
        private TextView current_mR_textview;

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            SetContentView (Resource.Layout.Main);

            Window.AddFlags(WindowManagerFlags.KeepScreenOn |
               WindowManagerFlags.DismissKeyguard |
               WindowManagerFlags.ShowWhenLocked |
               WindowManagerFlags.TurnScreenOn);

            ble = new Bluetooth(this,this,this);
            ble.ScanResultEvent += Ble_ScanResultEvent;

            mListView = FindViewById<ListView>(Resource.Id.listView1);
            adapterList = new DeviceAdapter(this, ble.mLeDevices);
            mListView.Adapter = adapterList;
            mListView.ItemClick += MListView_ItemClick;

            if (ble.mBluetoothAdapter != null)
            {
                if (!ble.mBluetoothAdapter.IsEnabled)
                {
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ActionRequestEnable);
                    StartActivityForResult(enableBtIntent, 2);
                }
                else
                {
                    ble.Scanner();
                }
            }
            else {
                Handler mHandler = new Handler();
                mHandler.PostDelayed(new Action(delegate {
                    Finish();
                }), 5000);
                Toast.MakeText(this, "BLE Not Supported!", ToastLength.Short).Show();
            }
        }

        //BBluetooth callback Telemetry -> Battery , Current Motor
        private void Blecallback_Telemetry_Event(float[] obj)
        {
             if((current_mL_textview != null)&&(current_mR_textview != null))
            {
                RunOnUiThread(() => {
                    battery_textview.Text = obj[0].ToString("0.0");
                    current_mL_textview.Text = obj[1].ToString("0.0");
                    current_mR_textview.Text = obj[2].ToString("0.0");
                });
            }
        }

        //Joystick , callback dal joystick
        private void Controller_view__event(int x, int y)
        {
            Console.WriteLine("x:" + x);
            Console.WriteLine("y:" + y);
            if (ble.blecallback._blegattjoy != null) {
                //STATE
                //scale 0-200 
                int xs = x + 160;
                int ys = y + 160;
                if (xs < 0) xs = 0;
                if (ys < 0) ys = 0;
                if (xs > 320) xs = 320;
                if (ys > 320) ys = 320;
                xs = (int)((float)xs * 0.625);
                ys = (int)((float)ys * 0.625);

                byte[] intBytesx = BitConverter.GetBytes(xs);
                byte[] intBytesy = BitConverter.GetBytes(ys);

                byte[] value = new byte[2];
                value[0] = intBytesx[0];
                value[1] = intBytesy[0];

                if (ble.blecallback.ble_ready)
                {
                    ble.blecallback._characteristicJOY.SetValue(value);
                    bool status = ble.blecallback._blegattjoy.WriteCharacteristic(ble.blecallback._characteristicJOY);
                }
            }
        }

        //callback -> StartActivityForResult
        protected override void OnActivityResult(int requestCode, Result resultCode, Intent data)
        {
            base.OnActivityResult(requestCode, resultCode, data);
            if (resultCode == Result.Ok)
            {
                ble.Scanner();
            }
            else {
                Handler mHandler = new Handler();
                mHandler.PostDelayed(new Action(delegate {
                    Finish();
                }), 5000);
                Toast.MakeText(this, "BLE Spento!", ToastLength.Short).Show();
            }
        }

        //Scan Finish -> Result !!!!!!
        private void Ble_ScanResultEvent(List<BluetoothDevice> obj)
        {


        }
        //Clicca  x scegliere il device da connettere
        private void MListView_ItemClick(object sender, AdapterView.ItemClickEventArgs e)
        {
            SetContentView(Resource.Layout.joystick);
            //var activity2 = new Intent(this, typeof(Joystick));
            //StartActivity(activity2);

            controller_view = FindViewById<ControllerView>(Resource.Id.controllerView1);
            controller_view._event += Controller_view__event;

            battery_textview = FindViewById<TextView>(Resource.Id.textBatt);
            current_mL_textview = FindViewById<TextView>(Resource.Id.CurrentLeft);
            current_mR_textview = FindViewById<TextView>(Resource.Id.CurrentRight);

            if (ble.mLeDevices[e.Position].Name == "Wheel")
            {
                ble.Connetti(ble.mLeDevices[e.Position]);

                ble.blecallback.Telemetry_Event += Blecallback_Telemetry_Event;
            }
            else
            {
                Handler mHandler = new Handler();
                mHandler.PostDelayed(new Action(delegate {
                    Finish();
                }), 2000);
                Toast.MakeText(this, "Device Not supported", ToastLength.Short).Show();
            }
        }

        protected override void OnStop()
        {
            base.OnStop();

            ble.Disconnetti();
            SetContentView(Resource.Layout.Main);

            if(controller_view != null) controller_view._event -= Controller_view__event;
        }


    }
}

