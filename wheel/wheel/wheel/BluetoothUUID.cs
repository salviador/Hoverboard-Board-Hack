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
using Android.Bluetooth.LE;
using Android.Util;
using Java.Util;


namespace wheel
{
    public class BluetoothUUID
    {
        public String DEVICE_SERVICE_UUID = "19B10010-E8F2-537E-4F6C-D11476EA1218";
        public String JOY_CHARACTERISTIC_CONFIG = "19B11E01-E8F2-537E-4F6C-D104768A1214";
        public String TELEMETRY_CHARACTERISTIC_CONFIG = "19B11E02-E8F2-537E-4F6C-D104768A1214";
        public String CLIENT_UUID = "00002902-0000-1000-8000-00805f9b34fb";

        public UUID DEVICE__UUID;
        public UUID JOY_UUID;
        public UUID Telemetry_UUID;

        public BluetoothUUID() {
            DEVICE__UUID = UUID.FromString(DEVICE_SERVICE_UUID);
            Telemetry_UUID = UUID.FromString(TELEMETRY_CHARACTERISTIC_CONFIG);
            JOY_UUID = UUID.FromString(JOY_CHARACTERISTIC_CONFIG);
        }
    }
}