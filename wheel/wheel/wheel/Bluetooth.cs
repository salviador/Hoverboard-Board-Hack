using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.Bluetooth;
using System.Threading;
using Android.Bluetooth.LE;
using Android.Util;
using Java.Util;



namespace wheel
{

    public class BluetoothScanner : ScanCallback
    {
        public event Action<List<BluetoothDevice>> ScanResultEvent;

        Context context;
        Activity activity;
        MainActivity mainActivity;

        public List<BluetoothDevice> mLeDevices;
        public BluetoothAdapter mBluetoothAdapter;
        public BluetoothManager bluetoothManager;
        public BluetoothLeScanner bluetoothLeScanner;

        private bool mScanning = false;

        public BluetoothScanner(Context context, Activity activity, MainActivity mainActivity) {
            this.context = context;
            this.activity = activity;
            this.mainActivity = mainActivity;
            mLeDevices = new List<BluetoothDevice>();
            bluetoothManager = (BluetoothManager)context.GetSystemService(Context.BluetoothService);
            mBluetoothAdapter = bluetoothManager.Adapter;
        }

        public void Scanner() {
            bluetoothLeScanner = mBluetoothAdapter.BluetoothLeScanner;
            mScanning = true;
            /*
            Handler mHandler = new Handler();
            mHandler.PostDelayed(new Action(delegate {
                mScanning = false;
                bluetoothLeScanner.StopScan(this);
                if (ScanResultEvent != null) {
                    ScanResultEvent(mLeDevices);
                }
            }), SCAN_PERIOD);
            */
            bluetoothLeScanner.StartScan(this);
        }
        public void StopScanner()
        {
            bluetoothLeScanner = mBluetoothAdapter.BluetoothLeScanner;
            if (mScanning) {
                bluetoothLeScanner.StopScan(this);
            }
            mScanning = false;
            /*
            Handler mHandler = new Handler();
            mHandler.PostDelayed(new Action(delegate {
                mScanning = false;
                bluetoothLeScanner.StopScan(this);
                if (ScanResultEvent != null) {
                    ScanResultEvent(mLeDevices);
                }
            }), SCAN_PERIOD);
            */
        }

        public override void OnScanResult(ScanCallbackType callbackType, ScanResult result)
        {
            base.OnScanResult(callbackType, result);
            bool inlistDevice = false;
            foreach (BluetoothDevice d in mLeDevices)
            {
                if (result.Device.Address.ToString() == d.Address.ToString())
                {
                    inlistDevice = true;
                }
            }
            if (!inlistDevice)
            {
                mLeDevices.Add(result.Device);
                mainActivity.adapterList.NotifyDataSetChanged();
            }
        }
    }



    public class BluetoothConnessione : BluetoothGattCallback
    {
        Context context;
        Activity activity;
        MainActivity mainActivity;

        public event Action<List<BluetoothServiceDevice>> Discovered_Event;
        public event Action<BluetoothGatt, BluetoothGattCharacteristic> BLE_Event_Char_Callback;
        public event Action<bool> Notification_Event;
        public event Action Errore_Password_Event;

        public List<BluetoothServiceDevice> storeService;
        private BondStatusBroadcastReceiver myReceiver; //for bond

        public AutoResetEvent WaitCharRecive = new AutoResetEvent(false);

        public BluetoothConnessione(Context context, Activity activity, MainActivity mainActivity)
        {
            this.context = context;
            this.activity = activity;
            this.mainActivity = mainActivity;

            storeService = new List<BluetoothServiceDevice>();
            WaitCharRecive.Reset();
        }

        public void Connetti(BluetoothDevice device)
        {
            Android.Bluetooth.Bond b = device.BondState;
            if (b == Bond.Bonded)
            {
                device.ConnectGatt(context, false, this);
            }
            else
            {
                this.myReceiver = new BondStatusBroadcastReceiver();
                this.myReceiver.Bounded_ConnectDevice_Event += this.MyReceiver_Bounded_ConnectDevice_Event;
                this.myReceiver.Bounded_ConnectDevice_Event_ERROR += this.MyReceiver_Bounded_ConnectDevice_Event_ERROR;

                IntentFilter intentFilter = new IntentFilter(BluetoothDevice.ActionFound);
                intentFilter.AddAction(BluetoothDevice.ActionBondStateChanged);
                context.RegisterReceiver(this.myReceiver, intentFilter);

                device.CreateBond();
            }
        }

        //Erorre password al device Bond (paired)
        private void MyReceiver_Bounded_ConnectDevice_Event_ERROR()
        {
            if (this.Errore_Password_Event != null) {
                this.Errore_Password_Event();
            }
        }

