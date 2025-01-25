package com.samp.mobile.launcher;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.ImageView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.LifecycleObserver;
import androidx.lifecycle.OnLifecycleEvent;
import androidx.lifecycle.ProcessLifecycleOwner;
import androidx.recyclerview.widget.RecyclerView;

import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.google.android.material.tabs.TabLayout;
import com.joom.paranoid.Obfuscate;
import com.samp.mobile.R;
import com.samp.mobile.launcher.adapters.FavouriteServerAdapter;
import com.samp.mobile.launcher.adapters.ServerAdapter;
import com.samp.mobile.launcher.config.Config;
import com.samp.mobile.launcher.data.FavoritesInfo;
import com.samp.mobile.launcher.fragments.HomeFragment;
import com.samp.mobile.launcher.fragments.ServerPagesItemFragment;
import com.samp.mobile.launcher.fragments.ServersFragment;
import com.samp.mobile.launcher.fragments.SettingsFragment;
import com.samp.mobile.launcher.util.ConfigValidator;
import com.samp.mobile.launcher.util.SAMPServerInfo;
import com.samp.mobile.launcher.util.SampQueryAPI;
import com.samp.mobile.launcher.util.SharedPreferenceCore;
import com.samp.mobile.launcher.util.SignatureChecker;
import com.samp.mobile.launcher.util.ViewPagerWithoutSwipe;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.net.DatagramSocket;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Objects;
import java.util.concurrent.TimeUnit;
@Obfuscate
public class MainActivity extends AppCompatActivity {

    public String[] tabTitles = { "Servers", "Info", "Settings" };
    public int[] tabImages = { R.drawable.ic_mainmenu, R.drawable.ic_server, R.drawable.ic_settingsmenu};
    public int[] tabSelectedImages = { R.drawable.ic_mainmenu_on, R.drawable.ic_serveron, R.drawable.ic_settingsmenu_on};

    public static ArrayList<SAMPServerInfo> mServersList = new ArrayList<>();
    public static ArrayList<SAMPServerInfo> mFavoriteServersList = new ArrayList<>();

    boolean bAdsInitialized = false;

    private int iCountQueue = 0;

    private int retryAttempt;

    int i1 = 0;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Config.currentContext = this;

        mServersList = new ArrayList<>();
        mFavoriteServersList = new ArrayList<>();

        ConfigValidator.validateConfigFiles(this);

        if(!SignatureChecker.isSignatureValid(this, getPackageName()))
        {
            Toast.makeText(this, "Use original launcher! No remake", Toast.LENGTH_LONG).show();
            return;
        }


        File file = new File(getExternalFilesDir(null) + "/download/update.apk");
        if (file.exists()) {
            file.delete();
        }

        FragmentManager fm = getSupportFragmentManager();
        ViewPagerAdapter sa = new ViewPagerAdapter(fm);
        ViewPagerWithoutSwipe pa = findViewById(R.id.fragment_place);
        pa.setAdapter(sa);

        TabLayout tabLayout = findViewById(R.id.constraintLayout);

        tabLayout.setupWithViewPager(pa);

