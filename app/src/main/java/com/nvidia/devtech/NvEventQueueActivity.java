//----------------------------------------------------------------------------------
// File:            libs\src\com\nvidia\devtech\NvEventQueueActivity.java
// Samples Version: Android NVIDIA samples 2 
// Email:           tegradev@nvidia.com
// Forum:           http://developer.nvidia.com/tegra/forums/tegra-forums/android-development
//
// Copyright 2009-2010 NVIDIA Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//----------------------------------------------------------------------------------
package com.nvidia.devtech;


import android.annotation.SuppressLint;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.util.TypedValue;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewParent;
import android.view.WindowManager;
import android.widget.FrameLayout;

import androidx.appcompat.app.AppCompatActivity;

import com.samp.mobile.R;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;

/**
A base class used to provide a native-code event-loop interface to an
application.  This class is designed to be subclassed by the application
with very little need to extend the Java.  Paired with its native static-link
library, libnv_event.a, this package makes it possible for native applciations
to avoid any direct use of Java code.  In addition, input and other events are
automatically queued and provided to the application in native code via a
classic event queue-like API.  EGL functionality such as bind/unbind and swap
are also made available to the native code for ease of application porting.
Please see the external SDK documentation for an introduction to the use of
this class and its paired native library.
*/
public abstract class NvEventQueueActivity extends AppCompatActivity implements SensorEventListener, View.OnTouchListener {

    protected Handler handler = null;

    private int SwapBufferSkip = 0;

    protected boolean paused = false;

	protected boolean supportPauseResume = true;

    //accelerometer related
  //  protected boolean wantsAccelerometer = false;
 //   protected SensorManager mSensorManager = null;
    protected ClipboardManager mClipboardManager = null;
    protected int mSensorDelay = SensorManager.SENSOR_DELAY_GAME; //other options: SensorManager.SENSOR_DELAY_FASTEST, SensorManager.SENSOR_DELAY_NORMAL and SensorManager.SENSOR_DELAY_UI
	protected Display display = null;

	FrameLayout mAndroidUI = null;

    private static final int EGL_RENDERABLE_TYPE = 0x3040;
    private static final int EGL_OPENGL_ES2_BIT = 0x0004;
    private static final int EGL_OPENGL_ES3_BIT = 64;
    private static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
    EGL10 egl = null;
    GL11 gl = null;

    private boolean ranInit = false;
    protected EGLSurface eglSurface = null;
    protected EGLDisplay eglDisplay = null;
    protected EGLContext eglContext = null;
    protected EGLConfig eglConfig = null;

	protected SurfaceHolder cachedSurfaceHolder = null;
    private int surfaceWidth = 0;
    private int surfaceHeight = 0;
    protected boolean ResumeEventDone = false;
    private int fixedWidth = 0;
    private int fixedHeight = 0;
    private boolean HasGLExtensions = false;
    private String glVendor = null;
    private String glExtensions = null;
    private String glRenderer = null;
    private String glVersion = null;
    //private boolean GameIsFocused = false;
    private boolean viewIsActive = false;

    private SurfaceView mSurfaceView = null;

    //private HeightProvider mHeightProvider = null;

    /* *
     * Helper function to select fixed window size.
     * */ 
    public void setFixedSize(int fw, int fh)
    {
    	fixedWidth = fw;
    	fixedHeight = fh;
    }

    private int mUseFullscreen = 0;

