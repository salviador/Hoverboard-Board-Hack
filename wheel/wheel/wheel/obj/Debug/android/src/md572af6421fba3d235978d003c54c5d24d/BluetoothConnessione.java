package md572af6421fba3d235978d003c54c5d24d;


public class BluetoothConnessione
	extends android.bluetooth.BluetoothGattCallback
	implements
		mono.android.IGCUserPeer
{
/** @hide */
	public static final String __md_methods;
	static {
		__md_methods = 
			"n_onConnectionStateChange:(Landroid/bluetooth/BluetoothGatt;II)V:GetOnConnectionStateChange_Landroid_bluetooth_BluetoothGatt_IIHandler\n" +
			"n_onServicesDiscovered:(Landroid/bluetooth/BluetoothGatt;I)V:GetOnServicesDiscovered_Landroid_bluetooth_BluetoothGatt_IHandler\n" +
			"n_onDescriptorWrite:(Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V:GetOnDescriptorWrite_Landroid_bluetooth_BluetoothGatt_Landroid_bluetooth_BluetoothGattDescriptor_IHandler\n" +
			"n_onDescriptorRead:(Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V:GetOnDescriptorRead_Landroid_bluetooth_BluetoothGatt_Landroid_bluetooth_BluetoothGattDescriptor_IHandler\n" +
			"n_onCharacteristicRead:(Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V:GetOnCharacteristicRead_Landroid_bluetooth_BluetoothGatt_Landroid_bluetooth_BluetoothGattCharacteristic_IHandler\n" +
			"n_onCharacteristicChanged:(Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;)V:GetOnCharacteristicChanged_Landroid_bluetooth_BluetoothGatt_Landroid_bluetooth_BluetoothGattCharacteristic_Handler\n" +
			"";
		mono.android.Runtime.register ("wheel.BluetoothConnessione, wheel, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", BluetoothConnessione.class, __md_methods);
	}


	public BluetoothConnessione () throws java.lang.Throwable
	{
		super ();
		if (getClass () == BluetoothConnessione.class)
			mono.android.TypeManager.Activate ("wheel.BluetoothConnessione, wheel, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", "", this, new java.lang.Object[] {  });
	}

	public BluetoothConnessione (android.content.Context p0, android.app.Activity p1, md572af6421fba3d235978d003c54c5d24d.MainActivity p2) throws java.lang.Throwable
	{
		super ();
		if (getClass () == BluetoothConnessione.class)
			mono.android.TypeManager.Activate ("wheel.BluetoothConnessione, wheel, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", "Android.Content.Context, Mono.Android, Version=0.0.0.0, Culture=neutral, PublicKeyToken=84e04ff9cfb79065:Android.App.Activity, Mono.Android, Version=0.0.0.0, Culture=neutral, PublicKeyToken=84e04ff9cfb79065:wheel.MainActivity, wheel, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null", this, new java.lang.Object[] { p0, p1, p2 });
	}


	public void onConnectionStateChange (android.bluetooth.BluetoothGatt p0, int p1, int p2)
	{
		n_onConnectionStateChange (p0, p1, p2);
	}

	private native void n_onConnectionStateChange (android.bluetooth.BluetoothGatt p0, int p1, int p2);


	public void onServicesDiscovered (android.bluetooth.BluetoothGatt p0, int p1)
	{
		n_onServicesDiscovered (p0, p1);
	}

	private native void n_onServicesDiscovered (android.bluetooth.BluetoothGatt p0, int p1);


	public void onDescriptorWrite (android.bluetooth.BluetoothGatt p0, android.bluetooth.BluetoothGattDescriptor p1, int p2)
	{
		n_onDescriptorWrite (p0, p1, p2);
	}

	private native void n_onDescriptorWrite (android.bluetooth.BluetoothGatt p0, android.bluetooth.BluetoothGattDescriptor p1, int p2);


	public void onDescriptorRead (android.bluetooth.BluetoothGatt p0, android.bluetooth.BluetoothGattDescriptor p1, int p2)
	{
		n_onDescriptorRead (p0, p1, p2);
	}

	private native void n_onDescriptorRead (android.bluetooth.BluetoothGatt p0, android.bluetooth.BluetoothGattDescriptor p1, int p2);


	public void onCharacteristicRead (android.bluetooth.BluetoothGatt p0, android.bluetooth.BluetoothGattCharacteristic p1, int p2)
	{
		n_onCharacteristicRead (p0, p1, p2);
	}

	private native void n_onCharacteristicRead (android.bluetooth.BluetoothGatt p0, android.bluetooth.BluetoothGattCharacteristic p1, int p2);


	public void onCharacteristicChanged (android.bluetooth.BluetoothGatt p0, android.bluetooth.BluetoothGattCharacteristic p1)
	{
		n_onCharacteristicChanged (p0, p1);
	}

	private native void n_onCharacteristicChanged (android.bluetooth.BluetoothGatt p0, android.bluetooth.BluetoothGattCharacteristic p1);

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
