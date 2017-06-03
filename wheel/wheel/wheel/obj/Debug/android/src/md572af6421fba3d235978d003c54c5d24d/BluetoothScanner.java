package md572af6421fba3d235978d003c54c5d24d;


public class BluetoothScanner
	extends android.bluetooth.le.ScanCallback
	implements
		mono.android.IGCUserPeer
{
/** @hide */
	public static final String __md_methods;
	static {
		__md_methods = 
			"n_onScanResult:(ILandroid/bluetooth/le/ScanResult;)V:GetOnScanResult_ILandroid_bluetooth_le_ScanResult_Handler\n" +
			"";
		mono.android.Runtime.register ("wheel.BluetoothScanner, wheel, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", BluetoothScanner.class, __md_methods);
	}


	public BluetoothScanner () throws java.lang.Throwable
	{
		super ();
		if (getClass () == BluetoothScanner.class)
			mono.android.TypeManager.Activate ("wheel.BluetoothScanner, wheel, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", "", this, new java.lang.Object[] {  });
	}

	public BluetoothScanner (android.content.Context p0, android.app.Activity p1, md572af6421fba3d235978d003c54c5d24d.MainActivity p2) throws java.lang.Throwable
	{
		super ();
		if (getClass () == BluetoothScanner.class)
			mono.android.TypeManager.Activate ("wheel.BluetoothScanner, wheel, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", "Android.Content.Context, Mono.Android, Version=0.0.0.0, Culture=neutral, PublicKeyToken=84e04ff9cfb79065:Android.App.Activity, Mono.Android, Version=0.0.0.0, Culture=neutral, PublicKeyToken=84e04ff9cfb79065:wheel.MainActivity, wheel, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", this, new java.lang.Object[] { p0, p1, p2 });
	}


	public void onScanResult (int p0, android.bluetooth.le.ScanResult p1)
	{
		n_onScanResult (p0, p1);
	}

	private native void n_onScanResult (int p0, android.bluetooth.le.ScanResult p1);

	private java.util.ArrayList refList;
	public void monodroidAddReference (java.lang.Object obj)
	{
		if (refList == null)
			refList = new java.util.ArrayList ();
		refList.add (obj);
	}

	public void monodroidClearReferences ()
	{
		if (refList != null)
			refList.clear ();
	}
}
