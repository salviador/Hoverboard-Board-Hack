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
using Java.Lang;
using Android.Bluetooth;

namespace wheel.Resources
{
    public class ViewHolder : Java.Lang.Object
    {
        public TextView txtName { get; set; }
        public TextView txtAddress { get; set; }
    }


    public class DeviceAdapter : BaseAdapter
    {
        private Activity activity;
        private List<BluetoothDevice> Device;

        public DeviceAdapter(Activity activity, List<BluetoothDevice> Device)
        {
            this.activity = activity;
            this.Device = Device;
        }
        public override int Count
        {
            get
            {
                return Device.Count;
            }
        }

        public override Java.Lang.Object GetItem(int position)
        {
            return Device[position];
        }

        public override long GetItemId(int position)
        {
            return position;
        }

        public override View GetView(int position, View convertView, ViewGroup parent)
        {
            var view = convertView ?? activity.LayoutInflater.Inflate(Resource.Layout.list_viewTemplate, parent, false);

            var txtName = view.FindViewById<TextView>(Resource.Id.textName);
            var txtadd = view.FindViewById<TextView>(Resource.Id.textAddress);

            txtName.Text = Device[position].Name;
            txtadd.Text = "" + Device[position].Address;
            return view;
        }


    }
}