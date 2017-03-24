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


        public void Disconnetti()
        {
            if (blecallback != null)
            {
                blecallback._blegattjoy.SetCharacteristicNotification(blecallback._characteristicLTELEMETRY, false);

                if (blecallback._characteristicLTELEMETRY.GetDescriptor(UUID.FromString(blecallback.CLIENT_UUID)) != null)
                {
                    BluetoothGattDescriptor desc = blecallback._characteristicLTELEMETRY.GetDescriptor(UUID.FromString(blecallback.CLIENT_UUID));
                    desc.SetValue(BluetoothGattDescriptor.DisableNotificationValue.ToArray<Byte>());

                    if (!blecallback._blegattjoy.WriteDescriptor(desc))
                    {

                    }
                }
                blecallback._blegattjoy.Close(); ;
                blecallback._blegattjoy.Disconnect();
            }
        }
    }

    public class BGattCallback : BluetoothGattCallback
    {
        public event Action<float[]> Telemetry_Event;

        public static String DEVICE_SERVICE_UUID = "19B10010-E8F2-537E-4F6C-D11476EA1218";
        public static String JOY_CHARACTERISTIC_CONFIG = "19B11E01-E8F2-537E-4F6C-D104768A1214";
        public static String TELEMETRY_CHARACTERISTIC_CONFIG = "19B11E02-E8F2-537E-4F6C-D104768A1214";
        public  String CLIENT_UUID = "00002902-0000-1000-8000-00805f9b34fb";

        private UUID JOY_UUID;
        public UUID Telemetry_UUID;

        public BluetoothGattCharacteristic _characteristicJOY;
        public BluetoothGatt _blegattjoy;
        public BluetoothGattCharacteristic _characteristicLTELEMETRY;

        public BGattCallback()
        {
            Telemetry_UUID = UUID.FromString(TELEMETRY_CHARACTERISTIC_CONFIG);
            JOY_UUID = UUID.FromString(JOY_CHARACTERISTIC_CONFIG);
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

        public bool ble_ready = false;

        public override void OnServicesDiscovered(BluetoothGatt gatt, GattStatus status)
        {
            base.OnServicesDiscovered(gatt, status);

            if (status == GattStatus.Success)
            { 

                foreach (BluetoothGattService service in gatt.Services) {
                    foreach (BluetoothGattCharacteristic characteristic in service.Characteristics)
                    {

                            if (characteristic.Uuid.Equals(JOY_UUID))
                            {
                                BluetoothGattCharacteristic characteristicLJOY = characteristic;
                                _characteristicJOY = characteristicLJOY;
                                _blegattjoy = gatt;
                            }else  if (characteristic.Uuid.Equals(Telemetry_UUID))
                            {
                                BluetoothGattCharacteristic characteristicLTELEMETRY = characteristic;
                                _characteristicLTELEMETRY = characteristicLTELEMETRY;




                                if (characteristicLTELEMETRY.GetDescriptor(UUID.FromString(CLIENT_UUID)) != null)
                                {
                                    BluetoothGattDescriptor desc = characteristicLTELEMETRY.GetDescriptor(UUID.FromString(CLIENT_UUID));
                                    desc.SetValue(BluetoothGattDescriptor.EnableNotificationValue.ToArray<Byte>());

                                if (!gatt.WriteDescriptor(desc))
                                    {
                                        Console.WriteLine("Couldn't write RX client descriptor value!");
                                    }
                                    else
                                    {
                                        Console.WriteLine("");
                                    }

                                }
                            if (!gatt.SetCharacteristicNotification(characteristicLTELEMETRY, true))
                            {
                                Console.WriteLine("Couldn't set notifications for RX characteristic!");
                            }




                        }
                    }
                }
            }
        }

   
        public override void OnDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, GattStatus status)
        {
            base.OnDescriptorWrite(gatt, descriptor, status);
            ble_ready = true;
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

            byte[] br = characteristic.GetValue();

            if (Telemetry_UUID.Equals(characteristic.Uuid))
            {
                if (Telemetry_Event != null)
                {
                    float[] myFloat = new float[3];
                    myFloat[0] = System.BitConverter.ToSingle(br, 0);
                    myFloat[1] = System.BitConverter.ToSingle(br, 4);
                    myFloat[2] = System.BitConverter.ToSingle(br, 8);
                    Telemetry_Event(myFloat);
                }
            }
        }

    }


}