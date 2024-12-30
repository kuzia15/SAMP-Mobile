package com.wardrumstudios.utils;

import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;

import com.nvidia.devtech.NvUtil;

import java.io.File;

public class WarMedia extends WarGamepad
{
    private String baseDirectory;
    private String baseDirectoryRoot;

    public String GetGameBaseDirectory() {
        if (Environment.getExternalStorageState().equals("mounted"))
        {
            try
            {
                File externalFilesDir = getExternalFilesDir(null);
                String absolutePath = externalFilesDir.getAbsolutePath();
                this.baseDirectoryRoot = absolutePath.substring(0, absolutePath.indexOf("/Android"));
                return externalFilesDir.getAbsolutePath() + "/";
                //return Environment.getExternalStorageDirectory() + "/SAMPMOBILE/";
            } catch (Exception e)
            {
            }
        }
        return "";
    }

    public void onCreate(Bundle bundle)
    {
        this.baseDirectory = GetGameBaseDirectory();

        NvUtil.getInstance().setActivity(this);
        NvUtil.getInstance().setAppLocalValue("STORAGE_ROOT", this.baseDirectory);
        NvUtil.getInstance().setAppLocalValue("STORAGE_ROOT_BASE", this.baseDirectoryRoot);

        super.onCreate(bundle);
    }
    public void ShowKeyboard(int i)
    {
        System.out.println("**** ShowKeyboard");
    }

    public boolean IsKeyboardShown()
    {
        System.out.println("**** IsKeyboardShown");
        return false;
    }

    public void PlayMovie(String str, float f)
    {
        System.out.println("**** PlayMovie");
    }

    public void PlayMovieInFile(String str, float f, int i, int i2)
    {
        System.out.println("**** PlayMovieInFile");
    }

    public void PlayMovieInWindow(String str, int i, int i2, int i3, int i4, float f, int i5, int i6, int i7) {
        System.out.println("**** PlayMovieInWindow");
    }

    public void StopMovie() {
        System.out.println("**** StopMovie");
    }

    public void MovieSetSkippable(boolean z)
    {
        System.out.println("**** MovieSetSkippable");
    }

    public int IsMoviePlaying()
    {
        System.out.println("**** IsMoviePlaying");
        return 0;
    }

    public boolean DeleteFile(String str)
    {
        System.out.println("**** DeleteFile");
        return true;
    }

    public boolean FileRename(String str, String str2, int i)
    {
        System.out.println("**** FileRename");
        return true;
    }
    public int GetDeviceLocale() {
        System.out.println("**** GetDeviceLocale");
        return 7;
    }

    public boolean IsPhone() // TODO: implement this
    {
        System.out.println("**** IsPhone");
        return true;
    }

    public int GetDeviceType() // TODO: implement this
    {

        int i = 0;
        System.out.println("Build info version device  " + Build.DEVICE);
        System.out.println("Build MANUFACTURER  " + Build.MANUFACTURER);
        System.out.println("Build BOARD  " + Build.BOARD);
        System.out.println("Build DISPLAY  " + Build.DISPLAY);
        System.out.println("Build CPU_ABI  " + Build.CPU_ABI);
        System.out.println("Build CPU_ABI2  " + Build.CPU_ABI2);
        System.out.println("Build HARDWARE  " + Build.HARDWARE);
        System.out.println("Build MODEL  " + Build.MODEL);
        System.out.println("Build PRODUCT  " + Build.PRODUCT);
        int i2 = 0;
        int numberOfProcessors = 1 * 4;
        int i3 = 8 * 64;
        if (IsPhone())
        {
            i = 1;
        }
        return i + i2 + numberOfProcessors + i3;
    }

    public int GetDeviceInfo(int i) // TODO: implement this
    {
        System.out.println("**** GetDeviceInfo");
        switch (i) {
            case 0:
                return 1; // num of cpu
            case 1:
                System.out.println("Return for touchsreen 1");
                return 1; // touch screen
            default:
                return -1;
        }
    }

    public String GetAndroidBuildinfo(int i) {
        System.out.println("**** GetAndroidBuildinfo " + i);
        switch (i) {
            case 0:
                return Build.MANUFACTURER;
            case 1:
                return Build.PRODUCT;
            case 2:
                return Build.MODEL;
            default:
                return "UNKNOWN";
        }
    }

    public String OBFU_GetDeviceID() {
        System.out.println("**** OBFU_GetDeviceID");
        return "no id";
    }

