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
using Android.Content.Res;
using Android.Util;
using Android.Graphics;
using Android.Media;
using Android.Hardware.Input;

namespace wheel
{
    public class ControllerView : SurfaceView, ISurfaceHolderCallback
    {
        public delegate void EventHandler(int x, int y);
        public event EventHandler _event;

        System.Timers.Timer timer1 = new System.Timers.Timer();
        private bool TouchPress = false;

        private ISurfaceHolder holder;
        private Joystick joy;
        private const String TAG = "ControllerView";
        private IWindowManager window_manager;
        private Point size = new Point();


        private Paint image_paint;
        private Paint background_paint;

        public ControllerView(Context context) :
            base(context)
        {
            Initialize();
        }

        public ControllerView(Context context, IAttributeSet attrs) :
            base(context, attrs)
        {
            Initialize();
        }

        public ControllerView(Context context, IAttributeSet attrs, int defStyle) :
            base(context, attrs, defStyle)
        {
            Initialize();
        }

        void Initialize()
        {
            holder = Holder;
            holder.AddCallback(this);

            window_manager = (Context.GetSystemService(Context.WindowService)).JavaCast<IWindowManager>();
            image_paint = new Paint();

            //set background paint properties
            background_paint = new Paint();
            background_paint.SetStyle(Paint.Style.Fill);
            background_paint.Dither = true;
            background_paint.AntiAlias = true;

            joy = new Joystick(Application.Context, 127, Resource.Drawable.axis);

            timer1.Elapsed += Timer1_Elapsed;
            timer1.Interval = 200;
            timer1.Enabled = true;
            timer1.AutoReset = true;
            TouchPress = false;
        }

        public void SurfaceChanged(ISurfaceHolder holder, Format format, int width, int height)
        {
        }

        public void SurfaceCreated(ISurfaceHolder holder)
        {
            Canvas c = holder.LockCanvas(null);
            OnDraw(c);
            holder.UnlockCanvasAndPost(c);
        }

        public void SurfaceDestroyed(ISurfaceHolder holder)
        {

        }
        protected override void OnMeasure(int WidthMeasureSpec, int HeightMeasureSpec)
        {
            int width = 0;
            int height = 0;

            Display display = window_manager.DefaultDisplay;
            display.GetSize(size);
            int display_width = size.X;
            int display_height = size.Y;
            display_width = Width;
            display_height = Height;

            int width_spec_mode = (int)MeasureSpec.GetMode(WidthMeasureSpec);
            int width_spec_size = MeasureSpec.GetSize(WidthMeasureSpec);
            int height_spec_mode = (int)MeasureSpec.GetMode(HeightMeasureSpec);
            int height_spec_size = MeasureSpec.GetSize(HeightMeasureSpec);
            Log.Debug(TAG, "width_spec_size=" + width_spec_size + ", height_spec_size=" + height_spec_size);

            if (width_spec_mode == (int)MeasureSpecMode.Exactly)
            {
                width = width_spec_size;
            }
            else if (width_spec_mode == (int)MeasureSpecMode.AtMost)
            {
                width = Math.Min(display_width, width_spec_size);
            }
            else
            {
                width = display_width;
            }

            if (height_spec_mode == (int)MeasureSpecMode.Exactly)
            {
                height = height_spec_size;
            }
            else if (height_spec_mode == (int)MeasureSpecMode.AtMost)
            {
                height = Math.Min(display_height, height_spec_size);
            }
            else
            {
                height = display_height;
            }

            SetMeasuredDimension(width, height);

            joy.Set_windows_size(width, height);
        }


        protected override void OnDraw(Canvas canvas)
        {
            base.OnDraw(canvas);

            //draw background
            canvas.DrawColor(Color.DarkGray);


            canvas.DrawBitmap(joy.image, joy.joy_x, joy.joy_y, image_paint);

            Paint c1 = new Paint();
            c1.Color = Color.Black;
            c1.SetStyle(Paint.Style.Stroke);
            canvas.DrawCircle(Width / 2, Height / 2, 80, c1);

            Paint c2 = new Paint();
            c2.Color = Color.Red;
            c2.StrokeWidth = 5;
            c2.SetStyle(Paint.Style.Stroke);
            canvas.DrawCircle(Width / 2, Height / 2, 160, c2);

            //  canvas.DrawRect(joy.joy_windows_x_min, joy.joy_windows_y_max, joy.joy_windows_x_max, joy.joy_windows_y_min, c2);
        }



