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
        private BluetoothScanner ble;

        private ListView mListView;
        public DeviceAdapter adapterList;

        private bool flag_connessioneBLE = true;

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            SetContentView (Resource.Layout.Main);
            flag_connessioneBLE = true;
        }

        protected override void OnResume()
        {
            base.OnResume();

            ble = new BluetoothScanner(this, this, this);
            ble.ScanResultEvent += Ble_ScanResultEvent;

            mListView = FindViewById<ListView>(Resource.Id.listView1);
            adapterList = new DeviceAdapter(this, ble.mLeDevices);
            mListView.Adapter = adapterList;
            mListView.ItemClick += MListView_ItemClick;

            if (ble.mBluetoothAdapter != null)
            {
                if (!ble.mBluetoothAdapter.IsEnabled)
                {
                    if (flag_connessioneBLE) {
                        Intent enableBtIntent = new Intent(BluetoothAdapter.ActionRequestEnable);
                        StartActivityForResult(enableBtIntent, 2);
                    }
                }
                else
                {
                    ble.Scanner();
                }
            }
            else
            {
                Handler mHandler = new Handler();
                mHandler.PostDelayed(new Action(delegate {
                    Finish();
                }), 2500);
                Toast.MakeText(this, "BLE Not Supported!", ToastLength.Short).Show();
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
                flag_connessioneBLE = false;
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
            if (ble.mLeDevices[e.Position].Name == "Wheel")
            {
                BluetoothDevice b = ble.mLeDevices[e.Position];
                Intent i = new Intent(this, typeof(joystickapp));
                i.PutExtra("wheellobjBle", b);
                StartActivity(i);
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
            ble.StopScanner();
            ble.Dispose();
            ble = null;
        }


    }
}

