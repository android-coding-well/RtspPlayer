package com.gosuncn.mpa;

import android.app.Application;

import com.tencent.bugly.crashreport.CrashReport;

/**
 * Created by hwj on 2016/11/16.
 */

public class MyApplication extends Application {
    /**
     * Called when the application is starting, before any activity, service,
     * or receiver objects (excluding content providers) have been created.
     * Implementations should be as quick as possible (for example using
     * lazy initialization of state) since the time spent in this function
     * directly impacts the performance of starting the first activity,
     * service, or receiver in a process.
     * If you override this method, be sure to call super.onCreate().
     */
    @Override
    public void onCreate() {
        super.onCreate();
        CrashReport.initCrashReport(getApplicationContext(), "6b437c22dc", true);
    }
}
