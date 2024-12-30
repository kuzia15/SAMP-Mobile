package com.samp.mobile.launcher;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.FileUtils;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.content.FileProvider;

import com.samp.mobile.R;
import com.samp.mobile.launcher.config.Config;

import java.io.File;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class UpdateActivity extends AppCompatActivity{
    public Messenger mMessenger = new Messenger(new IncomingHandler());
    public Messenger mService;
    boolean isBind = false;
    private UpdateMode mUpdateMode = UpdateMode.Undefined;
    public int mGpuType;

    private File mGameApk;

    boolean mIsStartingUpdate = false;

    private boolean mIsBind;

    public enum UpdateMode {
        Undefined,
        GameDataUpdate,
        GameUpdate
    }

    public enum GameStatus {
        Unknown,
        GameUpdateRequired,
        UpdateRequired,
        Updated
    }

    public enum UpdateStatus {
        Undefined,
        CheckUpdate,
        CheckFiles,
        DownloadGame,
        DownloadGameData
    }

    public enum eGPUType {
        DXT,
        PVR,
        ETC
    }

    class IncomingHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 4) {
                UpdateActivity.UpdateStatus valueOf = UpdateActivity.UpdateStatus.valueOf(msg.getData().getString("status", ""));
                if (valueOf == UpdateStatus.DownloadGameData) {
                    ((TextView)findViewById(R.id.installation_text)).setText("Updating game data...");
                    Log.d("x1y2z", "statusname = " + valueOf);
                    long j = msg.getData().getLong("total");
                    long j2 = msg.getData().getLong("current");
                    ((TextView) findViewById(R.id.fileName)).setText(msg.getData().getString("filename"));
                    ((TextView) findViewById(R.id.fileCount)).setText(j2/1048576 + "MB/" + j/1048576+"MB");
                    ProgressBar progressBar = findViewById(R.id.download_progress);
                    progressBar.setIndeterminate(false);
                    Log.d("UpdateActivity", (int) (j/1048576) + "/" + (int) (j2/1048576));
                    progressBar.setMax((int) (j/1048576));
                    progressBar.setProgress((int) (j2/1048576));

                    ((TextView) findViewById(R.id.fileProgressPercent)).setText(j2*100/(j+1) + "%");
                } else if (valueOf == UpdateActivity.UpdateStatus.CheckUpdate) {
                    Log.d("x1y2z", "statusname = " + valueOf);
                    long j = msg.getData().getLong("total");
                    long j2 = msg.getData().getLong("current");
                    ((TextView) findViewById(R.id.fileName)).setText(msg.getData().getString("filename"));
                    ProgressBar progressBar = (ProgressBar) UpdateActivity.this.findViewById(R.id.download_progress);
                    progressBar.setMax((int) (j)/1048576);
                    progressBar.setProgress((int) (j2)/1048576);
                } else if (valueOf == UpdateStatus.DownloadGame) {
                    ((TextView)findViewById(R.id.installation_text)).setText("Updating game...");
                    Log.d("x1y2z", "statusname = " + valueOf);
                    long j = msg.getData().getLong("total");
                    long j2 = msg.getData().getLong("current");
                    ((TextView) findViewById(R.id.fileName)).setText(msg.getData().getString("filename"));
                    ((TextView) findViewById(R.id.fileCount)).setText(msg.getData().getLong("currentfile") + "/" + msg.getData().getLong("totalfiles"));
                    ProgressBar progressBar = (ProgressBar) UpdateActivity.this.findViewById(R.id.download_progress);
                    progressBar.setIndeterminate(false);
                    progressBar.setMax((int) (j)/1048576);
                    progressBar.setProgress((int) (j2)/1048576);
                }else if (!mIsStartingUpdate) {
                    Message obtain2 = Message.obtain((Handler) null, 1);
                    obtain2.replyTo = UpdateActivity.this.mMessenger;
                    try {
                        UpdateActivity.this.mService.send(obtain2);
                    } catch (RemoteException e6) {
                        e6.printStackTrace();
                    }
                    mIsStartingUpdate = true;
                }
            }
            else if(msg.what == 2)
            {
                Intent intent;
                Log.d("x1y2z", "UpdateService.UPDATE_GAME_DATA");
                //resetProgress(true, 100, 100);
                if (msg.getData().getBoolean("status", false)) {
                    String string3 = msg.getData().getString("apkPath", "");
                    if (string3.length() > 0) {
                        mGameApk = new File(string3);
                    }
                    if (mGameApk == null || !mGameApk.exists()) {
                        Log.d("x1y2z", "Error update game data");
                        intent = new Intent(UpdateActivity.this, MainActivity.class);
                        startActivity(intent);
                        finish();
                    } else {
                        requestInstallGame();
                    }
                }
            }
            else if (msg.what == 1) {
                Log.i("UpdateActivity", "UpdateService.UPDATE_GAME");
                ((TextView)findViewById(R.id.installation_text)).setText("Installation...");
                ProgressBar progressBar = (ProgressBar) UpdateActivity.this.findViewById(R.id.download_progress);
                progressBar.setIndeterminate(true);
                startActivity(new Intent(UpdateActivity.this, MainActivity.class));
                finish();
            }
        }
    }

    void requestInstallGame()
    {
        Log.d("x1y2z", "request install game");

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            Uri uri = FileProvider.getUriForFile(getApplicationContext(),
                    getApplicationContext().getApplicationContext().getPackageName() + ".provider", mGameApk);
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.setDataAndType(uri, "application/vnd.android.package-archive");
            startActivity(intent);

        } else if(Build.VERSION.SDK_INT < Build.VERSION_CODES.N) {
            Uri apkUri = Uri.fromFile(mGameApk);
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setDataAndType(apkUri,
                    "application/vnd.android.package-archive");
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);
        }
        else {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(UpdateActivity.this, "Error, while installing apk. Reenter to the launcher.", Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Log.i("UpdateActivity", "onActivityResult -> code quest: " + resultCode + ", resultCode: " + data);
        if (requestCode == 0) {
            if (mGameApk != null && mGameApk.exists()) {
                mGameApk.delete();
            }
            startActivity(new Intent(this, SplashActivity.class));
            finish();
        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_update);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        Config.currentContext = this;

        GLSurfaceView.Renderer mGlRenderer = new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
                eGPUType egputype;
                String glGetString = gl10.glGetString(GL10.GL_EXTENSIONS);
                String glGetString2 = gl10.glGetString(GL10.GL_EXTENSIONS);
                if (glGetString2.contains("GL_IMG_texture_compression_pvrtc")) {
                    egputype = eGPUType.PVR;
                    mGpuType = 3;
                } else if (glGetString2.contains("GL_EXT_texture_compression_dxt1") || glGetString2.contains("GL_EXT_texture_compression_s3tc") || glGetString2.contains("GL_AMD_compressed_ATC_texture")) {
                    egputype = eGPUType.DXT;
                    mGpuType = 1;
                } else {
                    egputype = eGPUType.ETC;
                    mGpuType = 2;
                }
                Log.e("x1y2z", "GPU name: " + glGetString);
                Log.e("x1y2z", "GPU type: " + egputype.name());

                mUpdateMode = UpdateMode.valueOf(getIntent().getStringExtra("mode"));

                if (mService == null) {
                    Intent intent = new Intent(UpdateActivity.this, UpdateService.class);
                    mIsBind = bindService(intent, serviceConnection, Context.BIND_AUTO_CREATE);
                } else if (mUpdateMode == UpdateMode.GameDataUpdate) {
                    Message obtain = Message.obtain((Handler) null, 7);
                    obtain.getData().putInt("gputype", mGpuType);
                    obtain.replyTo = mMessenger;
                    try {
                        mService.send(obtain);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }

                }
            }

            @Override
            public void onSurfaceChanged(GL10 gl10, int i, int i1) {

            }

            @Override
            public void onDrawFrame(GL10 gl10) {

            }
        };

        ConstraintLayout gpuLayout = findViewById(R.id.gpu);
        GLSurfaceView mGlSurfaceView = new GLSurfaceView(this);
        mGlSurfaceView.setRenderer(mGlRenderer);
        gpuLayout.addView(mGlSurfaceView);
    }

    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            mService = new Messenger(iBinder);

            if (mUpdateMode == UpdateMode.GameDataUpdate) {
                Message obtain = Message.obtain((Handler) null, 7);
                obtain.getData().putInt("gputype", mGpuType);
                obtain.replyTo = mMessenger;
                try {
                    mService.send(obtain);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
            else if (mUpdateMode == UpdateMode.GameUpdate) {
                Message obtain = Message.obtain((Handler) null, 2);
                obtain.getData().putInt("gputype", mGpuType);
                obtain.replyTo = mMessenger;
                try {
                    mService.send(obtain);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

            isBind = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mService = null;
            isBind = false;
        }
    };

    @Override
    public void onDestroy() {
        super.onDestroy();
        if(mIsBind) {
            unbindService(serviceConnection);
            mIsBind = false;
        }
    }
}