    private void processCutout()
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P)
        {
            if(mUseFullscreen == 1)
            {
                getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
            }
        }
    }

    public static int dpToPx(float f, Context context) {
        return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, f, context.getResources().getDisplayMetrics());
    }

    public void hideSystemUI() {
        // Enables regular immersive mode.
        // For "lean back" mode, remove SYSTEM_UI_FLAG_IMMERSIVE.
        // Or for "sticky immersive," replace it with SYSTEM_UI_FLAG_IMMERSIVE_STICKY



        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE
                        // Set the content to appear under the system bars so that the
                        // content doesn't resize when the system bars hide and show.
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        // Hide the nav bar and status bar
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override
    public boolean onTouch(View view, MotionEvent event) {
        int x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
        // marshal up the data.
        int numEvents = event.getPointerCount();
        for (int i = 0; i < numEvents; i++) {
            // only use pointers 0 and 1, 2, 3
            int pointerId = event.getPointerId(i);
            if (pointerId == 0) {
                x1 = (int) event.getX(i);
                y1 = (int) event.getY(i);
            } else if (pointerId == 1) {
                x2 = (int) event.getX(i);
                y2 = (int) event.getY(i);
            } else if (pointerId == 2) {
                x3 = (int) event.getX(i);
                y3 = (int) event.getY(i);
            }
        }

        int pointerId = event.getPointerId(event.getActionIndex());
        int action = event.getActionMasked();
        customMultiTouchEvent(action, pointerId, x1, y1, x2, y2, x3, y3);
        return true;
    }

    public native void togglePlayer(int toggle);

    /**
     * Helper class used to pass raw data around.
     */
    public class RawData
    {
        /** The actual data bytes. */
        public byte[] data;
        /** The length of the data. */
        public int length;
    }
    /**
     * Helper class used to pass a raw texture around.
     */
    public class RawTexture extends RawData
    {
        /** The width of the texture. */
        public int width;
        /** The height of the texture. */
        public int height;
    }

    /**
     * Helper function to load a file into a {@link RawData} object.
     * It'll first try loading the file from "/data/" and if the file doesn't
     * exist there, it'll try loading it from the assets directory inside the
     * .APK file. This is to allow the files inside the apk to be overridden
     * or not be part of the .APK at all during the development phase of the
     * application, decreasing the size needed to be transmitted to the device
     * between changes to the code.
     * 
     * @param filename The file to load.
     * @return The RawData object representing the file's fully loaded data,
     * or null if loading failed. 
     */
    public RawData loadFile(String filename)
    {
        InputStream is = null;
        RawData ret = new RawData();
        try {
            try
            {
                is = new FileInputStream("/data/" + filename);
            }
            catch (Exception e)
            {
                try
                {
                    is = getAssets().open(filename); 
                }
                catch (Exception e2)
                {
                }
            }
            int size = is.available();
            ret.length = size;
            ret.data = new byte[size];
            is.read(ret.data);
        }
        catch (IOException ioe)
        {
        }
        finally
        {
            if (is != null)
            {
                try { is.close(); } catch (Exception e) {}
            }
        }
        return ret;
    }

    /**
     * Helper function to load a texture file into a {@link RawTexture} object.
     * It'll first try loading the texture from "/data/" and if the file doesn't
     * exist there, it'll try loading it from the assets directory inside the
     * .APK file. This is to allow the files inside the apk to be overridden
     * or not be part of the .APK at all during the development phase of the
     * application, decreasing the size needed to be transmitted to the device
     * between changes to the code.
     * 
     * The texture data will be flipped and bit-twiddled to fit being loaded directly
     * into OpenGL ES via the glTexImage2D call.
     * 
     * @param filename The file to load.
     * @return The RawTexture object representing the texture's fully loaded data,
     * or null if loading failed. 
     */
    public RawTexture loadTexture(String filename)
    {
        RawTexture ret = new RawTexture();
        try {
            InputStream is = null;
            try
            {
                is = new FileInputStream("/data/" + filename);
            }
            catch (Exception e)
            {
                try
                {
                    is = getAssets().open(filename); 
                }
                catch (Exception e2)
                {
                }
            }
            
            Bitmap bmp = BitmapFactory.decodeStream(is);
            ret.width = bmp.getWidth();
            ret.height = bmp.getHeight();
            int[] pixels = new int[bmp.getWidth() * bmp.getHeight()];
            bmp.getPixels(pixels, 0, bmp.getWidth(), 0, 0, bmp.getWidth(), bmp.getHeight());
    
            // Flip texture
            int[] tmp = new int[bmp.getWidth()];
            final int w = bmp.getWidth(); 
            final int h = bmp.getHeight();
            for (int i = 0; i < h>>1; i++)
            {
                System.arraycopy(pixels, i*w, tmp, 0, w);
                System.arraycopy(pixels, (h-1-i)*w, pixels, i*w, w);
                System.arraycopy(tmp, 0, pixels, (h-1-i)*w, w);
            }
    
            // Convert from ARGB -> RGBA and put into the byte array
            ret.length = pixels.length * 4;
            ret.data = new byte[ret.length];
            int pos = 0;
            int bpos = 0;
            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++, pos++)
                {
                    int p = pixels[pos];
                    ret.data[bpos++] = (byte) ((p>>16)&0xff);
                    ret.data[bpos++] = (byte) ((p>> 8)&0xff);
                    ret.data[bpos++] = (byte) ((p>> 0)&0xff);
                    ret.data[bpos++] = (byte) ((p>>24)&0xff);
                }
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return ret;
    }
    
    /**
     * Function called when app requests accelerometer events.
     * Applications need/should NOT overide this function - it will provide
     * accelerometer events into the event queue that is accessible
     * via the calls in nv_event.h
     * 
     * @param values0: values[0] passed to onSensorChanged(). For accelerometer: Acceleration minus Gx on the x-axis.
     * @param values1: values[1] passed to onSensorChanged(). For accelerometer: Acceleration minus Gy on the y-axis.
     * @param values2: values[2] passed to onSensorChanged(). For accelerometer: Acceleration minus Gz on the z-axis.
     * @return True if the event was handled.
     */
    public native boolean accelerometerEvent(float values0, float values1, float values2);
    
    /**
     * The following indented function implementations are defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
		public native void cleanup();
		public native boolean init(boolean z);
		public native void setWindowSize(int w, int h);
		public native void quitAndWait();
		public native void postCleanup();

        public native void imeClosed();

        public native void lowMemoryEvent(); // TODO: implement this
        public native boolean processTouchpadAsPointer(ViewParent viewParent, boolean z);
        public native void notifyChange(String str, int i);
        public native void changeConnection(boolean z);

		public native void pauseEvent();
		public native void resumeEvent();
		public native boolean touchEvent(int action, int x, int y, MotionEvent event);
		public native boolean multiTouchEvent(int action, int count, 
			int x0, int y0, int x1, int y1, MotionEvent event);
		public native boolean keyEvent(int action, int keycode, int unicodeChar, int metaState, KeyEvent event);

    public native boolean multiTouchEvent4(int i, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10, MotionEvent motionEvent);

    public native boolean customMultiTouchEvent(int action, int count, int x1, int y1, int x2, int y2,
                                                int x3, int y3);
	/**
	 * END indented block, see in comment at top of block
	 */

    /**
     * Declaration for function defined in nv_time/nv_time.cpp
     * It initializes and returns time through Nvidia's egl extension for time.
     * It is useful while debugging the demo using PerfHUD.
     * 
     * @see: nv_time/nv_time.cpp for implementation details.
     */
    public native void nvAcquireTimeExtension();
    public native long nvGetSystemTime();

    @SuppressLint("SuspiciousIndentation")
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        System.out.println("**** onCreate");
        super.onCreate(savedInstanceState);

		if(supportPauseResume)
		{
		    System.out.println("Calling init(false)");
            init(false);
        }
        handler = new Handler();

        mClipboardManager = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);

        NvUtil.getInstance().setActivity(this);
        NvAPKFileHelper.getInstance().setContext(this);

        display = ((WindowManager)this.getSystemService(WINDOW_SERVICE)).getDefaultDisplay();

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);

        systemInit();

        hideSystemUI();

        //((TextView)findViewById(R.id.main_version_text)).setText(BuildConfig.VERSION_NAME);

        getWindow().getDecorView().setOnSystemUiVisibilityChangeListener(i -> {
            if ((i & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
                hideSystemUI();
            } else {
                // TODO: The system bars are NOT visible. Make any desired
                // adjustments to your UI, such as hiding the action bar or
                // other navigational controls.
            }

        });

        processCutout();
    }

    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    public void onWindowFocusChanged(boolean hasFocus) {
        if (hasFocus) {
            hideSystemUI();
        }
        super.onWindowFocusChanged(hasFocus);
    }

    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
    @Override
    public void onResume() {
//        if (this.mSensorManager != null) {
//            this.mSensorManager.registerListener(this, this.mSensorManager.getDefaultSensor(1), this.mSensorDelay);
//        }
        this.paused = false;

        super.onResume();
       // this.inputPaused = false;
    }

    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
    @Override
    public void onRestart() {
        super.onRestart();
    }
    
    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
    public void onPause() {
        super.onPause();
    }
    
    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
	@Override
    public void onStop() {

        super.onStop();
    }

    /**
     * Implementation function: defined in libnvevent.a
     * The application should *probably* not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
	 *
	 * NOTE: An application may need to override this if the app has an
	 *       in-process instance of the Service class and the native side wants to
	 *       keep running. The app would want to execute the content of the
	 *       if(supportPauseResume) clause when it is time to exit.
     */
    @Override
    public void onDestroy() {
//        if (this.supportPauseResume) {
//            quitAndWait();
//            finish();
//        }
        finishAndRemoveTask();
        super.onDestroy();
        systemCleanup();
        //systemCleanup();
    }

    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */

    public void mSleep(long milis) {
        try {
            Thread.sleep(milis);
        } catch (InterruptedException ignored) {
        }
    }

	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// Auto-generated method stub
	}

    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
	public void onSensorChanged(SensorEvent event) {
		// Auto-generated method stub
//		if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
//        {
//            float roll = 0.0f;
//            float pitch = 0.0f;
//            switch (this.display.getRotation()) {
//                case 0:
//                    roll = -event.values[0];
//                    pitch = event.values[1];
//                    break;
//                case 1:
//                    roll = event.values[1];
//                    pitch = event.values[0];
//                    break;
//                case 2:
//                    roll = event.values[0];
//                    pitch = event.values[1];
//                    break;
//                case 3:
//                    roll = -event.values[1];
//                    pitch = event.values[0];
//                    break;
//            }
//            accelerometerEvent(roll, pitch, event.values[2]);
//        }
	}
    
    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        return super.onTouchEvent(event);
    }

    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        boolean ret = false;

        if (keyCode == 24 || keyCode == 25) {
            return super.onKeyDown(keyCode, event);
        }
        if (keyCode == 89 || keyCode == 85 || keyCode == 90) {
            return false;
        }
        if (!(keyCode == 82 || keyCode == 4)) {
            ret = super.onKeyDown(keyCode, event);
        }
        if (!ret) {
            ret = keyEvent(event.getAction(), keyCode, event.getUnicodeChar(), event.getMetaState(), event);
        }
        return ret;
    }
 
    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        if (keyCode == 115 && Build.VERSION.SDK_INT >= 11)
        {
            boolean capsLockOn = event.isCapsLockOn();
            keyEvent(capsLockOn ? 3 : 4, 115, 0, 0, event);
        }
        if (keyCode == 89 || keyCode == 85 || keyCode == 90)
        {
            return false;
        }
        boolean onKeyUp = super.onKeyUp(keyCode, event);
        if (onKeyUp)
        {
            return onKeyUp;
        }
        return keyEvent(event.getAction(), keyCode, event.getUnicodeChar(), event.getMetaState(), event);
    }

    public boolean InitEGLAndGLES2(int EGLVersion) {
        System.out.println("InitEGLAndGLES2");
        if (this.cachedSurfaceHolder == null) {
            System.out.println("InitEGLAndGLES2 failed, cachedSurfaceHolder is null");
            return false;
        }
        boolean eglInitialized = true;
        if (this.eglContext == null) {
            eglInitialized = false;
            if (EGLVersion >= 3) {
                try {
                    eglInitialized = initEGL(3, 24);
                } catch (Exception e) {
                }
                System.out.println("initEGL 3 " + eglInitialized);
            }
            if (!eglInitialized) {
                this.configAttrs = null;
                try {
                    eglInitialized = initEGL(2, 16);
                } catch (Exception e2) {
                }
                System.out.println("initEGL 2 " + eglInitialized);
                if (!eglInitialized) {
                    eglInitialized = initEGL(2, 16);
                    System.out.println("initEGL 2 " + eglInitialized);
                }
            }
        }
        if (eglInitialized) {
            System.out.println("Should we create a surface?");
            if (!this.viewIsActive) {
                System.out.println("Yes! Calling create surface");
                createEGLSurface(this.cachedSurfaceHolder);
                System.out.println("Done creating surface");
            }
            this.viewIsActive = true;
            this.SwapBufferSkip = 1;
            return true;
        }
        System.out.println("initEGLAndGLES2 failed, core EGL init failure");
        return false;
    }
		
    /**
     * Implementation function: defined in libnvevent.a
     * The application does not and should not overide this; nv_event handles this internally
     * And remaps as needed into the native calls exposed by nv_event.h
     */

    public SurfaceView GetSurfaceView()
    {
        return mSurfaceView;
    }

    protected boolean systemInit()
    {
        final NvEventQueueActivity act = this;

		System.out.println("ln systemInit");

        setContentView(R.layout.main_render_screen);

        SurfaceView view = findViewById(R.id.main_sv);
        getWindow().setSustainedPerformanceMode(true);
        mSurfaceView = view;

        mAndroidUI = findViewById(R.id.ui_layout);

        SurfaceHolder holder = view.getHolder();
        holder.setType(2);
        holder.setKeepScreenOn(true);

        view.setFocusable(true);
        view.setFocusableInTouchMode(true);

        mSurfaceView.setOnTouchListener(this);

        DoResumeEvent();

        holder.addCallback(new Callback()
        {
            // @Override
            public void surfaceCreated(SurfaceHolder holder)
            {
                System.out.println("systemInit.surfaceCreated");
                boolean firstRun = NvEventQueueActivity.this.cachedSurfaceHolder == null;
                cachedSurfaceHolder = holder;

                if (fixedWidth!=0 && fixedHeight!=0)
                {
                    System.out.println("Setting fixed window size");
                    holder.setFixedSize(fixedWidth, fixedHeight);
                }

                ranInit = true;
                if(!supportPauseResume && !init(true))
                {
                    handler.post(new Runnable() {
                        public void run() {
                            Log.d("TAG", "ERR handler.post gl");
                        }
                    });
                }

                if(!firstRun && ResumeEventDone)
                {
                    System.out.println("entering resumeEvent");
                    resumeEvent();
                    paused = false;
                    System.out.println("returned from resumeEvent");
                }
                setGameWindowSize(surfaceWidth, surfaceHeight);
            }

            /**
             * Implementation function: defined in libnvevent.a
             * The application does not and should not overide this; nv_event handles this internally
             * And remaps as needed into the native calls exposed by nv_event.h
             */
            // @Override
            public void surfaceChanged(SurfaceHolder holder, int format,
                                       int width, int height)
            {
                System.out.println("Surface changed: " + width + ", " + height);
                surfaceWidth = width;
                surfaceHeight = height;
                setGameWindowSize(surfaceWidth, surfaceHeight);
            }

            /**
             * Implementation function: defined in libnvevent.a
             * The application does not and should not overide this; nv_event handles this internally
             * And remaps as needed into the native calls exposed by nv_event.h
             */
            // @Override
            public void surfaceDestroyed(SurfaceHolder holder)
            {
                System.out.println("systemInit.surfaceDestroyed");
                viewIsActive = false;
                pauseEvent();
                paused = true;
                destroyEGLSurface();
            }
        });
        return true;
    }

    public void DoResumeEvent() {
        new Thread(new Runnable() {
            public void run() {
                while (NvEventQueueActivity.this.cachedSurfaceHolder == null) {
                    NvEventQueueActivity.this.mSleep(1000);
                }
                Log.d("TAG", "Call from DoResumeEvent");
                NvEventQueueActivity.this.resumeEvent();
                Log.d("TAG","DoResumeEvent done");
                ResumeEventDone = true;
            }
        }).start();
    }

    /** The number of bits requested for the red component */
    protected int redSize     = 5;
    /** The number of bits requested for the green component */
    protected int greenSize   = 6;
    /** The number of bits requested for the blue component */
    protected int blueSize    = 5;
    /** The number of bits requested for the alpha component */
    protected int alphaSize   = 0;
    /** The number of bits requested for the stencil component */
    protected int stencilSize = 0;
    /** The number of bits requested for the depth component */
    protected int depthSize   = 16;

    /** Attributes used when selecting the EGLConfig */
    protected int[] configAttrs = null;
    /** Attributes used when creating the context */
    protected int[] contextAttrs = null;

    /**
     * Called to initialize EGL. This function should not be called by the inheriting
     * activity, but can be overridden if needed.
     * 
     * @return True if successful
     */
    public boolean initEGL(int esVersion, int depthBits) {
        int i;
        int eglErr;
        if (esVersion > 2 && Build.VERSION.SDK_INT < 21) {
            return false;
        }
        if (this.configAttrs == null) {
            this.configAttrs = new int[]{12344};
        }
        int[] oldConf = this.configAttrs;
        this.configAttrs = new int[((oldConf.length + 3) - 1)];
        int i2 = 0;
        while (i2 < oldConf.length - 1) {
            this.configAttrs[i2] = oldConf[i2];
            i2++;
        }
        int i3 = i2 + 1;
        this.configAttrs[i2] = EGL_RENDERABLE_TYPE;
        if (esVersion == 3) {
            i = i3 + 1;
            this.configAttrs[i3] = EGL_OPENGL_ES3_BIT;
        } else {
            i = i3 + 1;
            this.configAttrs[i3] = 4;
        }
        int i4 = i + 1;
        this.configAttrs[i] = 12344;
        this.contextAttrs = new int[]{EGL_CONTEXT_CLIENT_VERSION, esVersion, 12344};
        if (this.configAttrs == null) {
            this.configAttrs = new int[]{12344};
        }
        int[] oldConfES2 = this.configAttrs;
        this.configAttrs = new int[((oldConfES2.length + 13) - 1)];
        int i5 = 0;
        while (i5 < oldConfES2.length - 1) {
            this.configAttrs[i5] = oldConfES2[i5];
            i5++;
        }
        int i6 = i5 + 1;
        this.configAttrs[i5] = 12324;
        int i7 = i6 + 1;
        this.configAttrs[i6] = this.redSize;
        int i8 = i7 + 1;
        this.configAttrs[i7] = 12323;
        int i9 = i8 + 1;
        this.configAttrs[i8] = this.greenSize;
        int i10 = i9 + 1;
        this.configAttrs[i9] = 12322;
        int i11 = i10 + 1;
        this.configAttrs[i10] = this.blueSize;
        int i12 = i11 + 1;
        this.configAttrs[i11] = 12321;
        int i13 = i12 + 1;
        this.configAttrs[i12] = this.alphaSize;
        int i14 = i13 + 1;
        this.configAttrs[i13] = 12326;
        int i15 = i14 + 1;
        this.configAttrs[i14] = this.stencilSize;
        int i16 = i15 + 1;
        this.configAttrs[i15] = 12325;
        int i17 = i16 + 1;
        this.configAttrs[i16] = depthBits;
        int i18 = i17 + 1;
        this.configAttrs[i17] = 12344;
        this.egl = (EGL10) EGLContext.getEGL();
        this.egl.eglGetError();
        this.eglDisplay = this.egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
        System.out.println("eglDisplay: " + this.eglDisplay + ", err: " + this.egl.eglGetError());
        boolean ret = this.egl.eglInitialize(this.eglDisplay, new int[2]);
        System.out.println("EglInitialize returned: " + ret);
        if (!ret || (eglErr = this.egl.eglGetError()) != 12288) {
            return false;
        }
        System.out.println("eglInitialize err: " + eglErr);
        EGLConfig[] config = new EGLConfig[20];
        int[] num_configs = new int[1];
        this.egl.eglChooseConfig(this.eglDisplay, this.configAttrs, config, config.length, num_configs);
        System.out.println("eglChooseConfig err: " + this.egl.eglGetError());
        System.out.println("num_configs " + num_configs[0]);
        int score = 16777216;
        int[] val = new int[1];
        for (int i19 = 0; i19 < num_configs[0]; i19++) {
            boolean cont = true;
            int j = 0;
            while (true) {
                if (j >= ((oldConfES2.length - 1) >> 1)) {
                    break;
                }
                this.egl.eglGetConfigAttrib(this.eglDisplay, config[i19], this.configAttrs[j * 2], val);
                if ((val[0] & this.configAttrs[(j * 2) + 1]) != this.configAttrs[(j * 2) + 1]) {
                    cont = false;
                    break;
                }
                j++;
            }
            if (cont) {
                this.egl.eglGetConfigAttrib(this.eglDisplay, config[i19], 12324, val);
                int r = val[0];
                this.egl.eglGetConfigAttrib(this.eglDisplay, config[i19], 12323, val);
                int g = val[0];
                this.egl.eglGetConfigAttrib(this.eglDisplay, config[i19], 12322, val);
                int b = val[0];
                this.egl.eglGetConfigAttrib(this.eglDisplay, config[i19], 12321, val);
                int a = val[0];
                this.egl.eglGetConfigAttrib(this.eglDisplay, config[i19], 12325, val);
                int d = val[0];
                this.egl.eglGetConfigAttrib(this.eglDisplay, config[i19], 12326, val);
                int currScore = ((((Math.abs(r - this.redSize) + Math.abs(g - this.greenSize)) + Math.abs(b - this.blueSize)) + Math.abs(a - this.alphaSize)) << 16) + (Math.abs(d - depthBits) << 8) + Math.abs(val[0] - this.stencilSize);
                if (currScore < score) {
                    for (int j2 = 0; j2 < ((this.configAttrs.length - 1) >> 1); j2++) {
                        this.egl.eglGetConfigAttrib(this.eglDisplay, config[i19], this.configAttrs[j2 * 2], val);
                    }
                    score = currScore;
                    this.eglConfig = config[i19];
                }
            }
        }
        if (this.eglConfig == null) {
            this.configAttrs = null;
            return false;
        }
        this.eglContext = this.egl.eglCreateContext(this.eglDisplay, this.eglConfig, EGL10.EGL_NO_CONTEXT, this.contextAttrs);
        System.out.println("eglCreateContext: " + this.egl.eglGetError());
        this.gl = (GL11) this.eglContext.getGL();
        return true;
    }

    /**
     * Called to create the EGLSurface to be used for rendering. This function should not be called by the inheriting
     * activity, but can be overridden if needed.
     * 
     * @param surface The SurfaceHolder that holds the surface that we are going to render to.
     * @return True if successful
     */
    public boolean createEGLSurface(SurfaceHolder surfaceHolder) {
        this.eglSurface = this.egl.eglCreateWindowSurface(this.eglDisplay, this.eglConfig, surfaceHolder, (int[]) null);
        Log.d("dfs","eglCreateWindowSurface err: " + this.egl.eglGetError());
        int[] iArr = new int[1];
        this.egl.eglQuerySurface(this.eglDisplay, this.eglSurface, 12375, iArr);
        this.surfaceWidth = iArr[0];
        this.egl.eglQuerySurface(this.eglDisplay, this.eglSurface, 12374, iArr);
        this.surfaceHeight = iArr[0];
        Log.d("dfs","checking glVendor == null?");
        if (this.glVendor == null) {
            Log.d("dfs","Making current and back");
            makeCurrent();
            unMakeCurrent();
        }
        Log.d("dfs","Done create EGL surface");
        return true;
    }

    /**
     * Destroys the EGLSurface used for rendering. This function should not be called by the inheriting
     * activity, but can be overridden if needed.
     */
    protected void destroyEGLSurface()
    {
        System.out.println("*** destroyEGLSurface");
        if (eglDisplay != null && eglSurface != null)
            egl.eglMakeCurrent(eglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
        if (eglSurface != null)
            egl.eglDestroySurface(eglDisplay, eglSurface);
        eglSurface = null;
    }

    /**
     * Called to clean up egl. This function should not be called by the inheriting
     * activity, but can be overridden if needed.
     */
    protected void cleanupEGL()
    {
		System.out.println("cleanupEGL");
        destroyEGLSurface();
        if (eglDisplay != null)
            egl.eglMakeCurrent(eglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
        if (eglContext != null)
            egl.eglDestroyContext(eglDisplay, eglContext);
        if (eglDisplay != null)
            egl.eglTerminate(eglDisplay);

        eglDisplay = null;
        eglContext = null;
        eglSurface = null;

		ranInit = false;
		eglConfig = null;

		cachedSurfaceHolder = null;
		surfaceWidth = 0;
		surfaceHeight = 0;
    }

    /**
     * Implementation function: 
     * The application does not and should not overide or call this directly
     * Instead, the application should call NVEventEGLSwapBuffers(),
     * which is declared in nv_event.h
     */

    public boolean swapBuffers()
    {
		//long stopTime;
		//long startTime = nvGetSystemTime();

        if (SwapBufferSkip > 0) {
            SwapBufferSkip--;
            System.out.println("swapBuffer wait");
            return true;
        }
        if (eglSurface == null)
        {
	        System.out.println("eglSurface is NULL");
	        return false;
	    }
        else if (!egl.eglSwapBuffers(eglDisplay, eglSurface))
        {
	        System.out.println("eglSwapBufferrr: " + egl.eglGetError());
	        return false;
	    }
		//stopTime = nvGetSystemTime();
		//String s = String.format("%d ms in eglSwapBuffers", (int)(stopTime - startTime));
		//Log.v("EventAccelerometer", s);
	    
	    return true;
    }    

	public boolean getSupportPauseResume()
	{
		return supportPauseResume;
	}
    
    public int getSurfaceWidth()
    {
    	return surfaceWidth;        
    }
    
    public int getSurfaceHeight()
    {
    	return surfaceHeight;           
    }
   
    /**
     * Implementation function: 
     * The application does not and should not overide or call this directly
     * Instead, the application should call NVEventEGLMakeCurrent(),
     * which is declared in nv_event.h
     */

    public void GetGLExtensions()
    {
        if (!HasGLExtensions && gl != null && this.cachedSurfaceHolder != null)
        {
           // gl.glEnable(GL10.GL_CULL_FACE); // ? сглаживание
            glVendor = gl.glGetString(GL10.GL_VENDOR);
            glExtensions = gl.glGetString(GL10.GL_EXTENSIONS);
            glRenderer = gl.glGetString(GL10.GL_RENDERER);
            glVersion = gl.glGetString(GL10.GL_VERSION);
            System.out.println("Vendor: " + glVendor);
            System.out.println("Extensions " + glExtensions);
            System.out.println("Renderer: " + glRenderer);
            System.out.println("GIVersion: " + glVersion);
            if (this.glVendor != null)
            {
                this.HasGLExtensions = true;
            }
        }
    }

    public boolean makeCurrent()
    {
        if (eglContext == null)
		{
	        System.out.println("eglContext is NULL");
	        return false;
	    }
        else if (eglSurface == null)
        {
	        System.out.println("eglSurface is NULL");
	        return false;
	    }
        else if (!egl.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
        {
            if (!egl.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
            {
                System.out.println("eglMakeCurrent err: " + egl.eglGetError());
                return false;
            }
        }
	    
        // This must be called after we have bound an EGL context
        //nvAcquireTimeExtension();
        GetGLExtensions();
	    return true;
    }

	public int getOrientation()
	{
        return display.getOrientation();
	}

    /**
     * Implementation function: 
     * The application does not and should not overide or call this directly
     * Instead, the application should call NVEventEGLUnmakeCurrent(),
     * which is declared in nv_event.h
     */
    public boolean unMakeCurrent()
    {
        if (!egl.eglMakeCurrent(eglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT))
		{
	        System.out.println("egl(Un)MakeCurrent err: " + egl.eglGetError());
	        return false;
	    }
	    
	    return true;
    }

    /**
     * Called when the Activity is exiting and it is time to cleanup.
     * Kept separate from the {@link #cleanup()} function so that subclasses
     * in their simplest form do not need to call any of the parent class' functions. This to make
     * it easier for pure C/C++ application so that these do not need to call java functions from C/C++
     * code.
     * 
     * @see #cleanup()
     */
    protected void systemCleanup()
    {
        if (ranInit)
            cleanup();
        cleanupEGL();

        //postCleanup();
    }

    public byte[] getClipboardText()
    {
        String retn = " ";

        if(mClipboardManager.getPrimaryClip() != null)
        {
            ClipData.Item item = mClipboardManager.getPrimaryClip().getItemAt(0);
            if(item != null)
            {
                CharSequence sequence = item.getText();
                if(sequence != null)
                {
                    retn = sequence.toString();
                }
            }
        }

        byte[] toReturn = null;
        try
        {
            toReturn = retn.getBytes("windows-1251");
        }
        catch(UnsupportedEncodingException e)
        {

        }
        return toReturn;
    }

    /*public void setPauseState(boolean z2) {
        if (mAndroidUI == null) {
            mAndroidUI = (FrameLayout) findViewById(R.id.ui_layout);
        }
        runOnUiThread(() -> mAndroidUI.setVisibility(z2 ? View.GONE:View.VISIBLE));
    }*/

    public boolean IsPortrait() {
        return false;
    }

    public void setGameWindowSize(int w, int h) {
        if ((!IsPortrait() || w <= h) && (IsPortrait() || h <= w)) {
            setWindowSize(w, h);
        } else {
            setWindowSize(h, w);
        }
    }
}