    public String FileGetArchiveName(int i) {
        System.out.println("**** FileGetArchiveName " + i);
        switch (i) {
            case 0:
                return ""; // apkFileName
            case 1:
                return ""; // expansionFileName
            case 2:
                return ""; // this.patchFileName;
            default:
                return "";
        }
    }

    public boolean IsAppInstalled(String str) {
        System.out.println("**** IsAppInstalled");
        return false;
    }

    public void OpenLink(String str)
    {
        startActivity(new Intent("android.intent.action.VIEW", Uri.parse(str)));
        System.out.println("**** OpenLink");
    }

    public void LoadAllGamesFromCloud() {
        System.out.println("**** LoadAllGamesFromCloud");
    }

    public String LoadGameFromCloud(int i, byte[] bArr) {
        System.out.println("**** LoadGameFromCloud");
        return "";
    }

    public void SaveGameToCloud(int i, byte[] bArr, int i2) {
        System.out.println("**** SaveGameToCloud");
    }

    public boolean IsCloudAvailable() {
        System.out.println("**** IsCloudAvailable");
        return false;
    }

    public boolean NewCloudSaveAvailable(int i) {
        System.out.println("**** NewCloudSaveAvailable");
        return false;
    }

    public void MovieKeepAspectRatio(boolean z) {
        System.out.println("**** MovieKeepAspectRatio");
    }

    public void MovieSetText(String str, boolean z, boolean z2) {
        System.out.println("**** MovieSetText");
    }

    public void MovieDisplayText(boolean z) {
        System.out.println("**** MovieDisplayText");
    }

    public void MovieClearText(boolean z) {
        System.out.println("**** MovieClearText");
    }

    public void MovieSetTextScale(int i) {
        System.out.println("**** MovieSetTextScale");
    }

    public int GetSpecialBuildType() {
        System.out.println("**** GetSpecialBuildType");
        return 0;
    }

    public void SendStatEvent(String str) {
        System.out.println("**** SendStatEvent");
    }

    public void SendStatEvent(String eventId, boolean timedEvent) {
        System.out.println("**** SendStatEvent");
    }

    public void SendStatEvent(String eventId, String paramName, String paramValue, boolean timedEvent) {
        System.out.println("**** SendStatEvent");
    }

    public void SendTimedStatEventEnd(String eventId) {
        System.out.println("**** SendTimedStatEventEnd");
    }

    public void SendStatEvent(String str, String str2, String str3) {
        System.out.println("**** SendStatEvent1");
    }

    public int GetTotalMemory() {
        System.out.println("**** GetTotalMemory");
        return 0;
    }

    public int GetLowThreshhold() {
        System.out.println("**** GetLowThreshhold");
        return 0;
    }

    public int GetAvailableMemory() {
        System.out.println("**** GetAvailableMemory");
        return 0;
    }

    public float GetScreenWidthInches() {
        System.out.println("**** GetScreenWidthInches");
        return 0.0f;
    }

    public String GetAppId()
    {
        System.out.println("**** GetAppId");
       return "";
    }

    public void ScreenSetWakeLock(boolean z) {
        System.out.println("**** ScreenSetWakeLock");
    }

    public boolean ServiceAppCommand(String str, String str2) {
        System.out.println("**** ServiceAppCommand " + str + " " + str2);
        return false;
    }

    public int ServiceAppCommandValue(String str, String str2) {
        System.out.println("**** ServiceAppCommandValue " + str + " " + str2);
        return 0;
    }

    public boolean ServiceAppCommandInt(String cmd, int args) {
        System.out.println("**** ServiceAppCommandInt " + cmd + " " + args);
        return false;
    }

    public boolean isWiFiAvailable() {
        System.out.println("**** isWiFiAvailable ");
        return false;
    }

    public boolean isNetworkAvailable() {
        System.out.println("**** isNetworkAvailable ");
        return false;
    }

    public boolean isTV() {
        System.out.println("**** isTV ");
        return false;
    }

    public void CreateTextBox(int id, int x, int y, int x2, int y2) {
        System.out.println("**** CreateTextBox ");
    }

    public boolean ConvertToBitmap(byte[] data, int length) {
        System.out.println("**** ConvertToBitmap ");
        return false;
    }

    public void VibratePhone(int numMilliseconds) {
        System.out.println("**** VibratePhone ");
    }

    public void VibratePhoneEffect(int effect) {
        System.out.println("**** VibratePhoneEffect ");
    }

    public void PlayMovieInWindow(String inFilename, int x, int y, int width, int height, float inVolume, int inOffset, int inLength, int looping, boolean forceSize) {
        System.out.println("**** PlayMovieInWindow ");
    }
}