        //Connetti da device Bond (paired)
        private void MyReceiver_Bounded_ConnectDevice_Event(BluetoothDevice obj)
        {
            obj.ConnectGatt(context, false, this);
        }


        public override void OnConnectionStateChange(BluetoothGatt gatt, GattStatus status, ProfileState newState)
        {
            Console.WriteLine("OnConnectionStateChange: ");
            base.OnConnectionStateChange(gatt, status, newState);

            switch (newState)
            {
                case ProfileState.Connected:
                    gatt.DiscoverServices();
                    break;
                case ProfileState.Disconnected:
                    break;
            }
        }

        public override void OnServicesDiscovered(BluetoothGatt gatt, GattStatus status)
        {
            base.OnServicesDiscovered(gatt, status);

            if (status == GattStatus.Success)
            {

                foreach (BluetoothGattService service in gatt.Services) {
                    foreach (BluetoothGattCharacteristic characteristic in service.Characteristics)
                    {
                        BluetoothServiceDevice b = new BluetoothServiceDevice();
                        b.service = service;
                        b.characteristic = characteristic;
                        b.gatt = gatt;
                        storeService.Add(b);
                    }
                }
                if (this.Discovered_Event != null) {
                    this.Discovered_Event(storeService);
                }
            }
        }
        public override void OnDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, GattStatus status)
        {
            base.OnDescriptorWrite(gatt, descriptor, status);
            //ble_ready = true;
            WaitCharRecive.Set();
        }
        public override void OnDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, GattStatus status)
        {
            base.OnDescriptorRead(gatt, descriptor, status);
        }

        public override void OnCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, GattStatus status)
        {
            base.OnCharacteristicRead(gatt, characteristic, status);
        }

        public override void OnCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic)
        {
            base.OnCharacteristicChanged(gatt, characteristic);

            if (BLE_Event_Char_Callback != null)
            {
                BLE_Event_Char_Callback(gatt, characteristic);
            }
        }


        public void SetCharacteristicNotification(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, string UUIDClient)
        {
            //Servizi trovati e compatibili
            //Attiva richiesta notifiche
            Thread thread = new Thread(new ThreadStart(() =>
            {
                bool notificationState = false;

                for (int i = 0; i < 3; i++)
                {
                    BluetoothGattDescriptor desc = characteristic.GetDescriptor(UUID.FromString(UUIDClient));
                    desc.SetValue(BluetoothGattDescriptor.EnableNotificationValue.ToArray<Byte>());
                    gatt.WriteDescriptor(desc);
                    gatt.SetCharacteristicNotification(characteristic, true);

                    if (WaitCharRecive.WaitOne(5000))
                    {
                        notificationState = true;
                        if (Notification_Event != null) {
                            Notification_Event(true);
                        }
                        break;
                    }
                }
                if (notificationState == false)
                {
                    if (Notification_Event != null)
                    {
                        Notification_Event(false);
                    }
                }
            }));
            thread.Start();
        }

    }

    //For pairing
    public class BondStatusBroadcastReceiver : BroadcastReceiver
    {
        public event Action<BluetoothDevice> Bounded_ConnectDevice_Event;
        public event Action Bounded_ConnectDevice_Event_ERROR;

        public override void OnReceive(Context context, Intent intent)
        {
            string action = intent.Action;

            if (BluetoothDevice.ActionBondStateChanged.Equals(action))
            {
                BluetoothDevice device = (BluetoothDevice)intent.GetParcelableExtra(BluetoothDevice.ExtraDevice);

                if (device.BondState == Bond.Bonded)
                {
                    // CONNECT
                    //Console.WriteLine("CONNECT");
                    if (this.Bounded_ConnectDevice_Event != null)
                    {
                        this.Bounded_ConnectDevice_Event(device);
                    }
                }
                else if (device.BondState == Bond.None) {
                    //Console.WriteLine("Errata password!!");
                    if (this.Bounded_ConnectDevice_Event_ERROR != null) {
                        this.Bounded_ConnectDevice_Event_ERROR();
                    }
                }
            }
            else if (BluetoothDevice.ActionFound.Equals(action))
            {
                BluetoothDevice device = (BluetoothDevice)intent.GetParcelableExtra(BluetoothDevice.ExtraDevice);
                // Discover new device
                Console.WriteLine("Discover new device");
            }

        }
    }




    public class BluetoothServiceDevice
    {
        public BluetoothGattService service { get; set; }
        public BluetoothGatt gatt { get; set; }
        public BluetoothGattCharacteristic characteristic { get; set; }
    }







}