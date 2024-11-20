//----------------------------------------------------------------------------------
// File:            libs\src\com\nvidia\devtech\AudioHelper.java
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

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.util.Log;

import java.io.IOException;

public class AudioHelper 
{
	private static final int MAX_SOUND_STREAMS = 10;
	private static final String TAG = "AudioHelper";
	private static String ResourceLocation = "com.nvidia.devtech.audio:raw/";
	
	private Context 			context = null;    
	private SoundPool			Sounds = null;
	private	MediaPlayer			MusicPlayer = null;
	private static AudioHelper 	instance = null; 
	
	private AudioHelper()
	{	
	}
	
    void Initialise()
    {     	
    	Sounds = new SoundPool(MAX_SOUND_STREAMS,  AudioManager.STREAM_MUSIC, 0); 
        
        if (Sounds == null)
        {
        	Log.e(TAG, "failed to create soundpool instance");
        }
        
        Log.i(TAG, "created sound pool");
    }	
    
    public void finalize()
	{		
		if (Sounds != null)
		{
			Sounds.release();
			Sounds = null;
		}
		
		if (MusicPlayer != null)
		{
			MusicPlayer.release();
			MusicPlayer = null;
		}
	}
    
    public void SetResouceLocation(String s)
    {
    	ResourceLocation = s;
    }

    public void setContext(Context context)
    {
        this.context = context;
    }
	
    public static AudioHelper getInstance()
    {
    	if (instance == null)
    	{
    		instance = new AudioHelper();
    		
    		instance.Initialise();    		
    	}
    	
    	return instance;
    }
	
	public int LoadSound(String filename, int Priority)
    {
		int 	SoundID;
		
		Log.i(TAG, "Load sound "+filename);
		
		String s = ResourceLocation + filename;
		
		int resID = context.getResources().getIdentifier(s, null, null);
		
		if (resID == 0)
		{
			Log.i(TAG, "unidentified resource id for "+filename);
			
			return 0;
		}
		
		SoundID = Sounds.load(context, resID, Priority);
		
	    return SoundID;
    } 
	
	public int LoadSoundAsset(String filename, int Priority)
    {
		int SoundID;
		
		AssetFileDescriptor FD = null;
		
		try 
		{
			FD = context.getAssets().openFd(filename);
		} 
		catch (IOException e) 
		{		
			e.printStackTrace();
		} 
	
		SoundID = Sounds.load(FD, Priority);
	    
	    return SoundID;
    } 
	
	public void PauseSound(int SoundID)
    {			
		Sounds.pause(SoundID);
    } 
	
	public void ResumeSound(int SoundID)
    {			
		Sounds.resume(SoundID);
    } 
	
	public void StopSound(int SoundID)
    {			
		Sounds.stop(SoundID);
    } 
	
	public int PlaySound(int SoundID, float lv, float rv, int priority, int loop, float rate)
    {					
		return Sounds.play(SoundID, lv, rv, priority, loop, rate); 
    }
	
	public void SetVolume(int SoundID, float vl, float vr)
    {					
		Sounds.setVolume(SoundID, vl, vr); 
    }
	
	public boolean UnloadSample(int SoundID)
    {					
		return Sounds.unload(SoundID); 
    }
	
	public void MusicSetDataSource(String filename)
	{		
		try 
		{	
			String s = ResourceLocation + filename;
			
			int resID = context.getResources().getIdentifier(s, null, null);
					
			if (resID == 0)
			{
				Log.i(TAG, "unidentified resource id for "+filename);
				
				return;
			}			
			
			 MusicPlayer = MediaPlayer.create(context, resID);
			 
			 if (MusicPlayer == null)
			 {
				 Log.i(TAG, "failed to create music player"+filename);
				 
				 return;
			 }
			 
			MusicPlayer.start();						
		} 
		catch (IllegalArgumentException e) 
		{		
			e.printStackTrace();
		}
		catch (IllegalStateException e) 
		{		
			e.printStackTrace();
		}  
	}

	public void MusicStart()
	{
		MusicPlayer.start();		
	}

	public void MusicVolume(float VolumeL, float VolumeR)
	{
		MusicPlayer.setVolume(VolumeL, VolumeR);
	}
	
	void SetMaxVolume()
	{
		AudioManager mAudioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
		int maxVolume = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);

		mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, maxVolume, 0);
	}

	public void MusicStop()
	{
		if (MusicPlayer != null && MusicPlayer.isPlaying())
		{
			MusicPlayer.stop();
			MusicPlayer.reset();
		}
	}
}