        for(int  i = 0; i < tabLayout.getTabCount(); i++)
        {
            View inflate = LayoutInflater.from(this).inflate(R.layout.tablayout_item, (ViewGroup) tabLayout, false);

            ImageView image = inflate.findViewById(R.id.imageView2);
            image.setBackgroundResource(tabImages[i]);

            Objects.requireNonNull(tabLayout.getTabAt(i)).setCustomView(inflate);

            tabLayout.clearOnTabSelectedListeners();
            tabLayout.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
                @Override
                public void onTabSelected(TabLayout.Tab tab) {
                    getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
                    pa.setCurrentItem(tab.getPosition(), true);
                    ((ImageView)tab.getCustomView().findViewById(R.id.imageView2)).setBackgroundResource(tabSelectedImages[tab.getPosition()]);
                }

                @Override
                public void onTabUnselected(TabLayout.Tab tab) {
                    ((ImageView)tab.getCustomView().findViewById(R.id.imageView2)).setBackgroundResource(tabImages[tab.getPosition()]);
                }

                @Override
                public void onTabReselected(TabLayout.Tab tab) {

                }
            });

        }
        ((ImageView)tabLayout.getTabAt(0).getCustomView().findViewById(R.id.imageView2)).setBackgroundResource(tabSelectedImages[0]);

        getServersInfo();
        getFavoriteServersInfo();
    }

    public final ArrayList<SAMPServerInfo> getServerList() {
        return mServersList;
    }

    public final ArrayList<SAMPServerInfo> getFavoriteServerList() {
        return mFavoriteServersList;
    }


    public boolean getServersInfo()
    {
        final boolean[] z = {false};
        Volley.newRequestQueue(getApplicationContext()).add(new StringRequest("https://samp-mobile.shop/hosted.json", new Response.Listener<String>() {

            @Override
            public void onResponse(String response) {
                try {
                    JSONObject jsonObject  = new JSONObject(new String(response.getBytes(StandardCharsets.ISO_8859_1), StandardCharsets.UTF_8));
                    JSONArray jsonArray = jsonObject.getJSONArray("query");
                    for(int i = 0; i<jsonArray.length(); i++) {
                        JSONObject jSONObject = jsonArray.getJSONObject(i);
                        SAMPServerInfo sAMPServerInfo = new SAMPServerInfo();
                        sAMPServerInfo.setId(jSONObject.getInt("number"));
                        sAMPServerInfo.setServerName(jSONObject.getString("name"));
                        sAMPServerInfo.setAddress(jSONObject.getString("ip"));
                        sAMPServerInfo.setPort(jSONObject.getInt("port"));
                        sAMPServerInfo.setCurrentPlayerCount(jSONObject.getInt("online"));
                        sAMPServerInfo.setMaxPlayerCount(jSONObject.getInt("maxplayers"));
                        sAMPServerInfo.setHasPassword(jSONObject.getBoolean("password"));
                        sAMPServerInfo.setServerStatus(SAMPServerInfo.Status.ONLINE);
                        sAMPServerInfo.setPing(12);
                        getServerList().add(sAMPServerInfo);
                    }
                } catch (JSONException e) {
                    throw new RuntimeException(e);
                }

                for (Fragment fragment : getSupportFragmentManager().getFragments()) {
                    if (fragment instanceof ServersFragment) {
                        while (!fragment.isAdded()) {
                            try {
                                Thread.sleep(1000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        if(fragment.isAdded()) {
                            for (Fragment fragment2 : fragment.getChildFragmentManager().getFragments()) {
                                if ((fragment2 instanceof ServerPagesItemFragment) && ((ServerPagesItemFragment) fragment2).getPage() == 1 && fragment2.getView() != null) {
                                    Log.d("AXL", "getserverslist");
                                    ((RecyclerView.Adapter) Objects.requireNonNull(((RecyclerView) ((View) fragment2.requireView()).findViewById(R.id.server_recycler)).getAdapter())).notifyDataSetChanged();
                                }
                            }
                        }
                    }
                }

                z[0] = true;
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Log.d("x1y2z", "error " + error.toString());
                getServersInfo();
                z[0] = false;
            }
        }));

        return z[0];
    }

    public void getFavoriteServersInfo()
    {
        for(int i = 0; i< FavoritesInfo.getServerList(this).size(); i++)
        {
            SAMPServerInfo serverInfo = new SAMPServerInfo(i, i, "Loading...", FavoritesInfo.getServerList(this).get(i).ip, FavoritesInfo.getServerList(this).get(i).port, 0, 0, 0, 0, 0, "English");
            serverInfo.setFavorite(true);
            serverInfo.setQueried(false);
            getFavoriteServerList().add(serverInfo);
            new ProcessInfo().execute(i);
        }

        refreshFavoriteServers();
    }

    public class ProcessInfo extends AsyncTask<Integer, Void, Void> {
        public Void doInBackground(Integer... numArr) {
            int intValue = numArr[0].intValue();
            if(getFavoriteServerList().size() > intValue) {
                if(!getFavoriteServerList().get(intValue).getQueried()) {
                    SampQueryAPI sampQuery = new SampQueryAPI(getFavoriteServerList().get(intValue).getAddress(), getFavoriteServerList().get(intValue).getPort());
                    if (!sampQuery.mo7166d()) {
                        return null;
                    }
                    String[] info = sampQuery.mo7164b();
                    getFavoriteServerList().set(intValue, new SAMPServerInfo(intValue, intValue, info[3], getFavoriteServerList().get(intValue).getAddress(), getFavoriteServerList().get(intValue).getPort(), Integer.parseInt(info[1]), Integer.parseInt(info[2]), Integer.parseInt(info[0]), 0, 0, info[5]));
                    DatagramSocket datagramSocket = sampQuery.f7277a;
                    if (datagramSocket != null) {
                        datagramSocket.close();
                    }
                    getFavoriteServerList().get(intValue).setQueried(true);

                    refreshFavoriteServers();
                }
                else
                    return null;
            }
            return null;
        }

        public void onPostExecute(Void r1) {
            super.onPostExecute(r1);
        }
    }

    public void refreshFavoriteServers()
    {
        for (Fragment fragment : getSupportFragmentManager().getFragments()) {
            if (fragment instanceof ServersFragment) {
                while (!fragment.isAdded()) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                if(fragment.isAdded()) {
                    for (Fragment fragment2 : fragment.getChildFragmentManager().getFragments()) {
                        if ((fragment2 instanceof ServerPagesItemFragment) && ((ServerPagesItemFragment) fragment2).getPage() == 0 && fragment2 != null) {
                            RecyclerView view = Objects.requireNonNull( fragment2.requireView().findViewById(R.id.server_recycler));
                            if(view != null) {
                                RecyclerView.Adapter adapter = view.getAdapter();
                                if(adapter != null) {
                                    view.post(new Runnable() {
                                        @Override
                                        public void run() {
                                            adapter.notifyDataSetChanged();
                                        }
                                    });
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    public static void hideKeyboard(Activity activity) {
        InputMethodManager inputManager = (InputMethodManager) activity
                .getSystemService(Context.INPUT_METHOD_SERVICE);

        // check if no view has focus:
        View currentFocusedView = activity.getCurrentFocus();
        if (currentFocusedView != null) {
            inputManager.hideSoftInputFromWindow(currentFocusedView.getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
        }
    }

    public class ViewPagerAdapter extends FragmentPagerAdapter {

        public ViewPagerAdapter(FragmentManager fragmentManager) {
            super(fragmentManager);
        }

        @NonNull
        @Override
        public Fragment getItem(int position) {
            if(position == 1)
                return new ServersFragment();
            else if(position == 0)
                return new HomeFragment();
            else if(position == 2)
                return new SettingsFragment();
            return new HomeFragment();
        }

        @Override
        public int getCount() {
            return 3;
        }
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }
}
