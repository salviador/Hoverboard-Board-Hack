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
    public class Bluetooth : ScanCallback
    {
        public event Action<List<BluetoothDevice>> ScanResultEvent;

        Context context;
        Activity activity;
        MainActivity mainActivity;

        public List<BluetoothDevice> mLeDevices;
        public BluetoothAdapter mBluetoothAdapter;
        public BluetoothManager bluetoothManager;
        public BluetoothLeScanner bluetoothLeScanner;
        public BGattCallback blecallback;

        static readonly long SCAN_PERIOD = 10000;
        private bool mScanning = false;

        public Bluetooth(Context context, Activity activity, MainActivity mainActivity) {
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
            Handler mHandler = new Handler();
            mHandler.PostDelayed(new Action(delegate {
                mScanning = false;
                bluetoothLeScanner.StopScan(this);
                if (ScanResultEvent != null) {
                    ScanResultEvent(mLeDevices);
                }
            }), SCAN_PERIOD);
            bluetoothLeScanner.StartScan(this);
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

        public void Connetti(BluetoothDevice device) {
            blecallback = new BGattCallback();
            device.ConnectGatt(context, false, blecallback);
        }
    }

    public class BGattCallback : BluetoothGattCallback
    {
        private static String DEVICE_SERVICE_UUID = "19B10010-E8F2-537E-4F6C-D11476EA1218";
        private static String JOY_CHARACTERISTIC_CONFIG = "19B11E01-E8F2-537E-4F6C-D104768A1214";
        private static String BATTERY_CHARACTERISTIC_CONFIG = "19B11E02-E8F2-537E-4F6C-D104768A1214";
        private static String CURRENT_CHARACTERISTIC_CONFIG = "19B11E03-E8F2-537E-4F6C-D104768A1214";
        public static String CLIENT_UUID = "00002902-0000-1000-8000-00805f9b34fb";

        private UUID Battery_UUID;

        public BluetoothGattCharacteristic _characteristicJOY;
        public BluetoothGatt _blegattjoy;

        public BGattCallback()
        {
            Battery_UUID = UUID.FromString(BATTERY_CHARACTERISTIC_CONFIG);
        }
        public override void OnConnectionStateChange(BluetoothGatt gatt, GattStatus status, ProfileState newState)
        {
            Console.WriteLine("OnConnectionStateChange: ");
            base.OnConnectionStateChange(gatt, status, newState);

            switch (newState)
            {
                case ProfileState.Connected:
                    gatt.DiscoverServices();
                    _blegattjoy = gatt;
                    break;
                case ProfileState.Disconnected:
                    _blegattjoy = null;
                    break;
            }
        }

        public override void OnServicesDiscovered(BluetoothGatt gatt, GattStatus status)
        {
            base.OnServicesDiscovered(gatt, status);

            if (status == GattStatus.Success)
            {

                BluetoothGattService service = gatt.GetService(UUID.FromString(DEVICE_SERVICE_UUID));

                if (service == null)
                {
                }
                else
                {
                    BluetoothGattCharacteristic characteristicLJOY = service.GetCharacteristic(UUID.FromString(JOY_CHARACTERISTIC_CONFIG));
                    BluetoothGattCharacteristic characteristicLBATT = service.GetCharacteristic(UUID.FromString(BATTERY_CHARACTERISTIC_CONFIG));
                    BluetoothGattCharacteristic characteristicLCURRENT = service.GetCharacteristic(UUID.FromString(CURRENT_CHARACTERISTIC_CONFIG));
                    _characteristicJOY = characteristicLJOY;
                    _blegattjoy = gatt;

                    if (!gatt.SetCharacteristicNotification(characteristicLBATT, true))
                    {
                        Console.WriteLine("Couldn't set notifications for RX characteristic!");
                    }

                    if (characteristicLBATT.GetDescriptor(UUID.FromString(CLIENT_UUID)) != null)
                    {
                        BluetoothGattDescriptor desc = characteristicLBATT.GetDescriptor(UUID.FromString(CLIENT_UUID));
                        desc.SetValue(BluetoothGattDescriptor.EnableNotificationValue.ToArray<Byte>());
                        if (!gatt.WriteDescriptor(desc))
                        {
                            Console.WriteLine("Couldn't write RX client descriptor value!");
                        }
                        else {
                            Console.WriteLine("");
                        }
                    }


                }
            }
        }
        public override void OnDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, GattStatus status)
        {
            base.OnDescriptorRead(gatt, descriptor, status);

            Console.WriteLine("OnDescriptorRead: " + descriptor.ToString());
        }

        public override void OnCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, GattStatus status)
        {
            base.OnCharacteristicRead(gatt, characteristic, status);

            Console.WriteLine("OnCharacteristicRead: " + characteristic.GetStringValue(0));
        }

        public override void OnCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic)
        {
            base.OnCharacteristicChanged(gatt, characteristic);

            //Console.WriteLine("OnCharacteristicChanged: " + characteristic.GetStringValue(0));
            byte[] br = characteristic.GetValue();

            float myFloat = System.BitConverter.ToSingle(br, 0);
            //  Console.WriteLine("^^^^^^^^^^^^^OnCharacteristicChanged^^^^^^^^^^^^^: " + br.ToString());

            if (Battery_UUID.Equals(characteristic.Uuid))
            {
                Log.Info("BLE", "Received new value for BATTERY.");
            }

        }

    }


}