        private void Timer1_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            if (TouchPress == false)
            {
                if (_event != null)
                {
                    _event(joy.joystick_x, joy.joystick_y);
                }
            }
        }

        //-motion
        private float START_X, START_Y;
        private bool START_VALIDATE = false;

        public override bool OnTouchEvent(MotionEvent ev)
        {
            MotionEventActions action = ev.Action & MotionEventActions.Mask;
            float x = ev.GetX();
            float y = ev.GetY();

            if (START_VALIDATE)
            {
                int offsetx = joy.centerJoy_x - (int)START_X;
                int offsety = joy.centerJoy_y - (int)START_Y;

                int virtualx = (int)x + offsetx + joy.size / 2;   //Center joystick
                int virtualy = (int)y + offsety + joy.size / 2;   //Center joystick

                double abs = Math.Sqrt((virtualx - joy.center_x) * (virtualx - joy.center_x) + (virtualy - joy.center_y) * (virtualy - joy.center_y));
                Log.Debug(TAG, "abs: " + abs.ToString());

                if (abs > 160)
                {
                    virtualx = (int)((virtualx - joy.center_x) * 160 / abs + joy.center_x);
                    virtualy = (int)((virtualy - joy.center_y) * 160 / abs + joy.center_y);
                }

                //****************STORE VALORE JOYSTICK !!!!!!!!!!!! ****************
                joy.joystick_x = virtualx - joy.center_x;
                joy.joystick_y = virtualy - joy.center_y;
                //Log.Debug(TAG, "x: " + joy.joystick_x.ToString());
                //Log.Debug(TAG, "y: " + joy.joystick_y.ToString());
                if (_event != null)
                {
                    _event(joy.joystick_x, joy.joystick_y);
                }


                joy.Move_Axis(virtualx - joy.size / 2, virtualy - joy.size / 2);
            }
            switch (action)
            {
                case MotionEventActions.Up:
                    // Log.Debug(TAG, "up");
                    START_VALIDATE = false;
                    joy.joy_x = joy.centerJoy_x;
                    joy.joy_y = joy.centerJoy_y;

                    joy.joystick_x = 0;
                    joy.joystick_y = 0;
                    if (_event != null)
                    {
                        _event(joy.joystick_x, joy.joystick_y);
                    }
                    TouchPress = false;
                    break;
                case MotionEventActions.Down:
                    // Log.Debug(TAG, "Down");
                    //Cattura posizione START dito
                    START_VALIDATE = false;
                    if ((x < joy.joy_windows_x_max) && (x > joy.joy_windows_x_min) && (y > joy.joy_windows_y_min) && (y < joy.joy_windows_y_max))
                    {
                        //Log.Debug(TAG, "INTERNAL");
                        START_X = x;
                        START_Y = y;
                        START_VALIDATE = true;

                        TouchPress = true;
                    }
                    break;
            }
            //Update screen OnREdraw
            Canvas c = holder.LockCanvas(null);
            OnDraw(c);
            holder.UnlockCanvasAndPost(c);
            Invalidate();


            return true;
        }
    }





    public class Joystick
    {
        public int size = 127;
        public Bitmap image;
        private Context c;

        private int windowsX, windowsY;
        public int center_x, center_y;
        public int centerJoy_x, centerJoy_y;
        public int joy_windows_x_min, joy_windows_y_min, joy_windows_x_max, joy_windows_y_max;

        public int joy_x, joy_y;

        public int joystick_x, joystick_y;  //Qui ho il valore del joystick effettivo 160 0 -160

        public Joystick(Context c, int size, int resurce_ID)
        {
            this.size = size;
            this.c = c;

            image = BitmapFactory.DecodeResource(c.Resources, resurce_ID);
            //scale the bitmaps based on the display ratio
            image = Bitmap.CreateScaledBitmap(image, size, size, true);

        }
        public void Set_windows_size(int windowsX, int windowsY)
        {
            this.windowsX = windowsX;
            this.windowsY = windowsY;
            center_x = windowsX / 2;
            center_y = windowsY / 2;
            centerJoy_x = center_x - size / 2;
            centerJoy_y = center_y - size / 2;

            joy_windows_x_min = centerJoy_x;
            joy_windows_y_min = centerJoy_y;
            joy_windows_x_max = centerJoy_x + size;
            joy_windows_y_max = centerJoy_y + size;

            joy_x = centerJoy_x;
            joy_y = centerJoy_y;
        }


        public void Move_Axis(int x, int y)
        {
            joy_x = x;
            joy_y = y;
        }
    }
}